#include "SDL/include/Window.hpp"
#include "SDL/include/Renderer.hpp"
#include "SDL/include/Event.hpp"
#include "SDL/include/Rect.hpp"

#include <iostream>
#include <array>

#include "Player.hpp"

int main() {
    sdl::Window wnd("Curve", sdl::Vector2i(100, 100), 640, 480);
    sdl::Renderer* renderer = wnd.createRenderer(SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    sdl::Event event;

    std::array<Player, 4> players;
    players[0] = Player(SDLK_LEFT, SDLK_RIGHT, 45, 11, sdl::Color::Red);
    players[1] = Player(SDLK_a, SDLK_d, 45, 11, sdl::Color::Blue);
    players[2] = Player(SDLK_q, SDLK_e, 45, 11, sdl::Color::Green);
    players[3] = Player(SDLK_y, SDLK_c, 45, 11, sdl::Color::Yellow);

    bool running = true;
    bool choose = true;

    sdl::Texture* choose_bg = renderer->createTexture("choose.png");

    std::array<sdl::Rect, 4> choosen;

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
                        players[0].toggleUsage();

                        if (players[0].used()) {
                            choosen[0] = sdl::Rect(112, 99, 300, 48);
                        } else {
                            choosen[0].collapse();
                        }
                    break;

                    case SDLK_F2:
                        players[1].toggleUsage();

                        if (players[1].used()) {
                            choosen[1] = sdl::Rect(112, 170, 300, 48);
                        } else {
                            choosen[1].collapse();
                        }
                    break;

                    case SDLK_F3:
                        players[2].toggleUsage();

                        if (players[2].used()) {
                            choosen[2] = sdl::Rect(112, 240, 300, 48);
                        } else {
                            choosen[2].collapse();
                        }
                    break;

                    case SDLK_F4:
                        players[3].toggleUsage();

                        if (players[3].used()) {
                            choosen[3] = sdl::Rect(112, 315, 300, 48);
                        } else {
                            choosen[3].collapse();
                        }
                    break;
                }
            }
        }

        renderer->copy(choose_bg, nullptr);

        u16_t i = 0;
        for (const sdl::Rect& rect : choosen) {
            renderer->setDrawColor(players[i].getColor());
            renderer->drawRect(rect);

            i++;
        }

        renderer->present();
    }

    u32_t win_width;
    u32_t win_height;

    wnd.fetchSize(&win_width, &win_height);

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
                        for (Player& p : players) {
                            p.update(event);
                        }
                }
            }
        }

        renderer->clear(&sdl::Color::White);

        for (Player& p : players) {
            p.drawOn(renderer);
        }

        u16_t stopped = 0;
        for (u16_t i = 0; i < players.size(); i++) {
            for (u16_t j = 0; j < players.size(); j++) {
                if (players[i].collideWith(players[j])) {
                    // std::cout << "Kollision mit anderem Spieler" << std::endl;
                    players[i].stop();
                }
            }

            if (!players[i].stopped() && players[i].outOfBounds(win_width, win_height)) {
                // std::cout << "Out of bounds" << std::endl;
                players[i].stop();
            }

            if (players[i].stopped()) {
                stopped++;
            }
        }

        // std::cout << stopped << std::endl;

        if (stopped >= (players.size() - 1)) {
            std::cout << "Over and out" << std::endl;
            running = false;
        }

        renderer->present();
    }
}
