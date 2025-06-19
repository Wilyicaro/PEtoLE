# PEtoLE

This project aims to port all the features from Minecraft Java Edition b1.7.3 to the early Minecraft: Pocket Edition, forked from the [ReMinecraftPE](https://github.com/ReMinecraftPE/)
My thanks to the contributors of ReMinecraftPE, without them this project would not be possible

## FAQ

**Why PEtoLE?**

Well, this project was born out of curiosity regarding the proximity of the Minecraft Pocket Edition codebase to Java Edition, which ended up giving me the idea of ​​trying to make it accurate with TU0 of [Legacy Edition](https://minecraft.wiki/w/Legacy_Console_Edition), but why not first make it accurate with b1.7.3? Since this would be necessary anyway, this project came about, and with this name (Pocket Edition to Legacy Edition)

Eventually there will be a branch that lives up to this name, but one step at a time, right?

**Why is the goal b1.7.3 instead of b1.6.6, since TU0 is based on it?**

Well, basically the difference between these two versions is just the scissors, pistons, textures and bug fixes, so since they are minimal, it makes more sense to have the most popular version as the goal.

Currently the platforms originally from the ReMinecraftPE are:
* Windows 11 (older versions have not been tested, most of the newer ones should work)
* Android (thanks to [Stom](https://github.com/Stommm) for the help)

These are the ports that have not been tested, but could certainly work with some fixes:
* Linux
* WebGL
* Mac OS (port by [BrentDaMage](https://github.com/BrentDaMage))
* iOS (3.0 and above; port by [BrentDaMage](https://github.com/BrentDaMage))
* HaikuOS (thanks to [SanyaSho](https://github.com/SanyaSho))
* Xbox 360 (work in progress; port by [BrentDaMage](https://github.com/BrentDaMage))

Note: Just like ReMinecraftPE, it should work on all Android devices, but with worse performance due to some optimizations from Pocket Edition being removed for accuracy.

### This is the main modded version. For the original decompilation, see [ReMinecraftPE/mcped](https://github.com/ReMinecraftPE/mcped)

## License information

This project is licensed under the [BSD 1 clause license](LICENSE.md). However, it contains third party
software with different but compatible licenses:

- [RakNet](https://github.com/facebookarchive/RakNet): [Licensed under the BSD 2 clause license](thirdparty/raknet/LICENSE)
- [zlib](https://github.com/madler/zlib): [View license](thirdparty/zlib/LICENSE)
- [GLES compatibility layer](https://github.com/TheBrokenRail/gles-compatibility-layer): [View license](https://github.com/TheBrokenRail/gles-compatibility-layer/blob/master/LICENSE). Embedded as a submodule.
- [coi-serviceworker](https://github.com/gzuidhof/coi-serviceworker): [View license](https://github.com/gzuidhof/coi-serviceworker/blob/master/LICENSE). Embedded as a submodule.
- [stb-vorbis.c](https://github.com/nothings/stb/tree/master): [View license](https://github.com/nothings/stb/blob/master/LICENSE) (Public domain)
- [json.h](https://github.com/sheredom/json.h): [View license](https://github.com/sheredom/json.h/blob/master/LICENSE) (Public domain)

# DISCLAIMER

This project does not provide game assets, you must have an original copy of Minecraft Java Edition to obtain them, or use an alternative to them.

## Setup

Before trying to build:

1. Run the following `git` commands, or perform the following actions with Git:
```
git submodule init
git submodule update
```

This fetches the three dependencies we have:
- [zlib](https://github.com/madler/zlib)
- [coi-serviceworker](https://github.com/gzuidhof/coi-serviceworker)
- [gles-compatibility-layer](https://github.com/TheBrokenRail/gles-compatibility-layer.git)

2. Copy the sounds from your `.minecraft/resources` inside the assets directory (download a b1.7.3 instance before this), then extract the textures from a b1.7.3 jar to this same directory

# ReMinecraftPE Description

## Building

### Windows

Click the thumbnail below to watch a video guide showcasing how to build ReMCPE.
<a href="https://youtu.be/Tx1u7C2DCPI" target="_blank">
  <img alt="How to Compile ReMCPE for Windows" src="http://i.ytimg.com/vi/Tx1u7C2DCPI/maxresdefault.jpg" />
</a>

The project is configured to target Windows XP by default by using "v141_xp" build tools. If you would like
to build with Windows XP support, please follow the guide [here](https://learn.microsoft.com/en-us/cpp/build/configuring-programs-for-windows-xp?view=msvc-170#install-the-windows-xp-platform-toolset)
to obtain the build tools via the Visual Studio installer.

**If you would like to strictly target more modern versions of Windows, you can change the build toolset by
doing the following:**

1. Right-click the project and click "Properties".

![Visual Studio Project](screenshots/visualstudio/mcpe_project.png)
![Visual Studio Project Properties Option](screenshots/visualstudio/properties_contextmenu.png)

2. Click the dropdown for the "Platform Toolset" property and select "Visual Studio 2022 (v143)" or newer.
![Selecting v143 Platform Toolset](screenshots/visualstudio/mcpe_project_properties_v143.png)

3. Click "Apply".

Install the [DirectX SDK (June 2010)](https://www.microsoft.com/en-US/download/details.aspx?id=6812), unless:
- You opt to use OpenAL instead (but that will require some hacking)
- You are using the latest platform toolset

Once you have the proper build tools installed (v140_xp or newer), you can now build it.

After building, place the `assets` folder you have prepared in the working directory of the output executable.
(if running from VS, it's in `game/`, otherwise, where your executable is)

### WebAssembly

**Make sure you have CMake and Ninja installed. On Windows, the ninja executable must be in your PATH.**
On Linux, the package names are `cmake` and `ninja-build` respectively if you are using apt.

The game will use the assets from within the `game/` directory. So put your assets there if you want to build
for wasm. Then run the `build-wasm` script corresponding to your platform (`-.sh` on linux/WSL, `-.bat` on
windows).

The output files will be in `./wasm/dist`, but you need to upload them to a web host (localhost works too) to
use. (problem with Emscripten)

**NOTE:** If you are using nginx, make sure the `*.wasm` file is served as `application/wasm`, and not
`application/octet-stream`. This can be done by opening `/etc/nginx/mime.types` as root and adding
`application/wasm wasm;` to the types block.

### Linux

This project uses CMake on Linux. Just like WebAssembly, the game assets must be placed in the `game/` directory.

#### Dependencies (Ubuntu/Debian)

- `build-essential` (C/C++ Toolchain)
- `cmake` (CMake)
- `ninja-build` (Ninja)
- `libsdl2-dev` (SDL2)
- `libopenal-dev` (OpenAL)
- `zlib1g-dev` (ZLib)

#### How To Build

```sh
cd platforms/sdl
mkdir build && cd build
cmake -GNinja ..
cmake --build .
# Run
./reminecraftpe
```

### HaikuOS

Dependencies:
- `libsdl2_devel` (SDL2)
- `glu_devel mesa_devel` (Mesa)
- `cmake` (CMake)
- `ninja` (Ninja)

To build, perform the same steps as on Linux.
 
## Have seams when playing?

I've had texture seams when playing Minecraft Classic, ClassiCube and this recreation of Minecraft PE, on a
fresh new laptop. If seams bother you, and you are using an NVIDIA graphics card, go to the NVIDIA Control
Panel, then in "Manage 3D Settings", change "Antialiasing - Mode" to "Application Controlled".

## Notes on assets

The terrain.png and related textures appear to have stayed the same between the E3 demo and the final release
for Xperia PLAY. It appears to have been fetched before Java Edition Beta 1.4's release. This can be seen
because the cookie's texture is missing. (it was added in Java Edition Beta 1.4)

There are plans to create a custom script which downloads and sets up all assets needed for the game.

## Screenshots (from the decomp)

![Title screen](screenshots/title_screen.png)
![Generating world](screenshots/loading.png)
![In-game](screenshots/ingame.png)
![Inventory](screenshots/inventory.png)
![Pause menu](screenshots/pause_screen.png)

## Enhancements

ReMinecraftPE is an enhanced version of the original. To see the original, (mostly) as it was back in the day,
[see the mcped repository](https://github.com/ReMinecraftPE/mcped).

