// 
// Copyright 2021 Clemens Cords
// Created on 09.08.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <complex>
#include <vector>
#include <SFML/System/Vector2.hpp>

namespace crisp
{
    class FourierTransformFilter;
    enum FourierTransformMode : bool {ACCURACY = true, SPEED = false};

    template<bool AccuracyOrSpeed = FourierTransformMode::ACCURACY>
    class FourierTransform
    {
        friend class FrequencyDomainFilter;

        public:
            FourierTransform() = default;

            template<typename Image_t>
            void create_from(const Image_t&);

            std::complex<double> get_coefficient(long x, long y);
            double get_magnitude(long x, long y);
            double get_phase_angle(long x, long y);

            template<typename Image_t>
            Image_t to_image() const;

            sf::Vector2<long> get_size();

        private:
            sf::Vector2<long> _size;
            std::vector<double> _magnitude,
                                _phase_angle;

    };

    template<>
    class FourierTransform<FourierTransformMode::ACCURACY>
    {};

    template<>
    class FourierTransform<FourierTransformMode::SPEED>
    {};

    /*
     *
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
    window.create(image_in.get_size().x * 2, image_in.get_size().y * 2);

    size_t n = image_in.get_size().x * 2;
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

    const auto hn = n / 2, hm = m / 2;
    size_t i = 0;

    double mean = 0;
    double min = std::numeric_limits<double>::max();
    double max = std::numeric_limits<double>::min();

    for (long x = 0; x < n; ++x)
    {
        for (long y = 0; y < m; ++y, ++i)
        {
            auto f = std::complex<double>(out[i][0], out[i][1]);

            double spectrum = abs(f);
            double angle = arg(f);

            // filter here

            f = std::polar(spectrum, angle);
            out[i][0] = f.real();
            out[i][1] = f.imag();

            double value = log(1 + spectrum);
            min = std::min(value, min);
            max = std::max(value, max);
        }
    }

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
    for (long x = 0; x < n; ++x)
        for (long y = 0; y < m; ++y, ++i)
            if (x < n/2 and y < m/2)
            {
                image_in(x, y) = in[i][0] / (m*n) * pow(-1, x+y);
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
     */
}