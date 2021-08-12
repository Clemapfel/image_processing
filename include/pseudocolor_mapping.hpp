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

            // @brief transform a grayscale image into a color image my mapping certain gray values to the specified hues
            // @param : the grayscale image
            // @returns resulting color image
            ColorImage transform(const GrayScaleImage&);

            // @brief set the transform function
            // @param mapping: function mapping float grayscale values onto colors
            void set_function(std::function<Color(float)>&& mapping);

            // @brief the identity function, maps gray value x to RBG(x, x, x, 1)
            static auto&& identity();

            // @brief map single gray value to single hue
            // @param gray: the exact gray value
            // @param hue: hue value
            // @returns lambda bindable with set_function
            //
            // @note equivalent to value_range_to_hue_range(gray, gray, hue, hue)
            static auto&& value_to_hue(float gray, float hue);

            // @brief map range of gray values onto single hue
            // @param min_gray: lower gray intensity bound
            // @param max_gray: upper gray intensity bound
            // @hue: hue value
            // @returns lambda bindable with set_function
            //
            // @note equivalent to value_range_to_hue_range(min_gray, max_gray, hue, hue)
            static auto&& value_range_to_hue(float min_gray, float max_gray, float hue);

            // @brief maps grayvalues in [min_gray, max_gray] onto HSV values with hue in [min_hue, max_hue]
            // @param min_gray: lower gray intensity bounds
            // @param max_gray: upper gray intensity bounds
            // @param min_hue: lower hue bounds, default 0
            // @param max_hue: upper hue bounds, default 1
            // @returns lambda bindable with set_function
            //
            // @note an assertion is raised unless min_gray <= max_gray and min_hue <= max_hue
            static auto&& value_range_to_hue_range(float min_gray, float max_gray, float min_hue = 0, float max_hue = 1);

            // @brief maps grayvalues in [min_gray, max_gray] onto HSV values with hue in [max_hue, min_hue]
            // @param min_gray: lower gray intensity bounds
            // @param max_gray: upper gray intensity bounds
            // @param min_hue: lower hue bounds, default 0
            // @param max_hue: upper hue bounds, default 1
            // @returns lambda bindable with set_function
            //
            // @note an assertion is raised unless min_gray <= max_gray and min_hue <= max_hue
            static auto&& value_range_to_inverse_hue_range(float min_gray, float max_gray, float min_hue = 0, float max_hue = 1);

            // container class that allows for specifying multiple ranges at the same time
            class RangeMapping
            {
                friend class PseudoColorTransform;

                public:
                    // @brief add exact value to single hue mappingm, c.f. PseudoColorTransform::value_to_hue
                    void add_value_to_hue(float gray, float hue);

                    // @brief add value range to single hue mapping, c.f. value_range_to_hue
                    void add_value_range_to_hue(float min_gray, float max_gray, float hue);

                    // @brief add value range to hue range mapping, c.f. PseudoColorTransform::value_range_to_hue_range
                    void add_value_range_to_hue_range(float min_gray, float max_gray, float min_hue, float max_hue);

                    // @brief add value range to inverse hue range mapping, c.f. PseudoColorTransform::value_to_range_to_inverse_hue_range
                    void add_value_range_to_inverse_hue_range(float min_gray, float max_gray, float min_hue, float max_hue);

                private:
                    std::vector<std::pair<std::pair<float, float>, std::pair<float, float>>> _gray_to_hue;
                    std::vector<std::pair<std::pair<float, float>, std::pair<float, float>>> _gray_to_inverse_hue;
            };

            // @brief map multiple gray intensity ranges onto multiple hue ranges simultaenously
            // @param mapping: range mapping, see above
            // @returns lambda bindable with set_function
            //
            // @note if one gray value fits into multiple ranges, the range of the mapping added **first** will be prioritized
            static auto&& value_ranges_to_hue_ranges(RangeMapping mapping);

        private:
            std::function<Color(float)> _function = [](float x) -> Color {return Color(x);};
    };
}

#include ".src/pseudocolor_mapping.inl"
