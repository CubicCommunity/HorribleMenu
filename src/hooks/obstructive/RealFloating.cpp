#include <Utils.h>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

#define THIS_ID "da_real_floating_btn"

static auto const o = Option::create(THIS_ID)
                          ->setName("da real floating button")
                          ->setDescription("You'll have a floating button constantly zooming around your screen during gameplay. If you press it, your character will immediately die.\n<cl>created by Cheeseworks</c>")
                          ->setCategory(category::obstructive)
                          ->setSillyTier(SillyTier::Medium)
                          ->autoRegister();