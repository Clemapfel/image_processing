// 
// Copyright 2021 Clemens Cords
// Created on 28.07.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <image.hpp>

namespace crisp
{
    // an image where each pixel has exactly one float intensity value
    class GrayScaleImage : public Image<float>
    {
        public:
            // @brief construct an image of size 0
            GrayScaleImage();

            // @override c.f. Image<...>::create in image.hpp
            void create(long width, long height, float init = -1.f) override;


            // @brief load an image from the disk
            // @param path: the absolute file path
            // @note: all common file types are supported, a color image will be converted into a gray-scale image
            //        via the formula: gray_v(x, y) = (color_v(x, y).r + .g + .b) / 3.f
            void create_from_file(std::string path);

            // @override, c.f. Image<...>::get_size in image.hpp
            [[nodiscard]] sf::Vector2<long> get_size() const override;

        protected:
            // @override, c.f. Image<...>::get_pixel in image.hpp
            virtual float  get_pixel(long x, long y) const override;
            virtual float& get_pixel(long x, long y) override;

        private:
            sf::Vector2<long> _size;
            Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> _value;
    };
}

#include ".src/grayscale_image.inl"
