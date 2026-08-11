#include <Util.h>

#include <util/Base.h>

#include <dasshu.badgified/include/Badgified.hpp>

using namespace geode::prelude;
using namespace horrible::prelude;
using namespace dasshu::badgified;

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

    class BadgeManager final : public base::Singleton<BadgeManager> {
    private:
        std::unordered_map<int, std::string> m_badges;

    public:
        void getBadge(int accountID, CopyableFunction<void(Result<std::string>)>&& callback) {
            if (auto it = m_badges.find(accountID); it != m_badges.end()) return callback(Ok(it->second));

            async::spawn(
                web::WebRequest().get(fmt::format("https://api.cubicstudios.xyz/breakeode/v1/horrible/badges/user?id={}", accountID)),
                [this, cb = std::move(callback)](web::WebResponse res) {
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

                    return cb(Ok(std::move(badgeRes).unwrap()));
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
};

$on_game(ModsLoaded) {
    if (dasshu::badgified::isLoaded()) {
        registerBadge(
            "lead-dev"_spr,
            "Horrible Menu Lead Developer",
            "This user, a member of <cf>[Team Breakeode](https://breakeode.cubicstudios.xyz/)</c>, is a <cy>Lead Developer</c> of ![ico](frame:cubicstudios.horriblemenu/icon.png?scale=0.35) <co>[Horrible Menu](mod:cubicstudios.horriblemenu)</c>. They coordinate development, review contributions, and oversee the overall direction of the mod.",
            [](Badge const& badge) {
                badges::handleBadge(badge);
            });

        registerBadge(
            "dev"_spr,
            "Horrible Menu Developer",
            "This user, a member of <cf>[Team Breakeode](https://breakeode.cubicstudios.xyz/)</c>, is an active <cb>Developer</c> of ![ico](frame:cubicstudios.horriblemenu/icon.png?scale=0.35) <co>[Horrible Menu](mod:cubicstudios.horriblemenu)</c>. They currently help maintain the codebase for the mod.",
            [](Badge const& badge) {
                badges::handleBadge(badge);
            });

        registerBadge(
            "contributor"_spr,
            "Horrible Menu Contributor",
            "This user is a <cj>Contributor</c> to ![ico](frame:cubicstudios.horriblemenu/icon.png?scale=0.35) <co>[Horrible Menu](mod:cubicstudios.horriblemenu)</c>. They've assisted development by contributing code to the mod's codebase.",
            [](Badge const& badge) {
                badges::handleBadge(badge);
            });

        registerBadge(
            "supporter"_spr,
            "Horrible Menu Supporter",
            "This user is a <cp>Supporter</c> of ![ico](frame:cubicstudios.horriblemenu/icon.png?scale=0.35) <co>[Horrible Menu](mod:cubicstudios.horriblemenu)</c>. They've subscribed to support Breakeode monthly on their Ko-fi page.",
            [](Badge const& badge) {
                badges::handleBadge(badge);
            });
    };
};