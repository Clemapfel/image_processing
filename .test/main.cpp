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
    long nrows = 301, ncols = 273;
    test.create(nrows, ncols);

    long half_nrows = (nrows - 1) / 2;
    long half_ncols = (ncols - 1) / 2;

    long row_i = 0;
    int  col_offset = 0;

    if (nrows >= ncols)
    {
        long row_i = 0;
        int col_offset = 0;

        while (row_i <= half_nrows)
        {
            for (int offset = -col_offset; offset < col_offset; ++offset)
            {
                test(row_i, half_ncols + offset) = 1;
                test(nrows - row_i, half_ncols + offset) = 1;
            }

            row_i += 1;
            col_offset += floor(float(ncols) / float(nrows));
        }
    }
    else
    {
        long col_i = 0;
        int row_offset = 0;

        while (col_i <= half_ncols)
        {
            for (int offset = -row_offset; offset < row_offset; ++offset)
            {
                test(half_nrows + offset, col_i) = 1;
                test(half_nrows + offset, ncols - col_i) = 1;
            }

            col_i += 1;
            row_offset += ncols / nrows;
        }
    }


    sprite.load_from(test);

    while (window.is_open())
    {
        auto time = window.update();

        auto offset_before = offset;

        if (InputHandler::was_key_pressed(SPACE))
        {
            blur.apply_to(image);
            sprite.load_from(image);
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

