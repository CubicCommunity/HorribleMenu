# v1.2.7 
- Fix an issue with Safe Mode always being active on startup ([#15](https://github.com/CubicCommunity/HorribleMenu/pull/15))
- Added Safe Mode indicator to level end screen ([#15](https://github.com/CubicCommunity/HorribleMenu/pull/15))
- Added credits for jumpscare levels' creators

# v1.2.6 
- Update credits

# v1.2.5 
- Fix visual issues with long text labels in *Horrible Options Menu*

# v1.2.4 
- Improved functionality of *Agree To Our Terms!* option ([#12](https://github.com/CubicCommunity/HorribleMenu/pull/12))
- Sound effect for *Charged Leap* option will now only play when leaping at full charge
- Lowered default chance setting for *Random Achievements* option to `20%`
- Added license information to Horrible Menu credits pop-up
- Internal tweaks

# v1.2.3 
- Rework game modification internals to allow safer option toggling mid-level
  - This should reduce crash issues with *Flicks* option
- Fix jump boost not working for *Ending Gamble*, *Charged Leap*, and *Super Jump* options while the player's gravity is upside-down
- Lowered cooldown range for *Blinking Icon* option
- Several tweaks

# v1.2.2 
- Tweaked charge-up flow for *Charged Leap* option
- Decreased speed multiplier for *Charged Leap* option by `16.7%`
- Fix issue with *Randomly Don't Jump* option
- Lowered default chance setting for *Randomly Don't Jump* option to `30%`
- Fix visual bug with fading color progress bars
- Added some helpful buttons to the Horrible Menu credits pop-up!
  - **Breakeode has opened [a support Discord server](https://dsc.gg/breakeode)** for players to report bugs or look for help with using this mod!
- Other tweaks

# v1.2.1 
- Raised *Inversed Inputs* option Silly Tier to **Medium**
- Raised *Size Changer* option Silly Tier to **Medium**
- Lowered *Player Health* option Silly Tier to **Low**
- Lowered *Earthquake* option Silly Tier to **Low**
- Lowered *Confetti Explosions* option Silly Tier to **Low**
- Added *Copy Enabled Options* button in mod settings
- Re-balanced leap power for *Charged Leap* option
  - Decreased speed multiplier by `20%`
  - Decreased jump power by `18.75%`
  - Increased charge-up rate by `30%`
- Decreased speed-up rate for *Powering Velocity* option by `52.5%` for flying game modes
- Floating menu button now resizes properly when changing graphics quality
- Fix sizing issues with thumbnails on *Level Ads* for lower graphics qualities
- Fix ad on *Level Ads* being stuck loading if already playing the advertised level
- Add notification hint for *'Tidal Wave' Jumpscare!* option to clarify it intentionally doesn't switch to the extreme demon we're all thinking about
- Internal optimizations

# v1.2.0 
- Added new option category **Mechanics**
  - Added new *Mechanics* option **Charged Leap**
  - Added new *Mechanics* option **Powering Velocity**
  - Moved *Double-Jump* option to Mechanics category
  - Moved *Inversed Inputs* option to Mechanics category
  - Moved *Parry Obstacles* option to Mechanics category
- Added new *Jumpscares* option **'Tidal Wave' Jumpscare!**
- Added new *Jumpscares* option **You're Getting Trolled**
- Added new *Obstructive* option **Verify Your Captchas...**
- Added new *Misc* option **Flicks**
- Floating menu button can now be hidden in the editor
  - Added toggle setting `Show in Editor`
- Moved floating menu button settings to their own section in mod settings
- Added fading color to progress bars on time-based options
- Unmarked all *Jumpscares* category options as cheats
- Tweaked *Double-Jump* and *Dementia* ground detection
- Clarify that *Parry Obstacles* is still in the works :)

# v1.1.2 
- Reworked internals for all *Jumpscares* category options
  - Level downloading reworked to better support all platforms
  - Options from this category will no longer conflict with each other if multiple are enabled at once
- Added confirmation pop-up to *Disable Cheats* button in mod settings
- Internal cleanup

# v1.1.1 
- Added *Disable Cheats* button in mod settings
- micro-optimzations :)

# v1.1.0 
- Raised minimum Geode loader support to `v5.4.0`
- Added dynamic safe mode setting ([#6](https://github.com/CubicCommunity/HorribleMenu/pull/6))
  - Added cheat marker to several options ([#6](https://github.com/CubicCommunity/HorribleMenu/pull/6))
- Added information label to Safe Mode section in mod settings
- Added new *Randoms* option **Random Reverses**
- Moved *Random Achievements* option to Misc category
- *Ending Gamble* option no longer tries to force the player to jump when failing
  - It will now just make the player bounce at a random height! :D
- Silliness filter dropdown in options menu now resets when clearing filters
- Fix touch issues with category list and Silliness filter dropdown in options menu
- Fix input detection issue with *Randomly Don't Jump* option
- Reworked *Horrible Options Menu* button section in mod settings

# v1.0.3 
- Added additional info button to *Safe Mode* label in options menu
- Raised *Ending Gamble* option Silly Tier to **High**
- Tweaked some options' description
- Minor tweaks

# v1.0.2 
- Fix issue with *Mock your 90%+ Fail* option fetching records
- Fix UI ordering issues

# v1.0.1 
- Fix file path issues with *Mock your 90%+ Fail* option on Android

# v1.0.0 
- Initial release