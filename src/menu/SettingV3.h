#pragma once

#include <Geode/Geode.hpp>

#include <Geode/loader/SettingV3.hpp>

namespace horrible {
    class HorribleSettingV3 final : public geode::SettingV3 {
    public:
        static geode::Result<std::shared_ptr<SettingV3>> parse(
            std::string key,
            std::string modID,
            matjson::Value const& json);

        bool load(matjson::Value const& json) override;
        bool save(matjson::Value& json) const override;

        bool isDefaultValue() const noexcept override;

        void reset() override;

        geode::SettingNodeV3* createNode(float width) override;
    };

    class HorribleSettingNodeV3 final : public geode::SettingNodeV3 {
    private:
        class Impl;
        std::unique_ptr<Impl> m_impl;

    protected:
        HorribleSettingNodeV3();
        ~HorribleSettingNodeV3();

        void updateState(cocos2d::CCNode* invoker) override;
        void onCommit() override;
        void onResetToDefault() override;

        bool init(std::shared_ptr<HorribleSettingV3> setting, float width);

    public:
        static HorribleSettingNodeV3* create(std::shared_ptr<HorribleSettingV3> setting, float width);

        bool hasUncommittedChanges() const noexcept override;
        bool hasNonDefaultValue() const noexcept override;

        std::shared_ptr<HorribleSettingV3> getSetting() const noexcept;
    };
};