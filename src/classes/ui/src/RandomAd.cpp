#include "../RandomAd.hpp"

#include <Utils.hpp>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

bool RandomAd::init() {
    if (!Popup::init(375.f, 250.f)) return false;

    setID("ad"_spr);
    setTitle("Sponsored");

    auto label = CCLabelBMFont::create("Check out this cool level we found!", "chatFont.fnt");
    label->setID("message");
    label->setAlignment(kCCTextAlignmentCenter);
    label->setPosition({ m_mainLayer->getScaledContentWidth() / 2.f, m_mainLayer->getScaledContentHeight() - 37.5f });
    label->setAnchorPoint({ 0.5, 0.5 });

    m_mainLayer->addChild(label);

    // featured project thumbnail
    auto projThumb = LazySprite::create({ 192.f, 108.f }, true);
    projThumb->setID("thumbnail");
    projThumb->setAnchorPoint({ 0.5, 0.5 });
    projThumb->setPosition({ m_mainLayer->getContentWidth() / 2.f, 110.f });

    projThumb->setLoadCallback([this, projThumb](Result<> res) {
        if (res.isOk()) {
            log::info("Sprite loaded successfully");

            projThumb->setScale(0.625);
        } else {
            log::error("Sprite failed to load: {}", res.unwrapErr());
            projThumb->removeMeAndCleanup();
        };
                               });

    projThumb->loadFromUrl("https://api.cubicstudios.xyz/avalanche/v1/fetch/random-thumbnail", CCImage::kFmtUnKnown, true);
    if (projThumb) m_mainLayer->addChild(projThumb);

    auto playBtnSprite = ButtonSprite::create("Play!");

    // takes u to congreg lol
    auto playBtn = CCMenuItemSpriteExtra::create(
        playBtnSprite,
        this,
        menu_selector(RandomAd::onPlayBtn)
    );
    playBtn->setPosition({ m_mainLayer->getScaledContentWidth() / 2.f, 2.5f });
    playBtn->ignoreAnchorPointForPosition(false);
    playBtn->setVisible(true);

    m_buttonMenu->addChild(playBtn, 3);

    return true;
};

void RandomAd::onPlayBtn(CCObject*) { // congregation jumpscare
    jumpscares::downloadCongregation();

    if (auto playLayer = PlayLayer::get()) {
        auto glm = GameLevelManager::get();

        if (auto congregLevel = glm->getSavedLevel(93437568)) {
            playLayer->onExit();

            auto scene = playLayer->scene(congregLevel, false, false);
            CCDirector::get()->replaceScene(scene);

            log::info("Switching to Congregation level (93437568)");
            return;
        } else {
            log::error("Failed to fetch congregation :9");
        };
    } else {
        log::error("Player not in a level");
    };
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