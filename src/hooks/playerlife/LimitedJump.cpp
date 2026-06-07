#include <Utils.h>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

#define THIS_ID "limited_jump"

static auto const o = Option::create(THIS_ID)
                          ->setName("Limited Jump Energy")
                          ->setDescription("You only have a limited amount of jumps you can make for a period of time, which will regenerate at a random pace over time. Run out of jumps too quickly, and your character will die.\n<cl>created by Cheeseworks</c>")
                          ->setCategory(category::playerlife)
                          ->setSillyTier(SillyTier::Medium)
                          ->autoRegister();