#include "../OptionMenu.h"

#include "../OptionItem.h"
#include "../OptionMenuCredits.h"
#include "../OptionCategoryItem.h"

#include <Utils.h>

#include <Geode/Geode.hpp>

#include <Geode/ui/GeodeUI.hpp>

#include <Geode/utils/terminate.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

OptionMenu* OptionMenu::s_inst = nullptr;

class OptionMenu::Impl final {
public:
    bool devMode = thisMod->getSettingValue<bool>("dev-mode");

    SillyTier selectedTier = SillyTier::None;
    std::string selectedCategory = "";

    std::string searchText = "";

    bool showIncompatible = thisMod->getSettingValue<bool>("show-incompatible");

    ScrollLayer* optionList = nullptr;
    ScrollLayer* categoryList = nullptr;
    TextInput* searchInput = nullptr;

    std::string theme = thisMod->getSettingValue<std::string>("theme");

    CCNode* safeModeContainer = nullptr;

    std::vector<WeakRef<OptionCategoryItem>> categoryItems;

    void filterOptions(std::span<const Option> optList, SillyTier tier = SillyTier::None, ZStringView category = "") {
        if (optionList) {
            optionList->m_contentLayer->removeAllChildren();

            std::vector<Option> list = {optList.begin(), optList.end()};

            std::sort(list.begin(), list.end(), [this](Option const& a, Option const& b) -> bool {
                auto aFav = options::isPinned(a.getID());
                auto bFav = options::isPinned(b.getID());

                return aFav > bFav;
            });

            auto useCategory = options::doesCategoryExist(category);

            for (auto const& opt : list) {
                // tier filter
                auto tierMatches = tier == SillyTier::None || tier == opt.getSillyTier();
                // category filter
                auto categoryMatches = !useCategory || (opt.getCategory() == category);

                // search filter
                auto searchMatches = true;
                if (!searchText.empty()) {
                    auto const searchLower = str::toLower(searchText);

                    searchMatches = str::contains(str::toLower(opt.getName()), searchLower) || str::contains(str::toLower(opt.getID()), searchLower) || str::contains(str::toLower(opt.getCategory()), searchLower);
                };

                if (tierMatches && categoryMatches && searchMatches) {
                    if (auto modOption = OptionItem::create(
                            {optionList->m_contentLayer->getScaledContentWidth(), 32.5f},
                            opt,
                            devMode)) {
                        if (modOption->isCompatible() || showIncompatible) {
                            modOption->setPinCallback([this]() {
                                filterOptions(options::getAll(), selectedTier, selectedCategory);  // re-filter to update sorting
                            });

                            optionList->m_contentLayer->addChild(modOption);
                        } else {
                            log::error("{} is incompatible with the current platform", modOption->getOption().getID());
                            modOption->removeMeAndCleanup();
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

OptionMenu::OptionMenu() : m_impl(std::make_unique<Impl>()) {};
OptionMenu::~OptionMenu() {};

void OptionMenu::setupSafeModeNode(bool safeMode) {
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
};

bool OptionMenu::init() {
    auto btns = themes::getCircleBaseColor(m_impl->theme);

    if (!Popup::init(450.f, 280.f, themes::getBackgroundSprite(m_impl->theme))) return false;

    setID("options"_spr);
    setTitle("Horrible Options");
    setCloseButtonSpr(CircleButtonSprite::createWithSpriteFrameName("geode.loader/close.png", 0.875f, btns, CircleBaseSize::Small));

    auto mainLayerSize = m_mainLayer->getScaledContentSize();

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
        if (auto categoryItem = OptionCategoryItem::create({m_impl->categoryList->getScaledContentWidth(), 20.f}, category)) {
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
    optionListBg->setPosition({(mainLayerSize.width / 2.f) - 82.5f, (mainLayerSize.height / 2.f) - 30.f});
    optionListBg->setContentSize({(mainLayerSize.width / 1.5f) - 35.f, mainLayerSize.height - 82.5f});
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
        if (auto btnSprite = ButtonSprite::create(filterBtn.label, 140, true, "bigFont.fnt", themes::getButtonSquareSprite(m_impl->theme), 0.f, 0.8f)) {
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
        [this](Button*) {
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
                 if (auto popup = OptionMenuCredits::create(m_impl->theme)) popup->show();
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

    setupSafeModeNode(thisMod->getSettingValue<bool>(setting::SafeMode));

    addEventListener(
        SettingChangedEvent(thisMod, setting::SafeMode),
        [this](std::shared_ptr<SettingV3> setting) {
            auto settingBool = std::static_pointer_cast<BoolSettingV3>(setting);
            setupSafeModeNode(settingBool->getValue());
        });

    return true;
};

void OptionMenu::onClose(CCObject* sender) {
    if (auto credits = OptionMenuCredits::get()) credits->removeMeAndCleanup();
    s_inst = nullptr;

    Popup::onClose(sender);
};

void OptionMenu::onExit() {
    if (auto credits = OptionMenuCredits::get()) credits->removeMeAndCleanup();
    s_inst = nullptr;

    Popup::onExit();
};

void OptionMenu::cleanup() {
    if (auto credits = OptionMenuCredits::get()) credits->removeMeAndCleanup();
    s_inst = nullptr;

    Popup::cleanup();
};

OptionMenu* OptionMenu::get() noexcept {
    return s_inst;
};

OptionMenu* OptionMenu::create() {
    auto ret = new OptionMenu();
    if (ret->init()) {
        ret->autorelease();
        s_inst = ret;
        return ret;
    };

    delete ret;
    return nullptr;
};