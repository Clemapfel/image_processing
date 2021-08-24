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
#include <complex>
#include <vector>
#include <fftw3.h>
#include <spatial_filter.hpp>
#include <frequency_domain_filter.hpp>
#include <fourier_transform.hpp>
#include <color_image.hpp>
#include <pseudocolor_mapping.hpp>
#include <binary_image.hpp>
#include <morphological_transform.hpp>
#include <edge_detection.hpp>
#include <segmentation.hpp>

using namespace crisp;

int main()
{
    auto image = ColorImage();
    image.create_from_file("/home/clem/Workspace/image_processing/docs/opal_color.png");

    auto grayscale = GrayScaleImage();
    grayscale.create_from_file("/home/clem/Workspace/image_processing/docs/opal_color.png");
    auto binary = Segmentation::manual_threshold(grayscale, 0.01);
    binary.invert();

    sf::Clock clock;
    auto segments = Segmentation::decompose_into_segments(binary, {true}, 100);
    std::cout << clock.restart().asSeconds() << std::endl;

    float hue = 0;
    for (auto& segment : segments)
    {
        auto color = Color(HSV{hue, 1, 1});
        hue = fmod((hue + 1.61803398875), 1);

        for (auto& point : segment)
            image(point.x(), point.y()) = HSV{hue, 1, 1};
    }


    auto window = RenderWindow();
    window.create(image.get_size().x() * 2, image.get_size().y() * 2);

    auto sprite = Sprite();
    sprite.create_from(image);
    sprite.scale(2, false);

    while (window.is_open())
    {
        window.update();
        window.clear();
        window.draw(sprite);
        window.display();
    }

    /*
    using namespace std;
    using namespace Eigen;

    using Kernel_t = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>;
    Kernel_t m = SpatialFilter<GrayScaleImage, double>::gaussian(3); //sobel(GrayScaleFilter::GradientDirection::X_DIRECTION);

    Kernel_t left, right;
    seperate(m, &left, &right);
    auto final = left * right;//svd.matrixU() * S * svd.matrixV().transpose(); // * v.transpose();

    std::cout << "before: \n" << m << std::endl;
    std::cout << "reassembled: \n" << final << std::endl;

    Kernel_t error = m - final;
    for (long x = 0; x < error.rows(); ++x)
        for (long y = 0; y < error.cols(); ++y)
            error(x, y) = abs<double>(error(x, y));

    std::cout << "error: \n" << error.mean() << std::endl;

    GrayScaleImage original;
    original.create_from_file("/home/clem/Workspace/image_processing/docs/opal_color.png");
    original.set_padding_type(ZERO);

    return 0;

    ColorImage with_padding;
    with_padding.create(original.get_size().x()*3, original.get_size().y*3);

    original.set_padding_type(REPEAT);

    for (long x = 0; x < with_padding.get_size().x(); ++x)
        for (long y = 0; y < with_padding.get_size().y; ++y)
            with_padding(x, y) = original.get_pixel_or_padding(x - original.get_size().x(), y - original.get_size().y);

    GrayScaleImage original;
    original.create_from_file("/home/clem/Workspace/image_processing/docs/opal_color.png");

    sf::Clock clock;
    SpatialFilter<GrayScaleImage> filter;
    filter.set_kernel(GrayScaleFilter::gaussian(3));
    //filter.apply_to(original);

    // gardient: 0.02  | sobel: 0.05
   auto edges = crisp::EdgeDetection::threshold_gradient(original);

   std::cout << clock.restart().asSeconds() << std::endl;

    MorphologicalTransform<bool> morph;
    auto se = MorphologicalTransform<bool>::StructuringElement::all_background(3, 3);
    se.set_foreground(1, 1);
    morph.set_structuring_element(se);
    morph.miss_or_hit_transform(edges);

    auto sprite = Sprite();
    sprite.load_from(edges);

    float zoom = 2;
    sprite.zoom(zoom);

    RenderWindow window;
    window.create(sprite.get_size().x() * zoom, sprite.get_size().y * zoom);

    window.clear();
    window.draw(sprite);
    window.display();

    while (window.is_open())
    {
        window.update();
        window.clear();
        window.draw(sprite);
        window.display();
    }

    return 0;

    /*
     *
    SpatialFilter<GrayScaleImage> filter;

    auto x_img = original,
         y_img = original;

    filter.set_kernel(GrayScaleFilter::sobel_x());
    filter.apply_to(x_img);
    filter.set_kernel(GrayScaleFilter::sobel_y());
    filter.apply_to(y_img);

    float min = std::numeric_limits<float>::max();
    float max = std::numeric_limits<float>::min();

    float mean = 0;

    // measure
    for (long x = 0; x < original.get_size().x(); ++x)
        for (long y = 0; y < original.get_size().y; ++y)
        {
            auto magnitude = sqrt(x_img(x, y) * x_img(x, y) + y_img(x, y) * y_img(x, y));
            min = std::min<float>(min, magnitude);
            max = std::max<float>(max, magnitude);
            mean += magnitude;
        }

    mean /= original.get_size().x() * original.get_size().y;

    float lower = mean;
    float upper = std::min<float>(2*mean, mean + (max - mean) * 0.5);

    std::cout << "min: " << min << " max: " << max << std::endl;
    std::cout << "lower: " << lower << " upper: " << upper << std::endl;

    for (long x = 0; x < original.get_size().x(); ++x)
    {
        for (long y = 0; y < original.get_size().y; ++y)
        {
            auto magnitude = sqrt(x_img(x, y) * x_img(x, y) + y_img(x, y) * y_img(x, y));
            auto angle = atan2(x_img(x, y), y_img(x, y)) * (180 / M_PI);

            Eigen::Matrix<float, 3, 3> neighborhood;
            for (int i = -1; i <= 1; i++)
                for (int j = -1; j <= 1; j++)
                    neighborhood(i + 1, j + 1) = sqrt(pow(x_img.get_pixel_or_padding(x + i, y + j), 2) +
                                                      pow(y_img.get_pixel_or_padding(x + i, y + j), 2));

            original(x, y) = 0;
            bool has_neighbor = false;

            // horizontal
            if ((angle < -67.5 and angle > -112.5) or (angle > 67.5 and angle < 112.5))
            {
                if ((magnitude > neighborhood(0, 1)) and (magnitude > neighborhood(2, 1)))
                    original(x, y) = 0.1;

                if ((neighborhood(0, 1) > lower) or (neighborhood(2, 1) > lower))
                    has_neighbor = true;
            }
            // vertical
            else if ((angle > -22.5 and angle < 22.5) or (angle < -157.5 and angle > -180) or (angle > 157.5 and angle < 180))
            {
                if ((magnitude > neighborhood(1, 0)) and (magnitude > neighborhood(1, 2)))
                    original(x, y) = 0.1;

                if ((neighborhood(1, 0) > lower) or (neighborhood(1, 2) > lower))
                    has_neighbor = true;
            }
            // +45°
            else if ((angle > 112.5 and angle < 157.5) or (angle < -22.5 and angle > -67.5))
            {
                if ((magnitude > neighborhood(0, 2)) and (magnitude > neighborhood(2, 0)))
                    original(x, y) = 0.1;

                if ((neighborhood(0, 2) > lower) or (neighborhood(2, 0) > lower))
                    has_neighbor = true;
            }
            // -45°
            else if ((angle > 22.5 and angle < 67.5) or (angle < -112.5 and angle > -157.5))
            {
                if ((magnitude > neighborhood(0, 0)) and (magnitude > neighborhood(2, 2)))
                    original(x, y) = 0.1;

                if ((neighborhood(0, 0) > lower) or (neighborhood(2, 2) > lower))
                    has_neighbor = true;
            }

            if (original(x, y) > 0)
            {
                if (magnitude >= upper and has_neighbor)
                    original(x, y) = 1;
                else
                    original(x, y) = 0;
            }

        }
    }
    auto copy = original;
    // post process
    for (long x = 0; x < original.get_size().x(); ++x)
        for (long y = 0; y < original.get_size().y; ++y)
        {
            size_t count = 0;
            for (int i = -1; i <= 1; i++)
                for (int j = -1; j <= 1; j++)
                {
                    if (original.get_pixel_or_padding(x + i, y + j) == 1)
                    {
                        count += 1;
                        if (count >= 2)
                            goto skip;
                    }
                }

            original(x, y) = 0;
            skip:;
        }

     *
    GrayScaleImage image;
    image.create_from_file("/home/clem/Workspace/image_processing/docs/opal_clean.png");

    auto transform = PseudoColorTransform();
    transform.set_function(PseudoColorTransform::value_range_to_inverse_hue_range(0, 0.1, 0, 0.1));

    auto color_image = transform.transform(image);

    auto sprite = Sprite();
    sprite.load_from(color_image);

    RenderWindow window;
    window.create(color_image.get_size().x(), image.get_size().y);

    window.clear();
    window.draw(sprite);
    window.display();

    while (window.is_open())
    {
        window.update();
    }

    return 0;*/

    /*
    GrayScaleImage image_in;
    image_in.create_from_file("/home/clem/Workspace/image_processing/docs/opal_clean.png");
    image_in.set_padding_type(GrayScaleImage::PaddingType::STRETCH);

    FourierTransform<SPEED> fourier;
    fourier.transform_from(image_in);

    auto filter = FrequencyDomainFilter(fourier.get_size().x(), fourier.get_size().y);
    filter.set_function(filter.gaussian_bandreject(10, 100));
    filter.set_offset(50, 50);

    auto filter_2 = FrequencyDomainFilter(fourier.get_size().x(), fourier.get_size().y);
    filter_2.set_function(filter.gaussian_bandreject(10, 100));
    filter_2.set_offset(-25, 50);
    filter *= filter_2;
    filter.apply_to(fourier);

    auto spectrum = fourier.spectrum_to_image<GrayScaleImage>();
    auto spectrum_sprite = Sprite();

    auto image_out = fourier.transform_to<GrayScaleImage>();
    auto image_sprite = Sprite();

    RenderWindow window;
    window.create(image_in.get_size().x() * 2, image_in.get_size().y * 2);

    auto filter_sprite = Sprite();

    spectrum_sprite.load_from(spectrum);
    image_sprite.load_from(image_out);
    filter_sprite.load_from(filter);

    bool which = false;
    while (window.is_open())
    {
        window.update();

        if (InputHandler::was_key_pressed(SPACE))
            which = not which;

        window.clear();
        window.draw(which ? spectrum_sprite : image_sprite);
        //window.draw(filter_sprite);
        window.display();
    }*/
}

