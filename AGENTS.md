# AGENTS.md — yutovo-desktop

## Project
Qt5 C++ desktop app — math WYSIWYG editor + calculator for Linux/Windows.

## Build (Linux)
```
export YUTOVO_DEPLOY=~/yutovo/deploy   # required; points to internal libs install
mkdir -p build/debug && cd build/debug
cmake -DCMAKE_BUILD_TYPE=Debug ../..
make -sj && make install
```
- Debug binary: `build/debug/src/yutovo-desktopd` (note the `d` suffix from `CMAKE_DEBUG_POSTFIX`)
- C++17, CMake ≥ 3.17
- Clang builds enable `-fsanitize=undefined,address` automatically

## Tests
```
# Build runs tests subdir by default (BUILD_TESTS=ON)
# Binary: build/debug/test/yutovo-desktop_test
./test/yutovo-desktop_test <testName>   # run single test
ctest -R <pattern>                       # run via CTest
```
Tests link against Qt5::Test and pull in most src/ files directly.

## Dependencies (must be pre-built)
Internal libs from the yutovo monorepo — build these first and install into `$YUTOVO_DEPLOY`:
- **yutovo-logger**
- **yutovo-calculator**
- **yutovo-solver**
- **yutovo-editor**

External: Qt5 (Widgets, Network, PrintSupport), MathGL2, fontconfig, zlib.

## Architecture
- `src/` — single flat target; all UI + logic in one executable
- `test/` — single test binary (`files.cpp`), reuses src/ .cpp files directly
- `setup/` — packaging for Flatpak, Snap, AppImage, Ubuntu/PPA, Windows, Arch
- Entry point: `src/main.cpp`
- Core windows: `mainwindow.cpp`, `document_widget.cpp`, `document_window.cpp`

## Key conventions
- `REMOTE_SOLVER` define (in CMakeLists.txt, currently commented out) switches from local yutovo-solver to a remote solver
- Translations: `.ts` → `.qm` via `lrelease` at build time
- Windows uses vcpkg; Linux uses `$YUTOVO_DEPLOY` for all deps
