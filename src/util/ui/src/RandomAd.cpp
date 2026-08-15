#include "../RandomAd.hpp"

#include <Util.h>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

bool RandomAd::init() {
    auto const theme = mod->getSettingValue<std::string>("theme");

    if (!Popup::init(375.f, 250.f, themes::getBackgroundSprite(theme))) return false;

    setID("ad"_spr);
    setTitle("Sponsored");
    setCloseButtonSpr(themes::createThemeCircleSprite(themes::getCircleBaseColor(theme)));

    popup::closeBtnID(m_closeBtn);

    auto label = Label::create("Check out this cool level we found!", font::chat);
    label->setID("message");
    label->setAlignment(Label::Alignment::Center);
    label->setPosition({m_mainLayer->getScaledContentWidth() / 2.f, m_mainLayer->getScaledContentHeight() - 37.5f});
    label->setAnchorPoint(anchor::center);

    m_mainLayer->addChild(label);

    // featured project thumbnail
    auto projThumb = LazySprite::create({228.f, 128.f}, true);
    projThumb->setID("thumbnail");
    projThumb->setAutoResize(true);
    projThumb->setPosition({m_mainLayer->getScaledContentWidth() / 2.f, 110.f});

    projThumb->setLoadCallback([](Result<> res) {
        res.isOk()
            ? log::info("Ad sprite loaded successfully")
            : log::error("Ad sprite failed to load: {}", res.unwrapErr());
    });

    std::string url = "https://api.cubicstudios.xyz/breakeode/v1/fetch/random-thumbnail";
    if (Loader::get()->isModLoaded("prevter.imageplus")) url = fmt::format("{}?webp=1", url);

    projThumb->loadFromUrl(std::move(url), CCImage::kFmtUnKnown, true);
    if (projThumb) m_mainLayer->addChild(projThumb);

    auto playBtnLoading = LoadingSpinner::create(37.5f);
    playBtnLoading->setID("loading-circle");
    playBtnLoading->setVisible(false);

    // takes u to congreg lol
    auto playBtn = Button::createWithNode(
        ButtonSprite::create(
            "Play!",
            font::big,
            themes::getButtonSquareSprite(theme)),
        [this, loading = WeakRef(playBtnLoading)](Button* sender) {
            sender->setVisible(false);
            if (auto load = loading.lock()) load->setVisible(true);

            if (auto pl = PlayLayer::get()) {
                if (pl->m_level->m_levelID == HORRIBLE_JUMPSCARES_CONGREG) return removeFromParent();

                log::info("Switching from ad to Congregation jumpscare");
                jumpscares::switchLevel(HORRIBLE_JUMPSCARES_CONGREG, false, false);
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