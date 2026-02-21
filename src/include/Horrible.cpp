#define GEODE_DEFINE_EVENT_EXPORTS
#include <Horrible.hpp>
#include <OptionalAPI.hpp>

#include <ranges>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace horrible;

void OptionManager::registerCategory(std::string category) {
    auto cats = getCategories();
    if (!utils::string::containsAny(category, { cats.begin(), cats.end() })) m_categories.push_back(std::move(category));
};

bool OptionManager::doesOptionExist(std::string_view id) const noexcept {
    for (auto const& option : getOptions()) if (option.id == id) return true;
    return false;
};

void OptionManager::registerOption(Option option) {
    if (doesOptionExist(option.id)) {
        log::error("Could not register option '{}' ({}) because it already exists!", option.name, option.id);
    } else {
        registerCategory(option.category);
        m_options.push_back(std::move(option));

        log::debug("Registered option {} of category {}", option.id, option.category);
    };
};

void OptionManager::addDelegate(std::string_view id, Function<void(bool)>&& callback) {
    auto& thisDelegate = m_delegates[id];
    thisDelegate.push_back(std::move(callback));

    log::debug("Added delegate for option {} ({} total)", id, thisDelegate.size());
};

std::span<const Option> OptionManager::getOptions() const noexcept {
    return m_options;
};

std::span<const std::string> OptionManager::getCategories() const noexcept {
    return m_categories;
};

bool OptionManager::getOption(std::string_view id) const noexcept {
    return Mod::get()->getSavedValue<bool>(id, false);
};

bool OptionManager::setOption(std::string_view id, bool enable) {
    auto it = m_delegates.find(id);
    if (it != m_delegates.end()) {
        for (auto& cb : it->second) cb(enable);
    };

    log::debug("Called {} delegates {} for option {}", it != m_delegates.end() ? it->second.size() : 0, enable ? "on" : "off", id);

    return Mod::get()->setSavedValue(id, enable);
};

OptionManager* OptionManager::get() noexcept {
    static auto inst = new (std::nothrow) OptionManager();
    return inst;
};

Result<> OptionManagerV2::registerOption(Option const& option) {
    if (auto om = OptionManager::get()) {
        om->registerOption(option);
        return Ok();
    };

    return Err("Failed to get OptionManager");
};

void horrible::delegateHooks(std::string_view id, geode::utils::StringMap<std::shared_ptr<geode::Hook>>& hooks) {
    if (auto om = OptionManager::get()) {
        auto value = om->getOption(id);

        std::vector<geode::Hook*> allHooks;
        for (auto& hook : hooks | std::views::values) {
            hook->setAutoEnable(value);
            geode::log::debug("Set default state of '{}' hook for option {} to {}", hook->getDisplayName(), id, value ? "ON" : "OFF");
            allHooks.push_back(hook.get());
        };

        geode::log::debug("Delegating {} hooks for {}", allHooks.size(), id);

        om->addDelegate(
            id,
            [allHooks = std::move(allHooks), id](bool value) {
                for (auto hook : allHooks) {
                    (void)hook->toggle(value);
                    geode::log::debug("{} hook '{}' for {}", value ? "ENABLED" : "DISABLED", hook->getDisplayName(), id);
                };
            }
        );
    };
};

Result<bool> OptionManagerV2::getOption(std::string_view id) {
    if (auto om = OptionManager::get()) return Ok(om->getOption(id));
    return Err("Failed to get OptionManager");
};

Result<bool> OptionManagerV2::setOption(ZStringView id, bool enable) {
    if (auto om = OptionManager::get()) return Ok(om->setOption(id, enable));
    return Err("Failed to get OptionManager");
};