#include "SDL/include/Event.hpp"
#include "SDL/include/Rect.hpp"
#include "SDL/include/Renderer.hpp"

#include <random>
#include <functional>
#include <cmath>
#include <iostream>

#include "Player.hpp"

namespace {
    std::default_random_engine gen;
    std::uniform_int_distribution<i16_t> dist(1, 250);

    auto dice = std::bind(dist, gen);
}

Player::Player(i32_t lhs, i32_t rhs, u16_t angle, u16_t move, const sdl::Color& col) :
    _lhsKey(lhs),
    _rhsKey(rhs),
    _angle(angle),
    _moveAngle(move),
    _color(col)
{
    const f32_t x = dice();
    const f32_t y = dice();

    _points.push_back(sdl::Vector2f(x, y));
}

void Player::setLineSize(u8_t size) {
    _lineSize = size;
}

void Player::update(const sdl::Event& event) {
    if (_run && event.type == SDL_KEYDOWN) {
        if (event.key.key == _lhsKey) {
            _angle -= _moveAngle;
        } else if (event.key.key == _rhsKey) {
            _angle += _moveAngle;
        }
    }
}

void Player::drawOn(sdl::Renderer* renderer) {
    if (!renderer)
        return;

    if (_run) {
        sdl::Vector2f vec = _points.back();
        vec.x += std::cos(_angle * M_PI / 180);
        vec.y += std::sin(_angle * M_PI / 180);

        _points.push_back(vec);
    }

    for (const sdl::Vector2f& point : _points) {
        const i32_t x = static_cast<i32_t>(point.x);
        const i32_t y = static_cast<i32_t>(point.y);

        renderer->setDrawColor(_color);
        renderer->fillRect(sdl::Rect(x, y, 5, 5));
    }
}

bool Player::collideWith(const Player& other) const {
    const u8_t line_size = std::max(_lineSize, other._lineSize);

    const sdl::Vector2f& pos = _points.back();
    
    for (const sdl::Vector2f& o_pos : other._points) {
        const f32_t dx = std::abs(pos.x - o_pos.x);
        const f32_t dy = std::abs(pos.y - o_pos.y);

        if (dx <= line_size && dy <= line_size) {
            return true;
        }
    }

    return false;
}

bool Player::outOfBounds(u32_t width, u32_t height) const {
    if (_points.size() < 10)
        return false;

    const sdl::Vector2f& last = _points.back();

    if (last.x < _lineSize || last.x > (width - _lineSize)) {
        return true;
    }

    if (last.y < _lineSize || last.y > (height - _lineSize)) {
        return true;
    }

    return false;
}
