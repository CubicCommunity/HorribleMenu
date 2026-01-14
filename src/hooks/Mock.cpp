#if !defined(GEODE_IS_MACOS) && !defined(GEODE_IS_IOS) // not available for these platforms
#include <Utils.hpp>

#include <Geode/Geode.hpp>

#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

namespace fs = std::filesystem; // Shortcut for std::filesystem

class $modify(MockMenuLayer, MenuLayer) {
    struct Fields {
        bool enabled = options::get("mock");
        int chance = options::getChance("mock");
    };

    bool init() {
        if (!MenuLayer::init()) return false;

        auto f = m_fields.self();

        // show a lazysprite for the first png found in the save dir
        if (f->enabled) {
            int rnd = randng::fast();
            log::debug("mock chance {}", rnd);

            if (rnd <= f->chance) {
                auto const mockConfigPath = fmt::format("{}\\mock.json", horribleMod->getSaveDir());
                auto const mockConfig = file::readJson(fs::path(mockConfigPath));

                log::debug("Reading path {}...", mockConfigPath);

                if (mockConfig.isOk()) {
                    log::debug("Read mocking config file");

                    auto const mockConfigUnwr = mockConfig.unwrapOr(matjson::Value());

                    auto lvlUnwr = mockConfigUnwr.begin();
                    std::advance(lvlUnwr, rnd % mockConfigUnwr.size());

                    auto const id = lvlUnwr->getKey().value_or("");
                    auto percent = lvlUnwr->asInt().unwrapOr(99);

                    if (!id.empty()) {
                        log::debug("ID {} with percentage {} is valid", id, percent);

                        auto const pngPath = fmt::format("{}\\{}.png", horribleMod->getSaveDir(), id);

                        log::info("Displaying {}", pngPath);

                        auto ss = LazySprite::create({ 192.f, 108.f });
                        ss->setID("mock"_spr);
                        ss->setScale(0.25);
                        ss->setAnchorPoint({ 0.5, 0.5 });
                        ss->setPosition({ -192.f, getScaledContentHeight() / 2.f });

                        ss->setLoadCallback([this, ss, percent, rnd](Result<> res) {
                            if (res.isOk()) {
                                log::info("Sprite loaded successfully from save dir PNG");

                                auto const percLabelText = fmt::format("{}%", percent);

                                auto percLabel = CCLabelBMFont::create(percLabelText.c_str(), "bigFont.fnt");
                                percLabel->setID("percentage");
                                percLabel->setPosition(ss->getScaledContentSize() / 2.f);
                                percLabel->setAlignment(CCTextAlignment::kCCTextAlignmentLeft);
                                percLabel->ignoreAnchorPointForPosition(false);
                                percLabel->setAnchorPoint({ 0, 0 });
                                percLabel->setScale(2.5);

                                ss->addChild(percLabel);

                                auto rA = randng::pc();
                                auto rB = randng::pc();

                                float yA = getScaledContentHeight() * rB; // starting height pos
                                float yB = getScaledContentHeight() * rA; // ending height pos

                                ss->setPositionY(getScaledContentHeight() * yA);
                                ss->setRotation(360.f * (yA * yB)); // random rotation

                                auto move = CCEaseIn::create(CCMoveTo::create(10.f, { getScaledContentWidth() + 192.f, getScaledContentHeight() * yB }), 1.f);
                                auto rotate = CCEaseOut::create(CCRotateBy::create(12.5f, 45.f), 1.f);

                                auto action = CCSpawn::createWithTwoActions(move, rotate);
                                ss->runAction(action);

                                log::info("Animated sprite successfully");
                            } else {
                                log::error("Sprite failed to load: {}", res.unwrapErr());
                                ss->removeMeAndCleanup();
                            }; });

                            ss->loadFromFile(fs::path(pngPath));
                            addChild(ss, 999);
                    } else {
                        log::error("ID is invalid");
                    };
                } else {
                    log::error("Mocking data file not found");
                };
            };
        };

        return true;
    };
};

