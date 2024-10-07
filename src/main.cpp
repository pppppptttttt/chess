#include "board.hpp"
#include "constants.hpp"
#include "raylib.h"
#include <memory>
#include <raylib-cpp.hpp>

#if 0

#ifdef WHITE
#undef WHITE
#endif // WHITE

#ifdef BLACK
#undef BLACK
#endif // BLACK

#endif

int main() {
  raylib::Window window(chess::WINDOW_WIDTH, chess::WINDOW_HEIGHT,
                        chess::WINDOW_TITLE.data(), FLAG_MSAA_4X_HINT);
  SetTargetFPS(chess::FPS);

  auto board = std::make_unique<chess::Board>();
  while (!window.ShouldClose()) {
    window.BeginDrawing();
    board->draw();
    if (board->mate()) {
      TraceLog(LOG_INFO, "Mate.");
      window.EndDrawing();
      break;
    }
    window.EndDrawing();
  }
}
