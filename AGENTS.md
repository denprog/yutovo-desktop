# Yutovo Project Agent Notes

## Project Structure
- `yutovo-calculator/` — Core math engine (Real, Complex, Integer, Rational, Symbolic)
- `yutovo-solver/` — Solver service (WebSocket-based calculation backend)
- `yutovo-editor/` — Document editor with MathML rendering and solver integration
- `yutovo-desktop/` — Desktop GUI application using `yutovo-editor` and `yutovo-solver`

## External Dependencies
- `../third-party/giac-2.0.0/` — Giac source code (used for symbolic integration and CAS operations)

## Agent Rules

### Mandatory tooling
- **Always use repowise — this rule is mandatory.** For every codebase question, exploration, or before modifying files, you must use repowise tools (`get_answer`, `get_context`, `get_risk`, `search_codebase`, etc.) first. Do not use raw `grep`, `Read`, or `Glob` as the primary way to understand code, find symbols, or assess risk. It is acceptable to use raw tools only to read a file path or line range that repowise has already identified.
- **Never delete files without explicit user permission.** Do not remove source files, test files, core dumps, logs, build artifacts, or any other files unless the user explicitly asks for it. When in doubt, leave the file in place and ask.
- **Never create separate namespaces (such as `namespace detail` or anonymous namespaces) without explicit user permission.** Helper functions should be placed in the common `yutovo_calculator` namespace, for example in `utils.h/utils.cpp` or `giac_utils.h/giac_utils.cpp`, or as `static` methods of the appropriate class.
- **Never commit without explicit user permission.** Do not run `git commit`, `git push`, `git reset`, `git rebase`, or any other git mutations unless explicitly asked to do so. Ask for confirmation each time when git mutations are needed.
- **Never create branches or delete stashes without explicit user permission.** Do not run `git branch`, `git checkout -b`, `git stash branch`, `git stash drop`, `git stash pop`, `git stash clear`, or similar commands that create branches or remove stashes unless the user explicitly asked for it.
- **Never delete existing tests.** When fixing regressions or refactoring, update test expectations to match the new correct behavior, but do not remove tests. If `git checkout` or similar commands are used to revert a file, verify that no user-added tests were lost.

### Code Style
Always place braces on their own line for control structures.

For constructors, place the colon of the initializer list on the same line as the constructor signature. Put each initializer/base on its own line with a normal 4-space indent, and place the opening brace on its own line:
```cpp
// CORRECT
ExpressionNode(const DefiniteIntegralNode<Number>& node)
    : first(node)
{
}

// WRONG
ExpressionNode(const DefiniteIntegralNode<Number>& node) : first(node) {}
```

If a function has a definition, separate it from surrounding code with blank lines before and after:
```cpp
// CORRECT
ExpressionNode(const DefiniteIntegralNode<Number>& node)
    : first(node)
{
}

ExpressionNode(const DerivativeAtPointNode<Number>& node)
    : first(node)
{
}

// WRONG
ExpressionNode(const DefiniteIntegralNode<Number>& node)
    : first(node)
{
}
ExpressionNode(const DerivativeAtPointNode<Number>& node)
    : first(node)
{
}
```

### Naming
Avoid abbreviations in identifiers; use full words. For example, prefer `Context` over `Ctx`, `Symbols` over `Syms`, etc.

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

## Current Work: Symbolic Integration

