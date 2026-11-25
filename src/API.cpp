#define GEODE_DEFINE_EVENT_EXPORTS
#include <horrible/API.h>
#include <horrible/OptionalAPI.hpp>

#include <Util.h>

#include <rhatt/prelude.h>

// #include <gdcord/gdc.h>

#include <ranges>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace cs::brkd::horrible;

Result<HorribleOptionSave> matjson::Serialize<HorribleOptionSave>::fromJson(Value const& value) {
    HorribleOptionSave out;

    GEODE_UNWRAP_INTO(out.enabled, value["enabled"].asBool());
    GEODE_UNWRAP_INTO(out.pin, value["pin"].asBool());
    GEODE_UNWRAP_INTO(out.viewed, value["viewed"].asBool());

    return Ok(out);
}

matjson::Value matjson::Serialize<HorribleOptionSave>::toJson(HorribleOptionSave const& value) {
    Value out;
    out["enabled"] = value.enabled;
    out["pin"] = value.pin;
    out["viewed"] = value.viewed;

    return out;
}

struct Option::Impl final {
    asp::BoxedString const id;                                // Unique ID of the option
    std::string name{ "Example Option" };                     // Name of the option
    std::string description;                                  // Description of the option
    std::string category{ "Uncategorized" };                  // Name of the category this option should be under
    SillyTier silly = SillyTier::None;                        // How silly the option is
    bool defaultTo = false;                                   // Default toggle state for this option
    bool online = false;                                      // If the option requires an active internet connection to work properly
    bool restart = false;                                     // If the option requires a game restart to take effect
    std::unordered_set<Platform> platforms{ Platform::All };  // Platforms that the option supports
    bool isCheating = false;                                  // If the option counts as cheating and will trigger dynamic safe mode
    const Mod* const integration = nullptr;                   // External mod that registered this option

    uint64_t idHash = 0;  // FNV-1a-hashed unique ID of the option

    Impl(std::string id, const Mod* integration) : id(std::move(id)), integration(integration) { idHash = (fnv1aHash(id)); }
};

Option::Option(std::string id, const Mod* integration) : m_impl(std::make_unique<Impl>(std::move(id), integration)) {}
Option::~Option() = default;

Option::SharedSelf Option::setName(std::string name) {
    m_impl->name = std::move(name);
    return shared_from_this();
}

Option::SharedSelf Option::setDescription(std::string description) {
    m_impl->description = std::move(description);
    return shared_from_this();
}

Option::SharedSelf Option::setCategory(std::string category) {
    m_impl->category = std::move(category);
    return shared_from_this();
}

Option::SharedSelf Option::setSillyTier(SillyTier tier) {
    m_impl->silly = tier;
    return shared_from_this();
}

Option::SharedSelf Option::setDefaultToggleState(bool state) {
    m_impl->defaultTo = state;
    return shared_from_this();
}

Option::SharedSelf Option::setOnline(bool online) {
    m_impl->online = online;
    return shared_from_this();
}

Option::SharedSelf Option::setRequiresRestart(bool required) {
    m_impl->restart = required;
    return shared_from_this();
}

Option::SharedSelf Option::setSupportedPlatforms(std::unordered_set<Platform> platforms) {
    m_impl->platforms = std::move(platforms);
    return shared_from_this();
}

Option::SharedSelf Option::setCheating(bool isCheat) {
    m_impl->isCheating = isCheat;
    return shared_from_this();
}

ZStringView Option::getID() const noexcept {
    return m_impl->id.c_str();
}

ZStringView Option::getName() const noexcept {
    return m_impl->name;
}

ZStringView Option::getDescription() const noexcept {
    return m_impl->description;
}

ZStringView Option::getCategory() const noexcept {
    return m_impl->category;
}

SillyTier Option::getSillyTier() const noexcept {
    return m_impl->silly;
}

bool Option::getDefaultToggleState() const noexcept {
    return m_impl->defaultTo;
}

bool Option::isOnline() const noexcept {
    return m_impl->online;
}

bool Option::isRestartRequired() const noexcept {
    return m_impl->restart;
}

std::unordered_set<Platform> const& Option::getSupportedPlatforms() const noexcept {
    return m_impl->platforms;
}

bool Option::isCheating() const noexcept {
    return m_impl->isCheating;
}

