# [<img src="../logo.png" width="30" alt="The mod's logo." />](https://www.geode-sdk.org/mods/cubicstudios.horriblemenu) Horrible Menu
A plethora of ways to ruin your gaming experience...

## API
Let's start off by adding this mod as a dependency in your `mod.json`!
```jsonc
"dependencies": {
    "cubicstudios.horriblemenu": ">=1.0.0"
}
```

You can directly access the Horrible Menu mod menu API by including the [`Horrible.h`](Horrible.h) file in your code. Make sure to include the **`horrible`** namespace to directly access all needed classes and methods.
```cpp
#include <cubicstudios.horriblemenu/include/Horrible.h>

using namespace horrible;
```