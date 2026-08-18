#include "../MenuExtras.hpp"

#include <Util.h>

#include <argon/argon.hpp>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

$on_game(Loaded) {
    if (auto as = AuthState::get()) as->startAuth([](Result<> res) { if (res.isErr()) log::error("Argon authorization failed: {}", std::move(res).unwrapErr()); });
};

static constexpr auto g_suggestWait = 60;

MenuSuggest* MenuSuggest::s_inst = nullptr;

asp::Instant MenuSuggest::s_lastSuggest = asp::Instant();

Result<DiscordLink> json::Serialize<DiscordLink>::fromJson(json::Value const& value) {
    if (!value.isObject()) return Err("Expected an object");

    GEODE_UNWRAP_INTO(std::string id, value["id"].asString());
    GEODE_UNWRAP_INTO(std::string username, value["username"].asString());
    GEODE_UNWRAP_INTO(std::string avatar, value["avatar"].asString());

    return Ok(DiscordLink{
        std::move(id),
        std::move(username),
        std::move(avatar),
    });
};

json::Value json::Serialize<DiscordLink>::toJson(DiscordLink const& value) {
    auto obj = json::Value();

    obj["id"] = value.id;
    obj["username"] = value.username;
    obj["avatar"] = value.avatar;

    return obj;
};

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
        "Here are some general guidelines regarding our etiquette and what to expect from this feature!\n\n"
        "- <cg>Breakeode receives every idea you send</c>. **Refrain** from <cr>spamming them</c>!\n"
        "- Provide <cy>as much detail as you can</c> about your idea to give developers **a clear idea** and therefore <cg>credit you</c>!\n"
        "- **Refrain** from <cy>suggesting existing features from prominent Geode mods</c>, <cr>we want originality</c>!\n"
        "- If your idea <cr>didn't make it to the following feature update</c>, it was **probably omitted**.\n"
        "- You're welcome to **join [Breakeode's Discord server](https://dsc.gg/breakeode)** to <cg>personally bring and check up on your ideas</c>, just <cy>be polite</c> is all!\n"
        "- <cy>Attempting to spam this form or overload our servers</c> will likely get you <cr>rate-limited or IP-banned</c>.",
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

    auto infoBtn = Button::createWithSpriteFrameName(
        "GJ_infoIcon_001.png",
        [](auto) {
            createQuickPopup(
                "Help",
                "This is the <cg>idea suggestion form</c>. You can use it to <cy>send Breakeode your feature ideas for Horrible Menu</c>. Be sure to <cr>read the guidelines before submitting any ideas</c>.",
                "OK",
                nullptr,
                nullptr);
        });
    infoBtn->setID("info-btn");
    infoBtn->setScale(0.75f);

    m_mainLayer->addChildAtPosition(infoBtn, Anchor::TopRight, {-13.75f, -13.75f});

    return true;
};

