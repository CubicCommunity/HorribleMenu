#pragma once

namespace horrible {
    namespace util {
        // Cursor utils
        namespace cursor {
            // Show the mouse cursor in-game
            void show();

            // Optionally hide the mouse cursor in-game
            void hide(bool force = false);
        };
    };
};