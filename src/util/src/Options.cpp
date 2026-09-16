#include "../Options.hpp"

#include <Util.h>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

std::vector<SharedOption> options::getAll() {
    if (auto om = OptionManager::get()) {
        auto opts = om->getAllOptions();

        std::vector<SharedOption> out;
        out.reserve(opts.size());

        for (auto const& [id, opt] : opts) out.push_back(opt);

        return out;
    };

    return {};
};

bool options::isEnabled(ZStringView id) {
    if (auto om = OptionManager::get()) return om->isEnabled(id);
    return false;
};

bool options::isPinned(ZStringView id) {
    if (auto om = OptionManager::get()) return om->isPinned(id);
    return false;
};

bool options::isViewed(ZStringView id) {
    if (auto om = OptionManager::get()) return om->isViewed(id);
    return false;
};

uint8_t options::getChance(std::string_view id) {
    return mod->getSettingValue<uint8_t>(fmt::format("{}-chance", id));
};

OptionSave options::get(ZStringView id) {
    if (auto om = OptionManager::get()) return om->getOption(id);
    return {};
};

void options::set(ZStringView id, bool enable, bool pin, bool viewed) {
    if (auto om = OptionManager::get()) om->setOption(id, enable, pin, viewed);
};

size_t options::getDelegates(std::string_view id) noexcept {
    if (auto om = OptionManager::get()) return om->getDelegateCount(id);
    return 0;
};

std::vector<std::string> options::getAllCategories() {
    if (auto om = OptionManager::get()) {
        auto opts = om->getAllCategories();

        std::vector<std::string> out;
        out.reserve(opts.size());

        for (auto const& [id, cat] : opts) out.push_back(id);

        return out;
    };
    return {};
};

bool options::doesCategoryExist(ZStringView category) {
    return str::containsAny(category, getAllCategories());
};