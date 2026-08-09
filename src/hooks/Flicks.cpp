#include <Util.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

#define THIS_ID "flick"

static auto const o = Option::create(THIS_ID)
                          ->setName("Flicks")
                          ->setDescription("Every time you respawn in a level, whether it be from the beginning or a checkpoint, one of your options get toggled on or off.\n<co>Careful! It might break some things...</c>\n<cl>suggested by scr33n_p4r45173</c>")
                          ->setCategory(category::misc)
                          ->setSillyTier(SillyTier::High)
                          ->setCheating(true)
                          ->autoRegister();

class $modify(FlicksPlayLayer, PlayLayer) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID);

    struct Fields final {
        std::vector<std::weak_ptr<Option>> options = OptionManager::get()->getOptions();

        bool firstTime = true;
    };

    void resetLevel() {
        PlayLayer::resetLevel();

        auto f = m_fields.self();

        if (f->firstTime) {
            f->firstTime = false;
            return;
        };

        if (f->options.empty()) return;

        auto opt = f->options[rng::get(f->options.size() - 1)];
        if (auto o = opt.lock()) {
            if (o->getID() == THIS_ID || !platformCompat(o->getSupportedPlatforms())) return Notification::create("Whoops! Missed an option!", NotificationIcon::Warning, 0.25f)->show();

            queueInMainThread([opt]() {
                if (auto o = opt.lock()) {
                    o->isEnabled() ? o->disable() : o->enable();
                    log::warn("Flicked option {} {} due to flicks", o->getID(), str::isOnOff(o->isEnabled()));

                    sfx::play(sfx::file::bad);
                    Notification::create(fmt::format("Flicked {} ({}) {}", o->getName(), o->getCategory(), str::isOnOff(o->isEnabled())).c_str(), NotificationIcon::Warning, 0.5f)->show();
                };
            });
        };
    };

    bool platformCompat(std::span<const Platform> plats) {
        for (auto const& p : plats) {
            if (p & GEODE_PLATFORM_TARGET) return true;
        };

        return false;
    };
};