#ifndef BOARD_HPP_
#define BOARD_HPP_

#include <raylib-cpp.hpp>
#include <string_view>
#include <unordered_set>
#include <unordered_map>
#include <vector>
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

  void draw_piece(int piece, raylib::Vector2 pos);

  std::vector<int> generate_moves(int from_pos) const;

  std::vector<int> generate_pawn_moves(int from_pos) const;
  std::vector<int> generate_knight_moves(int from_pos) const;
  std::vector<int> generate_king_moves(int from_pos) const;
  std::vector<int> generate_sliding_piece_moves(int from_pos) const;

  static constexpr int m_direction_offsets[] = {-8, 8, -1, 1, -7, 7, 9, -9};
  int m_squares_to_edge[64][8];

  void precompute_move_data();
  void parse_board_from_fen(std::string_view fen);

  std::unordered_map<int, raylib::Texture> m_piece_textures;
  std::unordered_map<int, std::string> m_textures_paths = {
      {pieces::WHITE | pieces::PAWN, "../bin/white-pawn.png"},
      {pieces::WHITE | pieces::KNIGHT, "../bin/white-knight.png"},
      {pieces::WHITE | pieces::BISHOP, "../bin/white-bishop.png"},
      {pieces::WHITE | pieces::ROOK, "../bin/white-rook.png"},
      {pieces::WHITE | pieces::QUEEN, "../bin/white-queen.png"},
      {pieces::WHITE | pieces::KING, "../bin/white-king.png"},

      {pieces::BLACK | pieces::PAWN, "../bin/black-pawn.png"},
      {pieces::BLACK | pieces::KNIGHT, "../bin/black-knight.png"},
      {pieces::BLACK | pieces::BISHOP, "../bin/black-bishop.png"},
      {pieces::BLACK | pieces::ROOK, "../bin/black-rook.png"},
      {pieces::BLACK | pieces::QUEEN, "../bin/black-queen.png"},
      {pieces::BLACK | pieces::KING, "../bin/black-king.png"}};


public:
  explicit Board(std::string_view fen);

  void draw();
};

} // namespace chess

#endif // BOARD_HPP_
