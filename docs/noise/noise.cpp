//
// Copyright 2021 Clemens Cords
// Created on 25.07.21 by clem (mail@clemens-cords.com)
//

#include <render_window.hpp>
#include <input_handler.hpp>
#include <grayscale_image.hpp>
#include <sprite.hpp>
#include <noise_model.hpp>

using namespace crisp;

int main()
{
    float border = 5;

    RenderWindow window;
    window.create((256 + border) * 6 + border, (2 * 256) + border * 10);
    window.set_background_color(Color(0.8, 0, 0.8, 1));

    float min = -0.15,
            max = +0.15;

    GaussianNoise gauss;
    gauss.set_interval(min, max);

    UniformNoise uniform;
    uniform.set_interval(min, max);

    SaltAndPepperNoise salt_pepper(0.05, 0.05);
    salt_pepper.set_interval(-1, 1);

    auto gamma = GammaNoise();
    gamma.set_interval(min, max);

    GrayScaleImage image;
    Histogram<uint8_t> histogram;

    Sprite image_sprite,
            histogram_sprite;

    redraw:

    sf::Vector2f image_pos = {border, border};
    sf::Vector2f hist_pos = {border, window.getSize().y - border - 256};

    auto clear_image = [&]() {
        image.create(256, 256, 0.3);
        for (long x = 75; x < 200; ++x)
            for (long y = 75; y < 200; ++y)
                image(x, y) = 0.6;
    };

    auto update_sprites = [&]() {

        histogram.create_from(image);
        image_sprite.load_from(image);
        histogram_sprite.load_from(histogram, 256);

        image_sprite.align_topleft_with({image_pos.x, image_pos.y});
        histogram_sprite.align_topleft_with({hist_pos.x, hist_pos.y});

        window.draw(image_sprite);
        window.draw(histogram_sprite);

        image_pos.x += image.get_size().x() + border;
        hist_pos.x += image.get_size().x() + border;
    };

    window.clear();

    // no noise
    clear_image();
    update_sprites();

    // uniform
    clear_image();
    for (auto& px : image)
        px += uniform();
    update_sprites();

    // gauss
    clear_image();
    for (auto& px : image)
        px += gauss();
    update_sprites();

    // gamma
    clear_image();
    for (auto& px : image)
        px += gamma();
    update_sprites();

    // exponential
    clear_image();
    gamma = GammaNoise(1, 0.2);
    gamma.set_interval(min, max);
    for (auto& px : image)
        px += gamma();
    update_sprites();

    // salt and pepper
    clear_image();
    for (auto& px : image)
        px += salt_pepper();
    update_sprites();

    window.display();

    while (window.is_open())
    {
        auto time = window.update();

        if (InputHandler::was_key_pressed(KeyID::SPACE))
            goto redraw;
    }

    return 0;
}