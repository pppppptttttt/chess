#include "board.hpp"
#include "raylib.h"
#include "src/constants.hpp"
#include "src/pieces.hpp"
#include <algorithm>
#include <utility>

namespace {

constexpr bool draw_checked = false;

} // namespace

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

void chess::Board::load_textures() {
  using namespace pieces;
  for (int piece = pieces::PAWN; piece <= pieces::KING; ++piece) {
    int piecew = piece | pieces::WHITE;
    int pieceb = piece | pieces::BLACK;

    raylib::Image imgw = raylib::Image(m_textures_paths[piecew])
                             .Resize(SQUARE_SIZE, SQUARE_SIZE)
                             .Mipmaps();

    raylib::Image imgb = raylib::Image(m_textures_paths[pieceb])
                             .Resize(SQUARE_SIZE, SQUARE_SIZE)
                             .Mipmaps();

    m_piece_textures[piecew] = raylib::Texture(imgw);
    m_piece_textures[pieceb] = raylib::Texture(imgb);
  }
}

void chess::Board::fill_checked_squares() {
  m_checked_squares.fill(false);
  for (int pos = 0; pos < 64; ++pos) {
    for (int square : generate_moves(pos, true)) {
      m_checked_squares[square] = true;
    }
  }
}

chess::Board::Board(std::string_view fen) {
  parse_board_from_fen(fen);
  precompute_move_data();
  load_textures();
  toggle_turn();
  fill_checked_squares();
  toggle_turn();
}

void chess::Board::draw_piece(int piece, raylib::Vector2 pos) {
  m_piece_textures[piece].Draw(pos);
}

void chess::Board::make_move(int pos) {
  // en passant
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

  // castling
  const int selected_piece = m_squares[m_selected_piece_square] & ~m_turn;
  if (selected_piece == pieces::ROOK || selected_piece == pieces::KING) {
    m_kingside_castle[m_turn != pieces::BLACK] = false;
    m_queenside_castle[m_turn != pieces::BLACK] = false;
  }

  if (selected_piece == pieces::KING && pos - m_selected_piece_square == 2) {
    std::swap(m_squares[pos - 1], m_squares[pos + 1]);
  } else if (selected_piece == pieces::KING &&
             pos - m_selected_piece_square == -2) {
    std::swap(m_squares[pos - 2], m_squares[pos + 1]);
  }

  if (selected_piece == pieces::KING) {
    m_king_pos[m_turn == pieces::BLACK] = pos;
  }

  m_squares[pos] =
      std::exchange(m_squares[m_selected_piece_square], pieces::NONE);

  // promotion (TODO: not only queen)
  if ((pos / 8 == 7 || pos / 8 == 0) && selected_piece == pieces::PAWN) {
    m_squares[pos] = m_turn | pieces::QUEEN;
  }

  fill_checked_squares();
  m_selected_piece_square = -1;
  m_possible_moves.clear();
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
        if (m_possible_moves.find(pos) != m_possible_moves.end()) {
          make_move(pos);
          toggle_turn();
        } else {
          m_selected_piece_square = pos;
          const auto &moves = generate_moves(pos);
          m_possible_moves =
              decltype(m_possible_moves)(moves.begin(), moves.end());
        }
      }

      const int square_color = (rank + file) % 2 == 1 ? raylib::Color::White()
                                                      : raylib::Color::Gray();
      rect.Draw(raylib::Color(square_color));
      if constexpr (draw_checked) {
        if (m_checked_squares[pos]) {
          rect.Draw(raylib::Color(0, 255, 0, 120));
        }
      }

      if (m_possible_moves.find(pos) != m_possible_moves.end()) {
        (rect.GetPosition() + Vector2{SQUARE_SIZE / 2.0f, SQUARE_SIZE / 2.0f})
            .DrawCircle(SQUARE_SIZE / 6.0f, raylib::Color::DarkPurple());
      }

      draw_piece(m_squares[pos],
                 raylib::Vector2{static_cast<float>(file) * SQUARE_SIZE,
                                 static_cast<float>(rank) * SQUARE_SIZE});
    }
  }
}

chess::Board::State chess::Board::game_state() {
  const bool check = king_checked();

  bool legal_moves = false;
  for (int i = 0; i < 64; ++i) {
    if (!generate_moves(i).empty()) {
      legal_moves = true;
      break;
    }
  }

  if (!legal_moves) {
    return check ? State::MATE : State::DRAW;
  }
  return State::PLAYING;
}
