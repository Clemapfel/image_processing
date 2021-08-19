// 
// Copyright 2021 Clemens Cords
// Created on 19.08.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <image.hpp>
#include <grayscale_image.hpp>
#include <color_image.hpp>
#include <vector.hpp>

namespace crisp
{
    // n-dimensional image for arbitrary formats
    template<typename VectorValue_t, size_t N>
    class MultiPlaneImage : public Image<Vector<VectorValue_t, N>>
    {
        public:
            using Value_t = Vector<VectorValue_t, N>>;

            MultiPlaneImage() = default;

            // @brief create the image
            // @param width: the x-dimension of the image
            // @param height: the y-dimension of the image
            // @param init: the initial value of each pixel
            void create(long width, long height, Value_t init = Value_t(0));

            // @brief access the nth vector component of each pixel as a grayscale image
            // @param n: the planes index
            // @returns: GrayScaleImage of the components
            GrayScaleImage get_nth_plane(size_t n);

            // @brief set the nth vector component of each pixel
            // @param : GrayScaleImage holding the values
            void set_nth_plane(const GrayScaleImage&);

            // @brief transform to rgb image by selecting three of the n planes
            // @param red_plane_i: component index translated to red
            // @param green_plane_i: component index translated to green
            // @param blue_plane_i: component index translated to blue
            // @returns rgb image
            ColorImage to_rgb_image(size_t red_plane_i, size_t green_plane_i, size_t blue_plane_i);

        protected:
            Vector2ui _size;
            Eigen::Matrix<Value_t, Eigen::Dynamic, Eigen::Dynamic> _values;

        private:
            Value_t get_pixel(long x, long y) const override;
            Value_t& get_pixel(long x, long y) override;
    };
}

#include ".src/multi_plane_image.inl"