### yutovo-calculator
- **Class `Symbolic`** (`src/symbolic.h/cpp`) wraps `giac::gen` via `std::unique_ptr`.
- `Parser<Symbolic>`, `Solver<Symbolic>`, `Expression<Symbolic>` are specialized for `Symbolic<Real>`, `Symbolic<Rational>`, and `Symbolic<Complex>`.
- `^` is parsed left-associatively in the generic grammar. Giac flattens `(x^y)^z` to `x^(y*z)`; the formatter reconstructs `pow(pow(x,y),z)` for left-associative chains and prints explicit right-associative chains like `x^(y^z)` as `pow(x,y**z)`.
- `evalf` returns `Symbolic`; explicit `to_real()` / `to_complex()` perform casting.
- `indefinite_integral(expr, var)` computes the symbolic antiderivative using `giac::_integrate`; the integration variable must be an identifier.
- `definite_integral(a, b, expr, var)` computes the exact antiderivative with `giac::_integrate`, simplifies it with `giac::simplify`, and, for `Symbolic<Real>`/`Symbolic<Complex>`, evaluates the result numerically when it is a constant (so numeric bounds produce a decimal number instead of an exact expression containing constants such as `e`).
- Numeric `definite_integral` for `Real`/`Complex` first tries giac integration; if giac fails (for example because the integrand calls a user-defined function), it falls back to adaptive Simpson quadrature implemented in `Solver::NumericalDefiniteIntegral`.
- `sqrt(x)` is implemented for all symbolic types via `root(x, 2)` (i.e. `pow(x, 1/2)`). `sqrt(-∞)` returns `nan` for Real/Complex and throws for Rational.
- `cot(x)`, `sec(x)`, `csc(x)`, `coth(x)`, `sech(x)`, `csch(x)` are rendered as inert functions (`yut_cot`, etc.) so that poles map to `∞` instead of being simplified away.
- Inverse hyperbolic functions (`asinh`, `acosh`, `atanh`, `acoth`, `asech`, `acsch`) and their `arc...`/`ars...` synonyms are registered for all symbolic parsers. Inert wrappers preserve names; singularities such as `acoth(1)` and `acsch(0)` evaluate to `∞` for Real/Complex and remain symbolic for Rational.
- `fact(x)` (postfix `!`) is implemented for all symbolic types as a product `1*2*...*n` for non-negative integer arguments; returns `factorial(x)` for symbolic/non-integer arguments. The product uses context-aware `giac::operator_times` so release builds use the parser's private `giac::context`.
- Inert trigonometric/hyperbolic wrappers (`cot`, `sec`, `csc`, `coth`, `sech`, `csch`, `asinh`, `acosh`, `atanh`, `acoth`, `asech`, `acsch`) and `factorial(x)` are built directly via `InertCall()` / `giac::symbolic(*giac::at_factorial, ...)` in `src/symbolic.h`, without converting the argument back to a string. `subs` compares singularities (`acoth(1)`, `acsch(0)`) using the same direct construction instead of printed strings.
- `Symbolic<Number>::ToJson()` builds an AST from the giac-printed string and emits JSON using yutovo-editor element type codes (7=CODE_ROW, 8=CODE_STRING, 10=SHAPE, 11=PLUS, 12=MINUS, 13=MULTIPLY, 14=DIVISION, 15=POWER, 16=SQUARE_ROOT, 17=NTH_ROOT, 45-47=SYMBOLIC_*_RESULT). Division operands are wrapped in a single `CODE_ROW`; scientific numbers are emitted as flat elements so they do not add extra nesting inside sums or products.
- `subs` uses `giac::limit` to detect essential singularities of `exp`, `sin`, `cos`, `sinh`, `cosh` and returns `nan` for Real/Complex (throws for Rational).
- All giac-specific helpers live in `src/giac_utils.h` / `src/giac_utils.cpp` in the common `yutovo_calculator` namespace; `src/utils.h` / `src/utils.cpp` contain only general-purpose utilities.
- `GiacOutputGuard` (`src/giac_utils.h`) redirects `stdout`/`stderr` to `/dev/null` around giac calls to suppress spurious diagnostic output (e.g. numeric integration progress comments).
- Numeric definite-integral results are converted from `giac::gen` to `Real`/`Complex`/`Rational`/`Array<Real>` via `FromGiac<>` in `src/giac_utils.h`, using direct MPFR/MPQ extraction plus decimal rounding for `Real`/`Complex` instead of string round-tripping.
- `WrongArgumentsCount` exceptions report a `size` that covers the full function call up to and including the closing parenthesis. `ExpressionPosition::size` is populated by `Annotation` for function-call nodes, and `Solver::CallSize()` returns it (falling back to the identifier length when unset).
- `RealNumberStr` evaluates rational strings such as `1333/1000` with MPFR so that `CombineLikeTerms` does not turn decimal reciprocals into bogus scientific notation (e.g., `1.333E+3`). `AddCoeffs` / `MultiplyCoeffs`, `EvaluateGiacExpression`, `IsGiacExpressionZero`, and Rational-mode evaluation in `EvaluateArithmeticExpr` / inert-function handling now convert the resulting `giac::gen` to `Real`/`Rational` via `FromGiac<>` instead of calling `giac::gen::print` and parsing the string.
- On Linux `yutovo-calculator` links the **system** MPFR/GMP libraries and a static **giac** library found in `${INSTALL_PATH}/lib`. Debug builds must link `${INSTALL_PATH}/lib/libgiacd.a` and release builds `${INSTALL_PATH}/lib/libgiac.a`; mixing configurations causes an ABI mismatch and memory corruption inside giac (e.g., crashes in `giac::expand`). The Emscripten/wasm build links `${INSTALL_PATH}/wasm/libgiac.a`, `${INSTALL_PATH}/wasm/libmpfr.a`, `${INSTALL_PATH}/wasm/libgmp.a`, and `${INSTALL_PATH}/wasm/libgmpxx.a`, and compiles `src/symbolic.cpp` against the wasm build of giac; the former header-only stub has been removed.
- Linux consumers of the installed `yutovo-calculator` target must also call `pkg_check_modules(mpfr REQUIRED IMPORTED_TARGET mpfr)` and `pkg_check_modules(gmp REQUIRED IMPORTED_TARGET gmp)` because they are recorded in the imported target's `INTERFACE_LINK_LIBRARIES`.
- Each `Parser<Symbolic<...>>` creates a private `giac::context` and publishes it via the thread-local `current_giac_context` pointer in `src/giac_utils.h`. All `Symbolic` operations in that thread use this context through `Symbolic::Context()` instead of their own per-object `context`, giving each parser its own independent giac symbol table without a process-wide parser mutex.
- Because giac shares mutable one-letter identifier objects (`x__IDNT_e`, `y__IDNT_e`, ...) during string parsing, `ParseGen()` in `src/giac_utils.cpp` takes a global `giac_parsing_mutex` and returns a copy of the parsed expression where every clonable single-letter variable (`a`..`z` except the constants `e` and `i`) is replaced by a freshly allocated `giac::identificateur`. After parsing, parallel threads operate on private identifier copies and do not race on giac's global identifier reference counts.
- Non-symbolic computations that call giac (for example `definite_integral` evaluation for `Real`, `Complex`, `Rational` and `Array<Real>` in `src/solver.h`) take a separate global `giac_evaluation_mutex`. Giac is not thread-safe for concurrent calls to `_integrate`, `_romberg`, `evalf` and similar operations even with separate `giac::context` objects, so these calls are serialized.
- `GiacParserMutex` and the `std::mutex` include in `giac_utils.h`/`symbolic.h` have been removed; the only remaining mutexes in the calculator source are `giac_parsing_mutex`, `giac_evaluation_mutex` and unrelated `export_mutex` locks in `src/export.cpp`.

