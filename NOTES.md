# BUILD NOTES:

SuperCollider source must be in same directory as plugin project.

## Build Scripts:

To create build files, run
```
cmake .. DSC_PATH=../supercollider -DCMAKE_BUILD_TYPE=Debug
```
...from project build directory (create if required).

Then run build from plugin project root
```
cmake --build build --config "Release" --target install
```

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

Add Low-Band-High-pass filter fade control (remove filter-select parameter)

Add sub-oscillator (squarewave at -1 octave?)

Add filter cutoff moduluation from oscillator?

Add noise

Add ChowDSP distortion

Compile for Norns RPi

Create updated Norns engine class
Add overdrive/distortion
