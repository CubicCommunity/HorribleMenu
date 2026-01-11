# Horrible Ideas
**A plethora of ways to ruin your experience...**

*by [ArcticWoof](user:7689052) & [Cheeseworks](user:6408873)!*

---

## About
This silly lil' mod adds a mod menu filled to the brim with **over 30 crazy troll options** to mess around with! Spice up your gameplay by adding some truly terrible features to absolutely wreck your entire game.

---

### Options
When <cf>pressing `\` on your keyboard</c> or by <cl>pressing the floating *Horrible Options* button on your screen</c>, a <cg>menu</c> will pop up with <cg>a list of horrible mod options</c> you can <cy>toggle anytime</c> on your game to do some interesting things to your gaming experience. You can view more information within the menu itself.

> ![ℹ️](frame:GJ_infoIcon_001.png?scale=0.5) <cj>*You can customize any keybinds in this mod via the *[Custom Keybinds](mod:geode.custom-keybinds)* mod.*</c>

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

> ![⚠️](frame:geode.loader/info-warning.png?scale=0.375) <cy>*Please keep in mind that certain game settings and hacks from mod menus may interfere with some parts of this mod's functionality.*</c>

### Safe Mode
By default, this mod implements its own safe mode to prevent accidental progression in levels. Remember, <cr>**using this mod counts as cheating**</c>! You can also disable this in the mod's settings while you're not actively using any horrible options.

---

### Developers
Want to add your own insane stuff to this mod? You can <cy>register your very own horrible options</c> by using <cg>this mod's API</c>! You can see its [documentation here](https://github.com/DumbCaveSpider/HorribleIdeas/tree/main/include). We're hyped to see how much more you can really mess up this game.

```cpp
using namespace horrible;

$execute{
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

> ![ℹ️](frame:GJ_infoIcon_001.png?scale=0.5) <cj>*If you plan on publishing a mod that acts as an add-on to Horrible Ideas, all we ask is to please be sure to follow the safe code practices as instructed in the [documentation](https://github.com/DumbCaveSpider/HorribleIdeas/tree/main/include) to the best of your ability!*</c>

---

**![YT](frame:gj_ytIcon_001.png?scale=0.375) Watch the [Horrible Mods](https://www.youtube.com/watch?v=Ssl49pNmW_0&list=PL0dsSu2pR5cERnq7gojZTKVRvUwWo2Ohu) series out now on YouTube!**