const Mod* Option::getIntegration() const noexcept {
    return m_impl->integration;
}

asp::BoxedString Option::getIDShared() const noexcept {
    return m_impl->id;
}

uint64_t Option::getIDHash() const noexcept {
    return m_impl->idHash;
}

struct OptionManager::Impl final {
    StringMap<SharedOption> options;  // Map of registered options

    StringMap<std::vector<SharedOption>> categoryMap;  // Map of options bound to category name
    StringMap<const Mod* const> integrations;          // Map of auto-registered external mods using this API

    HashedMapU64<asp::BoxedString> optHashes;  // Map of FNV-1a-hashed option IDs to their string IDs
    HashedMapU64<OptionSave> saveCache;        // Map of cached states

    StringMap<std::vector<Callback>> delegates;  // Map of option ID to array of delegates to call when that option is toggled
    std::unordered_set<uint64_t> enabledCheats;  // Map of currently enabled cheat options, used for dynamic safe mode

    bool isAnticheat = false;                 // If a mod enabled anti-cheat mode
    std::unordered_set<uint64_t> prevCheats;  // Options disable when anti-cheat was enabled
};

OptionManager::OptionManager() : m_impl(std::make_unique<Impl>()) {}
OptionManager::~OptionManager() = default;

bool Option::isEnabled() const& {
    if (auto om = OptionManager::get()) return om->isEnabled(getID());
    return false;
}

bool Option::isPinned() const& {
    if (auto om = OptionManager::get()) return om->isPinned(getID());
    return false;
}

void Option::enable() & {
    if (auto om = OptionManager::get()) om->toggleOption(getID(), true);
}

void Option::disable() & {
    if (auto om = OptionManager::get()) om->toggleOption(getID(), false);
}

Option::SharedSelf Option::autoRegister() {
    if (auto om = OptionManager::get()) (void)om->registerOption(shared_from_this());
    return shared_from_this();
}

Option::SharedSelf Option::create(std::string id, const Mod* integration) {
    return std::make_shared<Option>(std::move(id), integration);
}

void OptionManager::registerCategory(std::string category, SharedOption option) {
    auto it = m_impl->categoryMap.find(category);

    if (it != m_impl->categoryMap.end()) {
        auto& list = m_impl->categoryMap[category];
        list.push_back(option);
    } else {
        m_impl->categoryMap[category] = {};
    };
}

void OptionManager::registerMod(const Mod* mod) {
    if (!isModRegistered(mod->getID())) {
        log::trace("Attempting to register integrated mod {}...", mod->getID());
        if (mod->getID() == GEODE_MOD_ID) return;

        std::string id = mod->getID();
        m_impl->integrations.emplace(std::move(id), mod);
    };
}

bool OptionManager::doesOptionExist(std::string_view id) const noexcept {
    return m_impl->options.contains(id);
}

bool OptionManager::isModRegistered(std::string_view id) const noexcept {
    return m_impl->integrations.contains(id);
}

Result<> OptionManager::registerOption(SharedOption option) {
    if (doesOptionExist(option->getID())) {
        return Err("Could not register option '{}' ({}) because it already exists!", option->getName(), option->getID());
    } else {
        registerCategory(option->getCategory(), option);
        registerMod(option->getIntegration());

        std::string id = option->getID();

        auto cheats = m_impl->enabledCheats.size();

        if (option->isCheating() && isEnabled(id)) m_impl->enabledCheats.emplace(option->getIDHash());

        m_impl->options.emplace(std::move(id), option);
        log::debug("Registered option {} of category {}", option->getID(), option->getCategory());

        m_impl->optHashes.emplace(option->getIDHash(), option->getIDShared());

        if (cheats == 0 && m_impl->enabledCheats.size() > cheats) (void)OptionCheatingEvent().send(true);
    };

    return Ok();
}

void OptionManager::addDelegate(std::string id, Callback&& callback) {
    auto& thisDelegate = m_impl->delegates[std::move(id)];
    thisDelegate.push_back(std::move(callback));
}

void OptionManager::addDelegate(uint64_t id, Callback&& callback) {
    auto strIDRes = getOptionIDForHash(id);
    if (strIDRes.isErr()) return;

    auto const strID = std::move(strIDRes).unwrap();
    addDelegate(strID.c_str(), std::move(callback));
}

bool OptionManager::isCheatEnabled() const noexcept {
    return m_impl->enabledCheats.size() > 0;
}

