#include <chrono>
#include "game.hpp"
#include "constants.hpp"

chess::Game::Game(std::string_view fen) : m_board(chess::Board{fen}) {
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

unsigned long long chess::Game::perft(int depth) {
  unsigned long long nodes = 0;
  std::vector<std::pair<int, int>> moves;
  moves.reserve(256);

  for (int from = 0; from < 64; ++from) {
    const auto &m = m_board.generate_moves(from);
    for (int to : m) {
      moves.emplace_back(from, to);
    }
  }

  if (depth == 1) {
    return moves.size();
  }

  for (const auto &[from, to] : moves) {
    m_board.make_move(from, to);
    const auto pt = perft(depth - 1);
    nodes += pt;
    /*
    if (depth == 3)
      TraceLog(
          LOG_WARNING, "%s: %llu",
          (to_algebraic_notation(from) + to_algebraic_notation(to)).c_str(),
          pt);
    */
    m_board.unmake_move();
  }
  return nodes;
}

void chess::Game::draw_board() {
  // Perftest on space
  if (IsKeyPressed(KEY_SPACE)) {
    for (int depth = 1; depth < 4; ++depth) {
      using namespace std::chrono;
      const auto t1 = high_resolution_clock::now();
      const auto res = perft(depth);
      const auto t2 = high_resolution_clock::now();
      TraceLog(LOG_WARNING, "Found %llu positions in %d ms", res,
               duration_cast<milliseconds>(t2 - t1));
    }
    TraceLog(LOG_WARNING, "end");
  }

  for (int rank = 0; rank < 8; ++rank) {
    for (int file = 0, pos = rank * 8; file < 8; ++file, ++pos) {
      raylib::Rectangle rect{static_cast<float>(file) * SQUARE_SIZE,
                             static_cast<float>(rank) * SQUARE_SIZE,
                             static_cast<float>(SQUARE_SIZE),
                             static_cast<float>(SQUARE_SIZE)};

      if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
          rect.CheckCollision(GetMousePosition())) {
        if (m_possible_moves.find(pos) != m_possible_moves.end()) {
          m_board.make_move(m_selected_piece_square, pos);
          m_possible_moves.clear();
        } else {
          m_selected_piece_square = pos;
          m_possible_moves = m_board.generate_moves(pos);
        }
      }

      int square_color =
          (rank + file) % 2 == 1 ? m_black_square_color : m_white_square_color;
      if (pos == m_selected_piece_square) {
        square_color = m_circle_color;
      }
      rect.Draw(raylib::Color(square_color));
      if constexpr (m_draw_checked) {
        if (m_board.checked_squares()[pos]) {
          rect.Draw(raylib::Color(0, 255, 0, 120));
        }
      }

      m_piece_textures[m_board.square(pos)].Draw(
          raylib::Vector2{static_cast<float>(file) * SQUARE_SIZE,
                          static_cast<float>(rank) * SQUARE_SIZE});

      if (m_possible_moves.find(pos) != m_possible_moves.end()) {
        const auto center = (rect.GetPosition() +
                             Vector2{SQUARE_SIZE / 2.0f, SQUARE_SIZE / 2.0f});
        center.DrawCircle(SQUARE_SIZE / 7.0f, m_circle_color);
      }
    }
  }
}
