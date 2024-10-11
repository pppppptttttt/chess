#ifndef GAME_HPP_
#define GAME_HPP_

#include "board.hpp"
#include "constants.hpp"
#include <string_view>
#include <vector>

namespace chess {

class Game {
private:
  Board m_board;
  Board::Stack &m_history = m_board.history();

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

  static std::string to_algebraic_notation(int pos) {
    return std::string{static_cast<char>('a' + pos % 8),
                       static_cast<char>('0' + 8 - pos / 8)};
  }

  unsigned long long perft(int depth = 3);

public:
  explicit Game(std::string_view fen =
                    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

  void draw_board();
  Board::State state() { return m_board.game_state(); }

  ~Game() { TraceLog(LOG_ERROR, "%s", m_board.to_fen().c_str()); }
  Game(const Game &other) = delete;
  Game &operator=(const Game &other) = delete;
  Game(Game &&other) = delete;
  Game &operator=(Game &&other) = delete;
};

} // namespace chess

#endif // GAME_HPP_
