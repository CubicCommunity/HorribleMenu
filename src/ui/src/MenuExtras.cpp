#include "../MenuExtras.hpp"

#include <Util.h>

#include <gdcord/gdc.h>

#include <argon/argon.hpp>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

static constexpr auto g_suggestWait = 60;

$on_mod(Loaded) {
    if (auto ss = SupporterState::get()) ss->validateSupporter(
        [](Result<> res) {
            if (res.isErr()) return log::error("Supporter state check failed: {}", std::move(res).unwrapErr());
            log::info("User is a Ko-fi supporter!");
        });
};

MenuSuggest* MenuSuggest::s_inst = nullptr;

asp::Instant MenuSuggest::s_lastSuggest = asp::Instant();

bool MenuSuggest::init(ZStringView theme) {
    auto btns = themes::getCircleBaseColor(theme);

    if (!Popup::init({345.f, 225.f}, themes::getBackgroundSprite(theme))) return false;

    setID("suggestions"_spr);
    setTitle("Suggest a Feature!");
    setCloseButtonSpr(themes::createThemeCircleSprite(btns));

    popup::closeBtnID(m_closeBtn);

    auto const mainLayerSize = m_mainLayer->getScaledContentSize();

    m_topicInput = TextInput::create(mainLayerSize.width - 25.f, "What's your idea?");
    m_topicInput->setID("topic-input");
    m_topicInput->setMaxCharCount(48);
    m_topicInput->setPosition({mainLayerSize.width / 2.f, (mainLayerSize.height / 2.f) + 60.f});

    m_mainLayer->addChild(m_topicInput, 1);

    m_descriptionInput = TextInput::create(mainLayerSize.width - 25.f, "Describe your idea in more detail...", font::chat);
    m_descriptionInput->setID("description-input");
    m_descriptionInput->setMaxCharCount(512);
    m_descriptionInput->setCommonFilter(CommonFilter::Any);
    m_descriptionInput->setContentHeight(m_descriptionInput->getScaledContentHeight() * 1.5f);
    m_descriptionInput->setPosition({mainLayerSize.width / 2.f, (mainLayerSize.height / 2.f) + 26.5f});

    if (auto descInputNode = m_descriptionInput->getInputNode()) {
        descInputNode->setContentHeight(m_descriptionInput->getScaledContentHeight());  // bruh
        if (auto bg = m_descriptionInput->getBGSprite()) bg->setContentHeight(m_descriptionInput->getScaledContentHeight() * 2.f);
    };

    m_mainLayer->addChild(m_descriptionInput, 9);

    auto instructions = MDTextArea::create(
        "# BEFORE You Send...\n\n"
        "Here are some general guidelines regarding our etiquette and what to expect from this form!\n\n"
        "- <cg>Breakeode receives every idea you send</c>. **Refrain** from <cr>spamming them</c>!\n"
        "- Provide <cy>as much detail as you can</c> about your idea to give developers **a clear idea** and therefore <cg>credit you</c>!\n"
        "- **Refrain** from <cy>suggesting existing features from prominent Geode mods</c>, <cr>we want originality</c>!\n"
        "- If your idea <cr>didn't make it to the following feature update</c>, it was **probably omitted**.\n"
        "- You're welcome to **join [Breakeode's Discord server](https://dsc.gg/breakeode)** to <cg>personally bring and check up on your ideas</c>, just <cy>be polite</c> is all!\n"
        "- <cy>Attempting to overload Cubic's servers</c> will likely get you <cr>rate-limited or IP-banned</c>.",
        {mainLayerSize.width - 25.f, 92.5f});
    instructions->setID("instructions");
    instructions->setAnchorPoint({0.5, 0});

    m_mainLayer->addChildAtPosition(instructions, Anchor::Bottom, {0.f, 12.5f});

    auto submitBtn = Button::createWithNode(
        ButtonSprite::create(
            "Submit",
            font::gold,
            themes::getButtonSquareSprite(theme),
            0.875f),
        [this](Button* sender) {
            auto elapsed = asp::Instant::now().durationSince(s_lastSuggest).seconds();
            if (elapsed < g_suggestWait) {
                createQuickPopup(
                    "Slow Down!",
                    fmt::format("We appreciate your enthusiasm, but you must <co>wait {} seconds before sending your next suggestion</c>.", g_suggestWait - elapsed),
                    "OK",
                    nullptr,
                    nullptr);

                return;
            };

            processSuggestion(sender);
        });
    submitBtn->setID("submit-idea-btn");
    submitBtn->setScale(0.75f);

    m_mainLayer->addChildAtPosition(submitBtn, Anchor::Bottom);

    m_loading = LoadingSpinner::create(37.5f);
    m_loading->setVisible(false);
    m_loading->setPosition(submitBtn->getPosition());

    m_mainLayer->addChild(m_loading, 9);

    popup::addHelpButton(
        m_mainLayer,
        "This is the <cg>feature suggestion form</c>. You can use it to <cy>send Breakeode your feature ideas for Horrible Menu</c>. Be sure to <cr>read the guidelines below before submitting any ideas</c>.");

    return true;
};

