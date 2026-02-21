#include "../OptionMenuPopup.hpp"

#include <menu/OptionItem.hpp>
#include <menu/OptionCategoryItem.hpp>

#include <Utils.hpp>

#include <Geode/Geode.hpp>

#include <Geode/ui/GeodeUI.hpp>
#include <Geode/ui/TextInput.hpp>

#include <Geode/utils/terminate.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

OptionMenuPopup* OptionMenuPopup::s_inst = nullptr;

class OptionMenuPopup::Impl final {
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

OptionMenuPopup::OptionMenuPopup() : m_impl(std::make_unique<Impl>()) {};
OptionMenuPopup::~OptionMenuPopup() {};

bool OptionMenuPopup::init() {
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

    // filter buttons :o
    auto filterMenu = CCMenu::create();
    filterMenu->setID("filter-menu");
    filterMenu->setAnchorPoint({ 0.5, 1 });
    filterMenu->setPosition({ filterMenuBg->getPositionX(), mainLayerSize.height - 65.f });

    constexpr SillyFilter filterBtns[] = {
        { SillyTier::Low, "Low", colors::green },
        { SillyTier::Medium, "Medium", colors::yellow },
        { SillyTier::High, "High", colors::red },
    };

    auto fBtnY = 0.f;
    for (auto const& filterBtn : filterBtns) {
        if (auto btnSprite = ButtonSprite::create(filterBtn.label, 125, true, "bigFont.fnt", "GJ_button_01.png", 0.f, 0.8f)) {
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
                btn->setPosition({ 0.f, fBtnY });

                filterMenu->addChild(btn);

                fBtnY -= 35.f;
            } else {
                log::error("Failed to create filter button");
            };
        } else {
            log::error("Failed to create filter button sprite");
        };
    };

    // get the options data
    m_impl->filterOptions(options::getAll());

    m_mainLayer->addChild(filterMenu);

    // add a mod settings at the bottom left
    auto settingsBtnSprite = CircleButtonSprite::createWithSpriteFrameName("geode.loader/settings.png");
    settingsBtnSprite->setScale(0.625f);

    auto settingsBtn = CCMenuItemSpriteExtra::create(
        settingsBtnSprite,
        this,
        menu_selector(OptionMenuPopup::openModSettings)
    );
    settingsBtn->setID("settings-btn");

    m_buttonMenu->addChild(settingsBtn);

    auto resetFiltersBtnSprite = CCSprite::createWithSpriteFrameName("GJ_replayBtn_001.png");
    resetFiltersBtnSprite->setScale(0.375f);

    auto resetFiltersBtn = CCMenuItemSpriteExtra::create(
        resetFiltersBtnSprite,
        this,
        menu_selector(OptionMenuPopup::resetFilters)
    );
    resetFiltersBtn->setID("reset-filters-btn");
    resetFiltersBtn->setPositionX(m_mainLayer->getScaledContentWidth());

    m_buttonMenu->addChild(resetFiltersBtn);

    auto seriesBtnSprite = CCSprite::createWithSpriteFrameName("gj_ytIcon_001.png");
    seriesBtnSprite->setScale(0.75f);

    auto seriesBtn = CCMenuItemSpriteExtra::create(
        seriesBtnSprite,
        this,
        menu_selector(OptionMenuPopup::openSeriesPage)
    );
    seriesBtn->setID("horrible-mods-series-btn");
    seriesBtn->setPosition(mainLayerSize - 20.f);

    m_buttonMenu->addChild(seriesBtn);

    // @geode-ignore(unknown-resource)
    auto supporterBtnSprite = CCSprite::createWithSpriteFrameName("geode.loader/gift.png");
    supporterBtnSprite->setScale(0.75f);

    auto supporterBtn = CCMenuItemSpriteExtra::create(
        supporterBtnSprite,
        this,
        menu_selector(OptionMenuPopup::openSupporterPopup)
    );
    supporterBtn->setID("support-btn");
    supporterBtn->setPosition({ mainLayerSize.width - 45.f, mainLayerSize.height - 20.f });

    m_buttonMenu->addChild(supporterBtn);

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

void OptionMenuPopup::resetFilters(CCObject*) {
    createQuickPopup(
        "Reset Filters",
        "Would you like to <cr>reset all search filters</c>?",
        "Cancel", "OK",
        [this](bool, bool ok) {
            if (ok) {
                m_impl->selectedTier = SillyTier::None;
                CategoryEvent().send("", false);
            };
        });
};

void OptionMenuPopup::openModSettings(CCObject*) {
    openSettingsPopup(horribleMod);
};

void OptionMenuPopup::openSeriesPage(CCObject*) {
    createQuickPopup(
        "Horrible Mods",
        "Watch the series '<cr>Horrible Mods</c>' on <cl>Avalanche</c>'s YouTube channel?",
        "Cancel", "OK",
        [this](bool, bool ok) {
            if (ok) web::openLinkInBrowser("https://www.youtube.com/watch?v=Ssl49pNmW_0&list=PL0dsSu2pR5cERnq7gojZTKVRvUwWo2Ohu");
        }
    );
};

void OptionMenuPopup::openSupporterPopup(CCObject*) {
    openSupportPopup(horribleMod);
};

void OptionMenuPopup::onClose(CCObject* sender) {
    s_inst = nullptr;
    Popup::onClose(sender);
};

void OptionMenuPopup::onExit() {
    s_inst = nullptr;
    Popup::onExit();
};

void OptionMenuPopup::cleanup() {
    s_inst = nullptr;
    Popup::cleanup();
};

OptionMenuPopup* OptionMenuPopup::get() noexcept {
    return s_inst;
};

OptionMenuPopup* OptionMenuPopup::create() {
    auto ret = new OptionMenuPopup();
    if (ret->init()) {
        ret->autorelease();
        s_inst = ret;
        return ret;
    };

    delete ret;
    return nullptr;
};