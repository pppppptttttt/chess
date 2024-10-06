#ifndef PIECES_HPP_
#define PIECES_HPP_

namespace chess {

namespace pieces {

// first 3 bits of integer for piece type
constexpr int NONE = 0;
constexpr int PAWN = 1;
constexpr int KNIGHT = 2;
constexpr int BISHOP = 3;
constexpr int ROOK = 4;
constexpr int QUEEN = 5;
constexpr int KING = 6;

// idiotic raylib has color's macro
// NB: order of includes are important because of this lines

#ifdef WHITE
#undef WHITE
#endif // WHITE

#ifdef BLACK
#undef BLACK
#endif // BLACK

// left two bits for piece color
constexpr int WHITE = 8;
constexpr int BLACK = 16;

// use bitwise or to get a piece, e.g.
// int black_queen = chess::piece::BLACK | chess::piece::QUEEN;

} // namespace pieces

} // namespace chess

#endif // PIECES_HPP_