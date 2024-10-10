#ifndef GAME_HPP_
#define GAME_HPP_

#include "board.hpp"
#include "constants.hpp"
#include <string_view>
#include <vector>

namespace chess {

class Game {
private:
  std::vector<chess::Board> m_board_history;
  int m_selected_piece_square = -1;
  std::unordered_set<int> m_possible_moves;

  const raylib::Color m_white_square_color = raylib::Color(240, 217, 181);
  const raylib::Color m_black_square_color = raylib::Color(181, 136, 99);
  const raylib::Color m_circle_color = raylib::Color(130, 151, 105);
  static constexpr bool m_draw_checked = false;

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
  explicit Game(std::string_view fen =
                    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1")
      : m_board_history{chess::Board{fen}} {
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

  void draw_board();
  Board::State state() { return m_board_history.back().game_state(); }
};

} // namespace chess

#endif // GAME_HPP_