class $modify(MockPlayLayer, PlayLayer) {
    struct Fields {
        bool enabled = options::get("mock");
    };

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();

        auto f = m_fields.self();

        this->template addEventListener<OptionEventFilter>(
            [this, f](OptionEvent* ev) {
                f->enabled = ev->getToggled();
                return ListenerResult::Propagate;
            },
            "mock"
        );
    };

    void showNewBest(bool newReward, int orbs, int diamonds, bool demonKey, bool noRetry, bool noTitle) {
        int id = m_level->m_levelID;
        int percentage = m_level->m_normalPercent;

        log::info("Showing new best for level ID: {}", id);
        log::info("Level percentage: {}", percentage);

        if (m_fields->enabled && percentage >= 90) {
            CCDirector* director = CCDirector::sharedDirector();
            CCScene* scene = CCScene::get();

            // Get the window size in points and scale to pixels
            auto const winSize = director->getWinSize();

            int width = static_cast<int>(winSize.width);
            int height = static_cast<int>(winSize.height);

            CCRenderTexture* renderTexture = CCRenderTexture::create(width, height);

            renderTexture->begin();
            scene->visit();
            renderTexture->end();

            if (auto image = renderTexture->newCCImage()) {
                auto const path = fmt::format("{}\\{}.png", horribleMod->getSaveDir(), id);

                if (image->saveToFile(path.c_str(), false)) {
                    auto const mockConfigPath = fmt::format("{}\\mock.json", horribleMod->getSaveDir());
                    auto const mockConfig = file::readJson(fs::path(mockConfigPath)); // get the saved fails to mock the player with :)

                    auto toWrite = matjson::Value(); // what we're gonna write in the mock.json file

                    if (mockConfig.isOk()) {
                        // unwrap the whole thing
                        auto mockConfigUnwr = mockConfig.unwrapOr(matjson::Value());

                        // overwrite this field (or add it) with the percent
                        mockConfigUnwr[utils::numToString(id)] = percentage;

                        toWrite = mockConfigUnwr;
                    } else {
                        toWrite = matjson::makeObject({ {utils::numToString(id), percentage} });
                    };

                    if (!toWrite.isNull()) {
                        auto const mockJson = file::writeToJson(mockConfigPath, toWrite);

                        if (mockJson.isOk()) {
                            log::info("Saved highly mockable percentage of {} to data", percentage);
                        } else {
                            log::error("Aw man, failed to save mockable percentage of {} to data", percentage);
                        };
                    };

                    log::info("Saved screenshot to {}", path);
                } else {
                    log::error("Failed to save screenshot to {}", path);
                };

                CC_SAFE_DELETE(image);
            } else {
                log::error("Failed to create image from render texture");
            };
        };

        PlayLayer::showNewBest(newReward, orbs, diamonds, demonKey, noRetry, noTitle);
    };

    void levelComplete() {
        if (m_fields->enabled) {
            int id = m_level->m_levelID;
            int percentage = m_level->m_normalPercent;

            auto const mockConfigPath = fmt::format("{}\\mock.json", horribleMod->getSaveDir());
            auto const mockConfig = file::readJson(fs::path(mockConfigPath)); // get the saved levels to mock the player :)

            if (mockConfig.isOk()) {
                log::debug("Clearing mock record for {}", id);
                auto mockConfigUnwr = mockConfig.unwrapOr(matjson::Value());
                mockConfigUnwr[utils::numToString(id)].clear();

                auto const mockJson = file::writeToJson(mockConfigPath, mockConfigUnwr);

                if (mockJson.isOk()) {
                    log::info("Saved highly mockable percentage of {} to data", percentage);
                } else {
                    log::error("Aw man, failed to save mockable percentage of {} to data", percentage);
                };
            };
        };

        PlayLayer::levelComplete();
    };
};
#endif