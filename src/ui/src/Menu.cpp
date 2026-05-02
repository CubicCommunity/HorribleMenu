#include "../Menu.h"

#include "../MenuOption.h"
#include "../MenuCredits.h"
#include "../MenuOptionCategory.h"

#include <Utils.h>

#include <asp/iter.hpp>

#include <Geode/Geode.hpp>

#include <Geode/ui/GeodeUI.hpp>

#include <Geode/utils/terminate.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

namespace fs = std::filesystem;

bool MenuNothingNode::init(CCSize const& size, CCPoint const& pos) {
    if (!CCNode::init()) return false;

    setID("nothing-label");
    setPosition(pos);
    setContentSize(size);
    setAnchorPoint({0.5, 0.5});
    setVisible(false);

    auto label = CCLabelBMFont::create("No options found :(", "bigFont.fnt");
    label->setScale(0.5f);
    label->setOpacity(250);
    label->setAnchorPoint({0.5, 0});
    label->setAlignment(kCCTextAlignmentCenter);
    label->limitLabelWidth(getScaledContentWidth() * 0.875f, label->getScale(), label->getScale());
    label->setPosition(getScaledContentSize() / 2.f);

    addChild(label, 1);

    auto labelHint = CCLabelBMFont::create("Try searching other keywords, or change some filters!", "chatFont.fnt");
    labelHint->setScale(0.625f);
    labelHint->setOpacity(200);
    labelHint->setAnchorPoint({0.5, 1});
    labelHint->setAlignment(kCCTextAlignmentCenter);
    labelHint->limitLabelWidth(getScaledContentWidth() * 0.875f, labelHint->getScale(), labelHint->getScale());
    labelHint->setPosition(getScaledContentSize() / 2.f);

    addChild(labelHint);

    return true;
};

MenuNothingNode* MenuNothingNode::create(CCSize const& size, CCPoint const& pos) {
    auto ret = new MenuNothingNode();
    if (ret->init(size, pos)) {
        ret->autorelease();
        return ret;
    };

    delete ret;
    return nullptr;
};

Menu* Menu::s_inst = nullptr;

class Menu::Impl final {
public:
    bool devMode = thisMod->getSettingValue<bool>("dev-mode");

    SillyTier selectedTier = SillyTier::None;
    std::string selectedCategory = "";

    std::string searchText = "";

    bool showIncompatible = thisMod->getSettingValue<bool>("show-incompatible");

    ScrollLayer* optionList = nullptr;
    ScrollLayer* categoryList = nullptr;
    TextInput* searchInput = nullptr;

    bool hasInternet = GameToolbox::doWeHaveInternet();
    bool hideIfOffline = thisMod->getSettingValue<bool>("hide-if-offline");

    MenuNothingNode* nothingLabel = nullptr;

    bool safeMode = thisMod->getSettingValue<bool>(setting::SafeMode);
    std::string const theme = thisMod->getSettingValue<std::string>("theme");
    fs::path themeBgPath = thisMod->getSettingValue<fs::path>("theme-background");

    CCNode* safeModeContainer = nullptr;
    LazySprite* themeBackground = nullptr;
    CCClippingNode* themeBgContainer = nullptr;

    std::vector<WeakRef<MenuOptionCategory>> categoryItems;

