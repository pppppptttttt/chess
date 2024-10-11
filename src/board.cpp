#include "board.hpp"
#include "src/pieces.hpp"
#include <algorithm>
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
  toggle_turn();
  fill_checked_squares();
  toggle_turn();
  ++m_positions_counter[to_fen()];
  m_history.push(*this);
}

void chess::Board::make_move(int from, int to) {
  const int selected_piece = m_squares[from] & ~m_turn;

  if (selected_piece == pieces::PAWN ||
      (m_squares[to] != pieces::NONE &&
       (m_squares[from] >> 3) != (m_squares[to] >> 3))) {
    m_halfmoves_50rule_count = 0;
  } else {
    ++m_halfmoves_50rule_count;
  }

  // en passant
  if (selected_piece == pieces::PAWN && to == m_en_passant_target_square) {
    if (m_turn == pieces::WHITE) {
      m_squares[m_en_passant_target_square + 8] = pieces::NONE;
    } else {
      m_squares[m_en_passant_target_square - 8] = pieces::NONE;
    }
    m_en_passant_target_square = -1;
  } else if (selected_piece == pieces::PAWN && std::abs(to - from) == 16) {
    if (m_turn == pieces::WHITE) {
      m_en_passant_target_square = from - 8;
    } else {
      m_en_passant_target_square = from + 8;
    }
  } else {
    m_en_passant_target_square = -1;
  }

  // castling abilities
  if (selected_piece == pieces::KING) {
    m_kingside_castle[m_turn != pieces::BLACK] = false;
    m_queenside_castle[m_turn != pieces::BLACK] = false;
  } else if (selected_piece == pieces::ROOK) {
    if (from % 8 == 7) {
      m_kingside_castle[m_turn != pieces::BLACK] = false;
    } else {
      m_queenside_castle[m_turn != pieces::BLACK] = false;
    }
  }

  // castling
  if (selected_piece == pieces::KING && to - from == 2) {
    std::swap(m_squares[to - 1], m_squares[to + 1]);
  } else if (selected_piece == pieces::KING && to - from == -2) {
    std::swap(m_squares[to - 2], m_squares[to + 1]);
  }

  if (selected_piece == pieces::KING) {
    m_king_pos[m_turn != pieces::BLACK] = to;
  }

  m_squares[to] = std::exchange(m_squares[from], pieces::NONE);

  // promotion (TODO: not only queen)
  if ((to / 8 == 7 || to / 8 == 0) && selected_piece == pieces::PAWN) {
    m_squares[to] = m_turn | pieces::QUEEN;
  }

  m_selected_piece_square = -1;

  if (m_turn == pieces::BLACK) {
    ++m_moves_count;
  }

  std::string board_string_repr = to_fen();
  auto it = board_string_repr.find_first_of(' ');
  board_string_repr =
      std::string(board_string_repr.begin(), board_string_repr.begin() + it);
  m_position_occured_max_count = std::max(
      ++m_positions_counter[board_string_repr], m_position_occured_max_count);
  fill_checked_squares();
  toggle_turn();
  m_history.push(*this);
}

void chess::Board::unmake_move() {
  if (m_history.size() <= 1) {
    return;
  }
  m_history.pop();
  *this = m_history.top();
}

chess::Board::State chess::Board::game_state() {
  if (m_halfmoves_50rule_count >= 100 || m_position_occured_max_count >= 3) {
    return State::DRAW;
  }

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
