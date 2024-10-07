#include "board.hpp"
#include "constants.hpp"
#include <memory>
#include <raylib-cpp.hpp>

int main() {
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
}