StringMap<SharedOption> const& OptionManager::getAllOptions() const noexcept {
    return m_impl->options;
}

std::vector<SharedOption> OptionManager::getEnabledCheats() const {
    std::vector<SharedOption> out;
    out.reserve(m_impl->enabledCheats.size());

    for (auto const& id : m_impl->enabledCheats) out.push_back(getOptionInfo(id).lock());

    return out;
}

Result<std::vector<SharedOption>> OptionManager::getAllOptionsForCategory(std::string_view category) const noexcept {
    if (auto it = m_impl->categoryMap.find(category); it != m_impl->categoryMap.end()) return Ok(it->second);
    return Err("Category '{}' not found", category);
}

StringMap<std::vector<SharedOption>> const& OptionManager::getAllCategories() const noexcept {
    return m_impl->categoryMap;
}

std::vector<const Mod*> OptionManager::getMods() const {
    std::vector<const Mod*> out;
    out.reserve(m_impl->integrations.size());

    for (auto const& [k, v] : m_impl->integrations) out.push_back(v);

    return out;
}

void OptionManager::toggleAnticheat(bool enable) {
    m_impl->isAnticheat = enable;

    if (enable) m_impl->prevCheats = m_impl->enabledCheats;
    for (auto const& cheat : m_impl->prevCheats) toggleOption(cheat, !enable);
    if (!enable) m_impl->prevCheats.clear();
}

bool OptionManager::isAnticheatEnabled() const noexcept {
    return m_impl->isAnticheat;
}

bool OptionManager::isEnabled(std::string_view id) const {
    return getOption(id).enabled;
}

bool OptionManager::isEnabled(uint64_t id) const {
    return getOption(id).enabled;
}

bool OptionManager::isPinned(std::string_view id) const {
    return getOption(id).pin;
}

bool OptionManager::isPinned(uint64_t id) const {
    return getOption(id).pin;
}

bool OptionManager::isViewed(std::string_view id) const {
    return getOption(id).viewed;
}

bool OptionManager::isViewed(uint64_t id) const {
    return getOption(id).viewed;
}

bool OptionManager::isCheating(std::string_view id) const {
    if (auto o = getOptionInfo(id).lock()) return o->isCheating();
    return false;
}

bool OptionManager::getDefaultToggleState(std::string_view id) const noexcept {
    if (auto o = getOptionInfo(id).lock()) return o->getDefaultToggleState();
    return false;
}

OptionSave OptionManager::getOption(std::string_view id) const {
    if (auto opt = getOptionInfo(id).lock()) return getOption(opt->getIDHash());
    return mod->getSavedValue<OptionSave>(id, OptionSave{ getDefaultToggleState(id) });
}

OptionSave OptionManager::getOption(uint64_t id) const {
    if (auto const it = m_impl->saveCache.find(id); it != m_impl->saveCache.end()) return it->second;
    if (auto const it = m_impl->optHashes.find(id); it != m_impl->optHashes.end()) return mod->getSavedValue<OptionSave>(it->second, OptionSave{ getDefaultToggleState(it->second.c_str()) });

    return {};
}

std::weak_ptr<Option> OptionManager::getOptionInfo(std::string_view id) const noexcept {
    if (auto const it = m_impl->options.find(id); it != m_impl->options.end()) return it->second;
    return std::weak_ptr<Option>();
}

std::weak_ptr<Option> OptionManager::getOptionInfo(uint64_t id) const noexcept {
    if (auto const it = m_impl->optHashes.find(id); it != m_impl->optHashes.end()) return getOptionInfo(it->second.c_str());
    return std::weak_ptr<Option>();
}

Result<asp::BoxedString> OptionManager::getOptionIDForHash(uint64_t id) const noexcept {
    if (auto const it = m_impl->optHashes.find(id); it != m_impl->optHashes.end()) return Ok(it->second);
    return Err("No string ID found for this hash ID");
}

size_t OptionManager::getDelegateCount(std::string_view id) const noexcept {
    if (auto const it = m_impl->delegates.find(id); it != m_impl->delegates.end()) return it->second.size();
    return 0;
}

