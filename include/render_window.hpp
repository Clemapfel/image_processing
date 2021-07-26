// 
// Copyright 2021 Clemens Cords
// Created on 26.07.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <color.hpp>
#include <vector.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

namespace crisp
{
    class Drawable;
    class Shader;

    class RenderWindow : protected sf::RenderWindow
    {
        friend class InputHandler;

        public:
            RenderWindow();
            void create(size_t width, size_t height, bool fullscreen = false, size_t fps_limit = 60);

            bool is_open() const;
            using sf::RenderWindow::close;

            Vector2f get_resolution();
            void set_background_color(Color);

            sf::Time update();

            void draw(Drawable*, Shader* = nullptr);
            void clear(Color = Color(0, 0, 0, 0));
            using sf::RenderWindow::display;

        protected:
            using sf::RenderWindow::pollEvent;
            Vector2f get_mouse_position();

        private:
            sf::Clock _elapsed;
            Vector2ui _resolution = Vector2ui(0, 0);
            sf::RectangleShape _background_shape;
    };
}