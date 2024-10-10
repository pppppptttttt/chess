#ifndef BOARD_HPP_
#define BOARD_HPP_

#include <raylib-cpp.hpp>
#include <array>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include "pieces.hpp"

namespace chess {

class Board {
private:
  std::array<int, 64> m_squares = {pieces::NONE};
  int m_turn = pieces::WHITE;
  int m_selected_piece_square = -1;
  int m_en_passant_target_square = -1;
  std::array<bool, 2> m_kingside_castle = {false, false};
  std::array<bool, 2> m_queenside_castle = {false, false};
  std::array<int, 2> m_king_pos = {0, 0};
  int m_moves_count = 0;
  int m_halfmoves_50rule_count = 0;

  // TODO: `zobrist hashing`, much faster.
  std::unordered_map<std::string, int> m_positions_counter;
  int m_position_occured_max_count = 1;

public:
  void make_move(int from, int to);
  std::unordered_set<int> generate_moves(int from_pos,
                                         bool gen_threats = false);

  const std::array<bool, 64> &checked_squares() const {
    return m_checked_squares;
  }

  int square(int pos) const { return m_squares[pos]; }

private:
  bool king_checked() const {
    return m_checked_squares[m_king_pos[m_turn != pieces::BLACK]];
  }

  std::unordered_set<int> generate_pawn_moves(int from_pos,
                                              bool gen_threats = false) const;
  std::unordered_set<int> generate_knight_moves(int from_pos,
                                                bool gen_threats = false) const;
  std::unordered_set<int> generate_king_moves(int from_pos,
                                              bool gen_threats = false) const;
  std::unordered_set<int>
  generate_sliding_piece_moves(int from_pos, bool gen_threats = false) const;

  void toggle_turn() {
    if (m_turn == pieces::BLACK) {
      m_turn = pieces::WHITE;
    } else {
      m_turn = pieces::BLACK;
    }
  }

  static constexpr std::array<int, 8> m_direction_offsets = {-8, 8, -1, 1,
                                                             -7, 7, 9,  -9};
  std::array<std::array<int, 8>, 64> m_squares_to_edge;
  std::array<bool, 64> m_checked_squares;

  void precompute_move_data();
  void parse_board_from_fen(std::string_view fen);
  void fill_checked_squares();

public:
  enum class State { PLAYING, MATE, DRAW };

  explicit Board(std::string_view fen);

  State game_state();
  std::string to_fen() const;
};

} // namespace chess

#endif // BOARD_HPP_
