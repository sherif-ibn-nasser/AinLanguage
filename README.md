<p  align="center">
<img  src="assets/Ain-Logo.png"  width="100"  height="120">
</p>

# AinLanguage

**Ain programming language!**

A new **Arabic** programming language, built from scratch with **C++**, with no external libraries.

We plan to make the language self-hosted, which means, **Ain** will be written in **Ain**.

**Warning!** The language is under development and the code base may be changed in the future. [See open issues](https://gitlab.com/sherifnasser/AinLanguage/-/issues).

[Follow us on YouTube](https://www.youtube.com/@AinProgrammingLanguage)

# What's new in v0.3.0

#### New in the language

* Support `this` operator

# Ain and Unicode
**Ain** currently doesn't support some Unicode characters for some languages.

Also, **Ain** prevents Unicode characters that are considered Kufr or prohibited in Islam (Crosses, pride, music, etc.).

# Terminal RTL support
To support auto-detecting of RTL text in your terminal emulator, you could run the command

```console
printf "\e[?2501h"
```
This will make your Arabic text appear from right to left.
You could also add this command at the end of `~/.bashrc` file to enable it permanently.

# Editor support
[`ain.lang`](https://gitlab.com/sherifnasser/AinLanguage/-/blob/main/editor-support/ain.lang) is a syntax highlighting definition file that introduces the support for Ain in editors that use GtkSourceView (e.g. GNOME text editor gedit, GNOME Builder, mousepad, etc.)

#### How to install

* Create the directories for custom language files in your home directory

```console
mkdir -p ~/.local/share/gtksourceview-3.0/language-specs
mkdir -p ~/.local/share/gtksourceview-4/language-specs
```

* Copy [`ain.lang`](https://gitlab.com/sherifnasser/AinLanguage/-/blob/main/editor-support/ain.lang) to `~/.local/share/gtksourceview-3.0/language-specs` and `~/.local/share/gtksourceview-4/language-specs`

* Copy [`text-x-ain.xml`](https://gitlab.com/sherifnasser/AinLanguage/-/blob/main/editor-support/text-x-ain.xml) to `~/.local/share/mime/packages`

* Update mimetypes database
```console
update-mime-database ~/.local/share/mime
```

* Optional: copy [intellij-dracula.xml](https://gitlab.com/sherifnasser/AinLanguage/-/blob/main/editor-support/intellij-dracula.xml) to `~/.local/share/gtksourceview-3.0/styles` and `~/.local/share/gtksourceview-4/styles` to enable intellij dark theme

# Build & run
**Ain** uses Make as a build system and uses [CMake](https://gitlab.kitware.com/cmake/cmake) to generate the Makefile.

If you added new include files or ***.cpp** files, you should add them to the [`CMakeLists.txt`](https://gitlab.com/sherifnasser/AinLanguage/-/blob/main/CMakeLists.txt),

or you could run [`files_cmake`](ttps://gitlab.com/sherifnasser/AinLanguage/-/blob/main/files_cmake.cpp) to automatically add them.

Make a build directory (if not exist), enter it, and then make the project
``` console
mkdir build		# Make a build directory (if not exist)
cd build		# Enter it
cmake ..
make			# make the project
```

Once you've built the project, you'll find an executable in the build/bin directory called **ain**, run it in the terminal and pass your ***.ain** files.

# Debug
Make a debug directory (if not exist), enter it, and then run these commands
``` console
mkdir debug		# Make a debug directory (if not exist)
cd debug		# Enter it
cmake -DCMAKE_BUILD_TYPE=Debug  ..
cmake --build  .
```

# Run unit tests

Ain uses [Catch2](https://github.com/catchorg/Catch2) for unit testing. All tests are included in the [test directory](ttps://gitlab.com/sherifnasser/AinLanguage/-/blob/main/test/).

Once you've built the project the output executable **unit_tests** will be in the build/bin directory.

If you added new tests, you could run [`files_cmake`](ttps://gitlab.com/sherifnasser/AinLanguage/-/blob/main/files_cmake.cpp) to automatically add them.

