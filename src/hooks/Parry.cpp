// #include <Utils.hpp>

// #include <Geode/Geode.hpp>

// #include <Geode/modify/PlayLayer.hpp>
// #include <Geode/modify/PlayerObject.hpp>

// // 8 hours and still not working properly :(
// // imma take a nap

// using namespace geode::prelude;
// using namespace horrible::prelude;

// static constexpr auto id = "parry";

// static GameObject* s_pendingKiller1 = nullptr;
// static GameObject* s_pendingKiller2 = nullptr;

// static PlayerObject* s_pendingPlayer1 = nullptr;
// static PlayerObject* s_pendingPlayer2 = nullptr;

// static bool s_parryActive1 = false;
// static bool s_parryActive2 = false;

// static float s_parryTimer1 = 0.f;
// static float s_parryTimer2 = 0.f;

// static PlayerObject* s_activeParryPlayer = nullptr;

// constexpr float kParryWindow = 0.5f;  // seconds

    // {
    //     "parry",
    //     "Parry Obstacles",
    //     "Whenever your hitbox is inside of a hazard hitbox, you will instead destroy it if you time your input right.\n<cy>Credit: Wuffin</c>",
    //     category::misc,
    //     SillyTier::Low,
    //     true,
    //     {},
    // },

// class $modify(ParryPlayerObject, PlayerObject) {
//     struct Fields {
//         bool enabled = options::get(id);
//     };

//     bool pushButton(PlayerButton p0) {
//         const bool baseRet = PlayerObject::pushButton(p0);

//         if (!m_fields->enabled) return baseRet;
//         if (p0 != PlayerButton::Jump) return baseRet;

//         if (auto pl = PlayLayer::get()) {
//             // Player 1 cancel
//             if (this == pl->m_player1 && s_parryActive1) {
//                 s_parryActive1 = false;

//                 s_parryTimer1 = 0.f;

//                 s_pendingKiller1 = nullptr;
//                 s_pendingPlayer1 = nullptr;

//                 // hide UI
//                 if (auto bar = typeinfo_cast<ProgressBar*>(pl->getChildByID(id_spr))) bar->setVisible(false);
//                 if (auto lbl = typeinfo_cast<CCLabelBMFont*>(pl->getChildByID("parry_label"_spr))) lbl->setVisible(false);

//                 s_activeParryPlayer = nullptr;

//                 // play effects
//                 playSpawnEffect();
//                 playSfx("chestClick.ogg");

//                 // Player 2 cancel
//             } else if (this == pl->m_player2 && s_parryActive2) {
//                 s_parryActive2 = false;

//                 s_parryTimer2 = 0.f;

//                 s_pendingKiller2 = nullptr;
//                 s_pendingPlayer2 = nullptr;

//                 if (auto bar = typeinfo_cast<ProgressBar*>(pl->getChildByID(id_spr))) bar->setVisible(false);
//                 if (auto lbl = typeinfo_cast<CCLabelBMFont*>(pl->getChildByID("parry_label"_spr))) lbl->setVisible(false);

//                 s_activeParryPlayer = nullptr;

//                 playSpawnEffect();
//                 playSfx("chestClick.ogg");
//             };
//         };

//         return baseRet;
//     };
// };

// class $modify(ParryPlayLayer, PlayLayer) {
//     struct Fields {
//         bool enabled = options::get(id);

//         ProgressBar* m_parryBar = nullptr;
//         CCLabelBMFont* m_parryLabel = nullptr;
//     };

//     bool init(GJGameLevel * level, bool useReplay, bool dontCreateObjects) {
//         if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;
//         if (!m_fields->enabled) return true;

//         // Create shared UI and hide initially
//         if (!m_fields->m_parryBar) {
//             m_fields->m_parryBar = ProgressBar::create();
//             m_fields->m_parryBar->setID(id_spr);
//             m_fields->m_parryBar->setFillColor(colors::yellow);
//             m_fields->m_parryBar->setAnchorPoint({ 0.5f, 0.5f });
//             m_fields->m_parryBar->setVisible(false);

//             addChild(m_fields->m_parryBar, 9999);
//         };

