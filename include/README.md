# [<img src="../logo.png" width="30" alt="The mod's logo." />](https://www.geode-sdk.org/mods/arcticwoof.horribleideas) Horrible Ideas
A plethora of ways to ruin your gaming experience...

## API
Let's start off by adding this mod as a dependency in your `mod.json`!
```jsonc
"dependencies": {
    "arcticwoof.horribleideas": {
        "importance": "required",
        "version": ">=1.0.0"
    }
}
```

You can directly access the Horrible Ideas mod menu API by including the [`Horrible.hpp`](Horrible.hpp) file in your code. Make sure to include the **`horrible`** namespace to directly access all needed classes and methods.
```cpp
#include <arcticwoof.horribleideas/include/Horrible.hpp>

using namespace horrible;
```