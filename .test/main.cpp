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

#include <range/v3/algorithm/count.hpp> // specific includes
#include <range/v3/view.hpp>
#include <image.hpp>
#include <grayscale_image.hpp>

using namespace crisp;

int main()
{
    RenderWindow window;
    window.create(1280, 740);
    window.set_background_color(Color(1, 0, 1, 1));

    GrayScaleImage image;
    image.create_from_file("/home/clem/Workspace/image_processing/test_image.png");
    image.align_center_with(Vector2f(window.get_resolution().at(0) * 0.5f, window.get_resolution().at(1) * 0.5f));

    bool now = false;
    for (float& f : image)
    {
        if (now)
            f = 0;
        else
            f = f;

        now = not now;
    }

    std::cout << "done" << std::endl;

    image.update_image();

    float zoom = 1;
    float step = 3;
    sf::Vector2f offset = {0, 0};

    while (window.is_open())
    {
        auto time = window.update();

        auto offset_before = offset;

        if (InputHandler::was_key_pressed(SPACE))
            std::cout << "SPACE" << std::endl;

        if (InputHandler::was_key_pressed(KeyID::B))
            if (zoom > 1)
                image.zoom(++zoom);
            else
                image.zoom((zoom *= 2));

        if (InputHandler::was_key_pressed(KeyID::V))
             if (zoom > 1)
                image.zoom(--zoom);
            else
                image.zoom((zoom /= 2));

        if (InputHandler::is_key_down(KeyID::UP))
            offset.y -= step;

        if (InputHandler::is_key_down(KeyID::DOWN))
            offset.y += step;

        if (InputHandler::is_key_down(KeyID::LEFT))
            offset.x -= step;

        if (InputHandler::is_key_down(KeyID::RIGHT))
            offset.x += step;

        if (offset_before != offset)
            image.align_center_with(Vector2f(window.get_resolution().at(0) * 0.5f + offset.x, window.get_resolution().at(1) * 0.5f + offset.y));

        window.clear();
        window.draw(image);
        window.display();
    }
}

