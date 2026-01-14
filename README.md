# [<img src="logo.png" width="25" alt="The mod's logo." />](https://www.geode-sdk.org/mods/arcticwoof.horrible_ideas) Horrible Ideas
A plethora of ways to ruin your gaming experience...

> [<img alt="Latest Version" src="https://img.shields.io/github/v/release/DumbCaveSpider/HorribleIdeas?include_prereleases&sort=semver&display_name=release&style=for-the-badge&logo=github&logoColor=ffffff&label=Version">](../../releases/) [<img alt="Code License" src="https://img.shields.io/github/license/DumbCaveSpider/HorribleIdeas?style=for-the-badge&logo=gnu&logoColor=ffffff&label=License">](LICENSE.md)
>  
> [<img alt="Downloads" src="https://img.shields.io/github/downloads/DumbCaveSpider/HorribleIdeas/total?style=for-the-badge&logo=geode&logoColor=ffffff&label=Downloads">](https://www.geode-sdk.org/mods/arcticwoof.horrible_ideas)

---

## About
This silly lil' mod adds a mod menu filled to the brim with **over 35 crazy troll options** to mes around with! Spice up your gameplay by adding some truly terrible features to absolutely wreck your entire game.

---

### Options
When pressing `\` or by pressing the floating *Horrible Options* button on your screen, a menu will pop up with a list of joke mod options you can toggle anytime on your game to do some interesting things to your gaming experience. You can view more information within the menu itself.

> [!NOTE]
> *You can customize any keybinds in this mod via the *[Custom Keybinds](https://www.geode-sdk.org/mods/geode.custom-keybinds)* mod.*

#### Player Life
Give the player a limited health-like meter that must always stay above 0 to prevent the player from dying.

#### Jumpscares
Typically give a chance to teleport you to a whole different level, mid-level. Boo. Haha.

#### Randoms
Minor but possibly devastating inconveniences that just pop in from time to time.

#### Chances
Trolls that usually happen on some sort of player interaction.

#### Obstructive
Disturb the player's accessibility to the gameplay.

#### Misc
Probably the worst of it all...

> [!WARNING]
> *Please keep in mind that certain game settings and hacks from mod menus may interfere with some parts of this mod's functionality.*

### Safe Mode
By default, this mod implements its own safe mode to prevent accidental progression in levels. Remember, **using this mod counts as cheating**! You can also disable this in the mod's settings while you're not actively using any horrible options.

---

### Developers
Want to add your own insane stuff to this mod? You can register your very own horrible options by using this mod's API! You can see its [documentation here](./include). We're hyped to see how much more you can really mess up this game.

```cpp
using namespace horrible;

$on_mod(Loaded){
    OptionManager::get()->registerOption({
        "my-option"_spr,
        "My Very Cool Option!",
        "This option is so very cool!",
        "Cool Options",
        SillyTier::Medium
    });
};
```
```cpp
using namespace horrible;

class $modify(MyPlayLayer, PlayLayer) {
    struct Fields {
        bool enabled = OptionManager::get()->getOption("my-option"_spr);
    };

    void setupHasCompleted() {
        if (m_fields->enabled) {
            // do insane stuff with my option!
        };

        PlayLayer::setupHasCompleted();
    };
};
```

> [!NOTE]
> *If you plan on publishing a mod that acts as an add-on, all we ask is to please be sure to follow the safe code practices as instructed in the [documentation](./include) to the best of your ability!*

---

![preview](previews/preview-1.png)
![preview](previews/preview-2.png)
![preview](previews/preview-3.png)
![preview](previews/preview-4.png)
![preview](previews/preview-5.png)

---

### [<img alt="Trailer Likes" height="15" src="https://img.shields.io/youtube/likes/PI_poDUUauw?style=for-the-badge&logo=youtube">](https://www.youtube.com/watch?v=PI_poDUUauw&list=PL0dsSu2pR5cERnq7gojZTKVRvUwWo2Ohu) Watch the [Horrible Mods](https://www.youtube.com/watch?v=Ssl49pNmW_0&list=PL0dsSu2pR5cERnq7gojZTKVRvUwWo2Ohu) series out now on YouTube!