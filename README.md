![Valyria Tear](https://raw.githubusercontent.com/ValyriaTear/ValyriaTear/master/data/boot_menu/valyria_logo.png)

- [**Development blog**](http://valyriatear.blogspot.com)
- [**Bug Tracker**](https://github.com/ValyriaTear/ValyriaTear/issues)
- [**Forums**](http://forum.freegamedev.net/viewforum.php?f=76)

Valyria Tear is an open-source single-player medieval-fantasy 2D J-RPG based on an extended [Hero of Allacrost](https://allacrost.org/) engine and inspired by classic console RPGs.

The player incarnates Bronann, a young boy forced to take part into the struggle for the possession of a mysterious crystal.
This powerful artifact will lead him to discover the actual threat set upon his world, but also courage and love.

The game has all the features you can expect from JRPG classics: entertaining story development, colourful map exploration, active side-view battles, character management, puzzles...
It is also translated in several languages.

## Licensing

**Copyright (C) 2012-2017 by Bertram  
Copyright (C) 2004-2011 by The Allacrost Project**

The source code is licensed under the *GNU GPL*.
It is free software and you may modify it and/or redistribute it under the terms of this license.  
See https://www.gnu.org/copyleft/gpl.html for details.

The graphics, sounds, musics and script files are licensed according to the [LICENSES](https://raw.githubusercontent.com/ValyriaTear/ValyriaTear/master/LICENSES) file.

While the scripts are under the GNU GPL, the story concept is copyrighted and cannot be reused as is or part of it in another project without the author's consent.
This means you can package, and distribute Valyria Tear under the term of this license but you cannot take the story or parts of it in another project without agreement.

The game directories included and used are:  
`data/ po/`

## Compilation procedure

### Linux compilation

Once you've unpacked the tarball, if you have checked it out from the Git repository, run this first to get the latest luabind code:  
`git submodule update --init --remote --force`  
and then the usual `cmake . && make` from the top-level directory at the prompt.

You'll then be able to play by typing: `src/valyriatear`

### Windows compilation

#### Code::Blocks

A [Code::Blocks](http://www.codeblocks.org/) project file is also provided when compiling under Windows.
In that case, you might need the dependencies and headers files that can be downloaded [here](https://sourceforge.net/projects/valyriatear/files/win32-depends/valyriatear-win32-depends-sdl1.2-2014-12-11.zip/download).


#### MSys/MinGW

1. Download and install MSys2
   1. **Download MSYS2** from [MSYS2.io](https://msys2.github.io), either the 64 bit or the 32 bit version, depending on which type of build you want to create
   1. Depending on your version, **install** to `C:\msys64` or `C:\msys32`
   1. **Run** `C:\msys64\mingw64.exe` or `C:\msys32\mingw32.exe`
   1. Follow the [update steps](https://github.com/msys2/msys2/wiki/MSYS2-installation#iii-updating-packages). Running `pacman -Syuu` repeatedly and following the instructions on screen should do it.
1. Install the toolchain and the dependencies
   1. Install the mingw64 **toolchain** (or mingw32 toolchain)
      * 64bit: `pacman -S mingw-w64-x86_64-toolchain`
      * 32bit: `pacman -S mingw-w64-i686-toolchain`
   1. Install **CMake** and all **dependencies**
      * 64 bit: `pacman -S mingw-w64-x86_64-cmake mingw-w64-x86_64-ninja mingw-w64-x86_64-boost mingw-w64-x86_64-SDL2_ttf mingw-w64-x86_64-SDL2_image mingw-w64-x86_64-glew mingw-w64-x86_64-libpng mingw-w64-x86_64-libzip mingw-w64-x86_64-lua mingw-w64-x86_64-libogg mingw-w64-x86_64-libvorbis mingw-w64-x86_64-openal mingw-w64-x86_64-libiconv`
      * 32 bit: `pacman -S mingw-w64-i686-cmake mingw-w64-i686-ninja mingw-w64-i686-boost mingw-w64-i686-SDL2_ttf mingw-w64-i686-SDL2_image mingw-w64-i686-glew mingw-w64-i686-libpng mingw-w64-i686-libzip mingw-w64-i686-lua mingw-w64-i686-libogg mingw-w64-i686-libvorbis mingw-w64-i686-openal mingw-w64-i686-libiconv`
      * If any of the dependencies should be missing from the lists above, you can search for them on http://repo.msys2.org/mingw/
1. **Configure** the build
   * 64bit: `cmake -G "Ninja" -DCMAKE_C_COMPILER=C:/msys64/mingw64/bin/gcc.exe -DCMAKE_CXX_COMPILER=C:/msys64/mingw64/bin/g++.exe .`
   * 32 bit: `cmake -G "Ninja" -DCMAKE_C_COMPILER=C:/msys32/mingw32/bin/gcc.exe -DCMAKE_CXX_COMPILER=C:/msys32/mingw32/bin/g++.exe .`
1. **Run** the build: `ninja`


#### Microsoft Visual C++ 2013 and later

_**(Warning: not officially supported!)**_
Please contact [authenticate](https://github.com/authenticate) for support.  
To create a debug build on Windows with Visual Studio 2013, do this:  
- git clone https://github.com/authenticate/ValyriaTear-VS2013-Dependencies.git
- git submodule update --recursive --init --remote --force
- Copy the contents of ```ValyriaTear-VS2013-Dependencies/``` into a new folder ```ValyriaTearRepository/vs2013/libs``` in your Valyria Tear repository.
- Copy the debug versions of all the dependencies' dlls into ```ValyriaTearRepository/vs2013/Debug```
- Open the VS 2013 solution file: ```ValyriaTearRepository/vs2013/ValyriaTear.sln```
- Build ```F7```
- Run ```F5```

### Mac OS X compilation

- Use Macports or Homebrew to install things like boost.
- Open Xcode project, to quickly update the files, delete the `src` folder in project and add it again. Remove all unnecessary things.
- Press "Play"

## Dependencies

You will need the following in order to compile and run the game:

- **Boost headers** (Tested from 1.42 to 1.58)
- [**GNU Gettext**](https://www.gnu.org/software/gettext/) - (libiconv, libintl)
- **lua** (5.1.x - 5.2.x - 5.3.x) - (liblua)
- **libz**
- **libpng** (1.2 to 1.6)
- **Ogg/Vorbis** - (libogg, libvorbis, libvorbisenc, libvorbisfile)
- **OpenAL**
- **OpenGL**, **GLU** and **GLEW**
- [**SDL**](https://www.libsdl.org/) (2.0.0+)
- **SDL_ttf** (2.0.12+)
- **SDL_image** (2.0.0)

## Debug feature support compilation

- **Using CMake:**
  Use the `-D` parameter when invoking cmake to add **compilation flags**.

- **Add debug menus, and debug commands:**
  `cmake -DDEBUG_FEATURES=on .`

- On **Code::Blocks:**
  Go to Project->Build options, and add the flags in the `#defines` tab, i.e.:
  `DEBUG_MENU`
