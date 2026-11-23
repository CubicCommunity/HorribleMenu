#pragma once

namespace cs::brkd::horrible {
    namespace util {
        // Cursor utils
        namespace cursor {
            void show();                    // Show the mouse cursor in-game
            void hide(bool force = false);  // Optionally hide the mouse cursor in-game
        };
    };
};