void MenuSuggest::processSuggestion(Button* sender) {
    if (m_loading) m_loading->setVisible(true);
    sender->setVisible((false));

    async::spawn(
        argon::startAuth(),
        [self = WeakRef(this), btn = WeakRef(sender)](Result<std::string> res) {
            auto const toggleBack = [btn](Ref<MenuSuggest>& s) {
                if (auto b = btn.lock()) b->setVisible(true);
                if (s->m_loading) s->m_loading->setVisible(false);
            };

            if (res.isErr()) {
                Notification::create("Log in to send suggestions!", NotificationIcon::Warning)->show();
                if (auto s = self.lock()) toggleBack(s);

                return;
            };

            if (auto s = self.lock()) {
                if (s->m_topicInput && s->m_descriptionInput) {
                    auto const fallback = [&s, &toggleBack](ZStringView err) {
                        log::error("Suggestion request failed: {}", err);
                        Notification::create(err, NotificationIcon::Error)->show();
                        toggleBack(s);
                    };

                    auto topic = str::trim(s->m_topicInput->getString());
                    if (topic.empty()) return fallback("Topic field cannot be empty");
                    if (topic.size() > 48) return fallback("Topic text exceeds 48 characters");

                    auto desc = str::trim(s->m_descriptionInput->getString());
                    if (desc.empty()) return fallback("Description field cannot be empty");
                    if (desc.size() > 512) return fallback("Description text exceeds 512 characters");

                    auto reqJson = json::Value();
                    reqJson["topic"] = topic;
                    reqJson["description"] = desc;

                    auto const acc = argon::getGameAccountData();

                    reqJson["account_id"] = acc.accountId;
                    reqJson["user_id"] = acc.userId;
                    reqJson["username"] = acc.username;
                    reqJson["authtoken"] = std::move(res).unwrap();

                    reqJson["v"] = mod->getVersion().toVString();

                    auto req = web::WebRequest()
                                   .bodyJSON(reqJson);

                    async::spawn(
                        req.post("https://api.cubicstudios.xyz/breakeode/v1/horrible/suggest"),
                        [self, topic, toggleBack](web::WebResponse res) {
                            auto const fallback = [&self, &toggleBack](ZStringView err) {
                                log::error("Suggestion request failed: {}", err);
                                Notification::create(err, NotificationIcon::Error)->show();
                                if (auto s = self.lock()) toggleBack(s);
                            };

                            if (res.error()) return fallback(fmt::format("Request failed ({}: {})", res.code(), res.errorMessage()));

                            s_lastSuggest = asp::Instant::now();

                            Notification::create(fmt::format("Sent '{}'!", topic), NotificationIcon::Success)->show();
                            if (auto s = self.lock()) toggleBack(s);
                        });
                };
            };
        });
};

void MenuSuggest::onExit() {
    s_inst = nullptr;
    Popup::onExit();
};

MenuSuggest* MenuSuggest::get() noexcept {
    return s_inst;
};

MenuSuggest* MenuSuggest::create(ZStringView theme) {
    auto ret = new MenuSuggest();
    if (ret->init(theme)) {
        ret->autorelease();
        s_inst = ret;
        return ret;
    };

    delete ret;
    return nullptr;
};

