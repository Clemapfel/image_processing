//
// Copyright 2021 Clemens Cords
// Created on 25.07.21 by clem (mail@clemens-cords.com)
//

#include <render_window.hpp>
#include <input_handler.hpp>
#include <grayscale_image.hpp>
#include <sprite.hpp>
#include <noise_model.hpp>
#include <image_segment.hpp>

using namespace crisp;

int main()
{
    float border = 5;

    RenderWindow window;
    window.create(1000, 600);
    window.set_background_color(Color(0.8, 0, 0.8, 1));

    auto image = GrayScaleImage();
    image.create_from_file("/home/clem/Workspace/image_processing/docs/opal_additive_noise.png");

    auto image_sprite = Sprite();
    auto sample_sprite = Sprite();
    auto sample_hist_sprite = Sprite();

    sf::Vector2f center = {window.getSize().x * 0.5f, window.getSize().y * 0.5f};

    ImageSegment<float> sample_segment;

    std::vector<sf::Vector2<long>> sample_pixels;
    for (long x = 0.7 * image.get_size().x; x < image.get_size().x; ++x)
        for (long y = 0.2 * image.get_size().y; y < image.get_size().y; ++y)
                sample_pixels.emplace_back(x, y);

    auto sample_hist = Histogram<uint8_t>();

    sample_segment.create(&image, sample_pixels);


    GrayScaleImage sample = sample_segment.to_new_image<GrayScaleImage>();

    auto update = [&]()
    {
        sample_sprite.load_from(sample);
        sample_hist.create_from(sample);

        image_sprite.load_from(image);
        sample_hist_sprite.load_from(sample_hist, 256);
    };
    update();

    image_sprite.align_topleft_with({5, 5});
    sample_sprite.align_topleft_with({window.getSize().x - 5 - sample_sprite.get_size().x, 5});
    sample_hist_sprite.align_topleft_with({sample_sprite.get_topleft().x - sample_hist_sprite.get_size().x - 10, sample_sprite.get_topleft().y + sample_sprite.get_size().x * 0.5f});

    float min = 0;
    float max = 0;
    auto noise = GammaNoise();
    noise.set_interval(-0.3, 0);

    while (window.is_open())
    {
        auto time = window.update();

        if (InputHandler::was_key_pressed(SPACE))
        {
            sample = sample_segment.to_new_image<GrayScaleImage>();

            for (auto& px : sample)
                px += noise();

            update();
        }

        if (InputHandler::was_key_pressed(UP))
        {
            min -= 0.05;
            noise.set_interval(min, max);
        }

        if (InputHandler::was_key_pressed(DOWN))
        {
            min += 0.05;
            noise.set_interval(min, max);
        }

        window.clear();
        window.draw(image_sprite);
        window.draw(sample_sprite);
        window.draw(sample_hist_sprite);
        window.display();
    }

    return 0;
}

    /*
    GrayScaleImage image;
    image.create_from_file("/home/clem/Workspace/image_processing/test_image.png");
    Sprite sprite;

    Kernel<float> kernel = GrayScaleFilter::simple_gradient(crisp::SpatialFilter<GrayScaleImage, float>::Y_DIRECTION);

    GrayScaleFilter filter;
    filter.set_kernel(kernel);//crisp::SpatialFilter<GrayScaleImage, float>::Kernel::X_DIRECTION));

    sprite.load_from(image);

    ImageSegment<float> test;

    std::vector<sf::Vector2<long>> pixels;
    for (long x = 0; x < image.get_size().x; ++x)
        for (long y = 0; y < image.get_size().y; ++y)
            if (image(x, y) > 0.75)
                pixels.push_back({x, y});

    test.create(&image, pixels);

    auto new_image = GrayScaleImage(); //test.to_new_image<GrayScaleImage>();
    new_image.create(500, 500, 0.5);
    MorphologicalTransform<bool> erode;
    auto se = StructuringElement<bool>::all_foreground(2, 2);
    //se.set_foreground(1, 1);

    Histogram<uint8_t> histogram;
    histogram.create_from(new_image);

    sprite.load_from(histogram, 256); //histogram, 5);
    sprite.align_center_with(Vector2f(window.get_resolution().at(0) * 0.5f, window.get_resolution().at(1) * 0.5f));

    while (window.is_open())
    {
        auto time = window.update();

        if (InputHandler::was_key_pressed(SPACE))
        {
            for (auto& px : new_image)
                px += noise();

            Histogram<uint8_t> histogram;
            histogram.create_from(new_image);

            sprite.load_from(histogram, 300);
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
    }*/


