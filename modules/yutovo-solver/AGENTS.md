# Yutovo Solver Agent Notes

## Project
WebSocket-based calculation backend for the Yutovo editor.

## Repowise
- Always use the `repowise` MCP tools (`get_answer`, `get_context`, `get_risk`, `get_health`, `get_change_risk`, `search_codebase`, etc.) instead of manual `grep`/`glob` exploration when answering questions about code, architecture, risks, or file health.
- Prefer `get_answer` for natural-language questions, `get_context` for specific files/symbols, `get_risk` before editing risky files, and `get_health`/`get_change_risk` to assess code health or diff risk.

## Symbolic Integration
- `ResultType::SYMBOLIC_REAL`, `SYMBOLIC_RATIONAL`, `SYMBOLIC_COMPLEX` added in `types.h` (old `SYMBOLIC` removed).
- `CalculatorSolver` has `symbolic_parser` and `SolveSymbolicReal()`, `SolveSymbolicRational()`, `SolveSymbolicComplex()`.
- `AUTO` mode tries symbolic types last in `results_order[8]`.
- On Linux GMP/MPFR are inherited transitively from `yutovo-calculator`; `src/CMakeLists.txt` calls `pkg_check_modules(mpfr REQUIRED IMPORTED_TARGET mpfr)` and `pkg_check_modules(gmp REQUIRED IMPORTED_TARGET gmp)` to satisfy the imported target's interface.

## Code Style
- Opening braces always go on a new line (Allman style):
  ```cpp
  if (condition)
  {
      // ...
  }
  ```
- Naming: use `snake_case` for constants and variables; do not use a `k` prefix (e.g., `doc_count`, not `kDocCount`).
- Comments inside functions: keep on a single line if they fit, if a comment is a single sentence, start with a lowercase letter and omit the trailing period.
- Do not add a space after `//`.
- Keep expressions that fit on a single line inside `if`, `for`, `while`, and function calls on one line; only split them when they are too long.
- When splitting an expression across lines, indent the continuation by 4 spaces.
- Do not use `(void)var;` to silence unused-variable warnings. Remove the variable name from the signature, rewrite the loop to avoid the unused binding, when the name is needed conditionally.
- Do not create anonymous namespaces. Declare file-local helpers as `static` functions inside the named project namespace.

### Parenthesized expressions
Keep the contents of parentheses (function argument lists, conditions, initializers, etc.) on a single line when it fits. Only wrap to a new line if the expression would exceed **140 columns**.
When a parenthesized expression is wrapped, each continuation line uses the normal **4-space indent**; do not align arguments with the opening parenthesis.
```cpp
// CORRECT
void ShortFunction(int a, int b, int c);

void LongFunctionName(const std::u32string& first_argument, const std::u32string& second_argument,
    int third_argument);

auto result = SomeFunction(first_argument, second_argument,
    third_argument, fourth_argument);

if (condition_a && condition_b)
{
    // ...
}

// WRONG
void LongFunctionName(
    const std::u32string& first_argument,
    const std::u32string& second_argument,
    int third_argument);

void LongFunctionName(const std::u32string& first_argument,
                      const std::u32string& second_argument,
                      int third_argument);

try {
    // ...
} catch (...) {
    // ...
}
```

### Spaces around brackets
Do not put spaces before or after square brackets `[]` and round brackets `()`:
```cpp
// CORRECT
int arr[10];
void foo(int a);
arr[0] = foo(1);

// WRONG
int arr [10];
void foo (int a);
arr [0] = foo (1);
```

### Lambdas
Place the capture clause on a new line, indented by 4 spaces. Parameters, the `->` return type, and the opening brace follow the normal rules: parameters and return type stay on the same line as the capture clause, and the opening brace goes on its own line.
```cpp
// CORRECT
auto callback =
    [](int value) -> bool
    {
        return value > 0;
    };

auto reference =
    [&]() -> void
    {
        DoWork();
    };

// WRONG
auto callback = [](int value) -> bool {
    return value > 0;
};

auto callback =
    [](int value) -> bool {
    return value > 0;
};
```

## Build
- Use `-j16` maximum for building to avoid OOM kills.
- For Emscripten builds activate the toolchain first: `source ~/emsdk/emsdk_env.sh`.
- Web build directories are `build_web/debug` and `build_web/release`.
- Native Linux build directories are `build/debug` and `build/release`.

