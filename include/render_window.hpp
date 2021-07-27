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
    using Drawable = sf::Drawable;
    class Shader;

    // @brief class that opens a window for rendering, works on Linux, Mac and Windows
    class RenderWindow : public sf::RenderWindow
    {
        friend class InputHandler;

        public:
            // @brief default CTOR
            RenderWindow();

            // @brief initialize the window
            // @param width: the x-resolution of the window, in pixels
            // @param height: the y-resolution of the window, in pixels
            // @param fullscreen: should the window be opened in fullscreen mode
            // @param fps_limit: maximum number of render cycles per second
            void create(size_t width, size_t height, bool fullscreen = false, size_t fps_limit = 60);

            // @brief is the window currently not closed?
            // @returns true if either create(..) hasn't been called yet or close() was called, false otherwise
            bool is_open() const;

            // @brief close the window
            using sf::RenderWindow::close;

            // @brief get the specified resolution
            // @returns: Vector2f with x-resolution at position 0, y-resolution at position 1
            Vector2f get_resolution();

            // @brief set background color. This color will replace the transparent (black) background
            // @param Color: color of the background primitive
            void set_background_color(Color);

            // @brief get the time elapsed since the last update() call, useful for synchronizing animations
            // @return sf::Time object with elapsed time delta
            sf::Time update();

            // @brief bind a drawable for rendering, will need to be repeated each cycle
            // @param drawable: drawable such as crisp::Image
            // @param shader: optional shader of either type, will be applied to the drawable
            // @param states: optional transform to be applied before drawing
            void draw(Drawable* drawable, Shader* shader = nullptr, sf::Transform transform = sf::Transform());
            using sf::RenderWindow::draw;

            // @brief clear the render area
            // @param color: overrides all pixels with the specified current, RGB{0, 0, 0, 0} (transparency) by default
            void clear(Color = Color(0, 0, 0, 0));

            // @brief update the operating systems window by displayed all bound entities
            using sf::RenderWindow::display;

        //protected:
            using sf::RenderWindow::pollEvent;
            Vector2f get_mouse_position();

        //private:
            sf::Clock _elapsed;
            Vector2ui _resolution = Vector2ui(0, 0);
            sf::RectangleShape _background_shape;
    };
}