/*
int main()
{
    GrayScaleImage image_in;
    image_in.create_from_file("/home/clem/Workspace/image_processing/docs/opal_clean.png");
    image_in.set_padding_type(GrayScaleImage::PaddingType::MIRROR);

    RenderWindow window;
    window.create(image_in.get_size().x() * 2, image_in.get_size().y * 2);

    std::cout << "image: " << image_in.get_size().x() << "x" << image_in.get_size().y << std::endl;

    size_t n = image_in.get_size().x() * 2;
    size_t m = image_in.get_size().y * 2;

    auto* in = fftwl_alloc_complex(n*m);
    auto* out = fftwl_alloc_complex(n*m);

    {
    size_t i = 0;
    for (long x = 0; x < n; ++x)
        for (long y = 0; y < m; ++y, ++i)
        {
            in[i][0] = image_in.get_pixel_or_padding(x, y) * pow(-1, x + y);
            in[i][1] = 0;
        }
    }

    auto plan_to = fftwl_plan_dft_2d(n, m, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
    auto plan_from = fftwl_plan_dft_2d(n, m, out, in, FFTW_BACKWARD, FFTW_ESTIMATE);

    fftwl_execute(plan_to);

    // output
    GrayScaleImage image_out;
    image_out.create(n, m, 1);

    size_t i = 0;

    double mean = 0;
    double min = std::numeric_limits<double>::max();
    double max = std::numeric_limits<double>::min();
    size_t max_i = 0;

    UniformNoise noise;
    noise.set_interval(-1, 1);

    for (long x = 0; x < n; ++x)
    {
        for (long y = 0; y < m; ++y, ++i)
        {
            auto f = std::complex<double>(out[i][0], out[i][1]);

            double spectrum = abs(f);
            double angle = arg(f);

            f = std::polar(spectrum, angle);
            out[i][0] = f.real();
            out[i][1] = f.imag();

            double value = log(1 + spectrum);
            min = std::min(value, min);

            if (value > max)
                max_i = i;

            max = std::max(value, max);
        }
    }

    std::cout << max_i << std::endl;
    std::cout << n*m/2 + m/2 << std::endl;

    // normalize intensities
    i = 0;
    for (long x = 0; x < n; ++x)
    {
        for (long y = 0; y < m; ++y, ++i)
        {
            auto f = std::complex<double>(out[i][0], out[i][1]);
            double value = log(1 + abs(f));

            if (min < 0)
            {
                value += min;
                value /= max;
            }
            else
            {
                value -= min;
                value /= (max + min);
            }

            image_out(x, y) = value;
        }
    }

    Sprite sprite;
    sprite.load_from(image_out);

    image_in.create(n/2, m/2, 1);

    fftwl_execute(plan_from);
    i = 0;
    for (long x = 0; x < n/2; ++x, i += m/2)
    {
        for (long y = 0; y < m/2; ++y, ++i)
        {
            image_in(x, y) = in[i][0] / (m * n) * pow(-1, x + y);
        }
    }

    bool which = false;
    while (window.is_open())
    {
        window.update();

        if (InputHandler::was_key_pressed(SPACE))
        {
            which = not which;
            sprite.load_from(which ? image_in : image_out);
        }

        window.clear();
        window.draw(sprite);
        window.display();
    }

    fftwl_destroy_plan(plan_to);
    fftwl_destroy_plan(plan_from);
    fftwl_free(in);
    fftwl_free(out);
}

/*
template<typename T>
T project(T lower_bound, T upper_bound, T value)
{
    return value * fabs(lower_bound - upper_bound) + std::min(lower_bound, upper_bound);
}

int main()
{
    GrayScaleImage image_in;
    image_in.create_from_file("/home/clem/Workspace/image_processing/docs/opal_clean.png");
    image_in.set_padding_type(GrayScaleImage::PaddingType::ONE);

    RenderWindow window;
    window.create(image_in.get_size().x() * 2, image_in.get_size().y * 2);

    size_t m = image_in.get_size().x() * 2;
    size_t n = image_in.get_size().y * 2;

    auto filter = FrequencyDomainFilter(m, n);
    size_t order = 1;

    bool which = true;
    auto filter_sprite= Sprite();

    redraw:

    if (which)
        filter.set_function(filter.gaussian_bandreject(40, 75));
    else
        filter.set_function(filter.ideal_bandreject(40, 75));

    filter_sprite.load_from(filter);

    window.clear();
    window.draw(filter_sprite);
    window.display();

    while (window.is_open())
    {
        window.update();

        if (InputHandler::was_key_pressed(SPACE))
        {
            which = not which;
            goto redraw;
        }
    }
}


/*
int main()
{
    float border = 5;
log(spectrum)
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

    std::vector<Vector2ui> sample_pixels;
    for (long x = 0.7 * image.get_size().x(); x < image.get_size().x(); ++x)
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
    sample_sprite.align_topleft_with({window.getSize().x - 5 - sample_sprite.get_size().x(), 5});
    sample_hist_sprite.align_topleft_with({sample_sprite.get_topleft().x - sample_hist_sprite.get_size().x() - 10, sample_sprite.get_topleft().y + sample_sprite.get_size().x() * 0.5f});

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

    std::vector<Vector2ui> pixels;
    for (long x = 0; x < image.get_size().x(); ++x)
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


