#include <Utils.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayerObject.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

static constexpr auto id = "random_gamemode";

static auto const o = Option::create(id)
                          .setName("Game Mode Randomizer")
                          .setDescription("Switch to a different game mode every time you enter a portal.\n<co>Might cause funny behavior to happen elsewhere.</c>\n<cl>Credit: Cheeseworks</c>")
                          .setCategory(category::randoms)
                          .setSillyTier(SillyTier::Medium);
HORRIBLE_REGISTER_OPTION(o);

static constexpr auto s_modes = std::to_array<GameObjectType>({
    GameObjectType::CubePortal,
    GameObjectType::ShipPortal,
    GameObjectType::BallPortal,
    GameObjectType::UfoPortal,
    GameObjectType::WavePortal,
    GameObjectType::RobotPortal,
    GameObjectType::SpiderPortal,
    GameObjectType::SwingPortal,
});

#define HORRIBLE_RANDOMGAMEMODE()                                                                    \
    auto type = s_modes[randng::get(s_modes.size() - 1)];                                            \
                                                                                                     \
    switch (type) {                                                                                  \
        default: [[fallthrough]];                                                                    \
                                                                                                     \
        case GameObjectType::CubePortal: return PlayerObject::toggleRollMode(enable, noEffects);     \
        case GameObjectType::ShipPortal: return PlayerObject::toggleFlyMode(enable, noEffects);      \
        case GameObjectType::BallPortal: return PlayerObject::toggleRollMode(enable, noEffects);     \
        case GameObjectType::UfoPortal: return PlayerObject::toggleBirdMode(enable, noEffects);      \
        case GameObjectType::WavePortal: return PlayerObject::toggleDartMode(enable, noEffects);     \
        case GameObjectType::RobotPortal: return PlayerObject::toggleRobotMode(enable, noEffects);   \
        case GameObjectType::SpiderPortal: return PlayerObject::toggleSpiderMode(enable, noEffects); \
        case GameObjectType::SwingPortal: return PlayerObject::toggleSwingMode(enable, noEffects);   \
    }

class $modify(RandomGamemodePlayerObject, PlayerObject) {
    HORRIBLE_DELEGATE_HOOKS(id);

    void toggleRollMode(bool enable, bool noEffects) {
        HORRIBLE_RANDOMGAMEMODE();
    };

    void toggleFlyMode(bool enable, bool noEffects) {
        HORRIBLE_RANDOMGAMEMODE();
    };

    void toggleBirdMode(bool enable, bool noEffects) {
        HORRIBLE_RANDOMGAMEMODE();
    };

    void toggleDartMode(bool enable, bool noEffects) {
        HORRIBLE_RANDOMGAMEMODE();
    };

    void toggleRobotMode(bool enable, bool noEffects) {
        HORRIBLE_RANDOMGAMEMODE();
    };

    void toggleSpiderMode(bool enable, bool noEffects) {
        HORRIBLE_RANDOMGAMEMODE();
    };

    void toggleSwingMode(bool enable, bool noEffects) {
        HORRIBLE_RANDOMGAMEMODE();
    };
};