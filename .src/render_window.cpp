// 
// Copyright 2021 Clemens Cords
// Created on 26.07.21 by clem (mail@clemens-cords.com)
//

#include <render_window.hpp>
#include <SFML/Window/Window.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <input_handler.hpp>
#include <iostream>

namespace crisp
{
    RenderWindow::RenderWindow()
        : sf::RenderWindow()
    {
        InputHandler::set_window(this);
    }

    void RenderWindow::draw(Drawable* drawable, Shader* shader, sf::Transform)
    {
    }

    void RenderWindow::clear(Color color)
    {
        sf::RenderWindow::clear(sf::Color(color.r * 255, color.g * 255, color.b * 255, color.a * 255));
        draw(_background_shape);
    }

    Vector2f RenderWindow::get_mouse_position()
    {
        auto pos = sf::Mouse::getPosition(*this);
        return Vector2f(pos.x, pos.y);
    }

    sf::Time RenderWindow::update()
    {
        auto time = _elapsed.restart();
        InputHandler::update(time);
        return time;
    }

    void RenderWindow::create(size_t width, size_t height, bool fullscreen, size_t fps_limit)
    {
        _resolution[0] = width;
        _resolution[1] = height;

        sf::ContextSettings context_settings;
        context_settings.antialiasingLevel = 0;

        auto style = sf::Style::Titlebar | sf::Style::Close;

        if (fullscreen)
            style |= sf::Style::Fullscreen;

        sf::RenderWindow::create(sf::VideoMode(width, height), "", style, context_settings);
        _background_shape.setPosition(0, 0);
        _background_shape.setSize(sf::Vector2f(width, height));
    }

    bool RenderWindow::is_open() const
    {
        return sf::RenderWindow::isOpen();
    }

    Vector2f RenderWindow::get_resolution()
    {
        auto x = float(_resolution.at(0));
        auto y = float(_resolution.at(1));
        auto out = Vector2f(x, y);
        return out;
    }

    void RenderWindow::set_background_color(Color color)
    {
        _background_shape.setPosition(0, 0);

        auto to_uint = [](float in) -> uint8_t {return static_cast<uint8_t>(in * uint8_t(255));};

        _background_shape.setFillColor(sf::Color(to_uint(color.r), to_uint(color.g), to_uint(color.b), to_uint(color.a)));
    }
}
