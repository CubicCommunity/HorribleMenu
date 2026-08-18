#include "../LabelArea.hpp"

#include <Util.h>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

bool LabelArea::init(std::string text, float width, float scale, cocos2d::ccColor3B const& bgColor) {
    if (!CCNode::init()) return false;

    setAnchorPoint(anchor::center);
    setContentWidth(width);

    auto textLabel = Label::createRich(std::move(text), font::chat);
    textLabel->setZOrder(1);
    textLabel->setScale(scale);
    textLabel->setAlignment(Label::Alignment::Center);
    textLabel->setMaxWidth((getScaledContentWidth() - 8.75f) / scale);

    setContentHeight(textLabel->getScaledContentHeight() + 5.f);

    auto textContainer = NineSlice::create(themes::square);
    textContainer->setColor(bgColor);
    textContainer->setContentSize(getScaledContentSize());

    textContainer->addChildAtPosition(textLabel, Anchor::Center, {}, false);

    addChildAtPosition(textContainer, Anchor::Center, {}, false);

    return true;
};

LabelArea* LabelArea::create(std::string text, float width, float scale, cocos2d::ccColor3B const& bgColor) {
    auto ret = new LabelArea();
    if (ret->init(std::move(text), width, scale, bgColor)) {
        ret->autorelease();
        return ret;
    };

    delete ret;
    return nullptr;
};