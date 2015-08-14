#ifndef CURVE_PLAYER_HPP
#define CURVE_PLAYER_HPP

#include "SDL/include/types.hpp"
#include "SDL/include/Vector2.hpp"
#include "SDL/include/Color.hpp"

#include <vector>

namespace sdl {
    struct Event;

    class Renderer;
}

class Player {
public:
    u16_t coins = 0;

    Player() = default;
    explicit Player(i32_t, i32_t, u16_t, u16_t, const sdl::Color&);

    void setLineSize(u8_t);
    void reset();

    void stop() {
        _run = false;
    }

    bool stopped() const {
        return !_run;
    }

    void toggleUsage() {
        _humanUsage = !_humanUsage;
    }

    bool used() const {
        return _humanUsage;
    }

    const sdl::Color& getColor() const {
        return _color;
    }

    void update(const sdl::Event&);
    void drawOn(sdl::Renderer*);

    bool collideWith(const Player&) const;
    bool outOfBounds(u32_t, u32_t) const;

private:
    bool _run = true;
    bool _humanUsage = false;

    i32_t _lhsKey = 0;
    i32_t _rhsKey = 0;

    u16_t _angle = 45;
    u16_t _moveAngle = 0;

    u8_t _lineSize = 5;

    sdl::Color _color;

    std::vector<sdl::Vector2f> _points;
};

#endif
