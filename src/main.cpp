#include "board.hpp"
#include "constants.hpp"
#include <memory>
#include <raylib-cpp.hpp>

int main() {
  SetTraceLogLevel(LOG_WARNING);
  raylib::Window window(chess::BOARD_WIDTH, chess::BOARD_HEIGHT,
                        chess::WINDOW_TITLE.data(),
                        FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
  window.SetMinSize(chess::BOARD_WIDTH, chess::BOARD_HEIGHT);
  SetTargetFPS(chess::FPS);

  auto board = std::make_unique<chess::Board>();
  while (!window.ShouldClose()) {
    window.BeginDrawing();
    board->draw();
    if (const auto s = board->game_state(); s != chess::Board::State::PLAYING) {
      TraceLog(LOG_WARNING, s == chess::Board::State::MATE ? "Mate." : "Draw.");
      window.EndDrawing();
      break;
    }
    window.EndDrawing();
  }
}
