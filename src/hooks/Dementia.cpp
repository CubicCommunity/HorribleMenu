#include <Utils.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/EnhancedGameObject.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

static Option const o = {
    "dementia",
    "Dementia",
    "Chance of the player randomly teleports back. This is more like player lagging to be honest!\n<cy>Credit: imdissapearinghelp</c>",
    category::misc,
    SillyTier::Medium,
};
REGISTER_HORRIBLE_OPTION(o);

class $modify(DementiaPlayerObject, PlayerObject) {
    struct Fields {
        bool enabled = options::get(o.id);
        int chance = options::getChance(o.id);

        int lastMusicTime = 0; // last music time in milliseconds

        float lastX = 0.f;     // last recorded X position
        float lastY = 0.f;     // last recorded Y position
    };

    bool pushButton(PlayerButton p0) {
        if (m_gameLayer) {
            auto f = m_fields.self();

            if (f->enabled) {
                int rnd = randng::fast();
                log::debug("player teleport chance {}", rnd);

                FMOD::Channel* musicChannel = nullptr;
                auto fmod = FMODAudioEngine::sharedEngine();

                auto bgchannel = fmod->m_backgroundMusicChannel;
                auto channel = bgchannel->getChannel(0, &musicChannel);

                auto onGround = m_isOnGround || m_isOnGround2 || m_isOnGround3 || m_isOnGround4;
                // dementia
                if (rnd <= f->chance) {
                    setPosition({ f->lastX, f->lastY });
                    log::debug("player has dementia to ({}, {}), play time {}", f->lastX, f->lastY, f->lastMusicTime);

                    // set the music time back to the last recorded time
                    if (musicChannel) musicChannel->setPosition(f->lastMusicTime, FMOD_TIMEUNIT_MS);

                    return PlayerObject::pushButton(p0);
                } else if (onGround) { // save the position only if on ground
                    f->lastX = getPositionX();
                    f->lastY = getPositionY();

                    f->lastMusicTime = fmod->getMusicTimeMS(1);

                    log::debug("position recorded to ({}, {}) and music time {}", f->lastX, f->lastY, f->lastMusicTime);
                };
            };
        };

        return PlayerObject::pushButton(p0);
    };
};

class $modify(DementiaEnhancedGameObject, EnhancedGameObject) {
    struct Fields {
        bool enabled = options::get(o.id);
    };

    bool hasBeenActivated() {
        return m_fields->enabled || EnhancedGameObject::hasBeenActivated();
    };

    bool hasBeenActivatedByPlayer(PlayerObject * p0) {
        return m_fields->enabled || EnhancedGameObject::hasBeenActivatedByPlayer(p0);
    };
};