//         if (!m_fields->m_parryLabel) {
//             m_fields->m_parryLabel = CCLabelBMFont::create("PARRY", "bigFont.fnt");
//             m_fields->m_parryLabel->setColor(colors::yellow);
//             m_fields->m_parryLabel->setScale(0.35f);
//             m_fields->m_parryLabel->setAnchorPoint({ 0.5f, 0.5f });
//             m_fields->m_parryLabel->setVisible(false);
//             m_fields->m_parryLabel->setID("parry_label"_spr);

//             addChild(m_fields->m_parryLabel, 10000);
//         };

//         scheduleUpdate();
//         return true;
//     };

//     void showParryUI(PlayerObject * player) {
//         if (!m_fields->enabled || !player) return;

//         s_activeParryPlayer = player;

//         float pct = (player == m_player1) ? (s_parryTimer1 / kParryWindow) * 100.f : (s_parryTimer2 / kParryWindow) * 100.f;
//         if (pct < 0.f) pct = 0.f;

//         if (m_fields->m_parryBar) {
//             m_fields->m_parryBar->updateProgress(pct);
//             m_fields->m_parryBar->setVisible(true);

//             auto const worldPos = player->convertToWorldSpaceAR({ 0.f, 0.f });
//             auto const localPos = convertToNodeSpaceAR(worldPos);

//             m_fields->m_parryBar->setPosition({ localPos.x, localPos.y + 30.f });
//         };

//         if (m_fields->m_parryLabel) {
//             auto const label = fmt::format("PARRY\n{:.2f}s", std::max(0.f, (player == m_player1 ? s_parryTimer1 : s_parryTimer2)));

//             m_fields->m_parryLabel->setString(label.c_str());
//             m_fields->m_parryLabel->setVisible(true);

//             auto const worldPos = player->convertToWorldSpaceAR({ 0.f, 0.f });
//             auto const localPos = convertToNodeSpaceAR(worldPos);

//             m_fields->m_parryLabel->setPosition({ localPos.x, localPos.y + 38.f });
//         };
//     };

//     void hideParryUI(PlayerObject * player) {
//         if (!m_fields->enabled) return;

//         if (m_fields->m_parryBar) m_fields->m_parryBar->setVisible(false);
//         if (m_fields->m_parryLabel) m_fields->m_parryLabel->setVisible(false);

//         if (s_activeParryPlayer == player) s_activeParryPlayer = nullptr;

//         // unschedule in case it is scheduled
//         if (player == m_player1) unschedule(schedule_selector(ParryPlayLayer::parryExpire1));
//         if (player == m_player2) unschedule(schedule_selector(ParryPlayLayer::parryExpire2));
//     };

//     // the scheduled expire call - handles finalizing the kill for player1
//     void parryExpire1(float dt) {
//         unschedule(schedule_selector(ParryPlayLayer::parryExpire1));
//         if (!s_parryActive1) return;

//         PlayerObject* toKill = s_pendingPlayer1;
//         GameObject* killer = s_pendingKiller1;

//         s_parryActive1 = false;

//         s_parryTimer1 = 0.f;

//         s_pendingKiller1 = nullptr;
//         s_pendingPlayer1 = nullptr;

//         hideParryUI(m_player1);

//         if (toKill) {
//             if (killer) PlayLayer::destroyPlayer(toKill, killer);

//             if (!toKill->m_isDead) {
//                 toKill->m_isDead = true;
//                 toKill->playDeathEffect();
//             };
//         };
//     };

//     void parryExpire2(float dt) {
//         unschedule(schedule_selector(ParryPlayLayer::parryExpire2));

//         if (!s_parryActive2) return;

//         PlayerObject* toKill = s_pendingPlayer2;
//         GameObject* killer = s_pendingKiller2;

//         s_parryActive2 = false;

//         s_parryTimer2 = 0.f;

//         s_pendingKiller2 = nullptr;
//         s_pendingPlayer2 = nullptr;

//         hideParryUI(m_player2);

//         if (toKill) {
//             if (killer) PlayLayer::destroyPlayer(toKill, killer);

//             if (!toKill->m_isDead) {
//                 toKill->m_isDead = true;
//                 toKill->playDeathEffect();
//             };
//         };
//     };

//     void destroyPlayer(PlayerObject * player, GameObject * game) {
//         if (!m_fields->enabled) {
//             PlayLayer::destroyPlayer(player, game);
//             return;
//         };

//         if (!game || (player && player->m_isDead)) {
//             PlayLayer::destroyPlayer(player, game);
//             return;
//         };

//         if (game == m_anticheatSpike && player && !player->m_isDead) return;

