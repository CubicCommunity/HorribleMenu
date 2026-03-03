#include "../Options.hpp"

#include <Utils.hpp>

using namespace horrible;
using namespace horrible::util;

std::span<const Option> options::getAll() noexcept {
    if (auto om = OptionManager::get()) return om->getOptions();
    return {};
};

bool options::isEnabled(std::string_view id) noexcept {
    if (auto om = OptionManager::get()) return om->isEnabled(id);
    return false;
};

bool options::isPinned(std::string_view id) noexcept {
    if (auto om = OptionManager::get()) return om->isPinned(id);
    return false;
};

int options::getChance(std::string_view id) {
    return horribleMod->getSettingValue<int>(fmt::format("{}-chance", id));
};

HorribleOptionSave options::get(std::string_view id) {
    if (auto om = OptionManager::get()) return om->getOption(id);
    return {};
};

void options::set(ZStringView id, bool enable, bool pin) {
    if (auto om = OptionManager::get()) om->setOption(id, enable, pin);
};

size_t options::getDelegates(std::string_view id) noexcept {
    if (auto om = OptionManager::get()) return om->getDelegateCount(id);
    return 0;
};

std::span<const std::string> options::getAllCategories() noexcept {
    if (auto om = OptionManager::get()) return om->getCategories();
    return {};
};

bool options::doesCategoryExist(ZStringView category) noexcept {
    return str::containsAny(category, getAllCategories());
};