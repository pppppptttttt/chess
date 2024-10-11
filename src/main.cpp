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

  // TODO: position with possible promotion on next move. Do not only queen.
  [[maybe_unused]] constexpr std::string_view bug =
      "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q2/PPPBBPpP/R3K2R b kq - 1 1";
  auto game = std::make_unique<chess::Game>();
  while (!window.ShouldClose()) {
    window.BeginDrawing();
    game->draw_board();
    if (IsKeyPressed(KEY_ENTER)) {
      if (const auto s = game->state(); s != chess::Board::State::PLAYING) {
        TraceLog(LOG_WARNING,
                 s == chess::Board::State::MATE ? "Mate." : "Draw.");
        window.EndDrawing();
        break;
      }
    }
    window.EndDrawing();
  }
}
