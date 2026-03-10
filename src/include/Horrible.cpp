#define GEODE_DEFINE_EVENT_EXPORTS
#include <Horrible.hpp>
#include <OptionalAPI.hpp>

#include <ranges>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace horrible;

Result<HorribleOptionSave> matjson::Serialize<HorribleOptionSave>::fromJson(matjson::Value const& value) {
    GEODE_UNWRAP_INTO(bool enabled, value["enabled"].asBool());
    GEODE_UNWRAP_INTO(bool pin, value["pin"].asBool());

    return Ok(HorribleOptionSave{enabled, pin});
};

matjson::Value matjson::Serialize<HorribleOptionSave>::toJson(HorribleOptionSave const& value) {
    auto obj = matjson::Value();
    obj["enabled"] = value.enabled;
    obj["pin"] = value.pin;

    return obj;
};

void OptionManager::registerCategory(std::string category) {
    if (!utils::string::containsAny(category, getCategories())) m_categories.push_back(std::move(category));
};

bool OptionManager::doesOptionExist(std::string_view id) const noexcept {
    for (auto const& option : getOptions())
        if (option.id == id) return true;
    return false;
};

void OptionManager::registerOption(Option option) {
    if (doesOptionExist(option.id)) {
        log::error("Could not register option '{}' ({}) because it already exists!", option.name, option.id);
    } else {
        registerCategory(option.category);

        log::debug("Registered option {} of category {}", option.id, option.category);
        m_options.push_back(std::move(option));
    };
};

void OptionManager::addDelegate(ZStringView id, Function<void(bool)>&& callback) {
    auto& thisDelegate = m_delegates[id];
    thisDelegate.push_back(std::move(callback));
};

std::span<const Option> OptionManager::getOptions() const noexcept {
    return m_options;
};

std::span<const std::string> OptionManager::getCategories() const noexcept {
    return m_categories;
};

bool OptionManager::isEnabled(std::string_view id) const {
    return getOption(id).enabled;
};

bool OptionManager::isPinned(std::string_view id) const {
    return getOption(id).pin;
};

HorribleOptionSave OptionManager::getOption(std::string_view id) const {
    return Mod::get()->getSavedValue<HorribleOptionSave>(id);
};

Result<Option> OptionManager::getOptionInfo(std::string_view id) const noexcept {
    for (auto const& option : getOptions()) {
        if (option.id == id) return Ok(option);
    };

    return Err("Option not found");
};

size_t OptionManager::getDelegateCount(std::string_view id) const noexcept {
    for (auto const& [optionID, delegates] : m_delegates) {
        if (optionID == id) return delegates.size();
    };

    return 0;
};

void OptionManager::setOption(ZStringView id, bool enable, bool pin) {
    auto it = m_delegates.find(id);
    if (it != m_delegates.end()) {
        for (auto& cb : it->second)
            cb(enable);
    };

    log::trace("Called {} delegates {} for option {}", it != m_delegates.end() ? it->second.size() : 0, enable ? "on" : "off", id);

    auto save = HorribleOptionSave{enable, pin};

    (void)Mod::get()->setSavedValue(id, save);
    (void)OptionEventV2(id).send(std::move(save));
};

OptionManager* OptionManager::get() noexcept {
    static auto inst = new (std::nothrow) OptionManager();
    return inst;
};

void horrible::delegateHooks(ZStringView id, utils::StringMap<std::shared_ptr<Hook>>& hooks) {
    if (auto om = OptionManager::get()) {
        auto value = om->isEnabled(id);

        std::vector<Hook*> allHooks;
        for (auto& hook : hooks | std::views::values) {
            hook->setAutoEnable(value);
            log::trace("Set default state of '{}' hook for option {} to {}", hook->getDisplayName(), id, value ? "ON" : "OFF");
            allHooks.push_back(hook.get());
        };

        om->addDelegate(
            id,
            [allHooks = std::move(allHooks)](bool value) {
                for (auto hook : allHooks)
                    (void)hook->toggle(value);
            });
    } else {
        log::error("Failed to get OptionManager to delegate hooks for option {}", id);
    };
};

Result<> OptionManagerV2::registerOption(Option option) {
    if (auto om = OptionManager::get()) {
        om->registerOption(std::move(option));
        return Ok();
    };

    return Err("Failed to get OptionManager");
};

Result<bool> OptionManagerV2::isEnabled(std::string_view id) {
    if (auto om = OptionManager::get()) return Ok(om->isEnabled(id));
    return Err("Failed to get OptionManager");
};

Result<> OptionManagerV2::setOption(ZStringView id, bool enable, bool pin) {
    if (auto om = OptionManager::get()) {
        om->setOption(id, enable, pin);
        return Ok();
    };

    return Err("Failed to get OptionManager");
};