void MenuSuggest::processSuggestion(Button* sender) {
    if (m_loading) m_loading->setVisible(true);
    sender->setVisible((false));

    if (auto as = AuthState::get()) {
        as->startAuth([as, self = WeakRef(this), btn = WeakRef(sender)](Result<> res) {
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

                    reqJson["account_id"] = as->getAccountID();
                    reqJson["user_id"] = as->getUserID();
                    reqJson["username"] = as->getUsername();
                    reqJson["authtoken"] = as->getToken();

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
    } else {
        sender->setVisible((true));
        if (m_loading) m_loading->setVisible(false);
    };
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

void AuthState::setAuthInfo(int accountID, int userID, std::string username, std::string token) {
    m_accountID = accountID;
    m_userID = userID;
    m_username = std::move(username);
    m_token = std::move(token);

    m_authorized = !m_token.empty();
};

void AuthState::setDiscordLinkInfo(DiscordLink discord) {
    m_discord = std::move(discord);

    m_discordLinked = !m_discord.id.empty();

    if (auto gjam = GJAccountManager::sharedState()) {
        log::trace("Checking Ko-fi supporter status...");

        auto req = web::WebRequest()
                       .param("id", gjam->m_accountID);

        async::spawn(
            req.get("https://api.cubicstudios.xyz/breakeode/v1/discord/supporter"),
            [this](web::WebResponse res) {
                if (res.ok()) log::info("User is a supporter of Breakeode");
                m_supporter = res.ok();
            });
    };
};

bool AuthState::isAuthorized() const noexcept {
    return m_authorized;
};

bool AuthState::isAuthValid() const {
    return isAuthorized() && (getAccountID() == argon::getGameAccountData().accountId);
};

int AuthState::getAccountID() const noexcept {
    return m_accountID;
};

int AuthState::getUserID() const noexcept {
    return m_userID;
};

ZStringView AuthState::getUsername() const noexcept {
    return m_username;
};

ZStringView AuthState::getToken() const noexcept {
    return m_token;
};

Result<DiscordLink> AuthState::getDiscord() const {
    if (!m_discordLinked) return Err("Discord account not linked");
    return Ok(m_discord);
};

bool AuthState::isSupporter() const noexcept {
    return m_supporter;
};

void AuthState::startAuth(CopyableFunction<void(Result<>)>&& callback) {
    if (!argon::signedIn()) return callback(Err("Player is logged out"));

    if (isAuthValid()) return callback(Ok());

    async::spawn(
        argon::startAuth(),
        [this, cb = std::move(callback)](Result<std::string> res) {
            if (res.isErr()) return cb(res.asErr());

            auto const acc = argon::getGameAccountData();

            setAuthInfo(acc.accountId, acc.userId, acc.username, std::move(res).unwrap());
            log::info("Authorized {} ({}) with Argon", acc.username, acc.accountId);

            return cb(Ok());
        });

    if (auto gjam = GJAccountManager::sharedState()) {
        auto req = web::WebRequest()
                       .param("id", gjam->m_accountID);

        async::spawn(
            req.get("https://api.cubicstudios.xyz/breakeode/v1/discord"),
            [this](web::WebResponse res) {
                auto const fallback = [](std::string_view err = "") {
                    log::error("Discord link web request failed ({})", err);
                };

                if (res.error()) return fallback(res.errorMessage());

                auto jsonRes = res.json();
                if (jsonRes.isErr()) return fallback(std::move(jsonRes).unwrapErr());

                auto json = std::move(jsonRes).unwrap();

                auto discordRes = json.as<DiscordLink>();
                if (discordRes.isErr()) return fallback(std::move(discordRes).unwrapErr());

                setDiscordLinkInfo(std::move(discordRes).unwrap());

                auto discordLinkRes = getDiscord();
                if (discordLinkRes.isErr()) return fallback(std::move(discordLinkRes).unwrapErr());

                log::info("Authorized as Discord user {}", std::move(discordLinkRes).unwrap().username);
            });
    };
};

MenuDiscord* MenuDiscord::s_inst = nullptr;

bool MenuDiscord::init(ZStringView theme) {
    auto btns = themes::getCircleBaseColor(theme);

    if (!Popup::init({330.f, 225.f}, themes::getBackgroundSprite(theme))) return false;

    setID("discord"_spr);
    setTitle("Discord Community");
    setCloseButtonSpr(themes::createThemeCircleSprite(btns));

    popup::closeBtnID(m_closeBtn);

    if (auto as = AuthState::get()) {
        auto discordRes = as->getDiscord();
        if (discordRes.isOk()) {  // testing, will replace with actual ui later..!
            auto const discord = std::move(discordRes).unwrap();

            auto label = Label::createRich(fmt::format("Authorized as <cg>@{}</c>", discord.username), font::chat);
            m_mainLayer->addChildAtPosition(label, Anchor::Center);
        } else {
            log::error("{}", std::move(discordRes).unwrapErr());

            m_loading = LoadingSpinner::create(12.5f);
            m_mainLayer->addChildAtPosition(m_loading, Anchor::Bottom);

            m_state = utils::random::generateUUID();
            m_since = asp::Instant::now();
            web::openLinkInBrowser(fmt::format("https://api.cubicstudios.xyz/breakeode/v1/discord/link/auth?state={}", m_state));

            scheduleOnce(schedule_selector(MenuDiscord::checkDiscordStatus), 1.25f);
        };
    };

    auto infoBtn = Button::createWithSpriteFrameName(
        "GJ_infoIcon_001.png",
        [](auto) {
            createQuickPopup(
                "Help",
                "This is the <cg>Discord community menu</c>. You join <cf>Cubic Studios</c>'s Discord community server to chat with others, or <cc>Breakeode</c>'s Discord server to get help with using <cg>Horrible Menu</c> or suggest ideas.\n\n"
                "You can also <cy>link your Discord account with your Geometry Dash account</c> in this menu, which is required to receive any <cd>Ko-fi support perks</c>.",
                "OK",
                nullptr,
                nullptr);
        });
    infoBtn->setID("info-btn");
    infoBtn->setScale(0.75f);

    m_mainLayer->addChildAtPosition(infoBtn, Anchor::TopRight, {-13.75f, -13.75f});

    return true;
};

void MenuDiscord::checkDiscordStatus(float) {
    if (auto as = AuthState::get()) {
        if (!as->isAuthValid()) return unschedule(schedule_selector(MenuDiscord::checkDiscordStatus));

        if (asp::Instant::now().durationSince(m_since).seconds() > 15) {
            Notification::create("Authorization flow timed out", NotificationIcon::Error)->show();
            return unschedule(schedule_selector(MenuDiscord::checkDiscordStatus));
        };

        auto reqJson = json::Value();
        reqJson["account_id"] = as->getAccountID();
        reqJson["user_id"] = as->getUserID();
        reqJson["username"] = as->getUsername();
        reqJson["authtoken"] = as->getToken();
        reqJson["state"] = m_state;

        auto req = web::WebRequest()
                       .bodyJSON(reqJson);

        log::debug("Checking endpoint for Discord link status...");

        m_listener.spawn(
            req.post("https://api.cubicstudios.xyz/breakeode/v1/discord/link/check"),
            [self = WeakRef(this)](web::WebResponse res) {
                if (auto s = self.lock()) {
                    auto const fallback = [&s](std::string_view err = "") {
                        log::error("Discord link web request failed ({}), trying again in 1.25s", err);
                        s->scheduleOnce(schedule_selector(MenuDiscord::checkDiscordStatus), 1.25f);
                    };

                    if (res.error()) return fallback(res.errorMessage());

                    auto jsonRes = res.json();
                    if (jsonRes.isErr()) return fallback(std::move(jsonRes).unwrapErr());

                    auto json = std::move(jsonRes).unwrap();

                    auto discordRes = json.as<DiscordLink>();
                    if (discordRes.isErr()) return fallback(std::move(discordRes).unwrapErr());

                    auto discord = std::move(discordRes).unwrap();

                    log::info("Successfully authorized as {}", discord.username);

                    auto label = Label::createRich(fmt::format("Authorized as <cg>@{}</c>", discord.username), font::chat);
                    s->m_mainLayer->addChildAtPosition(label, Anchor::Center);

                    if (auto as = AuthState::get()) as->setDiscordLinkInfo(std::move(discord));

                    s->unschedule(schedule_selector(MenuDiscord::checkDiscordStatus));
                    cue::resetNode(s->m_loading);

                    s->m_listener.cancel();
                    s->m_since = asp::Instant();
                    s->m_state.clear();
                };
            });
    } else {
        unschedule(schedule_selector(MenuDiscord::checkDiscordStatus));
    };
};

void MenuDiscord::onExit() {
    s_inst = nullptr;

    if (m_listener.isPending()) log::trace("Cancelling Discord link tasks");
    unschedule(schedule_selector(MenuDiscord::checkDiscordStatus));
    m_listener.cancel();

    Popup::onExit();
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

MenuKofi* MenuKofi::s_inst = nullptr;

bool MenuKofi::init(ZStringView theme) {
    auto btns = themes::getCircleBaseColor(theme);

    if (!Popup::init({365.f, 220.f}, themes::getBackgroundSprite(theme))) return false;

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
    bg->setSpeed(0.75f);

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

    auto infoContainer = NineSlice::create(themes::square);
    infoContainer->setContentSize({m_mainLayer->getScaledContentWidth() - 25.f, 37.5f});
    infoContainer->setColor(colors::black);

    auto as = AuthState::get();

    std::string infoLabelTxt = as->isSupporter()
                                   ? "<cg>Thanks for supporting Breakeode</c>! You can now <cj>press the badge below</c> to see the <cd>Supporter badge</c> on your profile. Feel free to <cb>join Breakeode's Discord server</c> for even more perks."
                                   : "Mods like <co>Horrible Menu</c> <cc>wouldn't be possible without community support</c>. <cd>Donate to Breakeode on Ko-fi</c> and unlock cool perks such as that cool badge below!\n<cj>Press the badge</c> to get started.";

    auto infoLabel = Label::createRich(std::move(infoLabelTxt), font::chat);
    infoLabel->setScale(0.625f);
    infoLabel->setAlignment(Label::Alignment::Center);
    infoLabel->setMaxWidth((infoContainer->getScaledContentWidth() - 7.5f) / infoLabel->getScale());

    infoContainer->setContentHeight(infoLabel->getScaledContentHeight() + 5.f);

    infoContainer->addChildAtPosition(infoLabel, Anchor::Center);

    m_mainLayer->addChildAtPosition(infoContainer, Anchor::Center, {0.f, 12.5f + infoContainer->getScaledContentHeight()});

    auto supportBtn = Button::createWithSpriteFrameName(
        "badge_supporter.png"_spr,
        [](auto) {
            if (auto as = AuthState::get()) {
                if (as->isSupporter()) {
                    ProfilePage::create(as->getAccountID(), false)->show();
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

    auto discordRes = as->getDiscord();

    if (discordRes.isErr()) {
        auto linkBtn = Button::createWithNode(
            ButtonSprite::create(
                "Link Account",
                font::gold,
                themes::getButtonSquareSprite(theme),
                0.875f),
            [theme](auto) {
                MenuDiscord::create(theme)->show();
            });
        linkBtn->setID("account-link-btn");
        linkBtn->setScale(0.875f);

        m_mainLayer->addChildAtPosition(linkBtn, Anchor::Bottom);
    };

    std::string linkLabelTxt = discordRes.isOk()
                                   ? fmt::format("Discord account <cj>@{}</c> <cg>authorized & linked</c>!", std::move(discordRes).unwrap().username)
                                   : "Discord account <cr>not linked</c>, this is <cy>required to receive supporter perks</c>!";

    auto linkLabel = Label::createRich(std::move(linkLabelTxt), font::chat);
    linkLabel->setScale(0.75f);
    linkLabel->setAlignment(Label::Alignment::Center);

    m_mainLayer->addChildAtPosition(linkLabel, Anchor::Bottom, {0.f, 37.5f});

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

    auto infoBtn = Button::createWithSpriteFrameName(
        "GJ_infoIcon_001.png",
        [](auto) {
            createQuickPopup(
                "Help",
                "This is the <cg>Ko-fi supporter menu</c>. Here you can find <cd>links to support Breakeode, and the perks for doing so</c>!",
                "OK",
                nullptr,
                nullptr);
        });
    infoBtn->setID("info-btn");
    infoBtn->setScale(0.75f);

    m_mainLayer->addChildAtPosition(infoBtn, Anchor::TopRight, {-13.75f, -13.75f});

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