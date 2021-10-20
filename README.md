# qlitehtmlbrowser
Q(Text)Browser like widget with [litehtml](https://github.com/litehtml/litehtml) as rendering engine.

This is an implementation alternative to [qlitehtml](https://code.qt.io/cgit/playground/qlitehtml.git/tree/) to use
 [litehtml](https://github.com/litehtml/litehtml) in QTextBrowser based applications that are used with the Qt LGPL license.
 
**This project is currently under development. Help us to improve by creating issues or sending pull-requests**. 
 
## Compilation
QLiteHtmlBrowser is currently supported with cmake and Qt 5.15.x. Clone the respository and update the litehtml submodule.
You should be able to configure the project standalone or integrate it into an existing cmake source tree.
 
## Testing
During build some test executable are created. These are currently for manual testing, althoug they should be able to run with ctest without any errors.
For "real" testing you have to call the text executables manually with the `--interactive` flag. The tests are then executed step by step to prove expected rendering manually.

Also there is a simple "testbrowser" application available, there you could load (lokal) html files as url or manually open QtHelp qch files to test the implementation. This could be also helpful to preview your HTML/QtHelp files without integration of QLiteHtmlBrowser in your programs.
