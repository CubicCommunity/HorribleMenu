#include <Utils.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/EnhancedGameObject.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

static constexpr auto id = "dementia";

static auto const o = Option::create(id)
                          .setName("Dementia")
                          .setDescription("Chance for the player to occasionally randomly teleport a few steps back while playing a level.\n<cl>suggested by imdissapearinghelp</c>")
                          .setCategory(category::misc)
                          .setSillyTier(SillyTier::Medium);
HORRIBLE_REGISTER_OPTION(o);

class $modify(DementiaPlayerObject, PlayerObject) {
    HORRIBLE_DELEGATE_HOOKS(id);

    struct Fields {
        unsigned int chance = options::getChance(id);

        int lastMusicTime = 0;  // last music time in milliseconds

        float lastX = 0.f;  // last recorded X position
        float lastY = 0.f;  // last recorded Y position
    };

    bool pushButton(PlayerButton p0) {
        if (m_gameLayer) {
            auto f = m_fields.self();

            int rnd = randng::fast();
            log::trace("player teleport chance {}", rnd);

            FMOD::Channel* musicChannel = nullptr;
            auto fmod = FMODAudioEngine::sharedEngine();

            auto bgchannel = fmod->m_backgroundMusicChannel;
            auto channel = bgchannel->getChannel(0, &musicChannel);

            auto onGround = m_isOnGround || m_isOnGround2 || m_isOnGround3 || m_isOnGround4;
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
    HORRIBLE_DELEGATE_HOOKS(id);

    bool hasBeenActivated() {
        return false;
    };

    bool hasBeenActivatedByPlayer(PlayerObject* p0) {
        return false;
    };
};