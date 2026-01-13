#define GEODE_DEFINE_EVENT_EXPORTS
#include <Horrible.hpp>
#include <OptionalAPI.hpp>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace horrible;

HorribleOptionEvent::HorribleOptionEvent(std::string id, bool toggled) : m_id(std::move(id)), m_toggled(toggled) {};

std::string const& HorribleOptionEvent::getId() const {
    return m_id;
};

bool HorribleOptionEvent::getToggled() const {
    return m_toggled;
};

HorribleOptionEventFilter::HorribleOptionEventFilter(std::string id) : m_ids({ std::move(id) }) {};
HorribleOptionEventFilter::HorribleOptionEventFilter(std::vector<std::string> ids) : m_ids(std::move(ids)) {};

ListenerResult HorribleOptionEventFilter::handle(std::function<Callback> fn, HorribleOptionEvent* event) {
    if (m_ids.empty()) {
        return fn(event);
    } else {
        for (auto const& id : m_ids) if (event->getId() == id) return fn(event);
    };

    return ListenerResult::Propagate;
};

class OptionManager::Impl final {
public:
    std::vector<Option> m_options; // Array of registered options
    std::vector<std::string> m_categories; // Array of auto-registered categories
};

OptionManager::OptionManager() {
    m_impl = std::make_unique<Impl>();
    retain();
};

OptionManager::~OptionManager() {};

void OptionManager::registerCategory(std::string_view category) {
    if (!utils::string::containsAny(category.data(), getCategories())) m_impl->m_categories.push_back(category.data());
};

bool OptionManager::doesOptionExist(std::string_view id) const {
    for (auto const& option : getOptions()) if (option.id == id) return true;
    return false;
};

void OptionManager::registerOption(Option const& option) {
    if (doesOptionExist(option.id)) {
        log::error("Could not register option '{}' ({}) because it already exists!", option.name, option.id);
    } else {
        m_impl->m_options.push_back(option);
        registerCategory(option.category.c_str());

        log::debug("Registered option {} of category {}", option.id, option.category);
    };
};

std::vector<Option> const& OptionManager::getOptions() const {
    return m_impl->m_options;
};

std::vector<std::string> const& OptionManager::getCategories() const {
    return m_impl->m_categories;
};

bool OptionManager::getOption(std::string_view id) const {
    return Mod::get()->getSavedValue<bool>(id.data(), false);
};

bool OptionManager::setOption(std::string_view id, bool enable) const {
    auto event = new HorribleOptionEvent(id.data(), enable);
    event->postFromMod(Mod::get());

    auto eventV2 = new HorribleOptionEventV2(id.data(), enable);
    eventV2->postFromMod(Mod::get());

    return Mod::get()->setSavedValue(id.data(), enable);
};

OptionManager* OptionManager::get() {
    static auto inst = new OptionManager();
    return inst;
};

Result<> OptionManagerV2::registerOption(Option const& option) {
    if (auto om = OptionManager::get()) om->registerOption(option);
    return Ok();
};

Result<bool> OptionManagerV2::getOption(std::string_view id) {
    if (auto om = OptionManager::get()) return Ok(om->getOption(id));
    return Err("Failed to get OptionManager");
};

Result<bool> OptionManagerV2::setOption(std::string_view id, bool enable) {
    if (auto om = OptionManager::get()) return Ok(om->setOption(id, enable));
    return Err("Failed to get OptionManager");
};