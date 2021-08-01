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

    GrayScaleImage test;
    //test.create_from_file("/home/clem/Workspace/image_processing/test_binary_image.png", 0.5);
    long dimensions = 201;
    test.create(dimensions, dimensions);

    auto element = NonFlatStructuringElement::cone(dimensions, 0.5, 1);

    for (long x = 0; x < element.get_size().x; ++x)
        for (long y = 0; y < element.get_size().y; ++y)
            test(x, y) = element.get_value(x, y).value_or(0);

    sprite.load_from(test);

    while (window.is_open())
    {
        auto time = window.update();


        if (InputHandler::was_key_pressed(SPACE))
        {
            //morph.erode(test);
            sprite.load_from(test);
        }

        window.clear();
        window.draw(sprite);
        window.display();
    }
}

