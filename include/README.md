# [<img src="../logo.png" width="30" alt="The mod's logo." />](https://www.geode-sdk.org/mods/arcticwoof.horrible_ideas) Horrible Ideas
A plethora of ways to ruin your gaming experience...

> [!WARNING]
> This is heavily outdated.

## API
Let's start off by adding this mod as a dependency in your `mod.json`!
```jsonc
"dependencies": {
    "arcticwoof.horrible_ideas": {
        "importance": "required",
        "version": ">=1.0.0"
    }
}
```

You can directly access the Horrible Ideas mod menu API by including the [`Horrible.hpp`](Horrible.hpp) file in your code. Make sure to include the **`horrible`** namespace to directly access all needed classes and methods.
```cpp
#include <arcticwoof.horrible_ideas/include/Horrible.hpp>

using namespace horrible;
```

### Classes
Here are some important classes we highly suggest you keep in mind while working with the API.

#### class `horrible::OptionManager`
The manager class for Horrible Ideas mod options.
- `static OptionManager*` **`get()`**: Get option manager singleton
- `void` **`registerOption(Option const& option)`**: Register a new option
  - `Option const&` **`option`**: Constructed option object
- `bool` **`getOption(std::string_view id)`** `const`: Returns the toggle state of an option
- `bool` **`setOption(std::string_view id, bool enable)`** `const`: Set the toggle state of an option
  - `std::string_view` **`id`**: The ID of the option to toggle
  - `bool` **`enable`**: Boolean to toggle to

#### enum class `horrible::SillyTier`
An enum class that defines how chaotic or funny an option is.

#### class `geode::PlatformID`
A dynamic enum class provided by Geode that defines the player's current platform.

#### struct `horrible::Option`
The object structure of an option.
- `std::string` **`id`**: Unique ID of the option
- `std::string` **`name`**: Name of the option
- `std::string` **`description`**: Description of the option
- `std::string` **`category`**: Name of the category this option should be under
- `SillyTier` **`silly`**: How silly the option is
- `bool` **`restart`** (`false`): If the option requires a restart to take effect
- `std::vector<PlatformID>` **`platforms`** (`{ PlatformID::Desktop, PlatformID::Mobile }`): Platforms that the option supports

#### class `horrible::HorribleOptionEvent`
An event that fires any time any option is changed.
- `std::string` **`getId()`** `const`: Get the unique ID of the option
- `bool` **`getToggled()`** `const`: Get the toggle boolean of the option

#### class `horrible::HorribleOptionEventFilter`
- `ListenerResult` **`handle(geode::Function<Callback> fn, HorribleOptionEvent* event)`**: Event handler
  - `geode::Function<Callback>` **`fn`**: Callback function containing a pointer to the event that fired
  - `HorribleOptionEvent*` **`event`**: Pointer to the event that fired

#### Summary
| Type         | Name                        | Description                          |
| ------------ | --------------------------- | ------------------------------------ |
| `class`      | `OptionManager`             | Manager for Horrible Ideas options   |
| `enum class` | `SillyTier`                 | Defines how silly an option is       |
| `struct`     | `Option`                    | Represents a toggleable option       |
| `class`      | `HorribleOptionEvent`       | Fired when an option is toggled      |
| `class`      | `HorribleOptionEventFilter` | Filters through option toggle events |

### Options
You can register and check any and as many options as you desire through this API.

> [!IMPORTANT]
> To work with options, you will first be required to access the pointer to the **`OptionManager`** class by using `OptionManager::get()` to define a variable to use in your code.
>
> ```cpp
> auto optMgr = OptionManager::get();
> ```
>
> This way, you can now safely use its methods to work directly with Horrible Ideas's API to handle your own custom options.

#### Registering
This mod makes it easy for players to access the options they want to use. You can register your own options by using the **`OptionManager::registerOption`** method inside an `$on_mod(Loaded)` block. You will need to pass one parameter, which is a constructed **`Option`** object for the option you want to register.

*Required fields of the **`Option`** struct are, in order: `id`, `name`, `description`, `category`, and `silly`. Optional fields are `restart` and `platforms`.*

> [!TIP]
> Be sure to prefix your option's unique ID with your Geode mod ID by appending **`_spr`** after the end of the string to prevent conflicts with this mod or other mods that may also register options with possibly identical IDs.

```cpp
$on_mod(Loaded){
    auto optMgr = OptionManager::get();

    optMgr->registerOption({
        "something-interesting"_spr,
        "Something Interesting",
        "This is something that is very interesting.",
        "Stuff!",
        SillyTier::Medium,
    });
};
```

You can include optional fields **`restart`** and **`platforms`** as well! Set `restart` to `true` or `false` depending on whether your option is only meant to load once per session. The array for `platforms` uses Geode's dynamic **`PlatformID`** class to identify the exact platform the player is running Geometry Dash on. By default, Horrible Ideas sets every option to be compatible for `PlatformID::Desktop` and `PlatformID::Mobile`, essentially covering all platforms. However, you can also get very specific about the exact platform you can run your own options on if absolutely necessary, though such a case may not present itself often.

