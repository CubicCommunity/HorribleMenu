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
                typename = std::enable_if_t<std::is_arithmetic_v<T>>>
            inline T get(T max, T min = static_cast<T>(0)) {
                if constexpr (std::is_integral_v<T>) {
                    return geode::utils::random::generate<T>(min, max + static_cast<T>(1));
                } else {
                    return geode::utils::random::generate<T>(min, max);
                };
            };

            /**
             * Get any number between 0 and 2048
             * @note Recommended to balance chances when calling every frame
             */
            inline uint16_t tiny() {
                return get<uint16_t>(2048, 0);
            };

            // Get any number between 0 and 100
            inline uint8_t fast() {
                return get<uint8_t>(100, 0);
            };

            /**
             * Get any percent decimal between 0 and 1
             *
             * @param max The maximum number to get
             * @param min The minimum number to get
             */
            inline float pc(uint8_t precision = 2) {
                auto factor = static_cast<float>(std::pow(10, precision));
                return std::round(get(1.f) * factor) / factor;
            };
        };
    };
};