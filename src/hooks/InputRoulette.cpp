#include <Utils.h>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

#define THIS_ID "input_roulette"

static auto const o = Option::create(THIS_ID)
                          ->setName("Input Roulette")
                          ->setDescription("A meter will show on the side of the screen during gameplay hovering over a bar showing the safe and not-so-safe areas the floating arrow can be on when you press your jump button. If the arrow happens to be in an unsafe area when you press jump, your character has an 80% chance of dying.\n<cl>created by Cheeseworks</c>")
                          ->setCategory(category::misc)
                          ->setSillyTier(SillyTier::High)
                          ->autoRegister();