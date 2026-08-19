#include "../Options.hpp"

#include <Util.h>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

std::vector<std::weak_ptr<Option>> options::getAll() noexcept {
    if (auto om = OptionManager::get()) return om->getOptions();
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

HorribleOptionSave options::get(ZStringView id) {
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

std::span<const std::string> options::getAllCategories() noexcept {
    if (auto om = OptionManager::get()) return om->getCategories();
    return {};
};

bool options::doesCategoryExist(ZStringView category) {
    return str::containsAny(category, getAllCategories());
};