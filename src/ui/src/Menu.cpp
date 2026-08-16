#include "../Menu.h"

#include "../MenuOptionCell.hpp"
#include "../MenuCredits.hpp"
#include "../MenuExtras.hpp"
#include "../MenuFilterCells.hpp"

#include <Util.h>

#include <asp/iter.hpp>

#include <Geode/Geode.hpp>

#include <Geode/utils/terminate.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

Menu* Menu::s_inst = nullptr;

struct Menu::Impl final {
    bool devMode = mod->getSettingValue<bool>("dev-mode");

    SillyTier selectedTier = SillyTier::None;
    std::string selectedCategory = "";

    std::string searchText = "";

    bool showIncompatible = mod->getSettingValue<bool>("show-incompatible");

    ScrollLayer* optionList = nullptr;
    ScrollLayer* categoryList = nullptr;
    TextInput* searchInput = nullptr;

    bool hasInternet = GameToolbox::doWeHaveInternet();
    bool hideIfOffline = mod->getSettingValue<bool>("hide-if-offline");

    MenuNothingNode* nothingLabel = nullptr;

    bool safeMode = OptionManager::get()->shouldBeSafeMode();
    std::string const theme = mod->getSettingValue<std::string>("theme");

    CCNode* safeModeContainer = nullptr;

    LazySprite* themeBackground = nullptr;
    CCClippingNode* themeBgContainer = nullptr;

    cue::DropdownNode* sillyFilterDropdown = nullptr;

    std::vector<WeakRef<MenuCategoryFilterCell>> categoryItems;

    void filterOptions(std::vector<std::weak_ptr<Option>>&& optList, SillyTier tier = SillyTier::None, ZStringView category = "") {
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

        auto empty = list.empty();

        nothingLabel->setVisible(empty);
        optionList->setVisible(!empty);

        if (!empty) {
            for (auto& oRef : list) {
                if (auto o = oRef.lock()) {
                    if (auto modOption = MenuOptionCell::create(
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
                            cue::resetNode(modOption);
                        };
                    };
                };
            };
        };

        log::trace("Finished sorting {} options", list.size());

        optionList->m_contentLayer->updateLayout();
        optionList->scrollToTop();
    };

    Label* createFilterLabel(ZStringView text, std::string id, CCPoint const& pos) {
        auto label = Label::create(text.c_str(), font::big);
        label->setID(std::move(id));
        label->setScale(0.375f);
        label->setAnchorPoint(anchor::center);
        label->setPosition(pos);

        return label;
    };
};

Menu::Menu() : m_impl(std::make_unique<Impl>()) {};
Menu::~Menu() {};

void Menu::setupSafeModeNode(bool safeMode) {
    if (m_impl->safeModeContainer) {
        m_impl->safeModeContainer->removeAllChildren();

        auto icon = CCSprite::createWithSpriteFrameName(safeMode ? "GJ_completesIcon_001.png" : "GJ_deleteIcon_001.png");
        icon->setScale(0.425f);

        m_impl->safeModeContainer->addChild(icon);

        auto label = Label::create(safeMode ? "Safe Mode ON" : "Safe Mode OFF", font::big);
        label->setScale(0.325f);
        label->setColor(safeMode ? colors::green : colors::red);

        m_impl->safeModeContainer->addChild(label);

        auto infoBtn = Button::createWithSpriteFrameName(
            "GJ_infoIcon_001.png",
            [safeMode](auto) {
                createQuickPopup(
                    "Safe Mode",
                    fmt::format("{}\n\nUsing this mod's features in gameplay <cr>can count as cheating</c>, be sure to <cl>keep Safe Mode enabled while using options in gameplay</c>.\n\n<cy>You can toggle Safe Mode in settings.</c>", safeMode ? "Currently <cc>enabled</c>, meaning <co>progress on levels WILL NOT save</c>!" : "Currently <cy>disabled</c>, meaning <cf>progress on levels WILL save</c>!"),
                    "OK",
                    nullptr,
                    nullptr);
            });
        infoBtn->setID("info-btn");
        infoBtn->setScale(0.375f);

        m_impl->safeModeContainer->addChild(infoBtn);

        m_impl->safeModeContainer->updateLayout();
    };

    if (m_impl->safeMode != safeMode) m_impl->safeMode = safeMode;
};