    void filterOptions(std::vector<std::weak_ptr<Option>>&& optList, SillyTier tier = SillyTier::None, ZStringView category = "") {
        if (optionList) {
            optionList->m_contentLayer->removeAllChildren();

            auto useCategory = !category.empty() && options::doesCategoryExist(category);
            auto searchLower = str::toLower(searchText);

            auto list = asp::iter::consume(optList)
                            .filter([this, tier, category, useCategory, search = std::move(searchLower)](std::weak_ptr<Option> const& opt) {
                                if (auto o = opt.lock()) {
                                    auto tierMatches = tier == SillyTier::None || tier == o->getSillyTier();
                                    auto categoryMatches = !useCategory || (o->getCategory() == category);

                                    auto searchMatches = true;
                                    if (!search.empty()) searchMatches = str::contains(str::toLower(o->getName()), search) || str::contains(str::toLower(o->getID()), search) || str::contains(str::toLower(o->getCategory()), search);

                                    auto onlineCompat = o->isOnline() ? hasInternet || !hideIfOffline : true;

                                    return tierMatches && categoryMatches && searchMatches && onlineCompat;
                                };

                                return false;
                            })
                            .collect();

            std::sort(list.begin(), list.end(), [](auto const& a, auto const& b) -> bool {
                if (auto optA = a.lock(), optB = b.lock(); optA && optB) return options::isPinned(optA->getID()) > options::isPinned(optB->getID());
                return false;
            });

            if (list.empty()) {
                nothingLabel->setVisible(true);
                optionList->setVisible(false);
            } else {
                nothingLabel->setVisible(false);
                optionList->setVisible(true);

                for (auto& oRef : list) {
                    if (auto o = oRef.lock()) {
                        if (auto modOption = MenuOption::create(
                                {optionList->m_contentLayer->getScaledContentWidth(), 32.5f},
                                std::move(oRef),
                                theme,
                                devMode,
                                hasInternet)) {
                            if (modOption->isCompatible() || showIncompatible) {
                                modOption->setPinCallback([this]() {
                                    filterOptions(options::getAll(), selectedTier, selectedCategory);  // re-filter to update sorting
                                });

                                optionList->m_contentLayer->addChild(modOption);
                            } else {
                                log::error("{} is incompatible with the current platform", o->getID());
                                modOption->removeFromParent();
                            };
                        };
                    };
                };
            };

            log::trace("Finished sorting {} options", list.size());

            optionList->m_contentLayer->updateLayout();
            optionList->scrollToTop();
        } else {
            log::error("Option list layer not found");
        };
    };
};

Menu::Menu() : m_impl(std::make_unique<Impl>()) {};
Menu::~Menu() {};

void Menu::setupSafeModeNode(bool safeMode) {
    if (m_impl->safeModeContainer) {
        m_impl->safeModeContainer->removeAllChildrenWithCleanup(true);

        auto safeModeIcon = CCSprite::createWithSpriteFrameName(safeMode ? "GJ_completesIcon_001.png" : "GJ_deleteIcon_001.png");
        safeModeIcon->setScale(0.375f);

        m_impl->safeModeContainer->addChild(safeModeIcon);

        auto safeModeLabel = CCLabelBMFont::create(safeMode ? "Safe Mode ON" : "Safe Mode OFF", "bigFont.fnt");
        safeModeLabel->setColor(safeMode ? colors::green : colors::red);
        safeModeLabel->setAlignment(kCCTextAlignmentCenter);
        safeModeLabel->setScale(0.25f);

        m_impl->safeModeContainer->addChild(safeModeLabel);

        m_impl->safeModeContainer->updateLayout();
    };

    if (m_impl->safeMode != safeMode) m_impl->safeMode = safeMode;
};

void Menu::setupImageBackground(fs::path path) {
    if (auto themeBg = WeakRef(m_impl->themeBackground).lock()) themeBg.take()->removeFromParent();

    if (m_impl->themeBgContainer) {
        if (fs::exists(path)) {
            m_impl->themeBackground = LazySprite::create(m_bgSprite->getScaledContentSize(), false);
            m_impl->themeBackground->setID("theme-bg");
            m_impl->themeBackground->setPosition(m_bgSprite->getScaledContentSize() / 2.f);

            m_impl->themeBackground->setLoadCallback([self = WeakRef(this), themeBg = WeakRef(m_impl->themeBackground)](Result<> res) {
                if (auto s = self.lock()) {
                    if (auto bg = themeBg.lock()) {
                        if (res.isOk()) {
                            bg->setScaleX(s->m_bgSprite->getScaledContentWidth() / bg->getScaledContentWidth());
                            bg->setScaleY(s->m_bgSprite->getScaledContentHeight() / bg->getScaledContentHeight());

                            bg.take()->setOpacity(100);

                            log::debug("Successfully loaded theme background");
                        } else if (res.isErr()) {
                            log::error("Failed to load theme background: {}", res.unwrapErr());
                        } else {
                            log::error("Failed to load theme background for an unknown reason");
                        };
                    } else {
                        log::error("Theme background sprite was destroyed before load callback");
                    };
                } else {
                    log::error("Menu was destroyed before theme background load callback");
                };
            });

            m_impl->themeBgContainer->addChild(m_impl->themeBackground);
            m_impl->themeBackground->loadFromFile(path);
        };
    };

    if (m_impl->themeBgPath != path) m_impl->themeBgPath = std::move(path);
};

