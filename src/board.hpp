#ifndef BOARD_HPP_
#define BOARD_HPP_

#include <raylib-cpp.hpp>
#include <string_view>
#include <unordered_set>
#include <vector>
#include "constants.hpp"
#include "pieces.hpp"

namespace chess {

class Board {
private:
  int m_squares[64]{pieces::NONE};
  int m_turn = pieces::WHITE;
  int m_selected_piece_square = -1;
  std::unordered_set<int> m_possible_moves;
  int m_en_passant_target_square = -1;
  bool m_kingside_castle[2]{true, true};
  bool m_queenside_castle[2]{true, true};

  static void draw_piece(int piece, raylib::Vector2 pos);

  std::vector<int> generate_moves(int from_pos) const;

  std::vector<int> generate_pawn_moves(int from_pos) const;
  std::vector<int> generate_knight_moves(int from_pos) const;
  std::vector<int> generate_king_moves(int from_pos) const;
  std::vector<int> generate_sliding_piece_moves(int from_pos) const;

  static constexpr int m_direction_offsets[] = {-8, 8, -1, 1, -7, 7, 9, -9};
  int m_squares_to_edge[64][8];

  void precompute_move_data();
  void parse_board_from_fen(std::string_view fen);

public:
  explicit Board(std::string_view fen);

  void draw();
};

} // namespace chess

#endif // BOARD_HPP_