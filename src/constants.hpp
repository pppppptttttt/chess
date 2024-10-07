#ifndef CONSTANTS_HPP_
#define CONSTANTS_HPP_

#include <string_view>

namespace chess {

constexpr int SQUARE_SIZE = 80;
constexpr int WINDOW_WIDTH = SQUARE_SIZE * 8;
constexpr int WINDOW_HEIGHT = SQUARE_SIZE * 8;
constexpr std::string_view WINDOW_TITLE = "Chess";
constexpr int FPS = 60;

} // namespace chess

#endif // CONSTANTS_HPP_
