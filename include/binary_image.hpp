// 
// Copyright 2021 Clemens Cords
// Created on 28.07.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <image.hpp>

namespace crisp
{
    class BinaryImage : public Image<bool>
    {
        public:
            BinaryImage() = default;
            void create(long width, long height) override;
            void create_from_file(std::string path, float cutoff);

            bool& operator()(long x, long y) override;
            bool operator()(long x, long y) const override;
            sf::Vector2<long> get_size() const override;

            BinaryImage operator!() const;
            BinaryImage operator&(const BinaryImage&) const;
            BinaryImage operator|(const BinaryImage&) const;
            BinaryImage operator^(const BinaryImage&) const;

            BinaryImage& invert();   // equivalent to this = not this
            BinaryImage& operator&=(const BinaryImage&);
            BinaryImage& operator|=(const BinaryImage&);
            BinaryImage& operator^=(const BinaryImage&);

        private:
            sf::Vector2<long> _size;
            Eigen::Matrix<bool, Eigen::Dynamic, Eigen::Dynamic> _value;

    };
}