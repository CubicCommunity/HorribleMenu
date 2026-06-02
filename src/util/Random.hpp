#pragma once

#include <Geode/utils/random.hpp>

namespace horrible {
    namespace util {
        // Random number generator utilities
        namespace rng {
            namespace internal = geode::utils::random;

            /**
             * Get a random number between `min` and `max`
             *
             * @param max The maximum number to get
             * @param min The minimum number to get
             */
            template <
                typename T = int,
                typename = std::enable_if_t<std::is_arithmetic_v<T>>>
            inline T get(T max, T min = static_cast<T>(0)) {
                if constexpr (std::is_integral_v<T>) return internal::generate<T>(min, max + static_cast<T>(1));
                return internal::generate<T>(min, max);
            };

            // Get a random bool
            inline bool flip() {
                return internal::generate<bool>();
            };

            // Get a chance bool
            inline bool chance(uint8_t percent) {
                return get(100) <= percent;
            };

            // Get any integer between 0 and 100
            inline uint8_t fast() {
                return get<uint8_t>(100, 0);
            };

            /**
             * Get any percent decimal between 0 and 1
             *
             * @param precision The amount of decimal places to account for
             */
            inline float pc(uint8_t precision = 2) {
                auto factor = std::powf(10.f, precision);
                return std::round(get(1.f) * factor) / factor;
            };
        };
    };
};