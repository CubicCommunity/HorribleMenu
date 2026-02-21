#include "../SettingV3.hpp"

#include <Utils.hpp>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

Result<std::shared_ptr<SettingV3>> HorribleSettingV3::parse(std::string key, std::string modID, matjson::Value const& json) {
    auto res = std::make_shared<HorribleSettingV3>();
    auto root = checkJson(json, "HorribleSettingV3");

    res->init(std::move(key), std::move(modID), root);
    res->parseNameAndDescription(root);
    res->parseEnableIf(root);

    root.checkUnknownKeys();

    return root.ok(std::static_pointer_cast<SettingV3>(res));
};

bool HorribleSettingV3::load(matjson::Value const&) {
    return true;
};

bool HorribleSettingV3::save(matjson::Value&) const {
    return true;
};

bool HorribleSettingV3::isDefaultValue() const noexcept {
    return true;
};

void HorribleSettingV3::reset() {};

SettingNodeV3* HorribleSettingV3::createNode(float width) {
    return HorribleSettingNodeV3::create(std::static_pointer_cast<HorribleSettingV3>(shared_from_this()), width);
};

class HorribleSettingNodeV3::Impl final {
public:
    Ref<ButtonSprite> buttonSprite = nullptr;
    CCMenuItemSpriteExtra* button = nullptr;
};

HorribleSettingNodeV3::HorribleSettingNodeV3() : m_impl(std::make_unique<Impl>()) {};
HorribleSettingNodeV3::~HorribleSettingNodeV3() {};

bool HorribleSettingNodeV3::init(std::shared_ptr<HorribleSettingV3> setting, float width) {
    if (!SettingNodeV3::init(setting, width)) return false;

    m_impl->buttonSprite = ButtonSprite::create(
        "Horrible Options Menu",
        "bigFont.fnt",
        "GJ_button_01.png",
        0.875f
    );
    m_impl->buttonSprite->setScale(.5f);

    m_impl->button = CCMenuItemExt::createSpriteExtra(
        m_impl->buttonSprite,
        [](auto) {
            menu::open();
        }
    );

    if (auto menu = getButtonMenu()) {
        menu->setAnchorPoint({ 0.5, 0.5 });
        menu->setPosition(getScaledContentSize() / 2.f);
        menu->setContentSize({ getScaledContentWidth(), 0.f });

        menu->addChildAtPosition(m_impl->button, Anchor::Center);
        menu->updateLayout();
    } else {
        log::error("Couldn't find button menu in settings");
    };

    updateState(nullptr);

    return true;
};

void HorribleSettingNodeV3::updateState(CCNode* invoker) {
    SettingNodeV3::updateState(invoker);

    auto shouldEnable = getSetting()->shouldEnable();

    m_impl->button->setEnabled(shouldEnable);

    m_impl->buttonSprite->setCascadeColorEnabled(true);
    m_impl->buttonSprite->setCascadeOpacityEnabled(true);
    m_impl->buttonSprite->setOpacity(shouldEnable ? 255 : 155);
    m_impl->buttonSprite->setColor(shouldEnable ? ccWHITE : ccGRAY);
};

void HorribleSettingNodeV3::onCommit() {};
void HorribleSettingNodeV3::onResetToDefault() {};

HorribleSettingNodeV3* HorribleSettingNodeV3::create(std::shared_ptr<HorribleSettingV3> setting, float width) {
    auto ret = new HorribleSettingNodeV3();
    if (ret->init(setting, width)) {
        ret->autorelease();
        return ret;
    };

    delete ret;
    return nullptr;
};

bool HorribleSettingNodeV3::hasUncommittedChanges() const noexcept {
    return false;
};

bool HorribleSettingNodeV3::hasNonDefaultValue() const noexcept {
    return false;
};

std::shared_ptr<HorribleSettingV3> HorribleSettingNodeV3::getSetting() const noexcept {
    return std::static_pointer_cast<HorribleSettingV3>(
        SettingNodeV3::getSetting()
    );
};