<p  align="center">
<img  src="assets/Ain-Logo.png"  width="100"  height="120">
</p>

# AinLanguage

**Ain programming language!**

A new **Arabic** programming language, built from scratch with **C++**, with no external libraries.

We plan to make the language self-hosted, which means, **Ain** will be written in **Ain**.

[See us on YouTube](https://www.youtube.com/@AinProgrammingLanguage)

# The Shutdown

The project is now archived and we will no longer be actively working on it. The decision to shut down the project stems from the following issues:

* **Language Choice**: The project is implemented in C++, which has proven to be a significant impediment.
* **Maintainability**: The parser is difficult to maintain and extend with new features.
* **Coupling**: The Intermediate Representation (IR) and type-checking mechanisms are too closely coupled with the parsing stage.
* **Code Generation**: Code generation relies on hand-written assembly, which is less efficient and harder to maintain.
* **Error Handling**: The error messages provided by the compiler are inadequate.
* **Documentation**: The project lacks sufficient documentation.
* **Standard Library**: The standard library (STD) is challenging to maintain.

As a result, we have decided to redirect our efforts to [نظم](https://github.com/sherif-ibn-nasser/nazm-lang), a new Arabic programming language. The first version of the compiler for نظم is written in Rust, where we have addressed the shortcomings experienced in this project.

# What's new in v0.4 (Compiled Ain)

#### New in the language

* Generate binary executables for your ain project

* Start [`ainstd`](https://github.com/sherif-ibn-nasser/AinLanguage/blob/main/ainstd/) lib

* Memory management functions in [`ainmem.ain`](https://github.com/sherif-ibn-nasser/AinLanguage/blob/main/ainstd/ainmem.ain)

* Inline assembly

* Add byte, unsigned byte, short and unsigned short data types

* Add floor, ceil, round, and truncate functions for float and double as intrinsics

* Primitives names are short now

#### Bug fixes and enhancements

* Fix #4

* Fix #5

* Implement #8

* Fix #9

#### Deprecated in this release

* Methods of converting primitives to string are deprecated temporally and will be available in next releases after implementing pointers (See #10 and #11). One should use the functions in [`ainstd/util`](https://github.com/sherif-ibn-nasser/AinLanguage/blob/main/ainstd/util/) to perform conversions and write the result in unsigned byte array, then use the constructor of string passing the array as an argument

* Methods of converting strings to primitives are deprecated until the next releases

* Reading input function is deprecated and it depends on #10

* Note that these deprecations are temporal and will be available again in next releases when implementing pointers and the borrow checker

* The interpreter is partially deprecated and will will be removed completely in the next release

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

# Assembler

**Ain** uses [`nasm`](https://github.com/netwide-assembler/nasm) to assemble the generated assembly and produce the executable and it should be installed on your system first

# AinSTD lib

[`ainstd`](https://github.com/sherif-ibn-nasser/AinLanguage/blob/main/ainstd/) lib should be installed before using the compiler

#### How to install

* Download the directory [`ainstd`](https://github.com/sherif-ibn-nasser/AinLanguage/blob/main/ainstd/)

* Create a global environment variable `AIN_STD` or add this line to `/etc/profile` (use sudo)

```console
export AIN_STD="<path-to-dir>/ainstd/"
```

the path ends with `ainstd/`, e.g., `~/Downloads/ainstd/`

* Restart your machine

# Editor support
[`ain.lang`](https://github.com/sherif-ibn-nasser/AinLanguage/blob/main/editor-support/ain.lang) is a syntax highlighting definition file that introduces the support for Ain in editors that use GtkSourceView (e.g. GNOME text editor gedit, GNOME Builder, mousepad, etc.)

#### How to install

* Create the directories for custom language files in your home directory

```console
mkdir -p ~/.local/share/gtksourceview-3.0/language-specs
mkdir -p ~/.local/share/gtksourceview-4/language-specs
```

* Copy [`ain.lang`](https://github.com/sherif-ibn-nasser/AinLanguage/blob/main/editor-support/ain.lang) to `~/.local/share/gtksourceview-3.0/language-specs` and `~/.local/share/gtksourceview-4/language-specs`

* Copy [`text-x-ain.xml`](https://github.com/sherif-ibn-nasser/AinLanguage/blob/main/editor-support/text-x-ain.xml) to `~/.local/share/mime/packages`

* Update mimetypes database
```console
update-mime-database ~/.local/share/mime
```

* Optional: copy [intellij-dracula.xml](https://github.com/sherif-ibn-nasser/AinLanguage/blob/main/editor-support/intellij-dracula.xml) to `~/.local/share/gtksourceview-3.0/styles` and `~/.local/share/gtksourceview-4/styles` to enable intellij dark theme

# Build & run
**Ain** uses Make as a build system and uses [CMake](https://gitlab.kitware.com/cmake/cmake) to generate the Makefile.

If you added new include files or ***.cpp** files, you should add them to the [`CMakeLists.txt`](https://github.com/sherif-ibn-nasser/AinLanguage/blob/main/CMakeLists.txt),

or you could run [`files_cmake`](ttps://github.com/sherif-ibn-nasser/AinLanguage/blob/main/files_cmake.cpp) to automatically add them.

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