### yutovo-solver
- `ResultType::SYMBOLIC` added in `types.h`.
- `CalculatorSolver` has `symbolic_parser` and `SolveSymbolic()`.
- `AUTO` mode tries `SYMBOLIC` last in `results_order[6]`.
- On Linux `yutovo-solver` does **not** link GMP/MPFR directly; it inherits them transitively from `yutovo-calculator` / giac.

### yutovo-editor
- `ElementType::SYMBOLIC_RESULT` added.
- `Config::SymbolicResultConfig` (default `precision=10`) in `config.h/cpp`.
- `SymbolicResult` (`formulas/result.h/cpp`) parses the solver result string and builds formula elements (`CodeString`, `Plus`, `Minus`, `Multiply`, `OpenBracket`, `CloseBracket`, `Comma`) inside `ResultRow`.
- `SymbolicSolverTask` sends JSON with `result_type=7` (`SYMBOLIC`) and `symbolic_precision`.
- `Equation::UpdateResult` **must** handle `ResultType::SYMBOLIC` by creating `SymbolicResult`; missing this causes tests to hang because no solver task is ever dispatched.
- `ResultTask::Execute` **must** handle `ElementType::SYMBOLIC_RESULT` to deliver the solver response to `SymbolicResult::PutResult`; missing this leaves the waiting symbol (`~`) forever.
- The `yutovo-editor` target defines `HAVE_LIBMPFR` PUBLIC in `src/CMakeLists.txt` so that giac headers use the same `real_object` layout (MPFR-based) as the linked giac library.