void SupporterState::validateSupporter(Callback&& cb) {
    if (!gdc::isLinked()) {
        m_supporter = false;
        return cb(Err("Player is signed out or not linked with Discord"));
    };

    if (auto gjam = GJAccountManager::sharedState()) {
        log::trace("Checking Ko-fi supporter status...");

        auto req = web::WebRequest()
                       .param("id", gjam->m_accountID);

        async::spawn(
            req.get("https://api.cubicstudios.xyz/breakeode/v1/discord/supporter"),
            [this, cb = std::move(cb)](web::WebResponse res) {
                if (res.ok()) log::info("User is a supporter of Breakeode");
                m_supporter = res.ok();

                return cb(Ok());
            });
    };
};

bool SupporterState::isSupporter() const noexcept {
    return m_supporter;
};

MenuDiscord* MenuDiscord::s_inst = nullptr;

void MenuDiscord::setupAuthInterface() {
    cue::resetNode(m_discordCell);
    cue::resetNode(m_linkLabel);
    cue::resetNode(m_linkBtn);
    cue::resetNode(m_loading);
    cue::resetNode(m_label);

    std::string labelTxt;

    auto discordRes = gdc::getDiscordLink();
    if (discordRes.isOk()) {
        auto const discord = std::move(discordRes).unwrap();

        labelTxt = "Thanks for playing with <co>Horrible Menu</c>!";

        m_discordCell = MenuDiscordCell::create(discord);
        m_discordCell->setScale(0.75f);
        m_discordCell->setAnchorPoint({0, 0});

        m_mainLayer->addChildAtPosition(m_discordCell, Anchor::BottomLeft, {10.f, 10.f});

        m_linkLabel = Label::create("Playing as...", font::chat);
        m_linkLabel->setScale(0.75f);
        m_linkLabel->setAnchorPoint({0, 0});
        m_linkLabel->setPosition({m_discordCell->getPositionX(), m_discordCell->getPositionY() + m_discordCell->getScaledContentHeight() + 5.f});

        m_mainLayer->addChild(m_linkLabel, 1);
    } else {
        log::error("{}", std::move(discordRes).unwrapErr());

        labelTxt = "Your <cb>Discord</c> account is <cr>not yet linked</c>.";

        auto const hideBtns = [this]() {
            m_linkBtn->setVisible(false);

            m_loading = LoadingSpinner::create(25.f);
            m_loading->setPosition(m_linkBtn->getPosition());

            m_mainLayer->addChild(m_loading, 9);
        };

        m_linkBtn = Button::createWithNode(
            ButtonSprite::create(
                "Link Account",
                font::gold,
                themes::getButtonSquareSprite(mod->getSettingValue<std::string>("theme")),
                0.875f),
            [this, hideBtns](auto) {
                hideBtns();

                gdc::startLink([self = WeakRef(this)](Result<gdc::DiscordLink> res) {
                    if (res.isErr()) return log::error("{}", std::move(res).unwrapErr());

                    auto discord = std::move(res).unwrap();

                    log::info("Successfully authorized as {}", std::move(discord).username);

                    if (auto s = self.lock()) s->setupAuthInterface();
                });
            });
        m_linkBtn->setID("link-discord-account-btn");
        m_linkBtn->setScale(0.75f);
        m_linkBtn->setPosition({75.f, 25.f});

        m_mainLayer->addChild(m_linkBtn, 9);

        if (gdc::isLinkOngoing() || !gdc::isLinked()) hideBtns();
    };

    m_label = LabelArea::create(std::move(labelTxt), m_mainLayer->getScaledContentWidth() * 0.4f, 0.5f);
    m_label->setAnchorPoint({1, 0});

    m_mainLayer->addChildAtPosition(m_label, Anchor::BottomRight, {-15.f, 15.f});
};

