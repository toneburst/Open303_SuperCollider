# Open303_SuperCollider

Author: toneburst

SuperCollider wrapper around [Open303](https://github.com/toneburst/Open303/tree/main) by Robin Schmidt.
All credit for the DSP work goes to Robin. Big thanks to you and to the many contributors in the [KVR community](https://www.kvraudio.com/forum/viewtopic.php?t=262829) for this amazing project!

I've naively hacked in some additional features and exposed some original controls that were internal-only. See SuperCollider [helpfile](https://github.com/toneburst/Open303_SuperCollider/blob/main/plugins/Open303/Open303.schelp) and [example](https://github.com/toneburst/Open303_SuperCollider/tree/main/examples) SC script for details.

Will eventually be used in a replacement SuperCollider audio engine for my [bline](https://github.com/toneburst/bline) parametric bassline explorer script for the Monome Norns platform.

The author accepts no reponsibility for the fitness or otherwise of this SuperCollider extension for any other purpose.

### Requirements

- CMake >= 3.5
- SuperCollider source code

### Building

Clone the project:

    git clone https://github.com/toneburst/open303_supercollider
    cd open303_supercollider

Then, use CMake to configure and build it:

    cmake .. -DCMAKE_BUILD_TYPE=Release
    cmake --build . --config Release
    cmake --build . --config Release --target install

You may want to manually specify the install location in the first step to point it at your
SuperCollider extensions directory: add the option `-DCMAKE_INSTALL_PREFIX=/path/to/extensions`.

It's expected that the SuperCollider repo is cloned at `../supercollider` relative to this repo. If
it's not: add the option `-DSC_PATH=/path/to/sc/source`.

### Developing

Use the command in `regenerate` to update CMakeLists.txt when you add or remove files from the
project. You don't need to run it if you only change the contents of existing files. You may need to
edit the command if you add, remove, or rename plugins, to match the new plugin paths. Run the
script with `--help` to see all available options.
