#include <Utils.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

inline static constexpr auto id = "blurry";

inline static Option const o = {
    id,
    "Blurry Game",
    "Makes the entire game blurry. I think you forgot your glasses.\n<cy>'Blur API' mod is required.</c>\n<cl>Credit: ArcticWoof</c>",
    category::obstructive,
    SillyTier::Medium,
};
HORRIBLE_REGISTER_OPTION_DEPENDENCY(o, "thesillydoggo.blur-api");

class $modify(BlurPlayLayer, PlayLayer) {
    HORRIBLE_DELEGATE_HOOKS(id);

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();

        auto colorLayer = CCLayerColor::create({0, 0, 0, 0});
        colorLayer->setID("blur-color-layer"_spr);
        colorLayer->setContentSize({getScaledContentWidth(), getScaledContentHeight()});
        colorLayer->setPosition({0, 0});

        m_uiLayer->addChild(colorLayer, 98);

        blur::addBlur(colorLayer);
    };
};