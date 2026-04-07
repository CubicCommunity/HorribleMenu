#define GEODE_DEFINE_EVENT_EXPORTS
#include <horrible/API.h>
#include <horrible/OptionalAPI.hpp>

#include <ranges>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace horrible;

Result<HorribleOptionSave> matjson::Serialize<HorribleOptionSave>::fromJson(matjson::Value const& value) {
    if (!value.isObject()) return Err("Expected an object");

    GEODE_UNWRAP_INTO(bool enabled, value["enabled"].asBool());
    GEODE_UNWRAP_INTO(bool pin, value["pin"].asBool());
    GEODE_UNWRAP_INTO(bool viewed, value["viewed"].asBool());

    return Ok(HorribleOptionSave{enabled, pin, viewed});
};

matjson::Value matjson::Serialize<HorribleOptionSave>::toJson(HorribleOptionSave const& value) {
    auto obj = matjson::Value();
    obj["enabled"] = value.enabled;
    obj["pin"] = value.pin;
    obj["viewed"] = value.viewed;

    return obj;
};

Option::Option(std::string id) : m_id(std::move(id)) {};

std::shared_ptr<Option> Option::setID(std::string id) {
    m_id = std::move(id);
    return shared_from_this();
};

std::shared_ptr<Option> Option::setName(std::string name) {
    m_name = std::move(name);
    return shared_from_this();
};

std::shared_ptr<Option> Option::setDescription(std::string description) {
    m_description = std::move(description);
    return shared_from_this();
};

std::shared_ptr<Option> Option::setCategory(std::string category) {
    m_category = std::move(category);
    return shared_from_this();
};

std::shared_ptr<Option> Option::setSillyTier(SillyTier tier) {
    m_silly = tier;
    return shared_from_this();
};

std::shared_ptr<Option> Option::setOnline(bool online) {
    m_online = online;
    return shared_from_this();
};

std::shared_ptr<Option> Option::setRequiresRestart(bool required) {
    m_restart = required;
    return shared_from_this();
};

std::shared_ptr<Option> Option::setSupportedPlatforms(std::vector<Platform> platforms) {
    m_platforms = std::move(platforms);
    return shared_from_this();
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

bool Option::isOnline() const noexcept {
    return m_online;
};

bool Option::isRestartRequired() const noexcept {
    return m_restart;
};

std::span<const Platform> Option::getSupportedPlatforms() const noexcept {
    return m_platforms;
};

bool Option::isEnabled() const {
    if (auto om = OptionManager::get()) return om->isEnabled(getID());
    return false;
};

bool Option::isPinned() const {
    if (auto om = OptionManager::get()) return om->isPinned(getID());
    return false;
};

void Option::enable() {
    if (auto om = OptionManager::get()) om->toggleOption(getID(), true);
};

void Option::disable() {
    if (auto om = OptionManager::get()) om->toggleOption(getID(), false);
};

std::shared_ptr<Option> Option::create(std::string id) {
    return std::make_shared<Option>(std::move(id));
};

void OptionManager::registerCategory(std::string category) {
    if (!utils::string::containsAny(category, getCategories())) m_categories.push_back(std::move(category));
};

bool OptionManager::doesOptionExist(ZStringView id) const noexcept {
    return m_options.find(id) != m_options.end();
};

void OptionManager::registerOption(std::shared_ptr<Option> option) {
    if (doesOptionExist(option->getID())) {
        log::error("Could not register option '{}' ({}) because it already exists!", option->getName(), option->getID());
    } else {
        registerCategory(option->getCategory());

        std::string id = option->getID();

        m_options.emplace(std::move(id), option);
        log::debug("Registered option {} of category {}", option->getID(), option->getCategory());
    };
};

void OptionManager::addDelegate(ZStringView id, Callback&& callback) {
    auto& thisDelegate = m_delegates[id];
    thisDelegate.push_back(std::move(callback));
};

std::vector<std::weak_ptr<Option>> OptionManager::getOptions() const noexcept {
    std::vector<std::weak_ptr<Option>> out;
    out.reserve(m_options.size());

    for (const auto& [k, v] : m_options) out.push_back(v);

    return out;
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

bool OptionManager::isViewed(std::string_view id) const {
    return getOption(id).viewed;
};

HorribleOptionSave OptionManager::getOption(std::string_view id) const {
    return Mod::get()->getSavedValue<HorribleOptionSave>(id);
};

std::weak_ptr<Option> OptionManager::getOptionInfo(ZStringView id) const noexcept {
    if (auto it = m_options.find(id); it != m_options.end()) return it->second;
    return std::weak_ptr<Option>();
};

size_t OptionManager::getDelegateCount(std::string_view id) const noexcept {
    if (auto it = m_delegates.find(id); it != m_delegates.end()) return it->second.size();
    return 0;
};

void OptionManager::toggleOption(ZStringView id, bool enable) {
    setOption(id, enable, isPinned(id));
};

void OptionManager::setOption(ZStringView id, bool enable, bool pin, bool viewed) {
    auto it = m_delegates.find(id);
    if (it != m_delegates.end()) {
        for (auto& cb : it->second) cb(enable);
    };

    log::trace("Called {} delegates {} for option {}", it != m_delegates.end() ? it->second.size() : 0, enable ? "on" : "off", id);

    auto save = HorribleOptionSave{enable, pin, viewed};

    (void)Mod::get()->setSavedValue(id, save);
    (void)OptionEvent(id).send(save);
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
                       ->setName(option.name)
                       ->setDescription(option.description)
                       ->setCategory(option.category)
                       ->setSillyTier(option.silly)
                       ->setOnline(option.online)
                       ->setRequiresRestart(option.restart)
                       ->setSupportedPlatforms(option.platforms);

        om->registerOption(opt);
    };
};

Result<bool> OptionManagerV2::isEnabled(std::string_view id) {
    if (auto om = OptionManager::get()) return Ok(om->isEnabled(id));
    return Err("Failed to get OptionManager");
};

void OptionManagerV2::toggleOption(ZStringView id, bool enable) {
    if (auto om = OptionManager::get()) om->toggleOption(id, enable);
};