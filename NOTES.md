# SOURCES

[Open303 Repo](https://github.com/toneburst/Open303/tree/main)
[KVR thread](https://www.kvraudio.com/forum/viewtopic.php?t=262829&start=1095)

# BUILD NOTES:

SuperCollider source must be in same directory as plugin project!

## Building:

To create build files, run
```
cmake .. -DCMAKE_BUILD_TYPE=Release
```
...from project build directory (create if required).

Then run build from plugin project root
```
cmake --build build --config "Release" --target install
```

Set github username (global)

```
git config --global user.email "YOUR_EMAIL"
```

Test

```
git config --global user.email
```
[More info](https://docs.github.com/en/account-and-profile/setting-up-and-managing-your-personal-account-on-github/managing-email-preferences/setting-your-commit-email-address#setting-your-commit-email-address-in-git)

## Symlink to SuperCollider Extensions

Adding a symlink to the SC Extensions folder means when plugin is build it will automatically be installed.

```
ln -s /Users/<username>/Library/Application\ Support/SuperCollider/Extensions/toneburst <path to plugin project build folder> 
```

## Including Additional Source Files:

If including other C++ files, paths for all used used (implementation and header) must be added to CMaakeLists.txt file in project root dir.

#include path for included files is relative to path of including file! ie
#include "lib/Open303/Source/DSPCode/rosic_Open303.h"

# Git Setup:

Check out SuperCollider
```
git clone --recursive git://github.com/supercollider/supercollider.git
(from plugin project containing folder)
```

Set global git username and email
```
git config --global user.email yournew@email.com
git config --global user.name yournewgoodname
```

# TODO:

## Add note-handling from Shruthi-1?

Implement Open303 extended controls.

Add sub-oscillator (squarewave at -1 octave?)

Add filter cutoff moduluation from oscillator?

Add noise

Add overdrive/distortion
Use ChowDSP's ["AnalogVintageDistortion"](https://github.com/madskjeldgaard/portedplugins/tree/main?tab=readme-ov-file#building)

Bus setup to allow parallel distortion/compression
Busses [docs](https://doc.sccode.org/Tutorials/Getting-Started/11-Busses.html)

Compile for Norns RPi

Create updated Norns engine class