bool MenuDiscord::init(ZStringView theme) {
    auto btns = themes::getCircleBaseColor(theme);

    if (!Popup::init({300.f, 190.f}, themes::getBackgroundSprite(theme))) return false;

    setID("discord"_spr);
    setTitle("Discord Community");
    setCloseButtonSpr(themes::createThemeCircleSprite(btns));

    popup::closeBtnID(m_closeBtn);

    setupAuthInterface();

    if (!gdc::isLinked()) gdc::getLink([self = WeakRef(this)](auto) {
        if (auto s = self.lock()) s->setupAuthInterface();
    });

    auto cubicLabel = LabelArea::create("Want to join <cg>other gamers and hang out</c>?", m_mainLayer->getScaledContentWidth() * 0.875f, 0.75f);
    cubicLabel->setID("cubic-studios-discord-label");

    auto breakeodeLabel = LabelArea::create("Need <cy>help or want to suggest ideas</c>?", m_mainLayer->getScaledContentWidth() * 0.875f, 0.75f);
    breakeodeLabel->setID("breakeode-discord-label");

    m_mainLayer->addChildAtPosition(cubicLabel, Anchor::Center, {0.f, 50.f});
    m_mainLayer->addChildAtPosition(breakeodeLabel, Anchor::Center, {0.f, 0.f});

    auto cubicBtn = Button::createWithNode(
        ButtonSprite::create(
            "Join Cubic Studios",
            font::big,
            themes::getButtonSquareSprite(theme),
            0.875f),
        [](auto) {
            createQuickPopup(
                "Discord",
                "Join the <cf>Cubic Studios</c> <cj>official community Discord server</c>?",
                "Cancel",
                "OK",
                [](auto, bool ok) {
                    if (ok) web::openLinkInBrowser("https://www.dsc.gg/cubic");
                });
        });
    cubicBtn->setID("cubic-studios-discord-btn");
    cubicBtn->setScale(0.625f);
    cubicBtn->setPosition({cubicLabel->getPositionX(), cubicLabel->getPositionY() - (cubicBtn->getScaledContentHeight() + 2.f)});

    m_mainLayer->addChild(cubicBtn, 1);

    auto breakeodeBtn = Button::createWithNode(
        ButtonSprite::create(
            "Join Breakeode",
            font::gold,
            themes::getButtonSquareSprite(theme),
            0.875f),
        [](auto) {
            createQuickPopup(
                "Discord",
                "Join <cc>Breakeode</c>'s <cj>support Discord server</c>?",
                "Cancel",
                "OK",
                [](auto, bool ok) {
                    if (ok) web::openLinkInBrowser("https://www.dsc.gg/breakeode");
                });
        });
    breakeodeBtn->setID("breakeode-discord-btn");
    breakeodeBtn->setScale(0.625f);
    breakeodeBtn->setPosition({breakeodeLabel->getPositionX(), breakeodeLabel->getPositionY() - (breakeodeBtn->getScaledContentHeight() + 2.5f)});

    m_mainLayer->addChild(breakeodeBtn, 1);

    popup::addHelpButton(
        m_mainLayer,
        "This is the <cb>Discord community menu</c>. You can join <cf>Cubic Studios</c>'s Discord community server to chat with others, or <cc>Breakeode</c>'s Discord server to get help with using <co>Horrible Menu</c> or suggest ideas.\n\n"
        "You can also <cy>link your Discord account with your Geometry Dash account</c> here, which is <cr>required</c> in order to receive any <cd>Ko-fi support perks</c>.");

    return true;
};

MenuDiscord* MenuDiscord::get() noexcept {
    return s_inst;
};

MenuDiscord* MenuDiscord::create(ZStringView theme) {
    auto ret = new MenuDiscord();
    if (ret->init(theme)) {
        ret->autorelease();
        s_inst = ret;
        return ret;
    };

    delete ret;
    return nullptr;
};

std::string MenuDiscordCell::normalizeAvatarURL(std::string url) const {
    if (!mods::isImagePlus() && str::endsWith(url, "webp")) {
        str::replaceIP(url, ".webp", ".jpg");
    } else {
        url = fmt::format("{}?animated=true", url);  // nitro users lol, shouldnt affect static pfps
    };

    return url;
};

