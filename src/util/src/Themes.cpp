#include "../Themes.hpp"

#include <Geode/Geode.hpp>

using namespace geode::prelude;

using namespace horrible::util;

const char* themes::getBackgroundSprite(std::string_view theme) noexcept {
    if (theme == enums::Blue) return "GJ_square02.png";
    if (theme == enums::Green) return "GJ_square03.png";
    if (theme == enums::Dark) return "geode.loader/GE_square02.png";
    if (theme == enums::Light) return "geode.loader/white-square.png";
    if (theme == enums::Dim) return "GJ_square05.png";
    if (theme == enums::Girlypop) return "GJ_square04.png";
    if (theme == enums::Aqua) return "geode.loader/GE_square03.png";
    if (theme == enums::Geode) return "geode.loader/GE_square01.png";

    return "GJ_square01.png";
};

CircleBaseColor themes::getCircleBaseColor(std::string_view theme) noexcept {
    if (theme == enums::Blue) return CircleBaseColor::Blue;
    if (theme == enums::Green) return CircleBaseColor::Green;
    if (theme == enums::Dark) return CircleBaseColor::Gray;
    if (theme == enums::Light) return CircleBaseColor::Gray;
    if (theme == enums::Dim) return CircleBaseColor::Gray;
    if (theme == enums::Girlypop) return CircleBaseColor::Pink;
    if (theme == enums::Aqua) return CircleBaseColor::DarkAqua;
    if (theme == enums::Geode) return CircleBaseColor::DarkPurple;

    return CircleBaseColor::Green;
};

const char* themes::getButtonSquareSprite(std::string_view theme) noexcept {
    if (theme == enums::Blue) return "GJ_button_02.png";
    if (theme == enums::Green) return "GJ_button_01.png";
    if (theme == enums::Dark) return "GJ_button_05.png";
    if (theme == enums::Light) return "GJ_button_04.png";
    if (theme == enums::Dim) return "GJ_button_04.png";
    if (theme == enums::Girlypop) return "GJ_button_03.png";
    if (theme == enums::Aqua) return "geode.loader/GE_button_05.png";
    if (theme == enums::Geode) return "geode.loader/GE_button_05.png";

    return "GJ_button_01.png";
};