#include "SDL/include/Window.hpp"
#include "SDL/include/Renderer.hpp"
#include "SDL/include/Event.hpp"
#include "SDL/include/Color.hpp"
#include "SDL/include/Rect.hpp"

#include <iostream>
#include <vector>

int main() {
    sdl::Window wnd("Curve", sdl::Vector2i(100, 100), 640, 480);
    sdl::Renderer* renderer = wnd.createRenderer(SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    sdl::Event event;

    std::vector<sdl::Vector2f> points;
    points.reserve(1024);
    points.emplace_back(sdl::Vector2f(1, 1));

    f32_t angle = 45;

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

                    case SDLK_LEFT:
                        angle -= 11;
                        break;

                    case SDLK_RIGHT:
                        angle += 11;
                        break;
                }
            }
        }

        sdl::Vector2f vec = points.back();
        vec.x += std::cos(angle * M_PI / 180);
        vec.y += std::sin(angle * M_PI / 180);

        points.push_back(vec);

        renderer->clear(&sdl::Color::White);

        for (const sdl::Vector2f& point : points) {
            const i32_t x = static_cast<i32_t>(point.x);
            const i32_t y = static_cast<i32_t>(point.y);

            renderer->setDrawColor(sdl::Color::Red);
            renderer->fillRect(sdl::Rect(x, y, 5, 5));
        }

        renderer->present();
    }
}