bool MenuDiscordCell::init(gdc::DiscordLink const& profile) {
    if (!CCNode::init()) return false;

    setContentSize({45.f, 40.f});

    auto square = cue::createBackground(
        {40.f, 40.f},
        {
            .cornerRoundness = 1.25f,
            .id = "",
        });

    addChildAtPosition(square, Anchor::Left, {20.f, 0.f});

    auto iconContainer = CCClippingNode::create(square);
    iconContainer->setAnchorPoint(anchor::center);
    iconContainer->setPosition(square->getPosition());
    iconContainer->setContentSize(square->getScaledContentSize());
    iconContainer->setAlphaThreshold(0.f);

    addChild(iconContainer, 1);

    auto icon = LazySprite::create({40.f, 40.f});
    icon->setID("profile-icon");
    icon->setAutoResize(true);
    icon->setAnchorPoint(anchor::center);
    icon->setLoadCallback([icon](Result<> res) {
        if (res.isErr()) return log::error("Failed to load Discord profile icon: {}", std::move(res).unwrapErr());
        cue::rescaleToMatch(icon, 40.f);
    });

    iconContainer->addChildAtPosition(icon, Anchor::Center);

    icon->loadFromUrl(normalizeAvatarURL(profile.avatar));

    auto label = Button::createWithLabel(
        fmt::format("@{}", profile.username),
        font::big,
        [name = profile.username](auto) {
            clipboard::write(name);
            Notification::create("Copied to clipboard", NotificationIcon::Success)->show();
        });
    label->setID("copy-username-btn");
    label->setScale(0.625f);

    cue::rescaleToMatch(label, 147.5f);

    label->setPosition({(label->getScaledContentWidth() * 0.5f) + icon->getScaledContentWidth() + 5.f, getScaledContentHeight() / 2.f});

    addChild(label);

    return true;
};

MenuDiscordCell* MenuDiscordCell::create(gdc::DiscordLink const& profile) {
    auto ret = new MenuDiscordCell();
    if (ret->init(profile)) {
        ret->autorelease();
        return ret;
    };

    delete ret;
    return nullptr;
};

MenuKofi* MenuKofi::s_inst = nullptr;

