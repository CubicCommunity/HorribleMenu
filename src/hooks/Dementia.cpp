#include <Utils.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/EnhancedGameObject.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

#define THIS_ID "dementia"

static auto const o = Option::create(THIS_ID)
                          ->setName("Dementia")
                          ->setDescription("Chance for the player to occasionally randomly teleport a few steps back while playing a level.\n<cl>suggested by imdissapearinghelp</c>")
                          ->setCategory(category::misc)
                          ->setSillyTier(SillyTier::Medium)
                          ->setCheating(true)
                          ->autoRegister();

class $modify(DementiaPlayerObject, PlayerObject) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID);

    struct Fields final {
        uint8_t chance = options::getChance(THIS_ID);

        int lastMusicTime = 0;  // last music time in milliseconds

        float lastX = 0.f;  // last recorded X position
        float lastY = 0.f;  // last recorded Y position
    };

    bool pushButton(PlayerButton p0) {
        if (!m_gameLayer) return PlayerObject::pushButton(p0);

        auto f = m_fields.self();

        auto rnd = rng::fast();
        log::trace("player teleport chance {}", rnd);

        if (auto fmod = FMODAudioEngine::sharedEngine()) {
            FMOD::Channel* musicChannel = nullptr;

            auto bgchannel = fmod->m_backgroundMusicChannel;
            auto channel = bgchannel->getChannel(0, &musicChannel);

            auto onGround = m_isOnGround && m_isOnGround2 && m_isOnGround3 && m_isOnGround4;

            // dementia
            if (rnd <= f->chance) {
                setPosition({f->lastX, f->lastY});
                log::trace("player has dementia to ({}, {}), play time {}", f->lastX, f->lastY, f->lastMusicTime);

                // set the music time back to the last recorded time
                if (musicChannel) musicChannel->setPosition(f->lastMusicTime, FMOD_TIMEUNIT_MS);

                return PlayerObject::pushButton(p0);
            } else if (onGround) {  // save the position only if on ground
                f->lastX = getPositionX();
                f->lastY = getPositionY();

                f->lastMusicTime = fmod->getMusicTimeMS(1);

                log::trace("position recorded to ({}, {}) and music time {}", f->lastX, f->lastY, f->lastMusicTime);
            };
        };

        return PlayerObject::pushButton(p0);
    };
};

class $modify(DementiaEnhancedGameObject, EnhancedGameObject) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID);

    bool hasBeenActivated() {
        return false;
    };

    bool hasBeenActivatedByPlayer(PlayerObject* p0) {
        return false;
    };
};