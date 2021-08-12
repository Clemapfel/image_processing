// 
// Copyright 2021 Clemens Cords
// Created on 12.08.21 by clem (mail@clemens-cords.com)
//

namespace crisp
{
    ColorImage PseudoColorTransform::transform(const GrayScaleImage& grayscale)
    {
        ColorImage out;
        out.create(grayscale.get_size().x, grayscale.get_size().y, Color(0,0,0,1));

        for (long x = 0; x < grayscale.get_size().x; ++x)
            for (long y = 0; y < grayscale.get_size().y; ++y)
                out(x, y) = _function(grayscale(x, y));

        return out;
    }

    void PseudoColorTransform::set_function(std::function<Color(float)>&& mapping)
    {
        _function = mapping;
    }

    auto && PseudoColorTransform::identity()
    {
        return std::move([](float x) -> Color {return Color(x);});
    }

    auto && PseudoColorTransform::value_range_to_hue_range(float min_gray, float max_gray, float min_hue, float max_hue)
    {
        return std::move([min_gray, max_gray, min_hue, max_hue](float x) -> Color {

            assert(min_gray <= max_gray and min_hue <= max_hue);

            if (x < min_gray or x > max_gray)
                return Color(x);
            else
            {
                float hue = x;
                hue -= min_gray;
                hue /= abs(max_gray - min_gray);

                hue = project(min_hue, max_hue, hue);

                return Color(HSV{hue, 1, 1});
            }
        });
    }

    auto && PseudoColorTransform::value_range_to_inverse_hue_range(float min_gray, float max_gray, float min_hue, float max_hue)
    {
        return std::move([min_gray, max_gray, min_hue, max_hue](float x) -> Color {

            assert(min_gray <= max_gray);

            if (x < min_gray or x > max_gray)
                return Color(x);
            else
            {
                float hue = x;
                hue -= min_gray;
                hue /= abs(max_gray - min_gray);

                hue = project(min_hue, max_hue, hue);
                hue = max_hue - (hue - min_hue);

                return Color(HSV{hue, 1, 1});
            }
        });
    }
}