### yutovo-desktop
- `src/CMakeLists.txt` locates the static **giac** library with `find_library(GIAC_LIBRARY ...)` and creates an imported `giac_imported` target for Linux, because static libraries (`yutovo-calculator`, `yutovo-solver`) now depend on giac.
- Same for `test/CMakeLists.txt`.
- Added "Symbolic" tab in `ResultSettingsForm` (precision setting).
- Added `Present as → Symbolic` in context menu (`DocumentWindow`).
- Added `\eq_sym` command in `CommandMap`.
- Added `Symbolic` translations in `.ts` files (ru: Символьные, es: Simbólicos, pt_BR: Simbólicos).

## Editor Test Patterns

### Entering expressions in code blocks
Do **not** put operators inside `InsertString`:
```cpp
// WRONG
 document.InsertString("x+1", true);

// CORRECT
 document.InsertString("x", true);
 document.InsertPlus(true);
 document.InsertString("1", true);
```

Use dedicated insert methods for operators:
- `InsertPlus(true)` / `InsertMinus(true)`
- `InsertMultiply(true)` / `InsertDivision(true)`
- `InsertPower(true)` — wraps the preceding string element into the base automatically; caret moves to exponent.
- `InsertOpenRoundBracket(true)` / `InsertCloseRoundBracket(true)`
- `InsertComma(true)`

Functions without dedicated inserts (e.g. `sin`, `derivative`, `expand`, `subs`) are entered as strings followed by brackets.

### Power behavior
`InsertPower(true)` after a string moves that string into the base and places the caret in the exponent.
Example:
```cpp
document.InsertString("x", true);
document.InsertPower(true);
document.InsertString("2", true);
// Parser text: pow(x,2)
```

### Checking results
Check full `ToHtml()` exactly like `SolverSymbolicTest::solver1`:
```cpp
ASSERT_TRUE(document.ToHtml() ==
    "<body>"
        "<p>"
            "<math xmlns='http://www.w3.org/1998/Math/MathML'>"
                "<mrow>"
                    "<mrow>"
                        "<mi>x</mi>"
                        "<mo>+</mo>"
                        "<mi>1</mi>"
                    "</mrow>"
                    "<mo>=</mo>"
                    "<mrow>"
                        "<mrow>"
                            "<mi>1</mi>"
                            "<mo>+</mo>"
                            "<mi>x</mi>"
                        "</mrow>"
                    "</mrow>"
                "</mrow>"
            "</math>"
        "</p>"
    "</body>") << document.ToHtml();
```

HTML structure:
- `CodeParagraph` → `<math xmlns='http://www.w3.org/1998/Math/MathML'>`
- `CodeRow` → `<mrow>`
- `CodeString` → `<mi>text</mi>`
- `Plus` / `Minus` → `<mo>+</mo>` / `<mo>-</mo>`
- `Multiply` → `<mo>×</mo>` (not `<mo>*</mo>`)
- `Power` → `<msup><mrow>base</mrow><mrow>exp</mrow></msup>`
- `SymbolicResult` (inside last `CodeRow`) produces `<mrow><mrow>elements...</mrow></mrow>` because `ResultRow` (`CodeColumn`) wraps its `CodeRow` content.

### Undo checks
If the test calls `Undo()`, always assert the document state after undo:
```cpp
document.Undo();
document.WaitUndo();
std::this_thread::sleep_for(200ms);
ASSERT_TRUE(document.ToHtml() == "<body>...</body>") << document.ToHtml();
```

### Build
Each component is built and tested from its own `build/debug` subdirectory (in-tree builds are not used):
```bash
cd yutovo-calculator/build/debug && make -j4 yutovo-calculator_tests
./test/yutovo-calculator_tests

cd yutovo-solver/build/debug && make -j4

cd yutovo-editor/build/debug && make -j4 yutovo-editor_tests
./test/yutovo-editor_tests

cd yutovo-desktop/build/debug && make -j4 yutovo-desktop

# Emscripten/wasm build (from yutovo-calculator)
cd yutovo-calculator/build_web/debug && make -j4 yutovo-calculator
```
Use `-j16` maximum for building on any platform.

### Test runtime
Running the full `yutovo-editor_tests` suite takes approximately **25 minutes** (symbolic tests are particularly slow due to WebSocket solver round-trips).

