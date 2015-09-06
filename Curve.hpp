#ifndef CURVE_GAME_HPP
#define CURVE_GAME_HPP

#include <array>
#include <memory>

#include "SDL/include/Window.hpp"
#include "SDL/include/Renderer.hpp"
#include "SDL/include/types.hpp"

#include "Player.hpp"
#include "Text.hpp"

namespace sdl {
    struct Event;
}

class Curve {
public:
    explicit Curve(u32_t, u32_t);

    void run();
private:
    std::unique_ptr<sdl::Window> _wnd;
    std::unique_ptr<sdl::Renderer> _renderer;

    std::array<Player, 4> _players;
    std::array<std::unique_ptr<Text>, 4> _coins;

    std::unique_ptr<Text> _winnerText;

    bool _finished = false;
    bool _running = true;

    void _choose();
    void _game();

    void _update(const sdl::Event&);

    void _draw();
    void _review();
    void _setCredits();
    bool _checkWin();
    u16_t _getMaxCoins() const;
    void _reset();
    void _stop();
    void _drawWinner();
};

#endif
