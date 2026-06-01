#include <Utils.h>

#include <ranges>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

// concentrating all jumpscare options into this one file cuz we don't want em to interfere with each other through their own hooks

#define THIS_ID_GRIEF "grief"
#define THIS_ID_CONGREG "congregation"
#define THIS_ID_TIDAL "tidal_wave"

static auto const oGrief = Option::create(THIS_ID_GRIEF)
                               ->setName("Get Back on Grief")
                               ->setDescription("A chance of forcing you to play Grief when you die in a level.\n<cl>suggested by Sweep</c>")
                               ->setCategory(category::jumpscares)
                               ->setSillyTier(SillyTier::High)
                               ->setOnline(true)
                               ->autoRegister();

static auto const oCongreg = Option::create(THIS_ID_CONGREG)
                                 ->setName("Congregation Jumpscare")
                                 ->setDescription("A chance of forcing you to play Congregation when you die in a level.\n<cl>suggested by StaticGD</c>")
                                 ->setCategory(category::jumpscares)
                                 ->setSillyTier(SillyTier::High)
                                 ->setOnline(true)
                                 ->autoRegister();

static auto const oTidal = Option::create(THIS_ID_TIDAL)
                               ->setName("'Tidal Wave' Jumpscare!")
                               ->setDescription("A chance of forcing you to play the level 'Tidal Wave' when you die in a level. The level called 'Tidal Wave'. That one.\n<cl>suggested by liliam25</c>")
                               ->setCategory(category::jumpscares)
                               ->setSillyTier(SillyTier::Medium)
                               ->setOnline(true)
                               ->autoRegister();

static StringMap<bool> g_jsMap;
static std::vector<std::weak_ptr<Hook>> g_jsHookVector;

namespace js_internal {
    static constexpr auto getLevelInfo(std::string_view id) noexcept {
        if (id == THIS_ID_GRIEF) return jumpscares::level::grief;
        if (id == THIS_ID_CONGREG) return jumpscares::level::congregation;
        if (id == THIS_ID_TIDAL) return jumpscares::level::tidal;

        return jumpscares::level::grief;
    };

    static void toggleHooks(bool on) {
        log::trace("Toggling all jumpscare hooks {}", on ? "ON" : "OFF");

        for (auto& hook : g_jsHookVector) {
            if (auto h = hook.lock()) (void)h->toggle(on);
        };
    };

    static void toggleOption(ZStringView id, bool on) {
        auto size = g_jsMap.size();

        if (on) g_jsMap[id] = on;

        if (!on) {
            if (auto it = g_jsMap.find(id); it != g_jsMap.end()) g_jsMap.erase(it);
        };

        if (size == 0 && g_jsMap.size() > 0) toggleHooks(true);
        if (size > 0 && g_jsMap.size() == 0) toggleHooks(false);
    };
};

$on_mod(Loaded) {
    js_internal::toggleOption(THIS_ID_GRIEF, options::isEnabled(THIS_ID_GRIEF));
    js_internal::toggleOption(THIS_ID_CONGREG, options::isEnabled(THIS_ID_CONGREG));
    js_internal::toggleOption(THIS_ID_TIDAL, options::isEnabled(THIS_ID_TIDAL));

    listenForHorribleOptionChanges(
        THIS_ID_GRIEF,
        [](HorribleOptionSave data) {
            js_internal::toggleOption(THIS_ID_GRIEF, data.enabled);
        });

    listenForHorribleOptionChanges(
        THIS_ID_CONGREG,
        [](HorribleOptionSave data) {
            js_internal::toggleOption(THIS_ID_CONGREG, data.enabled);
        });

    listenForHorribleOptionChanges(
        THIS_ID_TIDAL,
        [](HorribleOptionSave data) {
            js_internal::toggleOption(THIS_ID_TIDAL, data.enabled);
        });
};

static void tryJumpscare(bool useReplay) {
    for (auto const& js : g_jsMap) {
        if (rng::fast() <= options::getChance(js.first)) {
            auto const level = js_internal::getLevelInfo(js.first);

            log::debug("jumpscare for {} triggered!", level);
            return jumpscares::switchLevel(level, false, useReplay);
        };
    };
};

class $modify(ForceLevelsPlayLayer, PlayLayer) {
    static void onModify(auto& self) {  // should auto disable when no jumpscare options are on
        utils::StringMap<std::shared_ptr<Hook>> const& hooks = self.m_hooks;

        for (auto const& hook : hooks | std::views::values) {
            hook->setAutoEnable(g_jsMap.size() > 0);
            g_jsHookVector.push_back(hook);
        };
    };

    void destroyPlayer(PlayerObject* player, GameObject* object) {
        PlayLayer::destroyPlayer(player, object);

        if (object == m_anticheatSpike && !player->m_isDead) return;

        tryJumpscare(m_useReplay);
    };
};