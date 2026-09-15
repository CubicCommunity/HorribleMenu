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
        if (id == supporter) return "This user is a <cp>Supporter</c> of <cf>[Team Breakeode](https://breakeode.cubicstudios.xyz/)</c>. They've subscribed to **support the developers monthly on their [Ko-fi page](https://ko-fi.com/breakeode/)**.";

        return "No description available for this badge... Sorry!";
    };

    using BadgeResult = Result<std::string>;
    using BadgeFuture = arc::Future<BadgeResult>;

    class BadgeManager final : public base::Singleton<BadgeManager> {
    private:
        asp::Mutex<std::unordered_map<int, std::string>> m_badges;

    public:
        BadgeFuture fetchBadge(int accountID, bool request = true) {
            {
                auto badges = m_badges.lock();
                if (auto const it = badges->find(accountID); it != badges->end()) co_return Ok(it->second);
            };

            if (!request) co_return Err("Badge not found in cache");

            auto res = co_await request::base().get(fmt::format("https://api.cubicstudios.xyz/breakeode/v1/horrible/badges/user?id={}", accountID));

            auto const fallback = [this](std::string err = "") {
                log::error("Badges web request failed ({})", err);
                return Err(std::move(err));
            };

            if (res.error()) co_return fallback(std::string{res.errorMessage()});

            auto jsonRes = res.json();
            if (jsonRes.isErr()) co_return fallback(std::move(jsonRes).unwrapErr());

            auto json = std::move(jsonRes).unwrap();

            auto badgeRes = json["badge"].asString();
            if (badgeRes.isErr()) co_return fallback(std::move(badgeRes).unwrapErr());

            auto badge = std::move(badgeRes).unwrap();

            {
                auto badges = m_badges.lock();
                (*badges)[accountID] = badge;
            };

            co_return Ok(std::move(badge));
        };

        void addBadge(Badge const& badge, BadgeResult badgeRes) {
            if (badgeRes.isErr()) return log::error("Couldn't show badge: {}", badgeRes.unwrapErr());

            if (!badge.user) return log::error("Badge user data is missing");
            if (!badge.target) return log::error("Badge target node is missing");

            auto const id = std::move(badgeRes).unwrap();
            log::debug("Comparing retrieved badge {} with {}", id, badge.badgeID);
            if (fmt::format("{}"_spr, id) != badge.badgeID) return log::error("Mismatching badge IDs");

            showBadge(badge, CCSprite::createWithSpriteFrameName(getSpriteForBadge(id)));
        };
    };

    static arc::Future<> handleBadge(Badge badge) {
        log::trace("Showing badge for {}", badge.user->m_userName);

        if (auto bm = BadgeManager::get()) {
            auto res = co_await bm->fetchBadge(badge.user->m_accountID, badge.location != Location::Comment);
            co_await async::waitForMainThread([bm, b = std::move(badge), r = std::move(res)]() {
                bm->addBadge(b, std::move(r));
            });
        };
    };

    static void addManualBadge(int id, CCNode* menu, bool request = true, float size = 21.5f) {
        if (auto bm = badges::BadgeManager::get()) async::spawn(
            bm->fetchBadge(id, request),
            [size, menu = WeakRef(menu)](BadgeResult badgeRes) {
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
        StringMap<std::shared_ptr<Hook>> const& hooks = self.m_hooks;

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
                async::spawn(badges::handleBadge(badge));
            });

        registerBadge(
            "dev"_spr,
            "Horrible Menu Developer",
            badges::getDescForBadge(badges::dev),
            [](Badge const& badge) {
                async::spawn(badges::handleBadge(badge));
            });

        registerBadge(
            "contributor"_spr,
            "Horrible Menu Contributor",
            badges::getDescForBadge(badges::contributor),
            [](Badge const& badge) {
                async::spawn(badges::handleBadge(badge));
            });

        registerBadge(
            "supporter"_spr,
            "Breakeode Supporter",
            badges::getDescForBadge(badges::supporter),
            [](Badge const& badge) {
                async::spawn(badges::handleBadge(badge));
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

        if (auto menu = m_mainLayer->querySelector("main-menu > user-menu > username-menu")) badges::addManualBadge(comment->m_accountID, menu, false, 15.f);
    };
};