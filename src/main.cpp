#include "board.hpp"
#include "constants.hpp"
#include "raylib.h"
#include <memory>
#include <raylib-cpp.hpp>

int main() {
  SetTraceLogLevel(LOG_WARNING);
  raylib::Window window(chess::WINDOW_WIDTH, chess::WINDOW_HEIGHT,
                        chess::WINDOW_TITLE.data(), FLAG_MSAA_4X_HINT);
  SetTargetFPS(chess::FPS);

  auto board = std::make_unique<chess::Board>();
  while (!window.ShouldClose()) {
    window.BeginDrawing();
    board->draw();
    if (const auto s = board->game_state(); s != chess::Board::State::PLAYING) {
      TraceLog(LOG_INFO, s == chess::Board::State::MATE ? "Mate." : "Draw.");
      window.EndDrawing();
      break;
    }
    window.EndDrawing();
  }
  TraceLog(LOG_WARNING, "%s", board->to_fen().c_str());
}
