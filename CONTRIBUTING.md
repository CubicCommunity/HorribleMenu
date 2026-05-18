# Contributing to Horrible Menu
First off, thank you for considering contributing! We simply ask that you follow the guidelines listed below before helping out.

## Pull Requests
You're free to fork our repository and [open a pull request](../../pulls/) if you'd like to submit a new feature or propose a fix for an unknown issue.

Before opening or marking your pull request as ready for review, please be sure to test your build and make sure it is stable and not conflicting with the upstream repository. Try not to diverge from our current code styling and practices in your branch.

### Practices
The things you should always do and not do when opening a new pull request for this project.
- **Be clear!** Explain the important details of and motives behind your changes, if not already obvious.
- **Don't change versioning metadata.** Our maintainers will do that once it's been decided a new update should release soon.
- **Don't write changelogs.** Maintainers will check back on past changes and document everything!
- **Isolate your changes.** Refrain from formatting or changing code outside of what you're actually working on in order to prevent conflicts.
- **Get ready for feedback!** Sometimes not everything done meets the current standard or is just questionable.

### Guidelines
This section mainly covers contributions for new features to Horrible Menu. For other pull requests, we simply ask that you justify your changes in detail when opening your request.

Every whole word in an option ID must be separated specifically by underscores (`_`), to keep things consistent!
```txt
your_option
```

Always define a `THIS_ID` macro in an option's source file to keep things consistent and maintainable in case things change.
```cpp
#define THIS_ID "your_option"
```

If you create your own option for the mod, we recommend adding your name to the end of the description with the following format.
```txt
created by {your preferred name}
```
Or, if it's someone else's idea, use this format.
```txt
suggested by {their preferred name}
```

> [!INFO]
> Your credits in the in-game credits pop-up will be automatically added by our maintainers if deemed meaningful enough - at least one functional option should qualify. Please refrain from modifying that code! We'll make sure your contribution(s) is/are credited appropriately!

Keep includes clean, make sure your IDE isn't configured to automatically add them, or things start to look messy!
```cpp
// Include internal utility headers
#include <Utils.h>

// Include Geode headers
#include <Geode/Geode.hpp>

// Whatever layer(s) are being hooked
#include <Geode/modify/PlayerObject.hpp>

// Include namespaces
using namespace geode::prelude;
using namespace horrible::prelude;

// Then get to writing!
```

#### AI Usage
View our stance on the use of AI in Cubic's work and how to use it for this project **[here](AGENTS.md)**.

## Reporting Issues
If you found a bug or want to make a suggestion, you're welcome to [open an issue](../../issues/) describing your case. Please be sure to provide as many details as possible. If you're having issues with crashing, please always provide a crashlog.

See our [Code of Conduct](CODE_OF_CONDUCT.md) for community standards.