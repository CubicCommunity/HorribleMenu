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

bool OptionManager::getOption(std::string_view id) const noexcept {
    return Mod::get()->getSavedValue<bool>(id, false);
};

Result<Option> OptionManager::getOptionInfo(std::string_view id) const noexcept {
    for (auto const& option : getOptions()) {
        if (option.id == id) return Ok(option);
    };

    return Err("Option not found");
};

size_t OptionManager::getDelegateCount(std::string_view id) const noexcept {
    for (auto const& [optionId, delegates] : m_delegates) {
        if (optionId == id) return delegates.size();
    };

    return 0;
};

bool OptionManager::setOption(ZStringView id, bool enable) {
    auto it = m_delegates.find(id);
    if (it != m_delegates.end()) {
        for (auto& cb : it->second) cb(enable);
    };

    log::debug("Called {} delegates {} for option {}", it != m_delegates.end() ? it->second.size() : 0, enable ? "on" : "off", id);

    OptionEventV2(id).send(enable);

    return Mod::get()->setSavedValue(id, enable);
};

OptionManager* OptionManager::get() noexcept {
    static auto inst = new (std::nothrow) OptionManager();
    return inst;
};

void horrible::delegateHooks(ZStringView id, utils::StringMap<std::shared_ptr<Hook>>& hooks) {
    log::debug("Attempting to delegate {} hooks for option {}", hooks.size(), id);

    if (auto om = OptionManager::get()) {
        auto value = om->getOption(id);

        std::vector<Hook*> allHooks;
        for (auto& hook : hooks | std::views::values) {
            hook->setAutoEnable(value);
            log::debug("Set default state of '{}' hook for option {} to {}", hook->getDisplayName(), id, value ? "ON" : "OFF");
            allHooks.push_back(hook.get());
        };

        log::debug("Delegating {} hooks for {}", allHooks.size(), id);

        om->addDelegate(
            id,
            [allHooks = std::move(allHooks), id](bool value) {
                for (auto hook : allHooks) (void)hook->toggle(value);
                log::debug("Toggled {} hooks {} for option {}", allHooks.size(), value ? "ON" : "OFF", id);
            }
        );
    } else {
        log::error("Failed to get OptionManager to delegate hooks for option {}", id);
    };
};

Result<> OptionManagerV2::registerOption(Option const& option) {
    if (auto om = OptionManager::get()) {
        om->registerOption(option);
        return Ok();
    };

    return Err("Failed to get OptionManager");
};

Result<bool> OptionManagerV2::getOption(std::string_view id) {
    if (auto om = OptionManager::get()) return Ok(om->getOption(id));
    return Err("Failed to get OptionManager");
};

Result<bool> OptionManagerV2::setOption(ZStringView id, bool enable) {
    if (auto om = OptionManager::get()) return Ok(om->setOption(id, enable));
    return Err("Failed to get OptionManager");
};