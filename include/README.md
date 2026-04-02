# [<img src="../logo.png" width="30" alt="The mod's logo." />](https://geode-sdk.org/mods/cubicstudios.horriblemenu) Horrible Menu
A plethora of ways to ruin your gaming experience...

## API
Let's start off by adding this mod as a dependency in your `mod.json`!
```jsonc
"dependencies": {
    "cubicstudios.horriblemenu": ">=1.0.0"
}
```

Optionally, you can shorten include paths in your code by adding the following line to your [`CMakeLists.txt`](../CMakeLists.txt). The documentation moving forward will use this practice in its code samples.
```cmake
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/geode-deps/cubicstudios.horriblemenu/include)
```

You can directly access the Horrible Menu mod menu API by including the [`API.h`](./horrible/API.h) file in your code. Make sure to include the **`horrible`** namespace to directly access all needed classes and methods.
```cpp
#include <horrible/API.h>

using namespace horrible;
```