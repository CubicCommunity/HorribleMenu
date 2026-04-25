#include "../RandomAd.hpp"

#include <Utils.h>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

bool RandomAd::init() {
    auto const theme = thisMod->getSettingValue<std::string>("theme");

    if (!Popup::init(375.f, 250.f, themes::getBackgroundSprite(theme))) return false;

    setID("ad"_spr);
    setTitle("Sponsored");
    setCloseButtonSpr(CircleButtonSprite::createWithSpriteFrameName(themes::close, 0.875f, themes::getCircleBaseColor(theme)));

    jumpscares::saveLevel(jumpscares::get::congregation());

    auto label = CCLabelBMFont::create("Check out this cool level we found!", "chatFont.fnt");
    label->setID("message");
    label->setAlignment(kCCTextAlignmentCenter);
    label->setPosition({m_mainLayer->getScaledContentWidth() / 2.f, m_mainLayer->getScaledContentHeight() - 37.5f});
    label->setAnchorPoint({0.5, 0.5});

    m_mainLayer->addChild(label);

    // featured project thumbnail
    auto projThumb = LazySprite::create({192.f, 108.f}, true);
    projThumb->setID("thumbnail");
    projThumb->setAnchorPoint({0.5, 0.5});
    projThumb->setPosition({m_mainLayer->getContentWidth() / 2.f, 110.f});

    projThumb->setLoadCallback([thumbnail = WeakRef(projThumb)](Result<> res) {
        if (auto thumb = thumbnail.lock()) {
            if (res.isOk()) {
                log::info("Sprite loaded successfully");
                thumb.take()->setScale(0.625);
            } else {
                log::error("Sprite failed to load: {}", res.unwrapErr());
                thumb.take()->removeMeAndCleanup();
            };
        } else {
            log::error("Thumbnail sprite was destroyed before load callback");
        };
    });

    projThumb->loadFromUrl("https://api.cubicstudios.xyz/avalanche/v1/fetch/random-thumbnail", CCImage::kFmtUnKnown, true);
    if (projThumb) m_mainLayer->addChild(projThumb);

    auto playBtnLoading = LoadingSpinner::create(37.5f);
    playBtnLoading->setID("loading-circle");
    playBtnLoading->setVisible(false);

    // takes u to congreg lol
    auto playBtn = Button::createWithNode(
        ButtonSprite::create(
            "Play!",
            "bigFont.fnt",
            themes::getButtonSquareSprite(theme)),
        [playBtnLoading](Button* sender) {
            sender->setVisible(false);
            playBtnLoading->setVisible(true);

            if (auto pl = PlayLayer::get()) {
                log::info("Switching from ad to Congregation jumpscare");
                jumpscares::switchToLevel(pl, jumpscares::get::congregation(), nullptr, nullptr, false, false);
            } else {
                log::error("Player not in a level");
            };
        });
    playBtn->setID("play-btn");
    playBtn->setPosition({m_mainLayer->getScaledContentWidth() / 2.f, 2.5f});

    playBtnLoading->setPosition(playBtn->getPosition());

    m_mainLayer->addChild(playBtn, 3);
    m_mainLayer->addChild(playBtnLoading, 2);

    sfx::play(sfx::file::pop);

    return true;
};

RandomAd* RandomAd::create() {
    auto ret = new RandomAd();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    };

    delete ret;
    return nullptr;
};