void Menu::setupImageBackground(fs::path const& path) {
    cue::resetNode(m_impl->themeBackground);

    if (fs::exists(path)) {
        if (m_impl->themeBgContainer) {
            m_impl->themeBackground = LazySprite::create(m_bgSprite->getScaledContentSize(), false);
            m_impl->themeBackground->setID("theme-bg");
            m_impl->themeBackground->setPosition(m_bgSprite->getScaledContentSize() / 2.f);

            m_impl->themeBackground->setLoadCallback([self = WeakRef(this), themeBg = WeakRef(m_impl->themeBackground)](Result<> res) {
                if (auto s = self.lock()) {
                    if (auto bg = themeBg.lock()) {
                        if (res.isOk()) {
                            cue::rescaleToMatch(bg, s->m_bgSprite, true);
                            bg->setOpacity(100);

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
};

void Menu::setupSillyFilterDropdown(CCPoint const& pos) {
    cue::resetNode(m_impl->sillyFilterDropdown);

    m_impl->sillyFilterDropdown = cue::DropdownNode::create(
        to4B(colors::black, 0),
        m_impl->categoryList->getScaledContentWidth(),
        17.5f,
        43.75f);
    m_impl->sillyFilterDropdown->setID("silly-filter-dropdown");
    m_impl->sillyFilterDropdown->setAnchorPoint({0.5, 1});
    m_impl->sillyFilterDropdown->setPosition(pos);

    m_impl->sillyFilterDropdown->setCallback([this](auto, CCNode* node) {
        if (auto cell = typeinfo_cast<MenuSillyFilterCell*>(node)) {
            if (m_impl->selectedTier != cell->getSillyTier()) m_impl->filterOptions(options::getAll(), cell->getSillyTier(), m_impl->selectedCategory);
            m_impl->selectedTier = cell->getSillyTier();
        };
    });

    constexpr TierFilterBtnData filterBtns[] = {
        {SillyTier::None, "Any", "filter-none-btn", colors::white},
        {SillyTier::Low, "Low", "filter-low-btn", colors::green},
        {SillyTier::Medium, "Medium", "filter-medium-btn", colors::yellow},
        {SillyTier::High, "High", "filter-high-btn", colors::red},
    };

    for (auto const& fBtn : filterBtns) {
        if (auto cell = MenuSillyFilterCell::create(
                {m_impl->sillyFilterDropdown->getScaledContentWidth(), 16.5f},
                fBtn.tier,
                fBtn.id,
                fBtn.label,
                fBtn.color)) {
            m_impl->sillyFilterDropdown->addCell(cell);
        } else {
            log::error("Failed to create filter button");
        };
    };

    m_mainLayer->addChild(m_impl->sillyFilterDropdown, 9);
};

bool Menu::init() {
    auto btns = themes::getCircleBaseColor(m_impl->theme);

    if (!Popup::init(450.f, 280.f, themes::getBackgroundSprite(m_impl->theme))) return false;

    setID("options"_spr);
    setTitle("Horrible Options");
    setCloseButtonSpr(themes::createThemeCircleSprite(btns));

    popup::closeBtnID(m_closeBtn);

    m_bgSprite->setZOrder(-9);

    auto const mainLayerSize = m_mainLayer->getScaledContentSize();

    m_impl->themeBgContainer = CCClippingNode::create(m_bgSprite);
    m_impl->themeBgContainer->setAnchorPoint(m_bgSprite->getAnchorPoint());
    m_impl->themeBgContainer->setContentSize(m_bgSprite->getScaledContentSize());
    m_impl->themeBgContainer->setPosition(m_bgSprite->getPosition());
    m_impl->themeBgContainer->setAlphaThreshold(0.f);

    m_mainLayer->addChild(m_impl->themeBgContainer, -8);

    auto border = cue::createBackground(
        m_bgSprite->getScaledContentSize(),
        {
            .opacity = 255,
            .texture = "GJ_square07.png",
            .id = "",
        });
    border->setPosition(m_bgSprite->getScaledContentSize() / 2.f);

    m_mainLayer->addChild(border, -1);

    // scroll layer
    m_impl->categoryList = ScrollLayer::create({(mainLayerSize.width / 3.f) - 20.f, 100.f});
    m_impl->categoryList->setID("category-list");
    m_impl->categoryList->setAnchorPoint(anchor::center);
    m_impl->categoryList->ignoreAnchorPointForPosition(false);
    m_impl->categoryList->setPosition({mainLayerSize.width - 82.5f, (mainLayerSize.height - 70.f) - (m_impl->categoryList->getScaledContentHeight() / 2.f)});

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
        sortedCats.push_back(std::move(miscCat));
    };

    for (auto const& category : sortedCats) {
        if (auto categoryItem = MenuCategoryFilterCell::create({m_impl->categoryList->getScaledContentWidth(), 20.f}, category)) {
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

    auto categoryListBg = cue::createBackground(
        {m_impl->categoryList->getScaledContentWidth() + 7.5f, m_impl->categoryList->getScaledContentHeight() + 7.5f},
        {
            .cornerRoundness = -0.875f,
            .zOrder = 1,
            .id = "",
        });
    categoryListBg->setPosition(m_impl->categoryList->getPosition());

    m_mainLayer->addChild(categoryListBg);

    m_mainLayer->addChild(m_impl->createFilterLabel("Categories", "category-list-label", {m_impl->categoryList->getPositionX(), mainLayerSize.height - 57.5f}), 1);

    m_impl->optionList = ScrollLayer::create({(mainLayerSize.width / 1.5f) - 43.75f, mainLayerSize.height - 93.25f});
    m_impl->optionList->setID("options-list");
    m_impl->optionList->setAnchorPoint(anchor::center);
    m_impl->optionList->ignoreAnchorPointForPosition(false);
    m_impl->optionList->setPosition({(mainLayerSize.width / 2.f) - 82.5f, (mainLayerSize.height / 2.f) - 31.25f});

    m_impl->optionList->m_contentLayer->setLayout(ScrollLayer::createDefaultListLayout(3.75f));

    auto optionListScroll = Scrollbar::create(m_impl->optionList);
    optionListScroll->setID("option-list-scrollbar");
    optionListScroll->setPosition({m_impl->optionList->getPositionX() + (m_impl->optionList->getScaledContentWidth() / 1.825f), m_impl->optionList->getPositionY()});

    auto optionListBg = cue::createBackground(
        {m_impl->optionList->getScaledContentWidth() + 8.75f, m_impl->optionList->getScaledContentHeight() + 10.f},
        {
            .cornerRoundness = -0.75f,
            .id = "",
        });
    optionListBg->setPosition(m_impl->optionList->getPosition());

    m_mainLayer->addChild(optionListBg);

    m_impl->nothingLabel = MenuNothingNode::create(m_impl->optionList->getScaledContentSize(), m_impl->optionList->getPosition());
    m_mainLayer->addChild(m_impl->nothingLabel, 9);

    // add search bar
    m_impl->searchInput = TextInput::create(m_impl->optionList->getScaledContentWidth() + 11.25f, "Search...", font::big);
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
    auto filterContainerBg = cue::createBackground(
        {(mainLayerSize.width / 3.f), mainLayerSize.height - 45.f},
        {
            .id = "",
        });
    filterContainerBg->setPosition({mainLayerSize.width - 82.5f, (mainLayerSize.height / 2.f) - 12.5f});

    m_mainLayer->addChild(filterContainerBg);

    auto filterContainerLabel = Label::create("Filters", font::gold);
    filterContainerLabel->setID("filter-container-label");
    filterContainerLabel->setScale(0.375f);
    filterContainerLabel->setAnchorPoint({0.5, 0});
    filterContainerLabel->setAlignment(Label::Alignment::Center);
    filterContainerLabel->setPosition({filterContainerBg->getPositionX(), mainLayerSize.height - 50.f});

    m_mainLayer->addChild(filterContainerLabel);

    CCPoint const dropdownPos = {filterContainerLabel->getPositionX(), 88.75f};
    setupSillyFilterDropdown(dropdownPos);

    // adding now to fix touch issue with cue dropdown
    m_mainLayer->addChild(m_impl->categoryList, 9);

    // same here lol
    m_mainLayer->addChild(m_impl->optionList, 9);
    m_mainLayer->addChild(optionListScroll);

    m_mainLayer->addChild(m_impl->createFilterLabel("Silliness", "silly-filter-label", {m_impl->categoryList->getPositionX(), m_impl->sillyFilterDropdown->getPositionY() + 8.75f}), 1);

    auto filterHint = SimpleTextArea::create(
        "Use different filters to search for certain options faster. Press the pin icon on an option cell to pin it to the top of the list.",
        font::chat,
        0.5f);
    filterHint->setID("filter-hint");
    filterHint->setPosition({filterContainerBg->getPositionX(), 47.5f});
    filterHint->setWidth(categoryListBg->getScaledContentWidth());
    filterHint->setAlignment(kCCTextAlignmentCenter);

    m_mainLayer->addChild(filterHint, 1);

    // get all the options data
    m_impl->filterOptions(options::getAll());
    log::debug("Processed {} options", options::getAll().size());

    auto settingsBtn = Button::createWithNode(
        CircleButtonSprite::createWithSpriteFrameName(
            "geode.loader/settings.png",
            1.f,
            btns),
        [](auto) {
            openSettingsPopup(mod);
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
        [this, dropdownPos](auto) {
            createQuickPopup(
                "Reset Filters",
                "Would you like to <cr>reset all search filters</c>?\n<cy>This will not clear your pins.</c>",
                "Cancel",
                "OK",
                [this, &dropdownPos](auto, bool ok) {
                    if (ok) {
                        m_impl->selectedTier = SillyTier::None;
                        m_impl->selectedCategory = "";

                        for (auto const& category : m_impl->categoryItems) {
                            if (auto cat = category.lock()) cat->setToggled(false);
                        };

                        m_impl->filterOptions(options::getAll(), m_impl->selectedTier, m_impl->selectedCategory);
                        setupSillyFilterDropdown(dropdownPos);
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
    socialContainer->setPosition({mainLayerSize.width - 8.75f, mainLayerSize.height - 20.f});
    socialContainer->setLayout(socialContainerLayout);

    auto socialBtns = std::to_array<SocialBtnData>(
        {
            {
                "btn_credits.png"_spr,
                "credits-btn",
                [this](auto) {
                    if (auto popup = MenuCredits::create(m_impl->theme)) popup->show();
                },
            },
            {
                "btn_ideas.png"_spr,
                "suggestions-btn",
                [this](auto) {
                    if (auto popup = MenuSuggest::create(m_impl->theme)) popup->show();
                },
            },
            {
                "gj_discordIcon_001.png",
                "discord-btn",
                [this](auto) {
                    if (auto popup = MenuDiscord::create(m_impl->theme)) popup->show();
                },
            },
            {
                "btn_kofi.png"_spr,
                "support-btn",
                [this](auto) {
                    if (auto popup = MenuKofi::create(m_impl->theme)) popup->show();
                },
            },
        });

    for (auto& socialBtn : socialBtns) {
        if (auto btn = Button::createWithSpriteFrameName(
                socialBtn.sprite,
                std::move(socialBtn.callback))) {
            btn->setID(std::move(socialBtn.id));

            cue::rescaleToMatch(btn, 23.75f);

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
    setupImageBackground(mod->getSettingValue<fs::path>("theme-background"));

    addEventListener(
        SettingChangedEvent(mod, setting::SafeMode),
        [this](std::shared_ptr<SettingV3> setting) {
            if (!mod->getSettingValue<bool>(setting::DynamicSafeMode)) {
                auto settingBool = std::static_pointer_cast<BoolSettingV3>(setting);
                setupSafeModeNode(settingBool->getValue());
            };
        });

    addEventListener(
        SettingChangedEvent(mod, setting::DynamicSafeMode),
        [this](std::shared_ptr<SettingV3>) {
            if (auto om = OptionManager::get()) setupSafeModeNode(om->shouldBeSafeMode());
        });

    addEventListener(
        SettingChangedEvent(mod, "theme-background"),
        [this](std::shared_ptr<SettingV3> setting) {
            auto settingPath = std::static_pointer_cast<FileSettingV3>(setting);
            setupImageBackground(settingPath->getValue());
        });

    addEventListener(
        OptionCheatingEvent(),
        [this](bool cheating) {
            if (mod->getSettingValue<bool>(setting::DynamicSafeMode)) setupSafeModeNode(cheating);
        });

    return true;
};

void Menu::onExit() {
    if (auto credits = MenuCredits::get()) credits->removeFromParent();
    if (auto suggest = MenuSuggest::get()) suggest->removeFromParent();
    if (auto discord = MenuDiscord::get()) discord->removeFromParent();
    if (auto kofi = MenuKofi::get()) kofi->removeFromParent();

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