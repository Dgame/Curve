#include "Curve.hpp"

#include "SDL/include/Renderer.hpp"
#include "SDL/include/Event.hpp"
#include "SDL/include/Rect.hpp"

#include <iostream>

Curve::Curve(u32_t width, u32_t height) {
    _wnd  = std::unique_ptr<sdl::Window>(new sdl::Window("Curve", sdl::Vector2i(100, 100), width, height));
    _renderer = _wnd->createRenderer(SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    _players[0] = Player(SDLK_LEFT, SDLK_RIGHT, 45, 11, sdl::Color::Red);
    _players[1] = Player(SDLK_a, SDLK_d, 45, 11, sdl::Color::Blue);
    _players[2] = Player(SDLK_q, SDLK_e, 45, 11, sdl::Color::Green);
    _players[3] = Player(SDLK_y, SDLK_c, 45, 11, sdl::Color::Yellow);
}

void Curve::run() {
    _choose();
    _game();
}

void Curve::_choose() {
    sdl::Texture* choose_bg = _renderer->createTexture("choose.png");

    std::array<sdl::Rect, 4> choosen;

    sdl::Event event;

    bool running = true;
    bool choose = true;
    while (running && choose) {
        while (sdl::PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                running = false;
            else if (event.type == SDL_KEYDOWN) {
                switch (event.key.key) {
                    case SDLK_ESCAPE:
                        running = false;
                    break;

                    case SDLK_SPACE:
                        choose = false;
                    break;

                    case SDLK_F1:
                        _players[0].toggleUsage();

                        if (_players[0].used()) {
                            choosen[0] = sdl::Rect(80, 99, 390, 54);
                        } else {
                            choosen[0].collapse();
                        }
                    break;

                    case SDLK_F2:
                        _players[1].toggleUsage();

                        if (_players[1].used()) {
                            choosen[1] = sdl::Rect(80, 180, 390, 54);
                        } else {
                            choosen[1].collapse();
                        }
                    break;

                    case SDLK_F3:
                        _players[2].toggleUsage();

                        if (_players[2].used()) {
                            choosen[2] = sdl::Rect(80, 260, 390, 54);
                        } else {
                            choosen[2].collapse();
                        }
                    break;

                    case SDLK_F4:
                        _players[3].toggleUsage();

                        if (_players[3].used()) {
                            choosen[3] = sdl::Rect(80, 340, 390, 54);
                        } else {
                            choosen[3].collapse();
                        }
                    break;
                }
            }
        }

        _renderer->copy(choose_bg, nullptr);

        u16_t i = 0;
        for (const sdl::Rect& rect : choosen) {
            _renderer->setDrawColor(_players[i].getColor());
            _renderer->drawRect(rect);

            i++;
        }

        _renderer->present();
    }
}

void Curve::_game() {
    sdl::Event event;

    bool running = true;
    while (running) {
        while (sdl::PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                running = false;
            else if (event.type == SDL_KEYDOWN) {
                switch (event.key.key) {
                    case SDLK_ESCAPE:
                        running = false;
                        break;

                    default:
                        _update(event);
                }
            }
        }

        _renderer->clear(&sdl::Color::White);

        _draw();
        if (!_review())
            running = false;

        _renderer->present();
    }
}

void Curve::_update(const sdl::Event& event) {
    for (Player& p : _players) {
        p.update(event);
    }
}

void Curve::_draw() {
    for (Player& p : _players) {
        p.drawOn(_renderer);
    }
}

bool Curve::_review() {
    // Kollision & out of bounds

    u32_t win_width, win_height;
    _wnd->fetchSize(&win_width, &win_height);

    u16_t stopped = 0;
    for (u16_t i = 0; i < _players.size(); i++) {
        for (u16_t j = 0; j < _players.size(); j++) {
            if (_players[i].collideWith(_players[j])) {
                // std::cout << "Kollision mit anderem Spieler" << std::endl;
                _players[i].stop();
            }
        }

        if (!_players[i].stopped() && _players[i].outOfBounds(win_width, win_height)) {
            // std::cout << "Out of bounds" << std::endl;
            _players[i].stop();
        }

        if (_players[i].stopped()) {
            stopped++;
        }
    }

    // win situation & coin colleciton

    bool finish = false;
    if (stopped >= (_players.size() - 1)) {
        finish = _checkWin();
    }

    return !finish;
}

bool Curve::_checkWin() {
    u16_t max_coins = 0;
    for (Player& p : _players) {
        if (!p.stopped()) {
            p.coins += 10;
        }

        max_coins = std::max(p.coins, max_coins);
    }

    if (max_coins >= ((_players.size() - 1) * 10)) {
        return true;
    }

    _reset();

    return false;
}

void Curve::_reset() {
    for (Player& p : _players) {
        p.reset();
    }
}
