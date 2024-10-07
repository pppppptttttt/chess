#include "board.hpp"
#include "constants.hpp"
#include "raylib.h"
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
  chess::Board board{
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"};
  while (!window.ShouldClose()) {
    window.BeginDrawing();
    board.draw();
    window.EndDrawing();
  }
}