> [!IMPORTANT]
> Even if `restart` is **enabled** for your option, the global event for it *will still fire* whenever the player changes it mid-game. What this setting does is actually just notify the player that your option will only load after they restart the game.

```cpp
$on_mod(Loaded){
    auto optMgr = OptionManager::get();

    optMgr->registerOption({
        "cool-things"_spr,
        "Cool Things",
        "Some really really cool things.",
        "Stuff!",
        SillyTier::Low,
        true, // Notify player to restart
        {
            PlatformID::Android32,
            PlatformID::X64 // Support specific platforms
        },
    });
};
```

This will automatically include your option in Horrible Ideas's pre-existing list of options, and will appear in the menu for the player whenever they open it.

#### Handling
Once you've registered an option on `$on_mod(Loaded)`, you can use other methods to work with the option.

##### Static Conditioning
You can begin by using **`OptionManager::getOption`** and provide your option's unique ID to check if an option is enabled or disabled.
```cpp
using namespace geode::prelude;
using namespace horrible;

class $modify(CoolThingsPlayLayer, PlayLayer) {
    struct Fields {
        bool enabled = OptionManager::get()->getOption("cool-things"_spr); // If this option is set to true or false
    };

    bool init(GJGameLevel * level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

        if (m_fields->enabled) {
            // do stuff!
        };

        return true;
    };
};
```

##### Events
If you would like or need to re-implement or remove an option's functionality live as the player clicks on the toggle for that option, you can listen to the **`HorribleOptionEvent`** global event, which fires any time any option is toggled.

> [!IMPORTANT]
> Be sure to return `ListenerResult::Propagate` after you're finished handling each event to ensure other remaining event instances function properly.

```cpp
using namespace geode::prelude;
using namespace horrible;

class $modify(SomethingInterestingMenuLayer, MenuLayer) {
    struct Fields {
        bool enabled = OptionManager::get()->getOption("something-interesting"_spr);
        EventListener<HorribleOptionEventFilter> m_optionListener; // Listen to any option being toggled
    };

    bool init() {
        if (!MenuLayer::init()) return false;

        if (m_fields->enabled) {
            // implemented logic here
        };

        this->template addEventListener<HorribleOptionEventFilter>(
            [this](HorribleOptionEvent* event) {
                if (event->getToggled()) {
                    // handle re-implementation here
                };

                return ListenerResult::Propagate;
            },
            "something-interesting"_spr
        );

        return true;
    };
};
```

## Optional API
Let's start off by adding this mod as an optional dependency in your `mod.json`!
```jsonc
"dependencies": {
    "arcticwoof.horrible_ideas": {
        "importance": "suggested",
        "version": ">=1.0.0"
    }
}
```

You can directly access the Horrible Ideas mod menu optional API by including the [`OptionalAPI.hpp`](OptionalAPI.hpp) file in your code. Make sure to include the **`horrible`** namespace to directly access all needed classes and methods.
```cpp
#include <arcticwoof.horrible_ideas/include/OptionalAPI.hpp>

using namespace horrible;
```

### Classes
These classes mirror the main API but return `geode::Result` values so callers can safely handle functions even if Horrible Ideas is not loaded.

#### class `horrible::HorribleOptionEventV2`
- `std::string` **`getId()`** `const`
- `bool` **`getToggled()`** `const`

#### class `horrible::HorribleOptionEventFilterV2`
- `ListenerResult` **`handle(geode::Function<Callback> fn, HorribleOptionEventV2* event)`**

#### class `horrible::OptionManagerV2`
- `static Result<>` **`registerOption(Option const& option)`**
- `static Result<bool>` **`getOption(std::string_view id)`**
- `static Result<bool>` **`setOption(std::string_view id, bool enable)`**

#### Summary
| Type    | Name                          | Description                          |
| ------- | ----------------------------- | ------------------------------------ |
| `class` | `OptionManagerV2`             | Manager for Horrible Ideas options   |
| `class` | `HorribleOptionEventV2`       | Fired when an option is toggled      |
| `class` | `HorribleOptionEventFilterV2` | Filters through option toggle events |

### Option
You can register and check any and as many options as you desire through this API. Most of the examples given and implied context here will be derived from earlier examples.

#### Registering
Here's how you can register your own options through the optional API.

```cpp
$on_mod(Loaded){
    auto res = OptionManagerV2::registerOption({
        "optional-something"_spr,
        "Optional Something",
        "An option registered through the optional API.",
        "Optional",
        SillyTier::Low,
    });
        
    if (res.isErr()) log::error("Failed to register option: {}", res.unwrapErr());
};
```

### Events
Listening for state changes to a specific option.

```cpp
EventListener<HorribleOptionEventFilterV2> listener = {
    [](HorribleOptionEventV2* ev) {
        if (ev->getToggled()) {
            // re-implement here
        };

        return ListenerResult::Propagate;
    },
    HorribleOptionEventFilterV2("optional-something"_spr)
};
```

## Watch Out!
Some common pitfalls may include the following.
- Forgetting to use `_spr` when defining unique option ID
- Not returning `ListenerResult::Propagate` in event callbacks
- Registering options outside the `$on_mod(Loaded)` block

Always double-check your code to make sure it follows safe practices.

*Happy modding!* 