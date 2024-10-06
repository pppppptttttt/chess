#include "board.hpp"
#include "constants.hpp"
#include <raylib-cpp.hpp>

#define dbg                                                                    \
  do {                                                                         \
    TraceLog(LOG_ERROR, "%s:%d", __FILE__, __LINE__);                          \
  } while (0);

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
                        chess::WINDOW_TITLE.data());

  SetTargetFPS(chess::FPS);
  // SetTraceLogLevel(LOG_WARNING);

  chess::Board board{
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"};
  while (!window.ShouldClose()) {
    window.BeginDrawing();
    board.draw();
    window.EndDrawing();
  }
}