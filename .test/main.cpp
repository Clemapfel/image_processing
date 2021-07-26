//
// Copyright 2021 Clemens Cords
// Created on 25.07.21 by clem (mail@clemens-cords.com)
//

#include <stdio.h>
#include <sol/sol.hpp>
#include <vector.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <color.hpp>

using namespace crisp;

int main()
{
    auto lua = sol::state();
    auto tex = sf::Texture();

    auto col = Color(213, 200, 23, 1);

    HSV hsv = convert_to<HSV>(col);


    std::cout << "hello world" << std::endl;
}

