#include "board.hpp"
#include <algorithm>
#include <string>
#include <unordered_map>
#include <utility>

void chess::Board::precompute_move_data() {
  for (int rank = 0; rank < 8; ++rank) {
    for (int file = 0; file < 8; ++file) {
      const int num_up = rank;
      const int num_down = 7 - rank;
      const int num_left = file;
      const int num_right = 7 - file;

      m_squares_to_edge[rank * 8 + file][0] = num_up;
      m_squares_to_edge[rank * 8 + file][1] = num_down;
      m_squares_to_edge[rank * 8 + file][2] = num_left;
      m_squares_to_edge[rank * 8 + file][3] = num_right;
      m_squares_to_edge[rank * 8 + file][4] = std::min(num_up, num_right);
      m_squares_to_edge[rank * 8 + file][5] = std::min(num_down, num_left);
      m_squares_to_edge[rank * 8 + file][6] = std::min(num_down, num_right);
      m_squares_to_edge[rank * 8 + file][7] = std::min(num_up, num_left);
    }
  }
}

chess::Board::Board(std::string_view fen) {
  parse_board_from_fen(fen);
  precompute_move_data();
}

void chess::Board::draw_piece(int piece, raylib::Vector2 pos) {
  using namespace chess::pieces;
  static std::unordered_map<int, raylib::Texture> cache;
  static std::unordered_map<int, std::string> paths = {
      {WHITE | PAWN, "bin/white-pawn.png"},
      {WHITE | KNIGHT, "bin/white-knight.png"},
      {WHITE | BISHOP, "bin/white-bishop.png"},
      {WHITE | ROOK, "bin/white-rook.png"},
      {WHITE | QUEEN, "bin/white-queen.png"},
      {WHITE | KING, "bin/white-king.png"},

      {BLACK | PAWN, "bin/black-pawn.png"},
      {BLACK | KNIGHT, "bin/black-knight.png"},
      {BLACK | BISHOP, "bin/black-bishop.png"},
      {BLACK | ROOK, "bin/black-rook.png"},
      {BLACK | QUEEN, "bin/black-queen.png"},
      {BLACK | KING, "bin/black-king.png"}};

  if (piece == NONE) {
    return;
  } else if (cache.find(piece) == cache.end()) {
    cache[piece] = raylib::Texture(paths[piece]);
  }
  cache[piece].Draw(pos);
}

void chess::Board::draw() {
  for (int rank = 0; rank < 8; ++rank) {
    for (int file = 0, pos = rank * 8; file < 8; ++file, ++pos) {
      raylib::Rectangle rect{static_cast<float>(file) * SQUARE_SIZE,
                             static_cast<float>(rank) * SQUARE_SIZE,
                             static_cast<float>(SQUARE_SIZE),
                             static_cast<float>(SQUARE_SIZE)};

      if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
          rect.CheckCollision(GetMousePosition())) {
        if (m_possible_moves.find(pos) !=
            m_possible_moves.end()) { // then we are moving piece

          if ((m_squares[m_selected_piece_square] & pieces::PAWN) != 0 &&
              pos == m_en_passant_target_square) {
            if (m_turn == pieces::WHITE) {
              m_squares[m_en_passant_target_square + 8] = pieces::NONE;
            } else {
              m_squares[m_en_passant_target_square - 8] = pieces::NONE;
            }
            m_en_passant_target_square = -1;
          } else if ((m_squares[m_selected_piece_square] & pieces::PAWN) != 0 &&
                     std::abs(pos - m_selected_piece_square) == 16) {
            if (m_turn == pieces::WHITE) {
              m_en_passant_target_square = m_selected_piece_square - 8;
            } else {
              m_en_passant_target_square = m_selected_piece_square + 8;
            }
          } else {
            m_en_passant_target_square = -1;
          }

          const int selected_piece =
              m_squares[m_selected_piece_square] & ~m_turn;
          if (selected_piece == pieces::ROOK ||
              selected_piece == pieces::KING) {
            m_kingside_castle[m_turn != pieces::BLACK] = false;
            m_queenside_castle[m_turn != pieces::BLACK] = false;
          }

          m_squares[pos] =
              std::exchange(m_squares[m_selected_piece_square], pieces::NONE);

          m_selected_piece_square = -1;
          m_possible_moves.clear();

          if (m_turn == pieces::BLACK)
            m_turn = pieces::WHITE;
          else
            m_turn = pieces::BLACK;

        } else { // then we are watching piece moves
          m_selected_piece_square = pos;
          const auto &moves = generate_moves(pos);
          m_possible_moves =
              decltype(m_possible_moves)(moves.begin(), moves.end());
        }
      }

      int square_color = (rank + file) % 2 == 1 ? raylib::Color::White()
                                                : raylib::Color::Gray();
      if (m_possible_moves.find(pos) != m_possible_moves.end()) {
        square_color = raylib::Color::Red();
      }
      rect.Draw(raylib::Color(square_color));

      draw_piece(m_squares[pos],
                 raylib::Vector2{static_cast<float>(file) * SQUARE_SIZE,
                                 static_cast<float>(rank) * SQUARE_SIZE});
    }
  }
}