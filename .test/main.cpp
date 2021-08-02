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

    Kernel<float> kernel = GrayScaleFilter::simple_gradient(crisp::SpatialFilter<GrayScaleImage, float>::Y_DIRECTION);

    GrayScaleFilter filter;
    filter.set_kernel(kernel);//crisp::SpatialFilter<GrayScaleImage, float>::Kernel::X_DIRECTION));

    sprite.load_from(image);
    sprite.align_center_with(Vector2f(window.get_resolution().at(0) * 0.5f, window.get_resolution().at(1) * 0.5f));

    while (window.is_open())
    {
        auto time = window.update();


        if (InputHandler::was_key_pressed(SPACE))
        {
            filter.apply_to(image);
            sprite.load_from(image);
        }

        if (InputHandler::was_key_pressed(UP))
        {
            kernel = convolute( GrayScaleFilter::laplacian_first_derivative(), kernel);
            filter.set_kernel(kernel);
            image.create_from_file("/home/clem/Workspace/image_processing/test_image.png");
            sprite.load_from(image);
        }

        if (InputHandler::was_key_pressed(DOWN))
        {
            kernel = convolute(kernel, GrayScaleFilter::laplacian_first_derivative());
            filter.set_kernel(kernel);
            image.create_from_file("/home/clem/Workspace/image_processing/test_image.png");
            sprite.load_from(image);
        }

        window.clear();
        window.draw(sprite);
        window.display();
    }
}

