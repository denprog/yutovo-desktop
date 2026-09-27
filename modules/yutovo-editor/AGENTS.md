# Yutovo Editor Agent Notes

## Project
Document editor with MathML rendering and solver integration.

## Agent Rules

- **Use `repowise` for codebase navigation and search.** Prefer `mcp__repowise__get_answer`, `mcp__repowise__get_context`, `mcp__repowise__search_codebase`, and related tools over manual `Read`/`Grep` when exploring unfamiliar code, locating symbols, or analyzing architecture.
- **Never delete files without explicit user permission.** Do not remove source files, test files, core dumps, logs, build artifacts, or any other files unless the user explicitly asks for it. When in doubt, leave the file in place and ask.
- **Never create separate namespaces (such as `namespace detail` or anonymous namespaces) without explicit user permission.** Helper functions should be placed in the common `yutovo_calculator` namespace, for example in `utils.h/utils.cpp` or `giac_utils.h/giac_utils.cpp`, or as `static` methods of the appropriate class.
- **Never commit without explicit user permission.** Do not run `git commit`, `git push`, `git reset`, `git rebase`, or any other git mutations unless explicitly asked to do so. Ask for confirmation each time when git mutations are needed.
- **Never delete existing tests.** When fixing regressions or refactoring, update test expectations to match the new correct behavior, but do not remove tests. If `git checkout` or similar commands are used to revert a file, verify that no user-added tests were lost.
- **Add new `ElementType` values only at the end of the enum.** `editor_utils.h` serializes element types as integers, so inserting values in the middle changes the numeric IDs of existing types and breaks saved documents.

## Symbolic Integration
- `ElementType::SYMBOLIC_REAL_RESULT`, `SYMBOLIC_RATIONAL_RESULT`, `SYMBOLIC_COMPLEX_RESULT` added (old `SYMBOLIC_RESULT` removed).
- `Config::AutoResultConfig` now contains `symbolic_real_result` (`RealResultConfig`), `symbolic_rational_result` (`RationalResultConfig`), `symbolic_complex_result` (`ComplexResultConfig`).
- `SymbolicRealResult` / `SymbolicRationalResult` / `SymbolicComplexResult` inherit from `RealResult` / `RationalResult` / `ComplexResult` and override `PutResult` to parse symbolic strings via `AddSymbolicElements` when the result contains alphabetic characters.
- `SymbolicRealSolverTask` / `SymbolicRationalSolverTask` / `SymbolicComplexSolverTask` send JSON with `result_type=SYMBOLIC_REAL/RATIONAL/COMPLEX`.
- `Equation::UpdateResult` **must** handle `ResultType::SYMBOLIC_REAL/RATIONAL/COMPLEX` by creating the corresponding result class; missing this causes tests to hang because no solver task is ever dispatched.
- `ResultTask::Execute` **must** handle `ElementType::SYMBOLIC_REAL/RATIONAL/COMPLEX_RESULT` to deliver the solver response to the corresponding `PutResult`; missing this leaves the waiting symbol (`~`) forever.

## Definite Integral Element
- `ElementType::DEFINITE_INTEGRAL`, class `DefiniteIntegral : public Formula` in `src/formulas/definite_integral.h/.cpp` (definite integral, symbol `∫`).
- Children: `CodeRow` lower limit (0), `Shape` integral symbol (1), `CodeRow` upper limit (2), `CodeRow` integrand (3), non-editable `CodeString` "d" (4, `editable = false`), `CodeRow` integration variable (5).
- `ToText()` and `ToParserString()` produce `definite_integral(lower,upper,integrand,var)` — all 4 editable rows are passed to the calculator through `definite_integral()`.
- Insert via `document.InsertDefiniteIntegral(with_undo)`; undo stores/restores children 0, 2, 3, 5 (see undo.cpp); registered in editor_utils.cpp `create_elements`.
- Tests: `test/definite_integral.cpp` (`FormulaTest.definite_integral1..definite_integral9`).

## Indefinite Integral Element
- `ElementType::INDEFINITE_INTEGRAL`, class `IndefiniteIntegral : public Formula` in `src/formulas/indefinite_integral.h/.cpp` (indefinite integral, symbol `∫`).
- Children: `Shape` integral symbol (0), `CodeRow` integrand (1), non-editable `CodeString` "d" (2, `editable = false`), `CodeRow` integration variable (3).
- `ToText()` and `ToParserString()` produce `indefinite_integral(integrand,var)` — the 2 editable rows are passed to the calculator through `indefinite_integral()`.
- Insert via `document.InsertIndefiniteIntegral(with_undo)`; undo stores/restores children 1, 3 (see undo.cpp); registered in editor_utils.cpp `create_elements`.
- Tests: `test/indefinite_integral.cpp` (`FormulaTest.indefinite_integral1..indefinite_integral9`).

## Derivative Element

Derivatives are represented by a regular editable `Division` fraction so that the `d`/`∂` prefixes, the function, and the variables are all editable.

