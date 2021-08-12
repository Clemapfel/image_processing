// 
// Copyright 2021 Clemens Cords
// Created on 12.08.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <color.hpp>
#include <color_image.hpp>
#include <detail/common.hpp>

namespace crisp
{
    // maps grayscale values to color values
    class PseudoColorTransform
    {
        public:
            PseudoColorTransform() = default;

            ColorImage transform(const GrayScaleImage&);
            ColorImage transform(const ColorImage& grayscale);

            void set_function(std::function<Color(float)>&& mapping);

            static auto&& identity();

            // @brief maps grayvalues in [min_gray, max_gray] onto full color HSV values where hue in [min_hue, max_hue]
            static auto&& value_range_to_hue_range(float min_gray, float max_gray, float min_hue = 0, float max_hue = 1);
            static auto&& value_range_to_inverse_hue_range(float min_gray, float max_gray, float min_hue = 0, float max_hue = 1);

        private:
            std::function<Color(float)> _function = [](float x) -> Color {return Color(x);};
    };

    // maps color values to color values
    class ColorTransform
    {

    };
}

#include ".src/color_transform.inl"
