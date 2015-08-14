#include "SDL/include/Window.hpp"
#include "SDL/include/Renderer.hpp"
#include "SDL/include/Event.hpp"

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

    u32_t win_width;
    u32_t win_height;

    wnd.fetchSize(&win_width, &win_height);

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

        std::cout << stopped << std::endl;

        if (stopped >= (players.size() - 1)) {
            std::cout << "Over and out" << std::endl;
            running = false;
        }

        renderer->present();
    }
}
