#include "board.hpp"
#include "raylib.h"
#include <utility>

std::unordered_set<int>
chess::Board::generate_pawn_moves(int from, bool gen_threats) const {
  const int file = from % 8;
  const int rank = from / 8;

  std::unordered_set<int> moves;

  switch (m_squares[from] & pieces::WHITE) {
  case pieces::WHITE:
    if (rank == 6 && m_squares[40 + file] == pieces::NONE &&
        m_squares[32 + file] == pieces::NONE && !gen_threats) {
      moves.insert(32 + file);
    }
    if (from - 8 > 0 && m_squares[from - 8] == pieces::NONE && !gen_threats) {
      moves.insert(from - 8);
    }

    if (file != 7 && from - 7 > 0 &&
        ((m_squares[from - 7] & pieces::BLACK) != 0 ||
         from - 7 == m_en_passant_target_square || gen_threats)) {
      moves.insert(from - 7);
    }
    if (file != 0 && from - 9 > 0 &&
        ((m_squares[from - 9] & pieces::BLACK) != 0 ||
         from - 9 == m_en_passant_target_square || gen_threats)) {
      moves.insert(from - 9);
    }

    break;

  case 0:
    if (rank == 1 && m_squares[16 + file] == pieces::NONE &&
        m_squares[24 + file] == pieces::NONE && !gen_threats) {
      moves.insert(24 + file);
    }
    if (from + 8 < 64 && m_squares[from + 8] == pieces::NONE && !gen_threats) {
      moves.insert(from + 8);
    }

    if (file != 0 && from + 7 < 64 &&
        ((m_squares[from + 7] & pieces::WHITE) != 0 ||
         from + 7 == m_en_passant_target_square || gen_threats)) {
      moves.insert(from + 7);
    }
    if (file != 7 && from + 9 < 64 &&
        ((m_squares[from + 9] & pieces::WHITE) != 0 ||
         from + 9 == m_en_passant_target_square || gen_threats)) {
      moves.insert(from + 9);
    }

    break;

  default:
    std::unreachable();
  }

  return moves;
}

std::unordered_set<int>
chess::Board::generate_knight_moves(int from, bool gen_threats) const {
  const int file = from % 8;
  const int rank = from / 8;

  std::unordered_set<int> moves;
  constexpr std::array<std::array<int, 2>, 8> shifts = {
      {{-2, -1}, {-2, 1}, {-1, 2}, {1, 2}, {2, 1}, {2, -1}, {1, -2}, {-1, -2}}};
  for (const auto [dy, dx] : shifts) {
    if (0 <= rank + dy && rank + dy < 8 && 0 <= file + dx && file + dx < 8 &&
        ((m_squares[(rank + dy) * 8 + file + dx] & m_turn) == 0 ||
         gen_threats)) {
      moves.insert((rank + dy) * 8 + file + dx);
    }
  }
  return moves;
}

std::unordered_set<int>
chess::Board::generate_sliding_piece_moves(int from, bool gen_threats) const {
  std::unordered_set<int> moves;

  int start_index = 0, end_index = 8;
  int sq_piece = m_squares[from] & ~m_turn;

  if (sq_piece == pieces::BISHOP) {
    start_index = 4;
  }

  if (sq_piece == pieces::ROOK) {
    end_index = 4;
  }

  for (int direction_index = start_index; direction_index < end_index;
       ++direction_index) {
    for (int i = 0; i < m_squares_to_edge[from][direction_index]; ++i) {
      const int target_pos =
          from + m_direction_offsets[direction_index] * (i + 1);
      if (target_pos > 63) {
        break;
      }

      if ((m_squares[target_pos] & m_turn) != 0) {
        if (gen_threats) {
          moves.insert(target_pos);
        }
        break;
      }

      moves.insert(target_pos);

      if ((m_squares[target_pos] &
           (m_turn == pieces::WHITE ? pieces::BLACK : pieces::WHITE)) != 0) {
        break;
      }
    }
  }
  return moves;
}

std::unordered_set<int>
chess::Board::generate_king_moves(int from, bool gen_threats) const {
  const int file = from % 8;
  const int rank = from / 8;

  std::unordered_set<int> moves;
  constexpr std::array<std::array<int, 2>, 8> shifts = {
      {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}}};
  for (const auto [dy, dx] : shifts) {
    if (0 <= rank + dy && rank + dy < 8 && 0 <= file + dx && file + dx < 8 &&
        ((m_squares[(rank + dy) * 8 + file + dx] & m_turn) == 0 ||
         gen_threats)) {
      moves.insert((rank + dy) * 8 + file + dx);
    }
  }

  if (m_kingside_castle[m_turn != pieces::BLACK] &&
      m_squares[from + 1] == pieces::NONE &&
      m_squares[from + 2] == pieces::NONE && !m_checked_squares[from + 1] &&
      !m_checked_squares[from + 2] &&
      ((m_squares[from + 3] & ~m_turn) == pieces::ROOK) && !king_checked()) {
    moves.insert(from + 2);
  }
  if (m_kingside_castle[m_turn != pieces::BLACK] &&
      m_squares[from - 1] == pieces::NONE &&
      m_squares[from - 2] == pieces::NONE &&
      m_squares[from - 3] == pieces::NONE && !m_checked_squares[from - 1] &&
      !m_checked_squares[from - 2] &&
      ((m_squares[from + 3] & ~m_turn) == pieces::ROOK) && !king_checked()) {
    moves.insert(from - 2);
  }

  return moves;
}

std::unordered_set<int> chess::Board::generate_moves(int from,
                                                     bool gen_threats) {
  if ((m_squares[from] & m_turn) == 0) {
    return {};
  }

  std::unordered_set<int> possible_moves_candidates;
  switch (m_squares[from] & ~m_turn) {
  case pieces::PAWN:
    possible_moves_candidates = generate_pawn_moves(from, gen_threats);
    break;
  case pieces::KNIGHT:
    possible_moves_candidates = generate_knight_moves(from, gen_threats);
    break;
  case pieces::BISHOP:
  case pieces::ROOK:
  case pieces::QUEEN:
    possible_moves_candidates = generate_sliding_piece_moves(from, gen_threats);
    break;
  case pieces::KING:
    possible_moves_candidates = generate_king_moves(from, gen_threats);
    break;
  default:
    std::unreachable();
  }

  if (gen_threats) {
    return possible_moves_candidates;
  }

  std::unordered_set<int> possible_moves;
  for (int to : possible_moves_candidates) {
    make_move(from, to);
    fill_checked_squares();
    toggle_turn();
    if (!king_checked()) {
      possible_moves.insert(to);
    } else {
      TraceLog(LOG_INFO, "Impossible move: %d %d", from, to);
    }
    unmake_move();
  }

  return possible_moves;
}
