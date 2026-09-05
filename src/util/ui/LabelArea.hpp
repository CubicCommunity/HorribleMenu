#pragma once

#include <util/Themes.hpp>

#include <Geode/Geode.hpp>

namespace horrible {
    namespace ui {
        class LabelArea final : public cocos2d::CCNode {
        private:
            geode::Label* m_textLabel;

        protected:
            bool init(std::string text, float width, float scale, cocos2d::ccColor3B const& bgColor);

        public:
            static LabelArea* create(std::string text, float width, float scale = 1.f, cocos2d::ccColor3B const& bgColor = colors::black);

            void setText(std::string text);
        };
    };
};