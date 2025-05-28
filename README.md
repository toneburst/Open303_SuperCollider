# Open303_SuperCollider

Author: toneburst

SuperCollider wrapper around the DSP code of [Open303](https://github.com/toneburst/Open303/tree/main) by [@RobinSchmidt](https://github.com/RobinSchmidt).
All credit for the DSP work goes to Robin. Big thanks to you and to the many contributors in the [KVR community](https://www.kvraudio.com/forum/viewtopic.php?t=262829) for this amazing project!

I've naively hacked in some additional features and exposed some original controls that were internal-only. See SuperCollider [helpfile](https://github.com/toneburst/Open303_SuperCollider/blob/main/plugins/Open303/Open303.schelp) and [example](https://github.com/toneburst/Open303_SuperCollider/tree/main/examples) SC script for details, but to summarise, the main additions are:

1. Lowpass (303 emulation) > Bandpass > Highpass morphing filter
1. External audio input to filter/VCA

Will eventually be used in a replacement SuperCollider audio engine for my [bline](https://github.com/toneburst/bline) parametric bassline explorer script for the Monome Norns platform.

The author accepts no reponsibility for the fitness or otherwise of this SuperCollider extension for any other purpose.

### Requirements

- CMake >= 3.5
- SuperCollider source code

### Building

Clone the project:

    git clone https://github.com/toneburst/open303_supercollider
    cd open303_supercollider

If you're on a macOS Linux, or Monome Norns device, you can use the scripts in the scripts directory to build the extension on your system. You may need to modify the extension install path on Linux.
See comments in scripts for usage details.

On Windows, you can use CMake directly to configure and build the extension from this source:

    cmake .. -DCMAKE_BUILD_TYPE=Release
    cmake --build . --config Release
    cmake --build . --config Release --target install

I don't have a Windows machine to test, so please reach out of you can help out with a build script for the platform, or if you find issues building or using the extension in Windows.

You may want to manually specify the install location in the first step to point it at your
SuperCollider extensions directory: add the option `-DCMAKE_INSTALL_PREFIX=/path/to/extensions`.

In all cases, it's expected that the SuperCollider repo is cloned at `../supercollider` relative to this repo. If
it's not: add the option `-DSC_PATH=/path/to/sc/source`.

Building of SuperNova plugins has been disabled, as it's not desirable to have these installed on a Norns system and because it was slowing building during development. Re-enable with the SUPERNOVA option [here](https://github.com/toneburst/Open303_SuperCollider/blob/40b4779a3064bff75a86fd1201328cb630eddd21/CMakeLists.txt#L51).

### Developing

Use the command in `regenerate` to update CMakeLists.txt when you add or remove files from the
project (you don't need to run it if you only change the contents of existing files).
You will need to edit the command to point to your SuperCollider source directory. You may also need to update it if you add, remove, or rename plugins, to match the new plugin paths. Run the script with `--help` to see all available options.
