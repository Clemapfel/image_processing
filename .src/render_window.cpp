// 
// Copyright 2021 Clemens Cords
// Created on 26.07.21 by clem (mail@clemens-cords.com)
//

#include <render_window.hpp>
#include <SFML/Window/Window.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <input_handler.hpp>

namespace crisp
{
    RenderWindow::RenderWindow()
        : sf::RenderWindow()
    {
        InputHandler::set_window(this);
    }

    void RenderWindow::draw(Drawable*, Shader*)
    {
        //sf::RenderWindow::draw
    }

    void RenderWindow::clear(Color color)
    {
        sf::RenderWindow::clear(sf::Color(color.r, color.g, color.b, color.a));
        sf::RenderWindow::draw(_background_shape);
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
        _resolution = Vector2ui(width, height);

        _background_shape.setFillColor(sf::Color::Transparent);
        _background_shape.setPosition(0, 0);
        _background_shape.setSize(sf::Vector2f(_resolution.at(0), _resolution.at(1)));

        sf::ContextSettings context_settings;
        context_settings.antialiasingLevel = 0;

        auto style = sf::Style::Titlebar | sf::Style::Close;

        if (fullscreen)
            style |= sf::Style::Fullscreen;

        sf::RenderWindow::create(sf::VideoMode(width, height), "", style, context_settings);
    }

    bool RenderWindow::is_open() const
    {
        return sf::RenderWindow::isOpen();
    }

    Vector2f RenderWindow::get_resolution()
    {
        return Vector2f(float(_resolution.at(0)), float(_resolution.at(1)));
    }

    void RenderWindow::set_background_color(Color color)
    {
        _background_shape.setFillColor(sf::Color(color.r, color.g, color.b, color.a));
    }
}
