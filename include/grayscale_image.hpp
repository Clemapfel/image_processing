// 
// Copyright 2021 Clemens Cords
// Created on 28.07.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <image.hpp>

namespace crisp
{
    class GrayScaleImage : public Image<float>
    {
        public:
            GrayScaleImage();
            void create(long width, long height) override;
            void create_from_file(std::string path);

            [[nodiscard]] sf::Vector2<long> get_size() const override;

        protected:
            virtual float  get_pixel(long x, long y) const override;
            virtual float& get_pixel(long x, long y) override;

        private:
            sf::Vector2<long> _size;
            Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> _value;
    };
}
