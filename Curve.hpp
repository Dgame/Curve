#ifndef CURVE_GAME_HPP
#define CURVE_GAME_HPP

#include <array>
#include <memory>

#include "SDL/include/Window.hpp"
#include "SDL/include/types.hpp"

#include "Player.hpp"

namespace sdl {
    struct Event;

    class Window;
    class Renderer;
}

class Curve {
public:
    explicit Curve(u32_t, u32_t);

    void run();
private:
    std::unique_ptr<sdl::Window> _wnd;
    sdl::Renderer* _renderer;

    std::array<Player, 4> _players;

    void _choose();
    void _game();

    void _update(const sdl::Event&);
    void _draw();
    bool _review();
    bool _checkWin();
    void _reset();
};

#endif
