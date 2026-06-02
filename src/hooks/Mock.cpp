#include <Utils.h>

#include <Geode/Geode.hpp>

#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;

#define THIS_ID "mock"

static auto const o = Option::create(THIS_ID)
                          ->setName("Mock your 90%+ Fail")
                          ->setDescription("Occasionally taunts you in the main menu with a screenshot of one of your 90%-99% fails.\n<co>Must be enabled with Safe Mode disabled to save new best records.</c>\n<cl>suggested by Wuffin</c>")
                          ->setCategory(category::misc)
                          ->setSillyTier(SillyTier::Medium)
                          ->setSupportedPlatforms({Platform::Windows, Platform::Android})
                          ->autoRegister();

#if !defined(GEODE_IS_MACOS) & !defined(GEODE_IS_IOS)  // not compat with these platforms
class $modify(MockMenuLayer, MenuLayer) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID);

    struct Fields final {
        uint8_t chance = options::getChance(THIS_ID);
    };

    bool init() {
        if (!MenuLayer::init()) return false;

        // show a lazysprite for the first png found in the save dir
        if (rng::chance(m_fields->chance)) {
            auto const mockConfigPath = mod->getSaveDir() / "mock.json";
            auto const mockConfig = file::readJson(mockConfigPath);

            log::trace("Reading path {}...", mockConfigPath);

            if (mockConfig.isOk()) {
                log::debug("Path is ok!");

                auto const mockConfigUnwr = mockConfig.unwrapOr(matjson::Value());

                auto const lvlUnwr = mockConfigUnwr.begin();
                auto const lvl = lvlUnwr->get(rng::get(lvlUnwr->size() - 1)).unwrapOr(matjson::Value());

                auto const id = lvl.getKey().value_or("");
                auto percent = lvl.asInt().unwrapOr(90);

                if (!id.empty()) {
                    log::trace("ID {} with percentage {} is valid", id, percent);

                    auto const pngPath = mod->getSaveDir() / fmt::format("{}.png", id);

                    log::info("Displaying {}", pngPath);

                    auto ss = LazySprite::create({192.f, 108.f});
                    ss->setID("mocked"_spr);
                    ss->setScale(0.25);
                    ss->setAnchorPoint(anchor::center);
                    ss->setPosition({-192.f, getScaledContentHeight() / 2.f});

                    ss->setLoadCallback([self = WeakRef(this), screenshot = WeakRef(ss), percent](Result<> res) {
                        if (res.isOk()) {
                            if (auto s = self.lock()) {
                                log::info("Sprite loaded successfully from save dir PNG");

                                if (auto ss = screenshot.lock()) {
                                    auto const percLabelText = fmt::format("{}%", percent);

                                    auto percLabel = CCLabelBMFont::create(percLabelText.c_str(), font::big);
                                    percLabel->setID("percentage");
                                    percLabel->setPosition(ss->getScaledContentSize() / 2.f);
                                    percLabel->setAlignment(kCCTextAlignmentLeft);
                                    percLabel->ignoreAnchorPointForPosition(false);
                                    percLabel->setAnchorPoint({0, 0});
                                    percLabel->setScale(2.5);

                                    ss->addChild(percLabel);

                                    auto rA = rng::pc();
                                    auto rB = rng::pc();

                                    float yA = s->getScaledContentHeight() * rB;  // starting height pos
                                    float yB = s->getScaledContentHeight() * rA;  // ending height pos

                                    ss->setPositionY(s->getScaledContentHeight() * yA);
                                    ss->setRotation(360.f * (yA * yB));  // random rotation

                                    auto move = CCEaseIn::create(CCMoveTo::create(10.f, {s->getScaledContentWidth() + 192.f, s->getScaledContentHeight() * yB}), 1.f);
                                    auto rotate = CCEaseOut::create(CCRotateBy::create(12.5f, 45.f), 1.f);

                                    auto action = CCSpawn::createWithTwoActions(move, rotate);
                                    ss->runAction(action);

                                    log::info("Animated sprite successfully");
                                } else {
                                    log::error("Mocked sprite was destroyed before load callback");
                                };
                            };
                        } else {
                            log::error("Sprite failed to load: {}", res.unwrapErr());
                            if (auto ss = screenshot.lock()) ss->removeFromParent();
                        };
                    });

                    ss->loadFromFile(pngPath);
                    addChild(ss, HIGHEST_Z);
                } else {
                    log::error("ID is invalid");
                };
            } else {
                log::error("Mocking data file not found");
            };
        };

        return true;
    };
};

class $modify(MockPlayLayer, PlayLayer) {
    HORRIBLE_DELEGATE_HOOKS(THIS_ID);

    void showNewBest(bool newReward, int orbs, int diamonds, bool demonKey, bool noRetry, bool noTitle) {
        int id = m_level->m_levelID;
        int percentage = m_level->m_normalPercent;

        log::debug("Showing new best for level ID: {}", id);
        log::debug("Level percentage: {}", percentage);

        if (percentage >= 90) {
            // Get the window size in points and scale to pixels
            auto const winSize = CCDirector::sharedDirector()->getWinSize();
            auto renderTexture = CCRenderTexture::create(static_cast<int>(winSize.width), static_cast<int>(winSize.height));

            renderTexture->begin();
            CCScene::get()->visit();
            renderTexture->end();

            if (auto image = renderTexture->newCCImage()) {
                auto const path = mod->getSaveDir() / fmt::format("{}.png", id);

                if (image->saveToFile(str::pathToString(path).c_str(), false)) {
                    auto const mockConfigPath = mod->getSaveDir() / "mock.json";
                    auto const mockConfig = file::readJson(mockConfigPath);  // get the saved fails to mock the player with :)

                    auto toWrite = matjson::Value();  // what we're gonna write in the mock.json file

                    if (mockConfig.isOk()) {
                        // unwrap the whole thing
                        auto mockConfigUnwr = mockConfig.unwrapOr(matjson::Value());

                        // overwrite this field (or add it) with the percent
                        mockConfigUnwr[utils::numToString(id)] = percentage;

                        toWrite = mockConfigUnwr;
                    } else {
                        toWrite = matjson::makeObject({{utils::numToString(id), percentage}});
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

                delete image;
            } else {
                log::error("Failed to create image from render texture");
            };
        };

        PlayLayer::showNewBest(newReward, orbs, diamonds, demonKey, noRetry, noTitle);
    };

    void levelComplete() {
        int id = m_level->m_levelID;
        int percentage = m_level->m_normalPercent;

        auto const mockConfigPath = mod->getSaveDir() / "mock.json";
        auto const mockConfig = file::readJson(mockConfigPath);  // get the saved levels to mock the player :)

        if (mockConfig.isOk()) {
            log::trace("Clearing mock record for {}", id);
            auto mockConfigUnwr = mockConfig.unwrapOr(matjson::Value());
            mockConfigUnwr[utils::numToString(id)].clear();

            auto const mockJson = file::writeToJson(mockConfigPath, mockConfigUnwr);

            if (mockJson.isOk()) {
                log::info("Saved highly mockable percentage of {} to data", percentage);
            } else {
                log::error("Aw man, failed to save mockable percentage of {} to data", percentage);
            };
        };

        PlayLayer::levelComplete();
    };
};
#endif