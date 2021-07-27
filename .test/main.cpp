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
#include <SFML/OpenGL.hpp>

#include <image.hpp>

using namespace crisp;

int main()
{
    RenderWindow window;
    window.create(1920, 1080);

    Image image;
    image.create_from_file("/home/clem/Workspace/image_processing/test_image.png");
    float x = window.get_resolution().at(0) * 0.5f;
    float y = window.get_resolution().at(1) * 0.5f;
    auto test = Vector2f(x, y);
    image.align_center_with(test);

    while (window.is_open())
    {
        auto time = window.update();

        if (InputHandler::was_key_pressed(SPACE))
            std::cout << "SPACE" << std::endl;

        window.clear();
        window.draw(image);
        window.display();
    }
}

