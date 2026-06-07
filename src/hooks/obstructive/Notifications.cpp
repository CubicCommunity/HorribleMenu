#include <Utils.h>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

#define THIS_ID "notifications"

static auto const o = Option::create(THIS_ID)
                          ->setName("Notifications")
                          ->setDescription("During gameplay, notifications will begin to pile up. If you let too many pile up before checking them, you'll be forced out of the level.\n<cl>created by Cheeseworks</c>")
                          ->setCategory(category::obstructive)
                          ->setSillyTier(SillyTier::Medium)
                          ->autoRegister();