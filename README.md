# Yutovo project
Yutovo is a powerful calculator with graphical representation of mathematics operations inside a text editor.

Yutovo library provides documents which contain help, scientific articles, calculators, examples and other info.

## Make library
Switch to the library branch and use make_library.sh to make library. The first parameter is a path, others are: WEB is making the web version, ZIP is making .yut files as archives.

Run the make_library.sh script for the desktop version:
```
./make_library.sh /home/user/programs/Math/yutovo/yutovo-desktop/build/debug/ ZIP
```

Run the make_library.sh script for the web version:
```
./make_library.sh /home/user/programs/Math/yutovo/yutovo-web/build_web/debug/ WEB ZIP
```