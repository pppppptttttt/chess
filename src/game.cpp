#include "game.hpp"
#include "constants.hpp"

void chess::Game::draw_board() {
  if (IsKeyPressed(KEY_SPACE)) {
    unmake_last_move();
  }

  for (int rank = 0; rank < 8; ++rank) {
    for (int file = 0, pos = rank * 8; file < 8; ++file, ++pos) {
      raylib::Rectangle rect{static_cast<float>(file) * SQUARE_SIZE,
                             static_cast<float>(rank) * SQUARE_SIZE,
                             static_cast<float>(SQUARE_SIZE),
                             static_cast<float>(SQUARE_SIZE)};

      if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
          rect.CheckCollision(GetMousePosition())) {
        if (m_possible_moves.find(pos) != m_possible_moves.end()) {
          chess::Board new_board = m_board_history.back();
          new_board.make_move(m_selected_piece_square, pos);
          m_board_history.push_back(new_board);
          m_possible_moves.clear();
        } else {
          m_selected_piece_square = pos;
          m_possible_moves = m_board_history.back().generate_moves(pos);
        }
      }

      auto &board = m_board_history.back();
      int square_color =
          (rank + file) % 2 == 1 ? m_black_square_color : m_white_square_color;
      if (pos == m_selected_piece_square) {
        square_color = m_circle_color;
      }
      rect.Draw(raylib::Color(square_color));
      if constexpr (m_draw_checked) {
        if (board.checked_squares()[pos]) {
          rect.Draw(raylib::Color(0, 255, 0, 120));
        }
      }

      m_piece_textures[board.square(pos)].Draw(
          raylib::Vector2{static_cast<float>(file) * SQUARE_SIZE,
                          static_cast<float>(rank) * SQUARE_SIZE});

      if (m_possible_moves.find(pos) != m_possible_moves.end()) {
        const auto center = (rect.GetPosition() +
                             Vector2{SQUARE_SIZE / 2.0f, SQUARE_SIZE / 2.0f});
        center.DrawCircle(SQUARE_SIZE / 7.0f, m_circle_color);
      }
    }
  }
}