## Critical Files Changed
- `yutovo-editor/src/formulas/equation.cpp` — added `ResultType::SYMBOLIC` case in `UpdateResult`
- `yutovo-editor/src/task.cpp` — added `ElementType::SYMBOLIC_RESULT` case in `ResultTask::Execute`
- `yutovo-editor/src/formulas/result.cpp` — `SymbolicResult::AddSymbolicElements` parses result string into formula elements
- `yutovo-editor/test/solver_symbolic.cpp` — rewritten to use proper element insertion and exact HTML checks, including undo assertions; added `solver6` (fraction result), `solver7` (complex result), `solver8` (power result `1/x`), `solver9` (power result `x^-1`)
- `yutovo-editor/test/solver_auto.cpp` — added 5 AUTO-mode symbolic fallback tests (`symbolic1..symbolic5`) using `ToText()` checks; added `symbolic6` (fraction) and `symbolic7` (complex)
- `yutovo-desktop/src/CMakeLists.txt` — added `find_library(GIAC_LIBRARY ...)` and imported `giac_imported` target
- `yutovo-desktop/test/CMakeLists.txt` — added `find_library(GIAC_LIBRARY ...)` and imported `giac_imported` target
- `yutovo-desktop/src/result_settings_form.ui` / `.cpp` — added "Symbolic" tab with precision
- `yutovo-desktop/src/document_window.cpp` / `.h` — `Present as → Symbolic` is now a submenu containing checkable items `Real`, `Rational`, `Complex`
- `yutovo-desktop/src/command_map.cpp` — added `\eq_sym` command
- `yutovo-desktop/src/mainwindow.cpp` — added symbolic result in settings load/save
- `yutovo-desktop/src/translations/*.ts` — added `Symbolic` translations

- `yutovo-calculator/src/giac_utils.h` / `giac_utils.cpp` — new files holding giac-specific helpers (moved from `symbolic.h`/`symbolic.cpp`)
- `yutovo-calculator/src/utils.h` / `utils.cpp` — restored to general-purpose utilities
- `yutovo-calculator/src/ast.h` — added `ExpressionPosition::size`
- `yutovo-calculator/src/annotation.h` — `Annotation` now computes `size` for function-call nodes by scanning from the identifier to the matching closing fence
- `yutovo-calculator/src/solver.h` / `solver.cpp` — added `Solver::CallSize()`; all `WrongArgumentsCount` throws now use `CallSize(op)`; `DefiniteIntegralNode` evaluation takes `giac_evaluation_mutex` because giac integration/evalf is not thread-safe; added `Solver::NumericalDefiniteIntegral` adaptive-Simpson fallback for `Real`/`Complex` when giac integration fails (e.g. integrands calling user-defined functions)
- `yutovo-calculator/src/CMakeLists.txt` — builds and installs `giac_utils.h` / `giac_utils.cpp`
- `yutovo-calculator/test/real.cpp` — updated `CalcTestReal.errors5` to expect `size == 6` for `sqrt();`; added `CalcTestReal.threads_integrals` (50 threads × 20 iterations computing 4 definite integrals each); added `CalcTestReal.definite_integral_user_function` for numeric integration of a user-defined function
- `yutovo-calculator/test/symbolic_real.cpp` — added `CalcTestSymbolicReal.simplify_wrong_args` regression test and `CalcTestSymbolicReal.threads_variables` (50 threads × 20 iterations with independent parsers); actual result vectors are cleared after each iteration; added `definite_integral_user_function` and `definite_integral_constant_with_e` tests
- `yutovo-calculator/test/symbolic.cpp` — added `variables_rational`, `variables_complex`, `user_functions_rational`, `user_functions_complex` tests
- `yutovo-calculator/src/symbolic.cpp` — fixed `ReplacePowerOperator` to correctly handle parenthesized exponents (e.g. `x**(-1)` → `pow(x,-1)`)
- `yutovo-calculator/src/parser.cpp` — registered `asinh`/`acosh`/`atanh`/`acoth`/`asech`/`acsch` and `arcsinh`/`arccosh`/`arctanh`/`arccoth`/`arcsech`/`arccsch`/`arccosech` synonyms for symbolic parsers; constructors now create a per-parser `giac::context` and set `current_giac_context`
- `yutovo-calculator/src/parser.h` — added `giac_context` member, destructor clears `current_giac_context`, `Parse()` re-publishes the context for the calling thread
- `yutovo-calculator/src/giac_utils.h` / `giac_utils.cpp` — removed `GiacParserMutex`; added `thread_local giac::context* current_giac_context`; added `giac_parsing_mutex`, `giac_evaluation_mutex`, `FreezeStaticGiacIdentifiers()` and `CloneSingleLetterIdentifiers()`; arithmetic helpers (`SimplifyPowerDivision`, `AddCoeffs`, `MultiplyCoeffs`) now use context-aware `giac::operator_plus`/`operator_minus`/`operator_times`/`rdiv`
- `yutovo-calculator/src/symbolic.h` / `symbolic.cpp` — renamed `Ctx()` to `Context()`; replaced `&context` with `Context()`, replaced binary `+`/`-`/`*`/`/` operators with context-aware giac equivalents; symbolic `definite_integral` now simplifies with `giac::simplify` and evaluates constant `Symbolic<Real>`/`Symbolic<Complex>` results numerically
- `yutovo-calculator/test/symbolic_real.cpp`, `symbolic_rational.cpp`, `symbolic_complex.cpp` — added `hyperbolic_inverse_synonyms` tests
- `yutovo-editor/src/formulas/result.cpp` — `SymbolicResult::AddSymbolicElements` now parses `pow(base,exp)` into `Power` formula elements
- `yutovo-editor/src/formulas/power.h` — added `GetBaseRow()` and `GetExponentRow()` public helpers

