#include "board.hpp"

void chess::Board::parse_board_from_fen(std::string_view fen) {
  using namespace pieces;
  int cur_file = 0, cur_rank = 0;

  std::size_t i = 0;
  for (; i < fen.size() && !(cur_file > 7 && cur_rank == 7); ++i) {

    switch (fen[i]) {
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
      cur_file += static_cast<int>(fen[i] - '0');
      break;

    case '/':
      ++cur_rank;
      cur_file = 0;
      break;

    case 'p':
      m_squares[cur_rank * 8 + cur_file++] = BLACK | PAWN;
      break;

    case 'n':
      m_squares[cur_rank * 8 + cur_file++] = BLACK | KNIGHT;
      break;

    case 'b':
      m_squares[cur_rank * 8 + cur_file++] = BLACK | BISHOP;
      break;

    case 'r':
      m_squares[cur_rank * 8 + cur_file++] = BLACK | ROOK;
      break;

    case 'q':
      m_squares[cur_rank * 8 + cur_file++] = BLACK | QUEEN;
      break;

    case 'k':
      m_squares[cur_rank * 8 + cur_file++] = BLACK | KING;
      break;

    case 'P':
      m_squares[cur_rank * 8 + cur_file++] = WHITE | PAWN;
      break;

    case 'N':
      m_squares[cur_rank * 8 + cur_file++] = WHITE | KNIGHT;
      break;

    case 'B':
      m_squares[cur_rank * 8 + cur_file++] = WHITE | BISHOP;
      break;

    case 'R':
      m_squares[cur_rank * 8 + cur_file++] = WHITE | ROOK;
      break;

    case 'Q':
      m_squares[cur_rank * 8 + cur_file++] = WHITE | QUEEN;
      break;

    case 'K':
      m_squares[cur_rank * 8 + cur_file++] = WHITE | KING;
      break;
    }
  }

  while (i < fen.size() && std::isspace(fen[i])) {
    ++i;
  }
  if (i == fen.size()) {
    return;
  }
  if (fen[i] == 'b') {
    m_turn = pieces::BLACK;
  } else {
    m_turn = pieces::WHITE;
  }
}