//
// Copyright 2021 Clemens Cords
// Created on 25.07.21 by clem (mail@clemens-cords.com)
//

#include <stdio.h>
#include <sol/sol.hpp>
#include <vector.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <color.hpp>
#include <render_window.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <input_handler.hpp>

using namespace crisp;

enum from : char {ONE = 1, TWO = 2, THREE = 4};
enum to : char {TO_TWO = TWO};

int main()
{
    std::cout << sf::Keyboard::Key::Num9 << std::endl;
    return 0;

    RenderWindow window;
    window.create(1280, 740);

    auto lua = sol::state();
    auto tex = sf::Texture();

    auto col = Color(213, 200, 23, 1);

    HSV hsv = convert_to<HSV>(col);

    while (window.is_open())
    {
        auto time = window.update();

        if (InputHandler::was_key_pressed(SPACE))
            std::cout << "SPACE" << std::endl;

        window.clear();
        window.display();
    }
}

