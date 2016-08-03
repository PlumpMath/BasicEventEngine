Copyright (c) 2015-16 Luke Montalvo <lukemontalvo@gmail.com>

[![Build Status](https://travis-ci.org/piluke/BasicEventEngine.svg?branch=master)](https://travis-ci.org/piluke/BasicEventEngine)

![In-Game Screenshot](https://github.com/piluke/BasicEventEngine/raw/master/screenshot.bmp)

BasicEventEngine is an event-driven game engine which the user can interface
with via the Sprite, Sound, Background, Font, Path, Object, and Room classes.
The Room class is used to organize the resources for each section of the game
and the Object class is used to act upon the events from the main loop. A small
example is included with all non-engine code in the resources directory.

The available events are the virtual functions of the Object class which are on
lines 101 to 127 in the file ``bee/resource_structures/object.hpp``

The software is in early alpha so do not expect any sort of stability. If you
have any questions or comments, feel free to email me. Also feel free to
contribute to the engine code, either by email or via github.

## How to use on Linux

This program can compile under Arch Linux, Ubuntu 15.10, and Windows 10 but at
the moment support will only be offered for Linux.

1. [Intall git-lfs][1] so that you can download the resources for the example.

        # For Arch Linux, it's available in the AUR
        pacaur -y git-lfs

2. Install the required libraries:

        # Arch Linux
        sudo pacman -S cmake sdl2 sdl2_image sdl2_ttf sdl2_mixer sdl2_net glew glm freeglut libxmu
        # Ubuntu
        sudo apt-get install cmake libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libsdl2-mixer-dev libsdl2-net-dev libglew-dev libglm-dev freeglut3-dev libxmu-dev

3. Download the additional library submodules:

        git submodule update --init --recursive

4. *(Optional)* Edit the file ``resources/resources.hpp`` in order to add or remove resources which are located in their corresponding subdirectory.

5. Create a build directory and compile the program with the following commands:

        mkdir build
        cd build
        cmake ..
        make

6. Run the program by moving back to the top directory:

        cd ..
        ./build/BasicEventEngine

## How to use on Windows

Even though this program can compile on Windows 10, I am not offering any
support for any Windows platform because it is terribly annoying to get
anything to compile correctly. If you dare attempt to compile on Windows, then
follow the below instructions.

1. *(Recommended)* Clone the project with [Github Desktop][2] for Windows to be able to easily download the resources for the example.

2. Install the required libraries from their official websites: SDL2, SDL2_image, SDL2_ttf, SDL2_mixer, SDL2_net, GLEW, GLM.
The current project setup assumes that each of these libraries has its DLL in the same directory as the executable (e.g. ``win/bin``) and has its header file in a ``win/include`` subdirectory.
Feel free to download the libraries from [here][3] and simply extract them to ``win/``.

3. [Install CMake][4] and [Code::Blocks with MingW setup][5] (or your favorite CMake-compatible IDE). Make sure to add ``C:\Program Files (x86)\CodeBlocks\MinGW\bin`` to the system path.

4. *(Optional)* Edit the file ``resources/resources.hpp`` in order to add or remove resources which are located in their corresponding subdirectory.

5. Open the CMake GUI and set the appropriate source and build directories, e.g. ``C:\Users\Luke\Documents\GitHub\BasicEventEngine`` and ``C:\Users\Luke\Documents\GitHub\BasicEventEngine\build``.

6. Configure the project and select the appropriate build system, e.g. ``CodeBlocks - MinGW Makefiles``, and then Generate the project.

7. Open the Code::Blocks project file from ``build/BasicEventEngine.cbp`` and compile it with ``Build > Build``.

8. Run the compiled executable by copying it to the directory which contains the required resources and DLLs, e.g. ``win/bin``.

This project is under the MIT License so feel free to do basically whatever you want with the code.

[1]: https://git-lfs.github.com/                        "Git LFS instructions"
[2]: https://desktop.github.com/                        "Github Desktop for Windows"
[3]: https://lukemontalvo.us/BasicEventEngine/win.zip   "Windows Library Files"
[4]: https://cmake.org/download/                        "CMake"
[5]: http://codeblocks.org/downloads/26                 "Code::Blocks"
