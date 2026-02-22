#include "../OptionMenu.hpp"

#include <menu/OptionItem.hpp>
#include <menu/OptionCategoryItem.hpp>

#include <Utils.hpp>

#include <Geode/Geode.hpp>

#include <Geode/ui/GeodeUI.hpp>
#include <Geode/ui/TextInput.hpp>

#include <Geode/utils/terminate.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

OptionMenu* OptionMenu::s_inst = nullptr;

class OptionMenu::Impl final {
public:
    SillyTier selectedTier = SillyTier::None;
    std::string selectedCategory = "";

    std::string searchText = "";

    bool showIncompatible = horribleMod->getSettingValue<bool>("show-incompatible");

    ScrollLayer* optionList = nullptr;
    ScrollLayer* categoryList = nullptr;
    TextInput* searchInput = nullptr;

    void filterOptions(std::span<const Option> optList, SillyTier tier = SillyTier::None, ZStringView category = "") {
        if (optionList) {
            optionList->m_contentLayer->removeAllChildren();

            auto useCategory = options::doesCategoryExist(category);

            for (auto const& opt : optList) {
                // tier filter
                auto tierMatches = tier == SillyTier::None || tier == opt.silly;
                // category filter
                auto categoryMatches = !useCategory || (opt.category == category);

                // search filter
                auto searchMatches = true;
                if (!searchText.empty()) {
                    auto const searchLower = str::toLower(searchText);
                    auto const nameLower = str::toLower(opt.name);

                    searchMatches = str::contains(nameLower, searchLower);
                };

                if (tierMatches && categoryMatches && searchMatches) {
                    if (auto modOption = OptionItem::create(
                        {
                            optionList->m_contentLayer->getScaledContentWidth(),
                            32.5f,
                        },
                        opt
                        )) {
                        if (modOption->isCompatible() || showIncompatible) {
                            optionList->m_contentLayer->addChild(modOption);
                        } else {
                            log::error("{} is incompatible with the current platform", modOption->getOption().id);
                            modOption->removeMeAndCleanup();
                        };
                    };
                };
            };

            optionList->m_contentLayer->updateLayout();
            optionList->scrollToTop();
        } else {
            log::error("Option list layer not found");
        };
    };
};

OptionMenu::OptionMenu() : m_impl(std::make_unique<Impl>()) {};
OptionMenu::~OptionMenu() {};

