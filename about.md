# Horrible Menu
###### A plethora of ways to ruin your experience...

## About
This silly lil' mod adds a funny mod menu filled to the brim with **over 40 crazy troll options** to mess around with! Spice up your gameplay by adding some truly insane features to absolutely wreck your entire game.

---

### Options
When <cf>pressing `\` on your keyboard</c> or by <cl>pressing the floating *![Horrible Options](frame:cubicstudios.horriblemenu/icon.png?scale=0.375)* button on your screen</c>, a <cg>menu</c> will pop up with <cg>a list of horrible mod options</c> you can <cy>toggle anytime</c> on your game to do some interesting things to it. You can view more information within the menu itself.

> ![ℹ️](frame:GJ_infoIcon_001.png?scale=0.5) <cj>*You can customize any keybinds in this mod through its settings.*</c>

### Safe Mode
By default, this mod implements its own safe mode system to prevent making accidental progression in levels while this mod is active. Remember, <cr>**using this mod may count as cheating**</c>! You can also disable this in the mod's settings while you're not actively using any horrible options.

> ![⚠️](frame:geode.loader/info-warning.png?scale=0.375) <cy>*Please keep in mind that certain game settings and hacks from mod menus may interfere with some parts of this mod's functionality.*</c>

---

### Integrations
Want to add your own insane stuff to this mod? You can <cy>register your very own horrible options</c> by using <cg>this mod's API</c>! You can find its [documentation in the Wiki](https://github.com/CubicCommunity/HorribleMenu/wiki). We're hyped to see how much more you can really mess up this game.

```cpp
using namespace horrible;

static auto const opt = Option::create("my-option"_spr)
    ->setName("My Very Cool Option!");
    ->setDescription("This option is so very cool!");
    ->setCategory("Cool Options");
    ->setSillyTier(SillyTier::Medium);

HORRIBLE_REGISTER_OPTION(opt);

class $modify(MyPlayLayer, PlayLayer) {
    HORRIBLE_DELEGATE_HOOKS("my-option"_spr);

    // a vanilla function hook
    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();

        // do evil stuff with my option!
    };
};
```

> ![✳️](frame:collaborationIcon_001.png) <cg>*If you plan on publishing **a mod that acts as an add-on**, all we ask is to please be sure to follow the safe code practices as instructed in the [documentation](https://github.com/CubicCommunity/HorribleMenu/wiki) to the best of your ability!*</c>

---

#### Contributions
[Horrible Menu is free and open-source](https://github.com/CubicCommunity/HorribleMenu). You're welcome to visit our GitHub repository and report any issues, make suggestions, or open pull requests aimed at fixing bugs or adding new exciting features.

![💻](frame:geode.loader/github.png?scale=0.375) **[Contribute to Horrible Menu on GitHub](https://github.com/CubicCommunity/HorribleMenu)**

#### Community
Projects like *Horrible Menu* are carried by community support and hype. If you're looking for a sprawling yet cozy space to hang out with other cool people, consider joining [Cubic Studios's community Discord server](https://www.dsc.gg/cubic)!

![🏘](frame:gj_discordIcon_001.png?scale=0.375) **[Join Cubic Community on Discord](https://www.dsc.gg/cubic)**

---

#### Thanks
- **[Cheeseworks](user:6408873)**: Internals, API/DX, UI/UX, options' features, mod branding
- **[ArcticWoof](user:7689052)**: UI, options' features, Horrible Menu logo

*as well as...*

- **[Geode SDK](mod:geode.loader)**: Created an incredible SDK that made this mod possible!
- **[RobTop Games](user:71)**: Made [Geometry Dash](https://youtu.be/k90y6PIzIaE)...

*and...*

- **[You](https://www.github.com/CubicCommunity/HorribleMenu/stargazers/)!**: For being there and keeping us motivated to continue this big ole' project.

---

This mod is published by **[Cubic Studios](https://www.cubicstudios.xyz/)**, on behalf of the [Breakeode](https://breakeode.cubicstudios.xyz/) developer team.