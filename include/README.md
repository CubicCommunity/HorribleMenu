# [<img src="../logo.png" width="30" alt="The mod's logo." />](https://geode-sdk.org/mods/cubicstudios.horriblemenu) Horrible Menu
A plethora of ways to ruin your gaming experience...

## API
Let's start off by adding this mod as a dependency in your `mod.json`!
```jsonc
"dependencies": {
    "cubicstudios.horriblemenu": ">=1.0.0"
}
```

> [!TIP]
> We recommend shortening include paths in your code by adding the the full path as a private include directory through your project's [`CMakeLists.txt`](../CMakeLists.txt) file. The rest of the documentation will use this practice in its code samples.
> ```cmake
> target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/geode-deps/cubicstudios.horriblemenu/include)
> ```

You can directly access the Horrible Menu mod menu API by including the [`API.h`](./horrible/API.h) file in your code. Make sure to include the **`horrible`** namespace to directly access all needed classes and methods.
```cpp
#include <horrible/API.h>

using namespace horrible;
```

### Options
The purpose of this mod is primarily focused on its huge list of togglable troll options. To create the metadata for an option, create a new **`horrible::Option`** object pointer.
```cpp
using namespace horrible;

auto myOption = Option::create("my-option"_spr);
```

> [!TIP]
> To avoid conflicts with this or other mods' option IDs, prefix your option IDs with you mod ID by appending `_spr` at the end of your string.

You should now have a new smart pointer to an option's metadata. Before proceeding, be sure to provide more information about your option with the following setter functions.

- **`setID(std::string)`**: Unique ID of the option, *already handled in `Option::create`*
- **`setName(std::string)`**: Name of the option
- **`setDescription(std::string)`**: Description of the option
- **`setCategory(std::string)`**: Name of the category this option should be under
- **`setSillyTier(SillyTier)`**: How silly the option is
- **`setDefaultToggleState(bool)`**: Default toggle state for this option
- **`setOnline(bool)`**: If the option requires an active internet connection to work properly
- **`setRequiresRestart(bool)`**: If the option requires a game restart to take effect
- **`setSupportedPlatforms(std::vector<Platform>)`**: Platforms that the option supports

Each of these setters has its respective getter in case you ever need to read the information you've stored in these objects.

The `Option` class was designed to allow a readable, syntax-rich builder pattern to simplify the creation process.
```cpp
auto myOption = Option::create("my-option"_spr)
    ->setName("My Very Cool Option")
    ->setDescription("A very detailed description about what this option does...")
    ->setCategory("My Stuff!")
    ->setSillyTier(SillyTier::Low);
```

> [!INFO] 
> This should automatically register your option into Horrible Menu's option manager. If you wish to register it manually later, set the **`autoRegister`** parameter in your `Option::create` call to `false`.
> ```cpp
> auto myOption = Option::create("my-option"_spr, false);
> ```