bool OptionMenu::init() {
    if (!Popup::init(450.f, 280.f)) return false;

    setID("options"_spr);
    setTitle("Horrible Options");

    auto mainLayerSize = m_mainLayer->getScaledContentSize();

    auto categoryListBg = NineSlice::create("square02_001.png");
    categoryListBg->setAnchorPoint({ 0.5, 0.5 });
    categoryListBg->setPosition({ mainLayerSize.width - 82.5f, 75.f });
    categoryListBg->setContentSize({ (mainLayerSize.width / 3.f) - 10.f, 95.f });
    categoryListBg->setScaleMultiplier(0.5f);
    categoryListBg->setOpacity(50);

    m_mainLayer->addChild(categoryListBg);

    auto layoutCategories = ColumnLayout::create()
        ->setGap(2.5f)
        ->setAxisReverse(true) // Top to bottom
        ->setAxisAlignment(AxisAlignment::End)
        ->setAutoGrowAxis(categoryListBg->getScaledContentHeight() - 8.75f);

    // scroll layer
    m_impl->categoryList = ScrollLayer::create({ categoryListBg->getScaledContentWidth() - 8.75f, categoryListBg->getScaledContentHeight() - 8.75f });
    m_impl->categoryList->setID("categories-list");
    m_impl->categoryList->setAnchorPoint({ 0.5, 0.5 });
    m_impl->categoryList->ignoreAnchorPointForPosition(false);
    m_impl->categoryList->setPosition(categoryListBg->getPosition());
    m_impl->categoryList->m_contentLayer->setLayout(layoutCategories);

    for (auto const& category : options::getAllCategories()) {
        if (auto categoryItem = OptionCategoryItem::create({ m_impl->categoryList->getScaledContentWidth(), 20.f }, category)) m_impl->categoryList->m_contentLayer->addChild(categoryItem);
    };

    m_impl->categoryList->m_contentLayer->updateLayout();
    m_impl->categoryList->scrollToTop();

    m_mainLayer->addChild(m_impl->categoryList, 1);

    // Add a background sprite to the popup
    auto optionListBg = NineSlice::create("square02_001.png");
    optionListBg->setAnchorPoint({ 0.5, 0.5 });
    optionListBg->setPosition({ (mainLayerSize.width / 2.f) - 77.5f, (mainLayerSize.height / 2.f) - 32.5f });
    optionListBg->setContentSize({ (mainLayerSize.width / 1.5f) - 20.f, mainLayerSize.height - 85.f });
    optionListBg->setOpacity(50);

    m_mainLayer->addChild(optionListBg);

    auto layoutOptions = ColumnLayout::create()
        ->setGap(5.f)
        ->setAxisReverse(true) // Top to bottom
        ->setAxisAlignment(AxisAlignment::End)
        ->setAutoGrowAxis(optionListBg->getScaledContentHeight() - 10.f);

    // scroll layer
    m_impl->optionList = ScrollLayer::create({ optionListBg->getScaledContentWidth() - 10.f, optionListBg->getScaledContentHeight() - 10.f });
    m_impl->optionList->setID("options-list");
    m_impl->optionList->setAnchorPoint({ 0.5, 0.5 });
    m_impl->optionList->ignoreAnchorPointForPosition(false);
    m_impl->optionList->setPosition(optionListBg->getPosition());
    m_impl->optionList->m_contentLayer->setLayout(layoutOptions);

    m_mainLayer->addChild(m_impl->optionList, 9);

    // add search bar
    m_impl->searchInput = TextInput::create(optionListBg->getScaledContentWidth(), "Search...", "bigFont.fnt");
    m_impl->searchInput->setID("search-input");
    m_impl->searchInput->setPosition({ optionListBg->getPositionX(), mainLayerSize.height - 52.5f });

    m_impl->searchInput->setCallback([this](std::string_view str) {
        m_impl->searchText = str;
        m_impl->filterOptions(
            options::getAll(),
            m_impl->selectedTier,
            m_impl->selectedCategory
        );  // lets search this crap
                                     });

    m_mainLayer->addChild(m_impl->searchInput);

    // add a list button background
    auto filterMenuBg = NineSlice::create("square02_001.png");
    filterMenuBg->setAnchorPoint({ 0.5, 0.5 });
    filterMenuBg->setPosition({ mainLayerSize.width - 82.5f, (mainLayerSize.height / 2.f) - 12.5f });
    filterMenuBg->setContentSize({ (mainLayerSize.width / 3.f), mainLayerSize.height - 45.f });
    filterMenuBg->setOpacity(50);

    m_mainLayer->addChild(filterMenuBg);

    auto filtersLabel = CCLabelBMFont::create("Filters", "goldFont.fnt");
    filtersLabel->setAnchorPoint({ 0.5, 0 });
    filtersLabel->setAlignment(kCCTextAlignmentCenter);
    filtersLabel->setPosition({ filterMenuBg->getPositionX(), mainLayerSize.height - 47.5f });
    filtersLabel->setScale(0.325f);

    m_mainLayer->addChild(filtersLabel);

    auto filterMenuLayout = ColumnLayout::create()
        ->setGap(2.5f)
        ->setAxisReverse(true) // Top to bottom
        ->setAxisAlignment(AxisAlignment::End)
        ->setAutoGrowAxis(0.f);

    // filter buttons :o
    auto filterMenu = CCMenu::create();
    filterMenu->setID("filter-menu");
    filterMenu->setAnchorPoint({ 0.5, 1 });
    filterMenu->setPosition({ filterMenuBg->getPositionX(), mainLayerSize.height - 55.f });
    filterMenu->setContentHeight(0.f);
    filterMenu->setLayout(filterMenuLayout);

    constexpr SillyFilterBtnData filterBtns[] = {
        { SillyTier::Low, "Low", "filter-low-btn", colors::green },
        { SillyTier::Medium, "Medium", "filter-medium-btn", colors::yellow },
        { SillyTier::High, "High", "filter-high-btn", colors::red },
    };

    for (auto const& filterBtn : filterBtns) {
        if (auto btnSprite = ButtonSprite::create(filterBtn.label, 150, true, "bigFont.fnt", "GJ_button_01.png", 0.f, 0.8f)) {
            btnSprite->m_label->setColor(filterBtn.color);
            btnSprite->setScale(0.8f);

            if (auto btn = CCMenuItemExt::createSpriteExtra(
                btnSprite,
                [this, filterBtn](auto) {
                    // Toggle: clicking same button disables filter
                    m_impl->selectedTier == filterBtn.tier ? m_impl->selectedTier = SillyTier::None : m_impl->selectedTier = filterBtn.tier;

                    m_impl->filterOptions(options::getAll(), m_impl->selectedTier, m_impl->selectedCategory);
                }
            )) {
                btn->setID(filterBtn.id);

                filterMenu->addChild(btn);
            } else {
                log::error("Failed to create filter button");
            };
        } else {
            log::error("Failed to create filter button sprite");
        };
    };

    filterMenu->updateLayout();

    m_mainLayer->addChild(filterMenu);

    // get the options data
    m_impl->filterOptions(options::getAll());

    // add a mod settings at the bottom left
    // @geode-ignore(unknown-resource)
    auto settingsBtnSprite = CircleButtonSprite::createWithSpriteFrameName("geode.loader/settings.png");
    settingsBtnSprite->setScale(0.625f);

    auto settingsBtn = CCMenuItemExt::createSpriteExtra(
        settingsBtnSprite,
        [](auto) {
            openSettingsPopup(horribleMod);
        }
    );
    settingsBtn->setID("settings-btn");

    m_buttonMenu->addChild(settingsBtn);

    auto resetFiltersBtnSprite = CCSprite::createWithSpriteFrameName("GJ_replayBtn_001.png");
    resetFiltersBtnSprite->setScale(0.375f);

    auto resetFiltersBtn = CCMenuItemExt::createSpriteExtra(
        resetFiltersBtnSprite,
        [this](auto) {
            createQuickPopup(
                "Reset Filters",
                "Would you like to <cr>reset all search filters</c>?",
                "Cancel", "OK",
                [this](bool, bool ok) {
                    if (ok) {
                        m_impl->selectedTier = SillyTier::None;
                        CategoryEvent().send("", false);
                    };
                }
            );
        }
    );
    resetFiltersBtn->setID("reset-filters-btn");
    resetFiltersBtn->setPositionX(m_mainLayer->getScaledContentWidth());

    m_buttonMenu->addChild(resetFiltersBtn);

    auto socialMenuLayout = RowLayout::create()
        ->setGap(1.25f)
        ->setAxisReverse(true)
        ->setAxisAlignment(AxisAlignment::End)
        ->setAutoGrowAxis(0.f);

    auto socialMenu = CCMenu::create();
    socialMenu->setID("social-menu");
    socialMenu->setAnchorPoint({ 1, 0.5 });
    socialMenu->setPosition({ mainLayerSize.width - 7.5f, mainLayerSize.height - 20.f });
    socialMenu->setContentWidth(0.f);
    socialMenu->setLayout(socialMenuLayout);

    constexpr SocialBtnData socialBtns[] = {
        {
            "gj_ytIcon_001.png",
            "horrible-mods-series-btn",
            [](auto) {
                createQuickPopup(
                    "Horrible Mods",
                    "Watch the series '<cr>Horrible Mods</c>' on <cl>Avalanche</c>'s YouTube channel?",
                    "Cancel", "OK",
                    [](bool, bool ok) {
                        if (ok) web::openLinkInBrowser("https://www.youtube.com/watch?v=Ssl49pNmW_0&list=PL0dsSu2pR5cERnq7gojZTKVRvUwWo2Ohu");
                    }
                );
            }
        },
        {
            "gj_discordIcon_001.png",
            "discord-btn",
            [](auto) {
                createQuickPopup(
                    "Discord",
                    "Join the <cj>Cubic Studios</c> Discord community server?",
                    "Cancel", "OK",
                    [](bool, bool ok) {
                        if (ok) web::openLinkInBrowser("https://www.dsc.gg/cubic");
                    }
                );
            }
        },
        {
            // @geode-ignore(unknown-resource)
            "geode.loader/gift.png",
            "support-btn",
            [](auto) {
                openSupportPopup(horribleMod);
            }
        }
    };

    for (auto const& socialBtn : socialBtns) {
        if (auto sprite = CCSprite::createWithSpriteFrameName(socialBtn.sprite)) {
            sprite->setScale(0.75f);

            if (auto btn = CCMenuItemExt::createSpriteExtra(
                sprite,
                socialBtn.callback
            )) {
                btn->setID(socialBtn.id);

                socialMenu->addChild(btn);
            } else {
                log::error("Failed to create social button");
            };
        } else {
            log::error("Failed to create social button sprite");
        };
    };

    socialMenu->updateLayout();

    m_mainLayer->addChild(socialMenu);

    auto safeModeLabel = CCLabelBMFont::create("Safe Mode OFF", "bigFont.fnt");
    safeModeLabel->setID("safe-mode-label");
    safeModeLabel->setColor(colors::red);
    safeModeLabel->setAlignment(kCCTextAlignmentCenter);
    safeModeLabel->setAnchorPoint({ 0.5, 0 });
    safeModeLabel->setPosition({ filterMenuBg->getPositionX(), 15.f });
    safeModeLabel->setScale(0.325f);

    // Set safemode label if active
    if (horribleMod->getSettingValue<bool>("safe-mode")) {
        safeModeLabel->setString("Safe Mode ON");
        safeModeLabel->setColor(colors::green);
    } else {
        log::warn("Safe mode is inactive");
    };

    m_mainLayer->addChild(safeModeLabel, 9);

    addEventListener(
        CategoryEvent(),
        [this](std::string_view category, bool enabled) {
            m_impl->selectedCategory = enabled ? category : "";
            m_impl->filterOptions(options::getAll(), m_impl->selectedTier, m_impl->selectedCategory);
        }
    );

    return true;
};

void OptionMenu::onClose(CCObject* sender) {
    s_inst = nullptr;
    Popup::onClose(sender);
};

void OptionMenu::onExit() {
    s_inst = nullptr;
    Popup::onExit();
};

void OptionMenu::cleanup() {
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