# AGENTS.md — yutovo-desktop

## Project
Qt5 C++ desktop app — math WYSIWYG editor + calculator for Linux/Windows.
License: GPL-3.0-only. Minimum Qt version: 5.14.

## Build (Linux)
```
export YUTOVO_DEPLOY=~/yutovo/deploy   # required; points to internal libs install
mkdir -p build/debug && cd build/debug
cmake -DCMAKE_BUILD_TYPE=Debug ../..
make -sj16 && make install   # installs to $YUTOVO_DEPLOY/bin
```
- Debug binary: `build/debug/src/yutovo-desktopd` (note the `d` suffix)
- C++17, CMake ≥ 3.17
- Clang builds enable `-fsanitize=undefined,address` automatically

## Build (Windows)
```
set VCPKG_ROOT=<path>   # or export on Git Bash
vcpkg install qt5-base qt5-widgets qt5-gui
mkdir build\debug && cd build\debug
cmake ../..
cmake --build . --config Debug
```
- Windows deps: vcpkg manages Qt5, Boost::property_tree, BZip2, RapidJSON, libharu, mpfr
- CMakeLists.txt applies `/utf-8 /bigobj /EHsc /MP` for MSVC
- **Debug builds require internal libs built in Debug** — otherwise MSVC LNK2038 `_ITERATOR_DEBUG_LEVEL` / `RuntimeLibrary` mismatches occur
- vcpkg cmake configs for internal libs must include a `*-debug.cmake` import file; if only Release is present, MSVC links the Release library even in Debug builds
- On Windows, Qt platform plugins (`qwindows.dll`/`qwindowsd.dll`) are copied next to the executable at build time via a post-build step

## Tests
```
# Tests build by default (BUILD_TESTS=ON); binary: build/debug/test/yutovo-desktop_test
./test/yutovo-desktop_test <testName>   # run single test
ctest -R <pattern>                       # run via CTest
```
- Tests are **GUI tests** — they create real `MainWindow` instances with `show()`. Requires a running display server (X11/Wayland). Headless CI will fail without xvfb.
- Test binary defines `TEST_APP` and compiles in most `src/*.cpp` files directly.
- Uses Qt5::Test; fixtures in `test/files.cpp`.

## Dependencies (must be pre-built)
Internal libs from the yutovo monorepo — build these first:
- Linux: install into `$YUTOVO_DEPLOY`
- Windows: install into vcpkg (`$VCPKG_ROOT/installed/x64-windows`)
- **yutovo-logger**
- **yutovo-calculator**
- **yutovo-solver**
- **yutovo-editor**

External (Linux): Qt5 (Widgets, Network, PrintSupport), MathGL2, fontconfig, zlib.
External (Windows): via vcpkg — Boost, BZip2, RapidJSON, libharu, mpfr.

## Architecture
- `src/` — single flat executable target; all .cpp/.h at root level
- `test/` — single test binary (`files.cpp`), reuses src/ sources directly
- `setup/` — packaging: Flatpak, Snap, AppImage, Ubuntu/PPA, Arch AUR, Windows installer
- Entry point: `src/main.cpp` — single-instance app via `QLocalServer`/`QLocalSocket`
- Core windows: `mainwindow.cpp`, `document_widget.cpp`, `document_window.cpp`

## Agent Rules

- **Never delete files without explicit user permission.** Do not remove source files, test files, core dumps, logs, build artifacts, or any other files unless the user explicitly asks for it. When in doubt, leave the file in place and ask.
- **Never create separate namespaces (such as `namespace detail` or anonymous namespaces) without explicit user permission.** Helper functions should be placed in the common `yutovo_calculator` namespace, for example in `utils.h/utils.cpp` or `giac_utils.h/giac_utils.cpp`, or as `static` methods of the appropriate class.
- **Never commit without explicit user permission.** Do not run `git commit`, `git push`, `git reset`, `git rebase`, or any other git mutations unless explicitly asked to do so. Ask for confirmation each time when git mutations are needed.
- **Never delete existing tests.** When fixing regressions or refactoring, update test expectations to match the new correct behavior, but do not remove tests. If `git checkout` or similar commands are used to revert a file, verify that no user-added tests were lost.
- **Never change test logic without explicit user permission.** Do not modify test sequences, assertions, fixtures, helpers, or expected values. When making tests pass, fix application code or test environment setup only. If a test itself must be changed, ask the user first.

## Key conventions
- `REMOTE_SOLVER` define (commented out in root CMakeLists.txt) switches from local yutovo-solver to remote
- Translations: 4 locales (en, ru, es, pt_BR); `.ts` → `.qm` via `lrelease` at build time — `lrelease` must be on PATH
- Windows uses vcpkg; Linux uses `$YUTOVO_DEPLOY` for all deps
- No CI, no pre-commit, no linting/formatting config in repo

## Code style
- Opening braces always on a new line (`K&R/Allman` style):
  ```cpp
  if (condition)
  {
      ...
  }
  ```
- `return` statements always on their own line:
  ```cpp
  if (condition)
      return;
  ```
  not `if (condition) return;`

Always place braces on their own line for control structures.

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

