#include <Util.h>

#include <util/Base.h>

#include <ranges>

#include <Geode/Geode.hpp>

#include <Geode/modify/ProfilePage.hpp>
#include <Geode/modify/CommentCell.hpp>

#include <dasshu.badgified/include/Badgified.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;
using namespace dasshu::badgified;

static std::vector<std::weak_ptr<Hook>> g_badgeHooks;

namespace horrible::badges {
#define HORRIBLE_BADGE_LEAD "lead-dev"
#define HORRIBLE_BADGE_DEVELOPER "developer"
#define HORRIBLE_BADGE_CONTRIBUTOR "contributor"
#define HORRIBLE_BADGE_SUPPORTER "supporter"

    static auto constexpr lead = HORRIBLE_BADGE_LEAD;
    static auto constexpr dev = HORRIBLE_BADGE_DEVELOPER;
    static auto constexpr contributor = HORRIBLE_BADGE_CONTRIBUTOR;
    static auto constexpr supporter = HORRIBLE_BADGE_SUPPORTER;

    static constexpr auto getSpriteForBadge(std::string_view id) {
        if (id == lead) return "badge_lead.png"_spr;
        if (id == dev) return "badge_developer.png"_spr;
        if (id == contributor) return "badge_contributor.png"_spr;
        if (id == supporter) return "badge_supporter.png"_spr;

        return "badge_lead.png"_spr;
    };

    static constexpr auto getDescForBadge(std::string_view id) {
        if (id == lead) return "This user, a member of <cf>[Team Breakeode](https://breakeode.cubicstudios.xyz/)</c>, is a <cy>Lead Developer</c> of ![ico](frame:cubicstudios.horriblemenu/icon.png?scale=0.35) <co>[Horrible Menu](mod:cubicstudios.horriblemenu)</c>. They coordinate development, review contributions, and oversee the overall direction of the mod.";
        if (id == dev) return "This user, a member of <cf>[Team Breakeode](https://breakeode.cubicstudios.xyz/)</c>, is an active <cb>Developer</c> of ![ico](frame:cubicstudios.horriblemenu/icon.png?scale=0.35) <co>[Horrible Menu](mod:cubicstudios.horriblemenu)</c>. They currently help maintain the codebase for the mod.";
        if (id == contributor) return "This user is a <cj>Contributor</c> to ![ico](frame:cubicstudios.horriblemenu/icon.png?scale=0.35) <co>[Horrible Menu](mod:cubicstudios.horriblemenu)</c>. They've assisted development by contributing code to the mod's codebase.";
        if (id == supporter) return "This user is a <cp>Supporter</c> of ![ico](frame:cubicstudios.horriblemenu/icon.png?scale=0.35) <co>[Horrible Menu](mod:cubicstudios.horriblemenu)</c>. They've subscribed to support Breakeode monthly on their Ko-fi page.";

        return "No description available for this badge... Sorry!";
    };

    class BadgeManager final : public base::Singleton<BadgeManager> {
    private:
        std::unordered_map<int, std::string> m_badges;

    public:
        void getBadge(int accountID, CopyableFunction<void(Result<std::string>)>&& callback) {
            if (auto it = m_badges.find(accountID); it != m_badges.end()) return callback(Ok(it->second));

            async::spawn(
                web::WebRequest().get(fmt::format("https://api.cubicstudios.xyz/breakeode/v1/horrible/badges/user?id={}", accountID)),
                [this, accountID, cb = std::move(callback)](web::WebResponse res) {
                    auto const fallback = [this, &cb](std::string_view err = "") {
                        log::error("Badges web request failed ({})", err);
                        return cb(Err(err));
                    };

                    if (res.error()) return fallback(res.errorMessage());

                    auto jsonRes = res.json();
                    if (jsonRes.isErr()) return fallback(std::move(jsonRes).unwrapErr());

                    auto json = std::move(jsonRes).unwrap();

                    auto badgeRes = json["badge"].asString();
                    if (badgeRes.isErr()) return fallback(std::move(badgeRes).unwrapErr());

                    auto badge = std::move(badgeRes).unwrap();
                    m_badges[accountID] = badge;

                    return cb(Ok(std::move(badge)));
                });
        };

