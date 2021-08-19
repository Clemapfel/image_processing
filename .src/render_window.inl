// 
// Copyright 2021 Clemens Cords
// Created on 26.07.21 by clem (mail@clemens-cords.com)
//

#include <input_handler.hpp>
#include <iostream>

namespace crisp
{
    inline RenderWindow::RenderWindow()
        : sf::RenderWindow()
    {
        InputHandler::set_window(this);
    }

    inline void RenderWindow::draw(Drawable* drawable, Shader* shader, sf::Transform)
    {
    }

    inline void RenderWindow::clear(Color color)
    {
        sf::RenderWindow::clear(sf::Color(color.red() * 255, color.green() * 255, color.blue() * 255, 255));
        draw(_background_shape);
    }

    inline Vector2f RenderWindow::get_mouse_position()
    {
        auto pos = sf::Mouse::getPosition(*this);
        return Vector2f(pos.x, pos.y);
    }

    inline sf::Time RenderWindow::update()
    {
        auto time = _elapsed.restart();
        InputHandler::update(time);
        return time;
    }

    inline void RenderWindow::create(size_t width, size_t height, bool fullscreen, size_t fps_limit)
    {
        _resolution[0] = width;
        _resolution[1] = height;

        sf::ContextSettings context_settings;
        context_settings.antialiasingLevel = 0;

        auto style = sf::Style::Titlebar | sf::Style::Close;

        if (fullscreen)
            style |= sf::Style::Fullscreen;

        sf::RenderWindow::create(sf::VideoMode(width, height), "", style, context_settings);
        setFramerateLimit(fps_limit);

        _background_shape.setPosition(0, 0);
        _background_shape.setSize(sf::Vector2f(width, height));
    }

    inline bool RenderWindow::is_open() const
    {
        return sf::RenderWindow::isOpen();
    }

    inline Vector2f RenderWindow::get_resolution()
    {
        auto x = float(_resolution.at(0));
        auto y = float(_resolution.at(1));
        auto out = Vector2f(x, y);
        return out;
    }

    inline void RenderWindow::set_background_color(Color color)
    {
        _background_shape.setPosition(0, 0);

        auto to_uint = [](float in) -> uint8_t {return static_cast<uint8_t>(in * uint8_t(255));};

        _background_shape.setFillColor(sf::Color(to_uint(color.red()), to_uint(color.green()), to_uint(color.blue()), 255));
    }
}