size_t OptionManager::getDelegateCount(uint64_t id) const noexcept {
    auto strIDRes = getOptionIDForHash(id);
    if (strIDRes.isErr()) return 0;

    auto const strID = std::move(strIDRes).unwrap();
    if (auto const it = m_impl->delegates.find(strID); it != m_impl->delegates.end()) return it->second.size();

    return 0;
}

bool OptionManager::shouldBeSafeMode() const noexcept {
    if (mod->getSettingValue<bool>("dyn-safe-mode")) return isCheatEnabled();
    return mod->getSettingValue<bool>("safe-mode");
}

void OptionManager::toggleOption(std::string_view id, bool enable) {
    setOption(id, enable, isPinned(id));
}

void OptionManager::toggleOption(uint64_t id, bool enable) {
    setOption(id, enable, isPinned(id));
}

void OptionManager::setOption(std::string_view id, bool enable, bool pin, bool viewed) {
    if (auto opt = getOptionInfo(id).lock()) {
        auto const it = m_impl->delegates.find(id);
        if (it != m_impl->delegates.end()) {
            for (auto& cb : it->second) cb(enable);
        };

        log::trace("Called {} delegates {} for option {}", it != m_impl->delegates.end() ? it->second.size() : 0, enable ? "on" : "off", id);

        auto cheats = m_impl->enabledCheats.size();

        auto const save = OptionSave{ enable, pin, viewed };

        queueInMainThread([id = std::string{ id }, save]() {
            (void)mod->setSavedValue(id, save);
        });

        m_impl->saveCache[opt->getIDHash()] = save;
        (void)OptionEvent(std::string{ id }).send(save);

        if (auto const it = m_impl->enabledCheats.find(opt->getIDHash()); it != m_impl->enabledCheats.end()) {
            if (!enable) m_impl->enabledCheats.erase(it);
        } else if (enable) {
            if (auto o = getOptionInfo(id).lock()) {
                if (o->isCheating()) {
                    log::debug("Enabled cheat option {}, adding to enabled cheats map", id);
                    m_impl->enabledCheats.emplace(opt->getIDHash());
                };
            };
        };

        auto cheatsNow = m_impl->enabledCheats.size();

        if (isCheating(id) && cheats != cheatsNow) {
            if (cheats == 0 && cheatsNow > 0) (void)OptionCheatingEvent().send(true);
            if (cheats > 0 && cheatsNow == 0) (void)OptionCheatingEvent().send(false);
        };
    } else {
        log::error("Option '{}' not found", id);
    };
}

void OptionManager::setOption(uint64_t id, bool enable, bool pin, bool viewed) {
    if (auto const it = m_impl->optHashes.find(id); it != m_impl->optHashes.end()) setOption(it->second.c_str(), enable, pin, viewed);
}

OptionManager* OptionManager::get() noexcept {
    static OptionManager inst;
    return &inst;
}

void cs::brkd::horrible::delegateHooks(ZStringView id, StringMap<std::shared_ptr<Hook>> const& hooks) {
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
                        log::trace("Toggling {} hook '{}' {}...", id, h->getDisplayName(), cw::str::isOnOff(value));
                        (void)h->toggle(value);
                    };
                };
            }
        );
    } else {
        log::error("Failed to get OptionManager to delegate hooks for option {}", id);
    };
}

Result<> OptionManagerV2::registerOption(OptionV2 const& option) {
    auto om = OptionManager::get();

    auto opt = Option::create(option.id, option.getIntegration())
                   ->setName(option.name)
                   ->setDescription(option.description)
                   ->setCategory(option.category)
                   ->setSillyTier(option.silly)
                   ->setDefaultToggleState(option.state)
                   ->setOnline(option.online)
                   ->setRequiresRestart(option.restart)
                   ->setSupportedPlatforms(option.platforms)
                   ->setCheating(option.cheating);

    auto res = om->registerOption(opt);
    (void)OptionEvent(opt->getID()).send(om->getOption(opt->getID()));

    return res;
}

void OptionManagerV2::toggleOption(std::string_view id, bool enable) {
    OptionManager::get()->toggleOption(id, enable);
}

bool OptionManagerV2::isEnabled(std::string_view id) {
    return OptionManager::get()->isEnabled(id);
}

void OptionManagerV2::toggleAnticheat(bool enable) {
    OptionManager::get()->toggleAnticheat(enable);
}

bool OptionManagerV2::isAnticheatEnabled() {
    return OptionManager::get()->isAnticheatEnabled();
}