bool Menu::init() {
    auto btns = themes::getCircleBaseColor(m_impl->theme);

    if (!Popup::init(450.f, 280.f, themes::getBackgroundSprite(m_impl->theme))) return false;

    setID("options"_spr);
    setTitle("Horrible Options");
    setCloseButtonSpr(CircleButtonSprite::createWithSpriteFrameName(themes::close, 0.875f, btns, CircleBaseSize::Small));

    m_bgSprite->setZOrder(-9);

    auto const mainLayerSize = m_mainLayer->getScaledContentSize();

    m_impl->themeBgContainer = CCClippingNode::create();
    m_impl->themeBgContainer->setID("bg-container");
    m_impl->themeBgContainer->setAnchorPoint({0.5, 0.5});
    m_impl->themeBgContainer->setContentSize(m_bgSprite->getScaledContentSize());
    m_impl->themeBgContainer->setPosition(m_bgSprite->getScaledContentSize() / 2.f);
    m_impl->themeBgContainer->setStencil(m_bgSprite);
    m_impl->themeBgContainer->setAlphaThreshold(0);

    m_mainLayer->addChild(m_impl->themeBgContainer, -8);

    auto border = NineSlice::create("GJ_square07.png");
    border->setContentSize(m_bgSprite->getScaledContentSize());
    border->setPosition(m_bgSprite->getScaledContentSize() / 2.f);

    m_mainLayer->addChild(border, -1);

    auto categoryListBg = NineSlice::create(themes::square);
    categoryListBg->setAnchorPoint({0.5, 0.5});
    categoryListBg->setPosition({mainLayerSize.width - 82.5f, 75.f});
    categoryListBg->setContentSize({(mainLayerSize.width / 3.f) - 10.f, 95.f});
    categoryListBg->setScaleMultiplier(0.5f);
    categoryListBg->setOpacity(50);

    m_mainLayer->addChild(categoryListBg);

    // scroll layer
    m_impl->categoryList = ScrollLayer::create(categoryListBg->getScaledContentSize() - 7.5f);
    m_impl->categoryList->setID("categories-list");
    m_impl->categoryList->setAnchorPoint({0.5, 0.5});
    m_impl->categoryList->ignoreAnchorPointForPosition(false);
    m_impl->categoryList->setPosition(categoryListBg->getPosition());

    m_impl->categoryList->m_contentLayer->setLayout(ScrollLayer::createDefaultListLayout());

    auto cats = options::getAllCategories();  // mrrp meow
    std::vector<std::string> sortedCats(cats.begin(), cats.end());

    std::sort(sortedCats.begin(), sortedCats.end(), [](auto const& a, auto const& b) {
        return str::toLower(a) < str::toLower(b);
    });

    auto misc = std::find_if(sortedCats.begin(), sortedCats.end(), [](auto const& s) {
        return s == category::misc;
    });

    if (misc != sortedCats.end()) {
        auto miscCat = *misc;

        sortedCats.erase(misc);
        sortedCats.push_back(miscCat);
    };

    for (auto const& category : sortedCats) {
        if (auto categoryItem = MenuOptionCategory::create({m_impl->categoryList->getScaledContentWidth(), 20.f}, category)) {
            categoryItem->setToggleCallback([this](std::string_view category, bool enabled) {
                if (enabled) {
                    m_impl->selectedCategory = category;

                    for (auto const& item : m_impl->categoryItems) {
                        if (auto cat = item.lock()) {
                            if (cat->getCategory() != category) cat->setToggled(false);
                        };
                    };
                } else if (m_impl->selectedCategory == category) {
                    m_impl->selectedCategory = "";
                };

                m_impl->filterOptions(options::getAll(), m_impl->selectedTier, m_impl->selectedCategory);
            });

            m_impl->categoryList->m_contentLayer->addChild(categoryItem);
            m_impl->categoryItems.push_back(categoryItem);
        };
    };

    m_impl->categoryList->m_contentLayer->updateLayout();
    m_impl->categoryList->scrollToTop();

    m_mainLayer->addChild(m_impl->categoryList, 1);

    // Add a background sprite to the popup
    auto optionListBg = NineSlice::create(themes::square);
    optionListBg->setAnchorPoint({0.5, 0.5});
    optionListBg->setPosition({(mainLayerSize.width / 2.f) - 82.5f, (mainLayerSize.height / 2.f) - 31.25f});
    optionListBg->setContentSize({(mainLayerSize.width / 1.5f) - 35.f, mainLayerSize.height - 83.25f});
    optionListBg->setOpacity(50);

    m_mainLayer->addChild(optionListBg);

    // scroll layer
    m_impl->optionList = ScrollLayer::create({optionListBg->getScaledContentWidth() - 8.75f, optionListBg->getScaledContentHeight() - 10.f});
    m_impl->optionList->setID("options-list");
    m_impl->optionList->setAnchorPoint({0.5, 0.5});
    m_impl->optionList->ignoreAnchorPointForPosition(false);
    m_impl->optionList->setPosition(optionListBg->getPosition());

    m_impl->optionList->m_contentLayer->setLayout(ScrollLayer::createDefaultListLayout(3.75f));

    auto optionListScroll = Scrollbar::create(m_impl->optionList);
    optionListScroll->setID("option-list-scrollbar");
    optionListScroll->setPosition({optionListBg->getPositionX() + (optionListBg->getScaledContentWidth() / 1.875f), optionListBg->getPositionY()});

    m_mainLayer->addChild(m_impl->optionList, 9);
    m_mainLayer->addChild(optionListScroll);

    m_impl->nothingLabel = MenuNothingNode::create(optionListBg->getScaledContentSize(), optionListBg->getPosition());
    m_mainLayer->addChild(m_impl->nothingLabel, 9);

    // add search bar
    m_impl->searchInput = TextInput::create(optionListBg->getScaledContentWidth() + 11.25f, "Search...", "bigFont.fnt");
    m_impl->searchInput->setID("search-input");
    m_impl->searchInput->setAnchorPoint({0, 0.5});
    m_impl->searchInput->setPosition({10.f, mainLayerSize.height - 51.25f});

    m_impl->searchInput->setCallback([this](std::string str) {
        m_impl->searchText = std::move(str);
        m_impl->filterOptions(
            options::getAll(),
            m_impl->selectedTier,
            m_impl->selectedCategory);  // lets search this crap
    });

    m_mainLayer->addChild(m_impl->searchInput);

    // add a list button background
    auto filterContainerBg = NineSlice::create(themes::square);
    filterContainerBg->setAnchorPoint({0.5, 0.5});
    filterContainerBg->setPosition({mainLayerSize.width - 82.5f, (mainLayerSize.height / 2.f) - 12.5f});
    filterContainerBg->setContentSize({(mainLayerSize.width / 3.f), mainLayerSize.height - 45.f});
    filterContainerBg->setOpacity(50);

    m_mainLayer->addChild(filterContainerBg);

    auto filterContainerLabel = CCLabelBMFont::create("Filters", "goldFont.fnt");
    filterContainerLabel->setID("filter-container-label");
    filterContainerLabel->setAnchorPoint({0.5, 0});
    filterContainerLabel->setAlignment(kCCTextAlignmentCenter);
    filterContainerLabel->setPosition({filterContainerBg->getPositionX(), mainLayerSize.height - 50.f});
    filterContainerLabel->setScale(0.375f);

    m_mainLayer->addChild(filterContainerLabel);

    auto filterContainerLayout = ColumnLayout::create()
                                     ->setGap(3.75f)
                                     ->setAutoScale(false)
                                     ->setAxisReverse(true)  // Top to bottom
                                     ->setAxisAlignment(AxisAlignment::End)
                                     ->setAutoGrowAxis(0.f);

    // filter buttons :o
    auto filterContainer = CCNode::create();
    filterContainer->setID("filter-container");
    filterContainer->setAnchorPoint({0.5, 1});
    filterContainer->setPosition({filterContainerBg->getPositionX(), mainLayerSize.height - 53.75f});
    filterContainer->setLayout(filterContainerLayout);

    constexpr TierFilterBtnData filterBtns[] = {
        {SillyTier::Low, "Low", "filter-low-btn", colors::green},
        {SillyTier::Medium, "Medium", "filter-medium-btn", colors::yellow},
        {SillyTier::High, "High", "filter-high-btn", colors::red},
    };

    for (auto const& filterBtn : filterBtns) {
        if (auto btnSprite = ButtonSprite::create(filterBtn.label, 115, true, "bigFont.fnt", themes::getButtonSquareSprite(m_impl->theme), 0.f, 0.8f)) {
            btnSprite->m_label->setColor(filterBtn.color);

            if (auto btn = Button::createWithNode(
                    btnSprite,
                    [this, filterBtn](auto) {
                        // Toggle: clicking same button disables filter
                        (m_impl->selectedTier == filterBtn.tier) ? m_impl->selectedTier = SillyTier::None : m_impl->selectedTier = filterBtn.tier;

                        m_impl->filterOptions(options::getAll(), m_impl->selectedTier, m_impl->selectedCategory);
                    })) {
                btn->setID(filterBtn.id);
                btn->setScale(0.8f);

                filterContainer->addChild(btn);
            } else {
                log::error("Failed to create filter button");
            };
        } else {
            log::error("Failed to create filter button sprite");
        };
    };

    m_mainLayer->addChild(filterContainer);
    filterContainer->updateLayout();

    // get all the options data
    m_impl->filterOptions(options::getAll());
    log::debug("Processed {} options", options::getAll().size());

    auto settingsBtn = Button::createWithNode(
        CircleButtonSprite::createWithSpriteFrameName(
            "geode.loader/settings.png",
            1.f,
            btns),
        [](auto) {
            openSettingsPopup(thisMod);
        });
    settingsBtn->setID("settings-btn");
    settingsBtn->setScale(0.625f);

    m_mainLayer->addChild(settingsBtn);

    auto resetFiltersBtnSpr = CircleButtonSprite::createWithSpriteFrameName(
        "geode.loader/reload.png",
        1.f,
        btns,
        CircleBaseSize::Small);
    resetFiltersBtnSpr->setTopOffset({1.f, -0.25f});

    auto resetFiltersBtn = Button::createWithNode(
        resetFiltersBtnSpr,
        [this](auto) {
            createQuickPopup(
                "Reset Filters",
                "Would you like to <cr>reset all search filters</c>?\n<cy>This will not clear your pins.</c>",
                "Cancel",
                "OK",
                [this](auto, bool ok) {
                    if (ok) {
                        m_impl->selectedTier = SillyTier::None;
                        m_impl->selectedCategory = "";

                        for (auto const& category : m_impl->categoryItems) {
                            if (auto cat = category.lock()) cat->setToggled(false);
                        };

                        m_impl->filterOptions(options::getAll(), m_impl->selectedTier, m_impl->selectedCategory);
                    };
                });
        });
    resetFiltersBtn->setID("reset-filters-btn");
    resetFiltersBtn->setScale(0.625f);
    resetFiltersBtn->setPositionX(m_mainLayer->getScaledContentWidth());

    m_mainLayer->addChild(resetFiltersBtn);

    auto socialContainerLayout = RowLayout::create()
                                     ->setGap(1.25f)
                                     ->setAutoScale(false)
                                     ->setAxisReverse(true)
                                     ->setAxisAlignment(AxisAlignment::End)
                                     ->setAutoGrowAxis(0.f);

    auto socialContainer = CCNode::create();
    socialContainer->setID("social-container");
    socialContainer->setAnchorPoint({1, 0.5});
    socialContainer->setPosition({mainLayerSize.width - 7.5f, mainLayerSize.height - 20.f});
    socialContainer->setLayout(socialContainerLayout);

    auto socialBtns = std::to_array<SocialBtnData>(
        {{"accountBtn_myLists_001.png",
             "credits-btn",
             [this](auto) {
                 if (auto popup = MenuCredits::create(m_impl->theme)) popup->show();
             },
             0.55f},
            {"gj_discordIcon_001.png",
                "discord-btn",
                [](auto) {
                    createQuickPopup(
                        "Discord",
                        "Join the <cj>Cubic Studios</c> official community Discord server?",
                        "Cancel",
                        "OK",
                        [](auto, bool ok) {
                            if (ok) web::openLinkInBrowser("https://www.dsc.gg/cubic");
                        });
                }},
            {"geode.loader/gift.png",
                "support-btn",
                [](auto) {
                    openSupportPopup(thisMod);
                }}});

    for (auto& socialBtn : socialBtns) {
        if (auto btn = Button::createWithSpriteFrameName(
                socialBtn.sprite,
                std::move(socialBtn.callback))) {
            btn->setID(socialBtn.id);
            btn->setScale(socialBtn.scale);

            socialContainer->addChild(btn);
        } else {
            log::error("Failed to create social button");
        };
    };

    m_mainLayer->addChild(socialContainer);
    socialContainer->updateLayout();

    auto safeModeContainerLayout = RowLayout::create()
                                       ->setGap(2.5f)
                                       ->setAutoScale(false)
                                       ->setAutoGrowAxis(0.f);

    m_impl->safeModeContainer = CCNode::create();
    m_impl->safeModeContainer->setID("safe-mode-container");
    m_impl->safeModeContainer->setAnchorPoint({0.5, 0});
    m_impl->safeModeContainer->setPosition({filterContainerBg->getPositionX(), 15.f});
    m_impl->safeModeContainer->setLayout(safeModeContainerLayout);

    m_mainLayer->addChild(m_impl->safeModeContainer, 9);

    setupSafeModeNode(m_impl->safeMode);
    setupImageBackground(m_impl->themeBgPath);

    addEventListener(
        SettingChangedEvent(thisMod, setting::SafeMode),
        [this](std::shared_ptr<SettingV3> setting) {
            auto settingBool = std::static_pointer_cast<BoolSettingV3>(setting);
            setupSafeModeNode(settingBool->getValue());
        });

    addEventListener(
        SettingChangedEvent(thisMod, "theme-background"),
        [this](std::shared_ptr<SettingV3> setting) {
            auto settingPath = std::static_pointer_cast<FileSettingV3>(setting);
            setupImageBackground(settingPath->getValue());
        });

    return true;
};

void Menu::onExit() {
    if (auto credits = MenuCredits::get()) credits->removeFromParent();
    s_inst = nullptr;

    Popup::onExit();
};

Menu* Menu::get() noexcept {
    return s_inst;
};

Menu* Menu::create() {
    auto ret = new Menu();
    if (ret->init()) {
        ret->autorelease();
        s_inst = ret;
        return ret;
    };

    delete ret;
    return nullptr;
};