## Worker process
- On desktop (`YUTOVO_SOLVER_WORKER` in `types.h`: not emscripten, not Windows) the parsers run in a separate process (`yutovo-solver-calculator-worker`), one worker per document, so a long-running calculation can be killed when `BREAK_SOLVING` cannot interrupt it cleanly.
- `Solvers::GetSolver()` creates a proxy `CalculatorSolver` with a null `parser_context`; all its methods forward the request over `SolverProcess` IPC (JSON lines over stdin/stdout). Inside the worker the same `CalculatorSolver` class runs with a real `parser_context` and does the actual parsing.
- The request envelope carries `language` and `max_time` alongside the action; the worker applies them to its solvers (`SetLocale` on change, `SetMaxTime` per request).
- `BREAK_SOLVING` sends the break to the worker (its reader thread handles it immediately), waits up to 1 second for the active request to finish, then SIGKILLs the process; a killed solve is reported as a `TimeExceed` parser error and the next request starts a fresh worker (its first solve returns `SOLVER_RESTARTED_ERROR` so the editor resynchronizes).
- `SendBreak` must not take `SolverProcess::action_mutex`: an in-flight `SendAction` holds it until its reply arrives.
- `CalculatorSolver::Solve` (real path) arms `SolveTimeoutWatchdog`: when `max_time` (+250 ms reserve for the in-parser CPU timer) expires while a giac evaluation ignores the parser timer, the watchdog sets the giac interrupt flags (`ctrl_c`/`interrupted`, the same mechanism `BREAK_SOLVING` uses) and the reply is reported as `TimeExceed`. This is what bounds long solves on the web, where there is no worker process to kill; on desktop the worker self-limits this way before the parent's `max_time + 15 s` kill.
- `CalculatorSolver::SetLocale` skips the parser rebuild when the language is unchanged (parsers are created with the current language); the editor always sends `SET_LOCALE` after `SOLVER_RESTARTED_ERROR`, and the unconditional rebuild cost ~3 s in the wasm build.
- The editor side (`yutovo-editor/src/solver.cpp`) drops queued solve tasks of a deleted result row by the row's `task_guid` when breaking (the logical id is reused across remakes), otherwise a stale heavy solve can outlive its row and burn the full `max_time`.
- Worker lookup order: explicit `YUTOVO_SOLVER_WORKER_PATH` → directory next to the running executable and its `../src` → `YUTOVO_DEPLOY/bin` → current directory → `PATH`. The worker executable must always sit next to the main one: `yutovo-editor/test/CMakeLists.txt` copies it next to the tests binary as a POST_BUILD step from `${INSTALL_PATH}/bin`, so run `make install` in `yutovo-solver` before rebuilding the editor.

## Web build notes
- Emscripten builds do not use a separate worker; the single document is solved in the main wasm thread and interrupted through the standard giac `break_solving` flag set by `BREAK_SOLVING`.
- Build and install the solver library from `yutovo-solver/build_web/debug` or `build_web/release`:
  `make -j16 -C build_web/debug && make install -C build_web/debug`
- Build `yutovo-web` debug (`make -j16 -C build_web/debug`) and then rebuild the Quasar SPA:
  `cd src/site && npx quasar build`

## Tests
- When verifying solver changes with `yutovo-editor` tests, run its test binary from the editor's `build/debug` directory (`./test/yutovo-editor_tests ...`), not from `build/debug/test`: tests that load `.yut` documents use `../../test/tests/...` paths relative to the working directory and hang forever in `WaitSolver` when started from the wrong directory (see the note in `yutovo-editor/AGENTS.md`).
- The web long-solving test runs from `yutovo-web/src/site` with the local `yutovo-serverd` (nginx 443 → 9001) and the SPA served from `src/site/dist/spa`:
  `env $(grep -v '^#' ./../../yutovo-server.env | xargs) npx playwright test -g 'interrupt long definite integral' --workers=1`
  After changing the solver, rebuild the chain first: solver `build_web/debug` (+install) → `yutovo-web/build_web/debug` → `cd src/site && npx quasar build`.
- `yutovo-web` Playwright tests run from `src/site` with `npx playwright test <spec>`.
- Tests that exercise login and document lists (e.g. `trace_open.spec.js`) need the real local backend proxied on the same origin:
  1. Start `yutovo-server` from `yutovo-server/build/debug` with:
     `env $(grep -v '^#' ../../yutovo-server.env | xargs) ./src/yutovo-serverd`
  2. Start the SPA+API proxy from `yutovo-solver` on port 9002:
     `node /home/denis/programs/Math/yutovo/yutovo-solver/test-proxy-server.js`
  3. Run Playwright tests from `yutovo-web/src/site` with the local backend environment:
     `env $(grep -v '^#' ./../../yutovo-server.env | xargs) npx playwright test --workers=1`