//         if (player == m_player1) {
//             if (s_parryActive1) return;  // already in parry window

//             s_parryActive1 = true;

//             s_parryTimer1 = kParryWindow;

//             s_pendingKiller1 = game;
//             s_pendingPlayer1 = player;

//             showParryUI(player);
//             scheduleOnce(schedule_selector(ParryPlayLayer::parryExpire1), kParryWindow);
//             log::debug("Parry started for player 1 ({}s)", kParryWindow);

//             return;
//         } else if (player == m_player2) {
//             if (s_parryActive2) return;
//             s_parryActive2 = true;

//             s_parryTimer2 = kParryWindow;

//             s_pendingKiller2 = game;
//             s_pendingPlayer2 = player;

//             showParryUI(player);
//             scheduleOnce(schedule_selector(ParryPlayLayer::parryExpire2), kParryWindow);

//             log::debug("Parry started for player 2 ({}s)", kParryWindow);

//             return;
//         };

//         PlayLayer::destroyPlayer(player, game);
//     };

//     void update(float dt) {
//         PlayLayer::update(dt);
//         if (!m_fields->enabled) return;
//         // only update visual state and timers here; finalization is done in parryExpire1/2
//         if (s_parryActive1) {
//             s_parryTimer1 -= dt;
//             if (s_parryTimer1 < 0.f) s_parryTimer1 = 0.f;

//             if (m_player1) {
//                 auto const worldPos = m_player1->convertToWorldSpaceAR({ 0.f, 0.f });
//                 auto const localPos = convertToNodeSpaceAR(worldPos);

//                 if (m_fields->m_parryBar) m_fields->m_parryBar->setPosition({ localPos.x, localPos.y + 30.f });
//                 if (m_fields->m_parryLabel) m_fields->m_parryLabel->setPosition({ localPos.x, localPos.y + 38.f });
//             };

//             if (m_fields->m_parryBar) {
//                 float pct = (s_parryTimer1 / kParryWindow) * 100.f;
//                 if (pct < 0.f) pct = 0.f;

//                 m_fields->m_parryBar->updateProgress(pct);
//             };

//             if (m_fields->m_parryLabel) {
//                 auto const label = fmt::format("PARRY\n{:.2f}s", std::max(0.f, s_parryTimer1));
//                 m_fields->m_parryLabel->setString(label.c_str());
//             };
//         };

//         if (s_parryActive2) {
//             s_parryTimer2 -= dt;
//             if (s_parryTimer2 < 0.f) s_parryTimer2 = 0.f;

//             if (m_player2) {
//                 auto const worldPos = m_player2->convertToWorldSpaceAR({ 0.f, 0.f });
//                 auto const localPos = convertToNodeSpaceAR(worldPos);

//                 if (m_fields->m_parryBar) m_fields->m_parryBar->setPosition({ localPos.x, localPos.y + 30.f });
//                 if (m_fields->m_parryLabel) m_fields->m_parryLabel->setPosition({ localPos.x, localPos.y + 38.f });
//             };

//             if (m_fields->m_parryBar) {
//                 float pct = (s_parryTimer2 / kParryWindow) * 100.f;
//                 if (pct < 0.f) pct = 0.f;

//                 m_fields->m_parryBar->updateProgress(pct);
//             };

//             if (m_fields->m_parryLabel) {
//                 auto const label = fmt::format("PARRY\n{:.2f}s", std::max(0.f, s_parryTimer2));
//                 m_fields->m_parryLabel->setString(label.c_str());
//             };
//         };
//     };

//     void resetLevel() {
//         s_pendingKiller1 = nullptr;
//         s_pendingKiller2 = nullptr;

//         s_pendingPlayer1 = nullptr;
//         s_pendingPlayer2 = nullptr;

//         s_parryActive1 = false;
//         s_parryActive2 = false;

//         s_parryTimer1 = 0.f;
//         s_parryTimer2 = 0.f;

//         s_activeParryPlayer = nullptr;

//         unschedule(schedule_selector(ParryPlayLayer::parryExpire1));
//         unschedule(schedule_selector(ParryPlayLayer::parryExpire2));

//         if (m_fields->m_parryBar) m_fields->m_parryBar->setVisible(false);
//         if (m_fields->m_parryLabel) m_fields->m_parryLabel->setVisible(false);

//         PlayLayer::resetLevel();
//     };
// };