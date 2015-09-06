#include "Curve.hpp"

#include "SDL/include/Surface.hpp"
#include "SDL/include/Renderer.hpp"
#include "SDL/include/Texture.hpp"
#include "SDL/include/Event.hpp"
#include "SDL/include/Rect.hpp"

#include "Text.hpp"

#include <iostream>

Curve::Curve(u32_t width, u32_t height) {
    _wnd  = std::unique_ptr<sdl::Window>(new sdl::Window("Curve", sdl::Vector2i(100, 100), width, height));
    _renderer = std::unique_ptr<sdl::Renderer>(new sdl::Renderer(*_wnd.get(), SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC));

    sdl::Surface icon("images/icon.png");
    _wnd->setIcon(icon);

    _players[0] = Player(SDLK_LEFT, SDLK_RIGHT, 45, 11, sdl::Color::Red);
    _players[0].setName("Red");

    _players[1] = Player(SDLK_a, SDLK_d, 45, 11, sdl::Color::Blue);
    _players[1].setName("Blue");

    _players[2] = Player(SDLK_q, SDLK_e, 45, 11, sdl::Color::Green);
    _players[2].setName("Green");

    _players[3] = Player(SDLK_y, SDLK_c, 45, 11, sdl::Color::Orange);
    _players[3].setName("Orange");

    u16_t y = 20;
    for (auto& coin : _coins) {
        coin = std::unique_ptr<Text>(new Text("font/arial.ttf", 20));
        coin->setPosition(_wnd->width() - 100, y);
        y += 20;
    }

    u32_t win_width, win_height;
    _wnd->fetchSize(&win_width, &win_height);

    _winnerText = std::unique_ptr<Text>(new Text("font/arial.ttf", 32));
    _winnerText->setPosition((win_width / 2) - 150, (win_height / 2) - 50);
}

void Curve::run() {
    _choose();
    _game();
}

void Curve::_choose() {
    sdl::Texture choose_bg(*_renderer.get(), "images/choose.png");

    std::array<sdl::Rect, 4> choosen;

    sdl::Event event;

    bool choose = true;
    while (_running && choose) {
        while (sdl::PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                _running = false;
            else if (event.type == SDL_KEYDOWN) {
                switch (event.keyboard.key) {
                    case SDLK_ESCAPE:
                        _running = false;
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

    while (_running) {
        while (sdl::PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                _running = false;
            else if (event.type == SDL_KEYDOWN) {
                switch (event.keyboard.key) {
                    case SDLK_ESCAPE:
                        _running = false;
                        break;

                    default:
                        _update(event);
                }
            }
        }

        _renderer->clear(&sdl::Color::White);

        _draw();

        if (!_finished)
            _review();
        else
            _drawWinner();

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
        p.drawOn(*_renderer.get());
    }

    u16_t i = 0;
    for (auto& coin : _coins) {
        const sdl::Vector2i& pos = coin->getPosition();

        _renderer->setDrawColor(_players[i].getColor());
        _renderer->fillRect(sdl::Rect(pos.x - 25, pos.y + 5, 15, 15));

        coin->format("%d credits", _players[i].coins);
        coin->renderOn(*_renderer);

        i++;
    }
}

void Curve::_review() {
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
        _setCredits();
        finish = _checkWin();
    }

    if (finish) {
        _stop();
    }
}

void Curve::_setCredits() {
    for (Player& p : _players) {
        if (!p.stopped()) {
            p.coins += 10;
        }
    }
}

bool Curve::_checkWin() {
    const u16_t max_coins = _getMaxCoins();

    if (max_coins >= ((_players.size() - 1) * 10)) {
        return true;
    }

    _reset();

    return false;
}

u16_t Curve::_getMaxCoins() const {
    u16_t max_coins = 0;
    for (const Player& p : _players) {
        max_coins = std::max(p.coins, max_coins);
    }

    return max_coins;
}

void Curve::_reset() {
    for (Player& p : _players) {
        p.reset();
    }
}

void Curve::_stop() {
    for (Player& p : _players) {
        p.stop();
    }

    _finished = true;
}

void Curve::_drawWinner() {
    const u16_t max_coins = _getMaxCoins();
    for (const Player& p : _players) {
        if (p.coins == max_coins) {
            const sdl::Vector2i& pos = _winnerText->getPosition();

            _renderer->setDrawColor(p.getColor());
            _renderer->drawRect(sdl::Rect(0, pos.y - 15, _wnd->width(), 80));

            _winnerText->format("And the winner is %s", p.getName().c_str());
            _winnerText->renderOn(*_renderer.get());

            break;
        }
    }
}
