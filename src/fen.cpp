#include "board.hpp"
#include <cstdlib>

#define early_return(cond)                                                     \
  do {                                                                         \
    if (static_cast<bool>(cond)) {                                             \
      return;                                                                  \
    }                                                                          \
  } while (0)

void chess::Board::parse_board_from_fen(const std::string &fen) {
  using namespace pieces;

  std::size_t i = 0;
  auto skip_spaces = [&i, &fen]() {
    while (i < fen.size() && std::isspace(fen[i])) {
      ++i;
    }
  };

  skip_spaces();
  early_return(i == fen.size());

  // pieces
  for (int cur_file = 0, cur_rank = 0;
       i < fen.size() && !(cur_file > 7 && cur_rank == 7); ++i) {
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
      m_king_pos[1] = cur_rank * 8 + cur_file;
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
      m_king_pos[0] = cur_rank * 8 + cur_file;
      m_squares[cur_rank * 8 + cur_file++] = WHITE | KING;
      break;
    }
  }

  skip_spaces();
  early_return(i == fen.size());

  // turn
  if (fen[i++] == 'b') {
    m_turn = pieces::BLACK;
  } else {
    m_turn = pieces::WHITE;
  }

  skip_spaces();
  early_return(i == fen.size());

  // catling abilities
  while (i < fen.size() && std::isalpha(fen[i])) {
    switch (fen[i]) {
    case 'K':
      m_kingside_castle[1] = true;
      break;
    case 'Q':
      m_queenside_castle[1] = true;
      break;
    case 'k':
      m_kingside_castle[0] = true;
      break;
    case 'q':
      m_queenside_castle[0] = true;
      break;
    }
    ++i;
  }

  skip_spaces();
  early_return(i == fen.size());

  // en passant targets
  if (fen[i] == '-') {
    ++i;
  } else {
    const int file = fen[i++] - 'a';
    const int rank = 7 - (fen[i++] - '1');
    m_en_passant_target_square = rank * 8 + file;
  }
}

std::string chess::Board::to_fen() const { return ""; }
