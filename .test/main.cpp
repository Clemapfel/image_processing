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

#include <intensity_transform.hpp>
#include <spatial_filter.hpp>
#include <morphological_transform.hpp>

#include <image_segment.hpp>
#include <noise_model.hpp>
#include <detail/salt_and_pepper_distribution.hpp>
#include <random>

using namespace crisp;

int main()
{

    ExponentialNoise noise;

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

    ImageSegment<float> test;

    std::vector<sf::Vector2<long>> pixels;
    for (long x = 0; x < image.get_size().x; ++x)
        for (long y = 0; y < image.get_size().y; ++y)
            if (image(x, y) > 0.75)
                pixels.push_back({x, y});

    test.create(&image, pixels);

    auto new_image = test.to_new_image<GrayScaleImage>();

    MorphologicalTransform<bool> erode;
    auto se = StructuringElement<bool>::all_foreground(2, 2);
    //se.set_foreground(1, 1);

    erode.set_structuring_element(se);


    for (auto& px : image)
        px += noise();

    sprite.load_from(image);

    while (window.is_open())
    {
        auto time = window.update();

        if (InputHandler::was_key_pressed(SPACE))
        {
            erode.erode(new_image);
            sprite.load_from(new_image);
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

