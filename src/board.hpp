#ifndef BOARD_HPP_
#define BOARD_HPP_

#include <raylib-cpp.hpp>
#include <array>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "pieces.hpp"

namespace chess {

class Board {
private:
  std::array<int, 64> m_squares = {pieces::NONE};
  int m_turn = pieces::WHITE;
  int m_selected_piece_square = -1;
  std::unordered_set<int> m_possible_moves;
  int m_en_passant_target_square = -1;
  std::array<bool, 2> m_kingside_castle = {false, false};
  std::array<bool, 2> m_queenside_castle = {false, false};
  std::array<int, 2> m_king_pos = {0, 0};
  int m_moves_count = 0;
  int m_halfmoves_50rule_count = 0;

  std::unordered_map<std::size_t, std::size_t> m_positions_counter;

  void draw_piece(int piece, raylib::Vector2 pos);
  void make_move(int pos);

  bool king_checked() const {
    return m_checked_squares[m_king_pos[m_turn == pieces::BLACK]];
  }

  std::vector<int> generate_moves(int from_pos, bool gen_threats = false);

  std::vector<int> generate_pawn_moves(int from_pos,
                                       bool gen_threats = false) const;
  std::vector<int> generate_knight_moves(int from_pos,
                                         bool gen_threats = false) const;
  std::vector<int> generate_king_moves(int from_pos,
                                       bool gen_threats = false) const;
  std::vector<int> generate_sliding_piece_moves(int from_pos,
                                                bool gen_threats = false) const;

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
  void load_textures();
  void fill_checked_squares();

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
  enum class State { PLAYING, MATE, DRAW };

  explicit Board(
      std::string_view fen =
          "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

  void draw();
  State game_state();
  std::string to_fen() const;
};

} // namespace chess

#endif // BOARD_HPP_
