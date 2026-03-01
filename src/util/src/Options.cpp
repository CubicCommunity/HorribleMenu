#include "../Options.hpp"

#include <Utils.hpp>

using namespace horrible;
using namespace horrible::util;

std::span<const Option> options::getAll() noexcept {
    if (auto om = OptionManager::get()) return om->getOptions();
    return {};
};

bool options::get(std::string_view id) noexcept {
    if (auto om = OptionManager::get()) return om->getOption(id);
    return false;
};

int options::getChance(std::string_view id) {
    return static_cast<int>(horribleMod->getSettingValue<int64_t>(fmt::format("{}-chance", id)));
};

bool options::set(ZStringView id, bool enable) {
    if (auto om = OptionManager::get()) return om->setOption(id, enable);
    return false;
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