- A derivative fraction has the form `d f / d x` (or `∂ f / ∂ x` for partial derivatives). For order `n > 1` the numerator starts with `pow(d, n)` / `pow(∂, n)`.
- `Division::BuildDerivativeParserString()` detects the derivative pattern in `ToText()`/`ToParserString()` and emits nested `derivative(...)` calls. The total differentiation order in the numerator must equal the sum of differentiation operators in the denominator; otherwise the fraction falls back to plain `(num)/(den)`.
  - `d f / d x` → `derivative(f,x)`
  - `pow(d,2) f / pow(d x, 2)` → `derivative(derivative(f,x),x)` (single-variable higher-order)
  - `pow(d,2) f / d x d y` → `derivative(derivative(f,y),x)` (rightmost denominator variable is the innermost derivative)
  - `pow(d,3) f(x,y,z) / d x d y d z` → `derivative(derivative(derivative(f(x,y,z),z),y),x)`
  - `∂ f / ∂ x` → `derivative(f,x)` (partial)
- The total differentiation order is the sum of operators in the denominator (`d`/`∂` = 1, `pow(d,n)`/`pow(∂,n)` = `n`). If it does not equal the numerator order, the fraction falls back to ordinary `(num)/(den)` output.
- The parser recognizes the derivative marker even when it is merged with the function or variable in a single `CodeString` (e.g. `dg(x,y)`/`dxdy`), because `BuildDerivativeParserString()` scans the text character-by-character.
- The `d`/`∂` prefix strings and the empty function/variable placeholders are created with `can_merge = false` so they remain distinct editable elements (see `String::Merge`).
- Insert via `document.InsertDerivative`, `InsertSecondDerivative`, `InsertPartialDerivative`, `InsertPartialSecondDerivative` (all implemented with `CreateDerivativeDivision()` in `document.cpp`).
- Undo/redo stores/restores the editable `CodeString` children inside the `Division` (see `undo.cpp`).
- Tests: `test/derivative.cpp` (`FormulaTest.derivative1..derivative17`) and `test/solver_derivative.cpp` (`SolverAutoTest.derivative1/derivative2/derivative_second1/derivative_third1/derivative_mixed_func/derivative_mixed_func_g/derivative_tan`, `SolverSymbolicTest.derivative1`). Derivative tests that target an explicit numeric result type (e.g. `ResultType::REAL`) belong in the matching `test/solver_<type>.cpp` file (e.g. `SolverRealTest.derivative_mixed_func_g` and `SolverRealTest.derivative_mixed_func_g_real` in `test/solver_real.cpp`).
- Test helpers for building derivative `Division` instances live as methods of `SolverTest` (`test/mock.h`), not as file-scope `static` functions. The helper **must emulate user input** (`InsertDerivative`, `InsertString`, `InsertPower`, `MoveCaretDown`, etc.) instead of constructing `Division`/`CodeString`/`Power` objects by hand, because the desktop insertion/remake path differs from direct element construction.
- Trigonometric/hyperbolic aliases (`tg`/`tan`, `ctg`/`cot`, `cosec`/`csc`, `sh`/`sinh`, `ch`/`cosh`, `th`/`tanh`, `cth`/`coth`, `sch`/`sech`, `cosech`/`csch`) and inverse trig aliases (`arctg`/`arctan`, `arcctg`/`arccot`, `arccosec`/`arccsc`) are registered in all three symbolic parsers. Canonical symbolic function names are `tg`, `arctg`, `arcctg` (renamed from `tan`/`arctan`/`arccot`).
- TermDegree `known_funcs` in `yutovo-calculator/src/symbolic.h` and `FunctionSortRank` in `yutovo-calculator/src/giac_utils.cpp` include all aliases and canonical output names (`tan`, `asin`, `acos`, `atan`, etc.).
- `CalcTestSymbolicReal.all_symbolic_functions` covers every symbolic trig/hyperbolic/inverse function with four variants: numeric evaluation, symbolic form, derivative, and alias equivalence.

## Code Style

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

### Power behavior
`InsertPower(true)` after a string moves that string into the base and places the caret in the exponent.
Example:
```cpp
document.InsertString("x", true);
document.InsertPower(true);
document.InsertString("2", true);
// Parser text: pow(x,2)
```

### Checking results with ToText()
For AUTO-mode symbolic fallback tests, `ToText()` is sufficient:
```cpp
ASSERT_TRUE(document.ToText() == U"x+1=1+x") << ToBasicString(document.ToText());
```

### Checking results with ToHtml()
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

### Undo/Redo in editing tests
Any test that edits an element (typing, inserting subformulas, clearing contents, etc.) must verify both `Undo()` and `Redo()`. Wait for each operation and assert the document state:
```cpp
document.Undo();
document.WaitUndo();
std::this_thread::sleep_for(200ms);
ASSERT_TRUE(document.ToText() == U"...") << ToBasicString(document.ToText());

document.Redo();
document.WaitRedo();
std::this_thread::sleep_for(200ms);
ASSERT_TRUE(document.ToText() == U"...") << ToBasicString(document.ToText());
```

