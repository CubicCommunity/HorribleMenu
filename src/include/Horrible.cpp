#define GEODE_DEFINE_EVENT_EXPORTS
#include <Horrible.h>
#include <OptionalAPI.hpp>

#include <ranges>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace horrible;

Result<HorribleOptionSave> matjson::Serialize<HorribleOptionSave>::fromJson(matjson::Value const& value) {
    GEODE_UNWRAP_INTO(bool enabled, value["enabled"].asBool());
    GEODE_UNWRAP_INTO(bool pin, value["pin"].asBool());
    GEODE_UNWRAP_INTO(unsigned int chance, value["chance"].asUInt());
    GEODE_UNWRAP_INTO(int64_t min, value["min"].asInt());
    GEODE_UNWRAP_INTO(int64_t max, value["max"].asInt());

    return Ok(HorribleOptionSave{enabled, pin, chance, min, max});
};

matjson::Value matjson::Serialize<HorribleOptionSave>::toJson(HorribleOptionSave const& value) {
    auto obj = matjson::Value();
    obj["enabled"] = value.enabled;
    obj["pin"] = value.pin;
    obj["chance"] = value.chance;
    obj["min"] = value.min;
    obj["max"] = value.max;

    return obj;
};

Option& Option::setID(std::string id) {
    m_id = std::move(id);
    return *this;
};

Option& Option::setName(std::string name) {
    m_name = std::move(name);
    return *this;
};

Option& Option::setDescription(std::string description) {
    m_description = std::move(description);
    return *this;
};

Option& Option::setCategory(std::string category) {
    m_category = std::move(category);
    return *this;
};

Option& Option::setSillyTier(SillyTier tier) {
    m_silly = tier;
    return *this;
};

Option& Option::setRequiresRestart(bool required) {
    m_restart = required;
    return *this;
};

Option& Option::setSupportedPlatforms(std::vector<Platform> platforms) {
    m_platforms = std::move(platforms);
    return *this;
};

ZStringView Option::getID() const noexcept {
    return m_id;
};

ZStringView Option::getName() const noexcept {
    return m_name;
};

ZStringView Option::getDescription() const noexcept {
    return m_description;
};

ZStringView Option::getCategory() const noexcept {
    return m_category;
};

SillyTier Option::getSillyTier() const noexcept {
    return m_silly;
};

bool Option::isRestartRequired() const noexcept {
    return m_restart;
};

std::span<const Platform> Option::getSupportedPlatforms() const noexcept {
    return m_platforms;
};

Option Option::create(std::string id) {
    return Option().setID(std::move(id));
};

void OptionManager::registerCategory(std::string category) {
    if (!utils::string::containsAny(category, getCategories())) m_categories.push_back(std::move(category));
};

bool OptionManager::doesOptionExist(std::string_view id) const noexcept {
    for (auto const& option : getOptions()) {
        if (option.getID() == id) return true;
    };

    return false;
};

void OptionManager::registerOption(Option option) {
    if (doesOptionExist(option.getID())) {
        log::error("Could not register option '{}' ({}) because it already exists!", option.getName(), option.getID());
    } else {
        registerCategory(option.getCategory());

        log::debug("Registered option {} of category {}", option.getID(), option.getCategory());
        m_options.push_back(std::move(option));
    };
};

void OptionManager::addDelegate(ZStringView id, Callback&& callback) {
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
        if (option.getID() == id) return Ok(option);
    };

    return Err("Option not found");
};

size_t OptionManager::getDelegateCount(std::string_view id) const noexcept {
    for (auto const& [optionID, delegates] : m_delegates) {
        if (optionID == id) return delegates.size();
    };

    return 0;
};

void OptionManager::toggleOption(ZStringView id, bool enable) {
    setOption(id, enable, isPinned(id));
};

void OptionManager::setOption(ZStringView id, bool enable, bool pin) {
    auto it = m_delegates.find(id);
    if (it != m_delegates.end()) {
        for (auto& cb : it->second) cb(enable);
    };

    log::trace("Called {} delegates {} for option {}", it != m_delegates.end() ? it->second.size() : 0, enable ? "on" : "off", id);

    auto save = HorribleOptionSave{enable, pin};

    (void)Mod::get()->setSavedValue(id, save);
    (void)OptionEvent(id).send(std::move(save));
};

OptionManager* OptionManager::get() noexcept {
    static auto inst = new (std::nothrow) OptionManager();
    return inst;
};

void horrible::delegateHooks(ZStringView id, utils::StringMap<std::shared_ptr<Hook>> const& hooks) {
    if (auto om = OptionManager::get()) {
        auto value = om->isEnabled(id);

        std::vector<std::weak_ptr<Hook>> allHooks;
        for (auto const& hook : hooks | std::views::values) {
            hook->setAutoEnable(value);
            log::trace("Set default state of '{}' hook for option {} to {}", hook->getDisplayName(), id, value ? "ON" : "OFF");
            allHooks.push_back(hook);
        };

        om->addDelegate(
            id,
            [id, allHooks = std::move(allHooks)](bool value) {
                for (auto const& hook : allHooks) {
                    if (auto const h = hook.lock()) {
                        log::trace("Toggling {} hook '{}' {}...", id, h->getDisplayName(), value ? "ON" : "OFF");
                        (void)h->toggle(value);
                    };
                };
            });
    } else {
        log::error("Failed to get OptionManager to delegate hooks for option {}", id);
    };
};

void OptionManagerV2::registerOption(OptionV2 const& option) {
    if (auto om = OptionManager::get()) {
        auto opt = Option::create(option.id)
                       .setName(option.name)
                       .setDescription(option.description)
                       .setCategory(option.category)
                       .setSillyTier(option.silly)
                       .setRequiresRestart(option.restart)
                       .setSupportedPlatforms(option.platforms);

        om->registerOption(std::move(opt));
    };
};

Result<bool> OptionManagerV2::isEnabled(std::string_view id) {
    if (auto om = OptionManager::get()) return Ok(om->isEnabled(id));
    return Err("Failed to get OptionManager");
};

void OptionManagerV2::toggleOption(ZStringView id, bool enable) {
    if (auto om = OptionManager::get()) om->toggleOption(id, enable);
};