#include "board.hpp"
#include <utility>

std::vector<int> chess::Board::generate_pawn_moves(int from_pos) const {
  const int file = from_pos % 8;
  const int rank = from_pos / 8;

  std::vector<int> moves;

  switch (m_squares[from_pos] & pieces::WHITE) {
  case pieces::WHITE:
    if (rank == 6 && m_squares[40 + file] == pieces::NONE &&
        m_squares[32 + file] == pieces::NONE) {
      moves.push_back(32 + file);
    }
    if (from_pos - 8 > 0 && m_squares[from_pos - 8] == pieces::NONE) {
      moves.push_back(from_pos - 8);
    }

    if (file != 7 && from_pos - 7 > 0 &&
        ((m_squares[from_pos - 7] & pieces::BLACK) != 0 ||
         from_pos - 7 == m_en_passant_target_square)) {
      moves.push_back(from_pos - 7);
    }
    if (file != 0 && from_pos - 9 > 0 &&
        ((m_squares[from_pos - 9] & pieces::BLACK) != 0 ||
         from_pos - 9 == m_en_passant_target_square)) {
      moves.push_back(from_pos - 9);
    }

    break;

  case 0:
    if (rank == 1 && m_squares[16 + file] == pieces::NONE &&
        m_squares[24 + file] == pieces::NONE) {
      moves.push_back(24 + file);
    }
    if (from_pos + 8 < 64 && m_squares[from_pos + 8] == pieces::NONE) {
      moves.push_back(from_pos + 8);
    }

    if (file != 0 && from_pos + 7 < 64 &&
        ((m_squares[from_pos + 7] & pieces::WHITE) != 0 ||
         from_pos + 7 == m_en_passant_target_square)) {
      moves.push_back(from_pos + 7);
    }
    if (file != 7 && from_pos + 9 < 64 &&
        ((m_squares[from_pos + 9] & pieces::WHITE) != 0 ||
         from_pos + 9 == m_en_passant_target_square)) {
      moves.push_back(from_pos + 9);
    }

    break;

  default:
    std::unreachable();
  }

  return moves;
}

std::vector<int> chess::Board::generate_knight_moves(int from_pos) const {
  const int file = from_pos % 8;
  const int rank = from_pos / 8;

  std::vector<int> moves;
  constexpr int shifts[][2] = {{-2, -1}, {-2, 1}, {-1, 2}, {1, 2},
                               {2, 1},   {2, -1}, {1, -2}, {-1, -2}};
  for (const auto [dy, dx] : shifts) {
    if (0 <= rank + dy && rank + dy < 8 && 0 <= file + dx && file + dx < 8 &&
        (m_squares[(rank + dy) * 8 + file + dx] & m_turn) == 0) {
      moves.push_back((rank + dy) * 8 + file + dx);
    }
  }
  return moves;
}

std::vector<int>
chess::Board::generate_sliding_piece_moves(int from_pos) const {
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
        TraceLog(LOG_ERROR, "%d %d", from_pos, target_pos);
        TraceLog(LOG_INFO, "%d", m_squares_to_edge[from_pos][direction_index]);
        break;
      }

      if ((m_squares[target_pos] & m_turn) != 0) {
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

std::vector<int> chess::Board::generate_king_moves(int from_pos) const {
  const int file = from_pos % 8;
  const int rank = from_pos / 8;

  std::vector<int> moves;
  constexpr int shifts[][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1},
                               {0, 1},   {1, -1}, {1, 0},  {1, 1}};
  for (const auto [dy, dx] : shifts) {
    if (0 <= rank + dy && rank + dy < 8 && 0 <= file + dx && file + dx < 8 &&
        (m_squares[(rank + dy) * 8 + file + dx] & m_turn) == 0) {
      moves.push_back((rank + dy) * 8 + file + dx);
    }
  }
  return moves;
}

std::vector<int> chess::Board::generate_moves(int from_pos) const {
  if ((m_squares[from_pos] & m_turn) == 0) {
    return {};
  }

  switch (m_squares[from_pos] & ~m_turn) {
  case pieces::PAWN:
    return generate_pawn_moves(from_pos);
  case pieces::KNIGHT:
    return generate_knight_moves(from_pos);
  case pieces::BISHOP:
  case pieces::ROOK:
  case pieces::QUEEN:
    return generate_sliding_piece_moves(from_pos);
  case pieces::KING:
    return generate_king_moves(from_pos);
    break;
  default:
    std::unreachable();
  }

  return {};
}
