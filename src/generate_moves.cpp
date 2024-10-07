#include "board.hpp"
#include "src/pieces.hpp"
#include <utility>

std::vector<int> chess::Board::generate_pawn_moves(int from_pos,
                                                   bool gen_threats) const {
  const int file = from_pos % 8;
  const int rank = from_pos / 8;

  std::vector<int> moves;

  switch (m_squares[from_pos] & pieces::WHITE) {
  case pieces::WHITE:
    if (rank == 6 && m_squares[40 + file] == pieces::NONE &&
        m_squares[32 + file] == pieces::NONE && !gen_threats) {
      moves.push_back(32 + file);
    }
    if (from_pos - 8 > 0 && m_squares[from_pos - 8] == pieces::NONE &&
        !gen_threats) {
      moves.push_back(from_pos - 8);
    }

    if (file != 7 && from_pos - 7 > 0 &&
        ((m_squares[from_pos - 7] & pieces::BLACK) != 0 ||
         from_pos - 7 == m_en_passant_target_square || gen_threats)) {
      moves.push_back(from_pos - 7);
    }
    if (file != 0 && from_pos - 9 > 0 &&
        ((m_squares[from_pos - 9] & pieces::BLACK) != 0 ||
         from_pos - 9 == m_en_passant_target_square || gen_threats)) {
      moves.push_back(from_pos - 9);
    }

    break;

  case 0:
    if (rank == 1 && m_squares[16 + file] == pieces::NONE &&
        m_squares[24 + file] == pieces::NONE && !gen_threats) {
      moves.push_back(24 + file);
    }
    if (from_pos + 8 < 64 && m_squares[from_pos + 8] == pieces::NONE &&
        !gen_threats) {
      moves.push_back(from_pos + 8);
    }

    if (file != 0 && from_pos + 7 < 64 &&
        ((m_squares[from_pos + 7] & pieces::WHITE) != 0 ||
         from_pos + 7 == m_en_passant_target_square || gen_threats)) {
      moves.push_back(from_pos + 7);
    }
    if (file != 7 && from_pos + 9 < 64 &&
        ((m_squares[from_pos + 9] & pieces::WHITE) != 0 ||
         from_pos + 9 == m_en_passant_target_square || gen_threats)) {
      moves.push_back(from_pos + 9);
    }

    break;

  default:
    std::unreachable();
  }

  return moves;
}

std::vector<int> chess::Board::generate_knight_moves(int from_pos,
                                                     bool gen_threats) const {
  const int file = from_pos % 8;
  const int rank = from_pos / 8;

  std::vector<int> moves;
  constexpr int shifts[][2] = {{-2, -1}, {-2, 1}, {-1, 2}, {1, 2},
                               {2, 1},   {2, -1}, {1, -2}, {-1, -2}};
  for (const auto [dy, dx] : shifts) {
    if (0 <= rank + dy && rank + dy < 8 && 0 <= file + dx && file + dx < 8 &&
        ((m_squares[(rank + dy) * 8 + file + dx] & m_turn) == 0 ||
         gen_threats)) {
      moves.push_back((rank + dy) * 8 + file + dx);
    }
  }
  return moves;
}

std::vector<int>
chess::Board::generate_sliding_piece_moves(int from_pos,
                                           bool gen_threats) const {
  std::vector<int> moves;

  int start_index = 0, end_index = 8;
  int sq_piece = m_squares[from_pos] & ~m_turn;

  if (sq_piece == pieces::BISHOP) {
    start_index = 4;
  }

  if (sq_piece == pieces::ROOK) {
    end_index = 4;
  }

  for (int direction_index = start_index; direction_index < end_index;
       ++direction_index) {
    for (int i = 0; i < m_squares_to_edge[from_pos][direction_index]; ++i) {
      const int target_pos =
          from_pos + m_direction_offsets[direction_index] * (i + 1);
      if (target_pos > 63) {
        break;
      }

      if ((m_squares[target_pos] & m_turn) != 0) {
        if (gen_threats) {
          moves.push_back(target_pos);
        }
        break;
      }

      moves.push_back(target_pos);

      if ((m_squares[target_pos] &
           (m_turn == pieces::WHITE ? pieces::BLACK : pieces::WHITE)) != 0) {
        break;
      }
    }
  }
  return moves;
}

std::vector<int> chess::Board::generate_king_moves(int from_pos,
                                                   bool gen_threats) const {
  const int file = from_pos % 8;
  const int rank = from_pos / 8;

  std::vector<int> moves;
  constexpr int shifts[][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1},
                               {0, 1},   {1, -1}, {1, 0},  {1, 1}};
  for (const auto [dy, dx] : shifts) {
    if (0 <= rank + dy && rank + dy < 8 && 0 <= file + dx && file + dx < 8 &&
        ((m_squares[(rank + dy) * 8 + file + dx] & m_turn) == 0 ||
         gen_threats)) {
      moves.push_back((rank + dy) * 8 + file + dx);
    }
  }

  if (m_kingside_castle[m_turn != pieces::BLACK] &&
      m_squares[from_pos + 1] == pieces::NONE &&
      m_squares[from_pos + 2] == pieces::NONE) {
    moves.push_back(from_pos + 2);
  }
  if (m_kingside_castle[m_turn != pieces::BLACK] &&
      m_squares[from_pos - 1] == pieces::NONE &&
      m_squares[from_pos - 2] == pieces::NONE &&
      m_squares[from_pos - 3] == pieces::NONE) {
    moves.push_back(from_pos - 2);
  }

  return moves;
}

std::vector<int> chess::Board::generate_moves(int from_pos,
                                              bool gen_threats) {
  if ((m_squares[from_pos] & m_turn) == 0) {
    return {};
  }

  std::vector<int> possible_moves_candidates;
  switch (m_squares[from_pos] & ~m_turn) {
  case pieces::PAWN:
    possible_moves_candidates = generate_pawn_moves(from_pos, gen_threats);
    break;
  case pieces::KNIGHT:
    possible_moves_candidates = generate_knight_moves(from_pos, gen_threats);
    break;
  case pieces::BISHOP:
  case pieces::ROOK:
  case pieces::QUEEN:
    possible_moves_candidates = generate_sliding_piece_moves(from_pos, gen_threats);
    break;
  case pieces::KING:
    possible_moves_candidates = generate_king_moves(from_pos, gen_threats);
    break;
  default:
    std::unreachable();
  }

  if (gen_threats) {
    return possible_moves_candidates;
  }

  std::vector<int> possible_moves;
  for (int move : possible_moves_candidates) {
    const int save1 = m_squares[from_pos];
    const int save2 = m_squares[move];
    const int save3 = m_king_pos[m_turn == pieces::BLACK];
    auto current_checked_squares = m_checked_squares;

    if ((m_squares[from_pos] & ~m_turn) == pieces::KING) {
      m_king_pos[m_turn == pieces::BLACK] = move;
    }
    m_squares[move] = std::exchange(m_squares[from_pos], pieces::NONE);
    toggle_turn();
    fill_checked_squares();
    toggle_turn();
    if (!king_checked()) {
      possible_moves.push_back(move);
    } else {
      TraceLog(LOG_INFO, "Impossible move: %d %d", from_pos, move);
    }

    std::swap(current_checked_squares, m_checked_squares);
    m_squares[from_pos] = save1;
    m_squares[move] = save2;
    m_king_pos[m_turn == pieces::BLACK] = save3;
  }

  return possible_moves;
}
