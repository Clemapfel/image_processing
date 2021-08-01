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
#include <grayscale_image.hpp>
#include <binary_image.hpp>
#include <image_handler.hpp>
#include <sprite.hpp>
#include <neighbourhood.hpp>

#include <intensity_transform.hpp>
#include <spatial_filter.hpp>
#include <morphological_transform.hpp>

using namespace crisp;

int main()
{
    RenderWindow window;
    window.create(1280, 740);
    window.set_background_color(Color(1, 0, 1, 1));

    GrayScaleImage image;
    image.create_from_file("/home/clem/Workspace/image_processing/test_image.png");
    Sprite sprite;

    GrayScaleFilter blur;
    blur.set_kernel(GrayScaleFilter::Kernel::isotropic_laplacian_3x3(true));

    sprite.load_from(image);
    sprite.align_center_with(Vector2f(window.get_resolution().at(0) * 0.5f, window.get_resolution().at(1) * 0.5f));

    float zoom = 1;
    float step = 3;
    sf::Vector2f offset = {0, 0};

    BinaryImage test;
    test.create_from_file("/home/clem/Workspace/image_processing/test_binary_image.png", 0.5);

    auto morph = MorphologicalTransform<bool>();
    morph.set_structuring_element(FlatStructuringElement::diamond(5));

    sprite.load_from(test);

    while (window.is_open())
    {
        auto time = window.update();

        auto offset_before = offset;

        if (InputHandler::was_key_pressed(SPACE))
        {
            morph.erode(test);
            sprite.load_from(test);
        }

        if (InputHandler::was_key_pressed(KeyID::UP))
        if (InputHandler::was_key_pressed(KeyID::DOWN))
        if (InputHandler::is_key_down(KeyID::LEFT))
            offset.x -= step;

        if (InputHandler::is_key_down(KeyID::RIGHT))
            offset.x += step;

        window.clear();
        window.draw(sprite);
        window.display();
    }
}