bool MenuKofi::init(ZStringView theme) {
    auto btns = themes::getCircleBaseColor(theme);

    if (!Popup::init({375.f, 220.f}, themes::getBackgroundSprite(theme))) return false;

    setID("kofi"_spr);
    setTitle("Support Breakeode");
    setCloseButtonSpr(themes::createThemeCircleSprite(btns));

    popup::closeBtnID(m_closeBtn);

    m_bgSprite->setZOrder(-9);

    auto bgClip = CCClippingNode::create(m_bgSprite);
    bgClip->setZOrder(-2);
    bgClip->setAnchorPoint(anchor::center);
    bgClip->setContentSize(m_mainLayer->getScaledContentSize());
    bgClip->setAlphaThreshold(0.f);

    m_mainLayer->addChildAtPosition(bgClip, Anchor::Center);

    auto bg = cue::RepeatingBackground::create("game_bg_11_001.png", 0.75f, cue::RepeatMode::X, m_mainLayer->getScaledContentSize());
    bg->setColor({48, 53, 86});
    bg->setSpeed(0.5f);

    bgClip->addChild(bg, -1);

    auto bgGrnd = cue::RepeatingBackground::create("groundSquare_04_001.png", 0.5f, cue::RepeatMode::X, m_mainLayer->getScaledContentSize());
    bgGrnd->setColor({74, 82, 137});
    bgGrnd->setSpeed(12.5f);

    bgClip->addChild(bgGrnd);

    auto border = cue::createBackground(
        m_bgSprite->getScaledContentSize(),
        {
            .opacity = 255,
            .texture = "GJ_square07.png",
            .id = "",
        });
    border->setPosition(m_bgSprite->getScaledContentSize() / 2.f);

    m_mainLayer->addChild(border, -1);

    auto as = SupporterState::get();

    std::string infoLabelTxt = as->isSupporter()
                                   ? "<cg>Thanks for supporting Breakeode</c>! You can now <cj>press the badge below</c> to see the <cd>Supporter badge</c> on your profile. Feel free to <cb>join Breakeode's Discord server</c> for even more perks."
                                   : "<co>Horrible Menu</c> <cc>couldn't be made possible without community support</c>. Feel free to <cd>donate through Ko-fi</c> and get cool perks such as the badge below!\n<cj>Press the badge</c> to get started.";

    auto infoContainer = LabelArea::create(std::move(infoLabelTxt), m_mainLayer->getScaledContentWidth() - 15.f, 0.625f, as->isSupporter() ? colors::gold : colors::purple);
    infoContainer->setID("kofi-description");

    m_mainLayer->addChildAtPosition(infoContainer, Anchor::Center, {0.f, 12.5f + infoContainer->getScaledContentHeight()});

    auto supportBtn = Button::createWithSpriteFrameName(
        "badge_supporter.png"_spr,
        [](auto) {
            if (auto as = SupporterState::get()) {
                if (as->isSupporter()) {
                    ProfilePage::create(argon::getGameAccountData().accountId, false)->show();
                } else {
                    createQuickPopup(
                        "Ko-fi",
                        "Visit <cc>Breakeode</c>'s <cd>Ko-fi page</c>?",
                        "Cancel",
                        "OK",
                        [](auto, bool ok) {
                            if (ok) web::openLinkInBrowser("https://ko-fi.com/breakeode");
                        });
                };
            };
        });
    supportBtn->setID("support-us-btn");
    supportBtn->setScale(0.f);

    m_mainLayer->addChildAtPosition(supportBtn, Anchor::Center, {0.f, -17.5f});

    gdc::getLink([self = WeakRef(this), theme = std::string{theme}](Result<gdc::DiscordLink> res) {
        if (auto s = self.lock()) {
            if (res.isErr()) {
                auto linkBtn = Button::createWithNode(
                    ButtonSprite::create(
                        "Link Account",
                        font::gold,
                        themes::getButtonSquareSprite(theme),
                        0.875f),
                    [theme](auto) {
                        if (auto popup = MenuDiscord::create(theme)) popup->show();
                    });
                linkBtn->setID("account-link-btn");
                linkBtn->setScale(0.875f);

                s->m_mainLayer->addChildAtPosition(linkBtn, Anchor::Bottom);
            };

            std::string linkLabelTxt = res.isOk()
                                           ? fmt::format("Discord account <cj>@{}</c> <cg>authorized & linked</c>!", std::move(res).unwrap().username)
                                           : "Discord account <cr>not linked</c>.\nThis is <cy>required to receive supporter perks</c>!";

            auto linkLabel = Label::createRich(std::move(linkLabelTxt), font::chat);
            linkLabel->setScale(0.75f);
            linkLabel->setAnchorPoint({0.5, 1});
            linkLabel->setAlignment(Label::Alignment::Center);

            s->m_mainLayer->addChildAtPosition(linkLabel, Anchor::Bottom, {0.f, 47.5f});
        };
    });

    supportBtn->runAction(
        CCEaseExponentialInOut::create(
            CCScaleTo::create(0.875f, 1.75f)));

    auto popupBtn = Button::createWithSpriteFrameName(
        "geode.loader/gift.png",
        [](auto) {
            openSupportPopup(mod);
        });
    popupBtn->setID("support-popup-btn");
    popupBtn->setScale(0.75f);

    m_mainLayer->addChildAtPosition(popupBtn, Anchor::BottomRight, {-17.5f, 17.5f});

    auto discordBtn = Button::createWithSpriteFrameName(
        "gj_discordIcon_001.png",
        [](auto) {
            createQuickPopup(
                "Discord",
                "Join <cc>Breakeode</c>'s <cj>support Discord server</c>?",
                "Cancel",
                "OK",
                [](auto, bool ok) {
                    if (ok) web::openLinkInBrowser("https://www.dsc.gg/breakeode");
                });
        });
    discordBtn->setID("discord-btn");
    discordBtn->setScale(0.75f);

    m_mainLayer->addChildAtPosition(discordBtn, Anchor::BottomLeft, {17.5f, 17.5f});

    popup::addHelpButton(
        m_mainLayer,
        "This is the <cg>Ko-fi supporter menu</c>. Here you can find <cd>links to support Breakeode, and the perks for doing so</c>!");

    return true;
};

void MenuKofi::onExit() {
    s_inst = nullptr;
    Popup::onExit();
};

MenuKofi* MenuKofi::get() noexcept {
    return s_inst;
};

MenuKofi* MenuKofi::create(ZStringView theme) {
    auto ret = new MenuKofi();
    if (ret->init(theme)) {
        ret->autorelease();
        s_inst = ret;
        return ret;
    };

    delete ret;
    return nullptr;
};