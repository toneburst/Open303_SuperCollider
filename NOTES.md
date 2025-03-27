# BUILD NOTES:

SuperCollider source must be in same directory as plugin project.

## Build Scripts:

To create build files, run
```
cmake .. DSC_PATH=../supercollider -DCMAKE_BUILD_TYPE=Debug
```

from project build directory (create if required).

Then run build from plugin project root
```
cmake --build build --config "Release" --target install
```

## Including Additional Source Files:

If including other C++ files, paths for all used used (implementation and header) must be added to CMaakeLists.txt file in project root dir.

#include path for included files is relative to path of including file! ie
#include "lib/Open303/Source/DSPCode/rosic_Open303.h"

# Git Setup:

Set global git username and email

```
git config --global user.email yournew@email.com
git config --global user.name yournewgoodname
```

# TODO:

## Add note-handling from Shruthi-1?

Replace builtin Open303 note-handling with a more monosynth-like behaviour

Note-stack
https://github.com/pichenettes/shruthi-1/blob/master/shruthi/note_stack.h

Shows implementation?
https://github.com/pichenettes/shruthi-1/blob/master/shruthi/part.h