        void addBadge(Badge const& badge, Ref<CCNode> const& target, Result<std::string> badgeRes) {
            if (badgeRes.isErr()) return;
            if (!target) return;

            auto const id = std::move(badgeRes).unwrap();
            log::debug("Comparing retrieved badge {} with {}", id, badge.badgeID);
            if (fmt::format("{}"_spr, id) != badge.badgeID) return;

            showBadge(badge, CCSprite::createWithSpriteFrameName(getSpriteForBadge(id)));
        };
    };

    static void handleBadge(Badge const& badge) {
        log::trace("Showing badge for {}", badge.user->m_userName);

        if (auto bm = BadgeManager::get()) bm->getBadge(
            badge.user->m_accountID,
            [bm, badge, &target = badge.target](Result<std::string> res) {
                bm->addBadge(badge, target, std::move(res));
            });
    };

    static void addManualBadge(int id, CCNode* menu, float size = 21.5f) {
        badges::BadgeManager::get()->getBadge(id, [size, menu = WeakRef(menu)](Result<std::string> badgeRes) {
            if (badgeRes.isErr()) return;

            if (auto m = menu.lock()) {
                auto const id = std::move(badgeRes).unwrap();

                auto badgeSpr = CCSprite::createWithSpriteFrameName(badges::getSpriteForBadge(id));
                cue::rescaleToMatch(badgeSpr, size);

                auto badge = CCMenuItemExt::createSpriteExtra(
                    badgeSpr,
                    [id](auto) {
                        MDPopup::create(
                            "Horrible Menu",
                            badges::getDescForBadge(id),
                            "OK")
                            ->show();
                    });
                badge->setID(fmt::format("horrible-menu-{}-badge", id));

                m->addChild(badge);
                m->updateLayout();
            };
        });
    };
};

namespace horrible::hooks {
    static void setup(auto& self) {
        utils::StringMap<std::shared_ptr<Hook>> const& hooks = self.m_hooks;

        auto loader = Loader::get();
        auto enable = loader->isModLoaded("geode.node-ids") && !loader->isModLoaded("dasshu.badgified");

        for (auto& hook : hooks | std::views::values) {
            hook->setAutoEnable(enable);
            (void)hook->toggle(enable);

            (void)self.setHookPriorityPre(hook->getDisplayName(), Priority::VeryEarlyPre);

            g_badgeHooks.push_back(hook);
        };
    };
};

$on_game(ModsLoaded) {
    auto enableBadgified = dasshu::badgified::isLoaded();

    if (enableBadgified) {
        registerBadge(
            "lead-dev"_spr,
            "Horrible Menu Lead Developer",
            badges::getDescForBadge(badges::lead),
            [](Badge const& badge) {
                badges::handleBadge(badge);
            });

        registerBadge(
            "dev"_spr,
            "Horrible Menu Developer",
            badges::getDescForBadge(badges::dev),
            [](Badge const& badge) {
                badges::handleBadge(badge);
            });

        registerBadge(
            "contributor"_spr,
            "Horrible Menu Contributor",
            badges::getDescForBadge(badges::contributor),
            [](Badge const& badge) {
                badges::handleBadge(badge);
            });

        registerBadge(
            "supporter"_spr,
            "Horrible Menu Supporter",
            badges::getDescForBadge(badges::supporter),
            [](Badge const& badge) {
                badges::handleBadge(badge);
            });
    };

    auto to = Loader::get()->isModLoaded("geode.node-ids") && !enableBadgified;

    for (auto const& hook : g_badgeHooks) {
        if (auto h = hook.lock()) {
            log::trace("Toggling badge hook '{}' {}...", h->getDisplayName(), str::isOnOff(to));
            (void)h->toggle(to);
        };
    };
};

#define HORRIBLE_HOOK_BADGES           \
    static void onModify(auto& self) { \
        hooks::setup(self);            \
    }

class $modify(HMProfilePage, ProfilePage) {
    HORRIBLE_HOOK_BADGES;

    void loadPageFromUserInfo(GJUserScore* score) {
        ProfilePage::loadPageFromUserInfo(score);

        if (auto menu = m_mainLayer->getChildByID("username-menu")) badges::addManualBadge(score->m_accountID, menu);
    };
};

class $modify(HMCommentCell, CommentCell) {
    HORRIBLE_HOOK_BADGES;

    void loadFromComment(GJComment* comment) {
        CommentCell::loadFromComment(comment);

        if (auto menu = m_mainLayer->querySelector("main-menu > user-menu > username-menu")) badges::addManualBadge(comment->m_accountID, menu, 15.f);
    };
};