- `yutovo-calculator/src/giac_utils.cpp` — removed `funcpow(args,exp)` shorthand in `EmitString`; powers of functions now render as `pow(func(args),exp)`
- `yutovo-calculator/src/symbolic.h` — `derivative(...)` now applies a single-level `giac::eval` so numeric coefficients like `2*2` collapse to `4`
- `yutovo-calculator/test/symbolic_real.cpp` — updated `all_symbolic_functions` expectations and added `derivative_mixed_second_order` test
- `yutovo-calculator/test/symbolic_rational.cpp` / `symbolic_complex.cpp` — updated `trig_power1` expectations

## Current Work: Multi-variable derivative at point

### yutovo-calculator
- `DerivativeAtPointNode<Number>` now stores a `std::list<DerivativeVariableNode<Number>>` instead of a single variable/value pair.
- Syntax changed from `derivative(expr, var, value)` to `derivative(expr, [var=value, ...])`. The first variable in the list is the differentiation variable; the remaining variables are substituted into the expression before evaluating the derivative at the point.
- `derivative_at_point` grammar rule uses `symbolic_arg` for the function/expression argument and a list of `derivative_variable = name >> '=' > expression` inside `[ ... ]`.
- `derivative_at_point` is placed first in the `unary` alternative list and wrapped with `qi::hold[]` so that a failed match backtracks cleanly instead of falling through to `array` or `function_call`.
- `Solver::operator()(DerivativeAtPointNode<Number>)` for symbolic types parses the function string into an `ExpressionNode`, computes the symbolic derivative with respect to the first variable, and substitutes all variable values.
- For non-symbolic `Real`, `Complex`, and `Rational`, `EvaluateDerivativeAtPoint` uses giac `derive`/`subst` for exact results; `NumericalDerivativeAtPoint` evaluates the partial derivative numerically by substituting all non-differentiated variables before taking the finite-difference step.
- The values of derivative-at-point variables are evaluated from the parsed `ExpressionNode` AST (just like `LoopNode` variables) and passed to giac directly via `NumberToGiac` in `src/giac_utils.h`/`giac_utils.cpp`; they are no longer stringified and re-parsed with `ParseGen`.
- The `^` power operator is now supported in the `Real`, `Complex`, and `Rational` expression grammars (previously it was only available for `Integer` and symbolic types). It binds tighter than `*`/`/`/`%` and produces an `OperationNode` with op `'^'`, which `Solver` evaluates via the existing `pow` functions.
- Tests added/updated in `test/real.cpp`, `test/rational.cpp`, `test/complex.cpp`, `test/symbolic_real.cpp`, `test/symbolic_rational.cpp`, `test/symbolic_complex.cpp`.

## Next Steps / Blockers
- All Linux calculator debug tests pass (1232 tests across Real, Complex, Integer, Rational, Array, Symbolic, etc.).
- `yutovo-solver` tests pass (40 tests).
- The three failing `SolverSymbolicTest` editor tests (`solver17`, `solver18`, `solver22`) have been fixed by adjusting calculator JSON output; no editor tests were modified.
- The Emscripten/wasm symbolic stub is implemented; native behavior is unchanged.
- No remaining blockers for the multi-variable derivative-at-point feature.
