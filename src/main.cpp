#include "constants.hpp"
#include "game.hpp"
#include <memory>
#include <raylib-cpp.hpp>

int main() {
  SetTraceLogLevel(LOG_WARNING);
  raylib::Window window(chess::BOARD_WIDTH, chess::BOARD_HEIGHT,
                        chess::WINDOW_TITLE.data(),
                        FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
  window.SetMinSize(chess::BOARD_WIDTH, chess::BOARD_HEIGHT);
  SetTargetFPS(chess::FPS);

  auto game = std::make_unique<chess::Game>();
  while (!window.ShouldClose()) {
    window.BeginDrawing();
    if (IsKeyPressed(KEY_SPACE)) {
      // board->unmake_last_move();
    }
    game->draw_board();
    if (const auto s = game->state(); s != chess::Board::State::PLAYING) {
      TraceLog(LOG_WARNING, s == chess::Board::State::MATE ? "Mate." : "Draw.");
      window.EndDrawing();
      break;
    }
    window.EndDrawing();
  }
}
