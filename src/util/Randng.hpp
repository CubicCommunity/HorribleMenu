#pragma once

#include <Geode/utils/random.hpp>

namespace horrible {
    namespace util {
        // Random number generator utilities
        namespace randng {
            /**
             * Get a random number between `min` and `max`
             *
             * @param max The maximum number to get
             * @param min The minimum number to get
             */
            template <
                typename T = int,
                typename = std::enable_if_t<std::is_arithmetic_v<T>>
            >
            inline T get(T max, T min = static_cast<T>(0)) {
                return geode::utils::random::generate<T>(min, max + static_cast<T>(1));
            };

            /**
             * Get any number between 0 and 3000
             * @note Recommended to balance chances when calling every frame
             */
            template <
                typename T = int,
                typename = std::enable_if_t<std::is_integral_v<T>>
            >
            inline T tiny() {
                return get<T>(static_cast<T>(3000), static_cast<T>(0));
            };

            // Get any number between 0 and 100
            template <
                typename T = int,
                typename = std::enable_if_t<std::is_integral_v<T>>
            >
            inline T fast() {
                return get<T>(static_cast<T>(100), static_cast<T>(0));
            };

            // Get any percent decimal between 0 and 1
            template <
                typename F = float,
                typename = std::enable_if_t<std::is_floating_point_v<F>>
            >
            inline F pc() {
                return static_cast<F>(fast<int>()) / static_cast<F>(100);
            };
        };
    };
};