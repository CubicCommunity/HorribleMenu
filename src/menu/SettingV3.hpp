#pragma once

#include <Geode/Geode.hpp>

#include <Geode/loader/SettingV3.hpp>

using namespace geode::prelude;

class HorribleSettingV3 : public SettingV3 {
public:
    static Result<std::shared_ptr<SettingV3>> parse(
        std::string_view key,
        std::string_view modID,
        matjson::Value const& json
    );

    bool load(matjson::Value const& json) override;
    bool save(matjson::Value& json) const override;

    bool isDefaultValue() const override;

    void reset() override;

    SettingNodeV3* createNode(float width) override;
};

class HorribleSettingNodeV3 : public SettingNodeV3 {
private:
    class Impl;
    std::unique_ptr<Impl> m_impl;

protected:
    HorribleSettingNodeV3();
    virtual ~HorribleSettingNodeV3();

    void updateState(CCNode* invoker) override;
    void onButton(CCObject*);
    void onCommit() override;
    void onResetToDefault() override;

    bool init(std::shared_ptr<HorribleSettingV3> setting, float width);

public:
    static HorribleSettingNodeV3* create(std::shared_ptr<HorribleSettingV3> setting, float width);

    bool hasUncommittedChanges() const override;
    bool hasNonDefaultValue() const override;

    std::shared_ptr<HorribleSettingV3> getSetting() const;
};