### Async operations and timeouts
Editor and caret methods return a task id and run asynchronously. Always wait for them with `document.WaitTask(id, timeout)` and assert the returned value; do not rely only on `std::this_thread::sleep_for`.

### Changing config
Do not mutate `document.config` directly and then call `document.SetConfig(document.config, false)` — `SetConfigTask` compares the passed config with the current `document.config`, so a direct mutation makes the comparison see no change. Instead, copy `document.config` into a local `Config`, modify the copy, and pass it to `SetConfig`.

### Emulate user input when building formulas in tests
Editor/solver tests should construct expressions the same way a user would — by calling the document's `Insert*` APIs — not by allocating `Division`/`Power`/`CodeString` objects directly and attaching them with `InsertFormula`. The desktop code path performs caret placement, merging, remake and other logic that direct construction skips, so tests built from raw elements can pass in the test binary but fail on desktop.

```cpp
// WRONG
Division* div = new Division(&document);
// ... manually populate numerator/denominator ...
document.WaitTask(document.InsertFormula(div, true));

// CORRECT
document.WaitTask(document.InsertDivision(true));
document.InsertString("d", true);
document.InsertPower(true);
document.InsertString("2", true);
document.InsertString("g", true);
document.InsertOpenRoundBracket(true);
document.InsertString("x", true);
document.InsertComma(true);
document.InsertString("y", true);
document.InsertCloseRoundBracket(true);
document.WaitTask(document.MoveCaretDown(false));
document.InsertString("d", true);
document.InsertString("x", true);
document.InsertString("d", true);
document.InsertString("y", true);
```

Shared test helpers (e.g. `SolverAutoTest::CreateDerivativeDivision` in `test/mock.h`) must also emulate input rather than construct elements by hand.

### Result-type-specific tests belong in the matching solver file
Tests that explicitly set a non-AUTO result type should be placed in the corresponding test file:
- `ResultType::REAL` → `test/solver_real.cpp` (`SolverRealTest`)
- `ResultType::INTEGER` → `test/solver_integer.cpp` (`SolverIntegerTest`)
- `ResultType::RATIONAL` → `test/solver_rational.cpp` (`SolverRationalTest`)
- `ResultType::COMPLEX` → `test/solver_complex.cpp` (`SolverComplexTest`)
- `ResultType::ARRAY_REAL` → `test/solver_array_real.cpp` (`SolverArrayRealTest`)
- `ResultType::SYMBOLIC_REAL`/`SYMBOLIC_RATIONAL`/`SYMBOLIC_COMPLEX` → `test/solver_derivative.cpp` / `test/solver_symbolic.cpp` / etc., depending on the feature.

AUTO-mode tests and generic derivative tests live in `test/solver_derivative.cpp` (`SolverAutoTest`/`SolverSymbolicTest`).

## File formats
- `.yut` files are ZIP archives (not plain text). Use `unzip -l file.yut` to list contents, `unzip -p file.yut` to extract.

## Build
Projects are built and tested in `build/debug/`:
```bash
cd build/debug && cmake ../.. && make -j16 yutovo-editor_tests && ./test/yutovo-editor_tests --gtest_filter="FormulaTest.power23"
```

### Run tests from `build/debug`, not from `build/debug/test`
Tests that load documents use paths relative to the current working directory, e.g. `../../test/tests/solver42.yut`. Run the binary as `./test/yutovo-editor_tests ...` with the working directory `build/debug` — from `build/debug/test` those paths resolve to a nonexistent `build/test/tests/` and the test fails with `File not open` and then hangs forever in `WaitLoad`/`WaitSolver` (observed with `solver42` and `lists1`); it looks like a product hang but is a runner error.

## Testing

### Full test suite
Before running the full `yutovo-editor_tests` suite, ask the user at the beginning of the task (after planning) whether to run the complete suite or only targeted tests.

### Debug giac linkage
`yutovo-editor` creates its own imported `giac_imported` target in `src/CMakeLists.txt`. Debug builds must use `${INSTALL_PATH}/lib/libgiacd.a` and release builds `${INSTALL_PATH}/lib/libgiac.a`; linking a debug `yutovo-calculator`/`yutovo-solver` against the release giac library causes an ABI mismatch and memory corruption inside giac (e.g., `CodeTest.code35` crashing in `giac::expand`).

## Network Errors
If an operation fails with a "Network connection failed" error:
1. Wait **2 seconds** and retry automatically on your own.
2. If the retry still fails, continue retrying with a **10-second** interval.

## Repowise
Use `repowise` tools (e.g. `mcp__repowise__get_overview`, `mcp__repowise__get_context`, `mcp__repowise__get_risk`, `mcp__repowise__get_answer`) for codebase exploration, risk analysis, and architecture understanding.
