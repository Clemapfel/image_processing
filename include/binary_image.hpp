// 
// Copyright 2021 Clemens Cords
// Created on 28.07.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <image.hpp>

namespace crisp
{
    // a black-and-white-only image where white = true, black = false
    class BinaryImage : public Image<bool>
    {
        public:
            // @brief construct an image of size 0
            BinaryImage() = default;

            // @override c.f. Image<...>::create in image.hpp
            void create(long width, long height) override;

            // @brief loads the image from the disk
            // @param path: the absolute path to the file
            // @param cutoff: a value in (0,1) such that the pixel will be white if the files value is >= cutoff
            void create_from_file(std::string path, float cutoff);

            // @override c.f. Image<...>::get_size in image.hpp
            sf::Vector2<long> get_size() const override;

            // bitwise operators
            // @brief element-wise binary NOT operator
            // @returns a copy of the processed image
            BinaryImage operator!() const;

            // @brief element-wise binary AND operator
            // @param : a const reference to another image of equal size
            // @returns a copy of the processed image
            BinaryImage operator&(const BinaryImage&) const;

            // @brief element-wise binary OR operator
            // @param : a const reference to another image of equal size
            // @returns a copy of the processed image
            BinaryImage operator|(const BinaryImage&) const;

            // @brief element-wise binary XOR operator
            // @param : a const reference to another image of equal size
            // @returns a copy of the processed image
            BinaryImage operator^(const BinaryImage&) const;

            // @brief applies the element-wise NOT operator to itself
            // @returns a reference to itself after processing
            BinaryImage& invert();

            // @brief element-wise binary AND operator assignment
            // @param : a const reference to another image of equal size
            // @returns a reference to itself after processing
            BinaryImage& operator&=(const BinaryImage&);

            // @brief element-wise binary OR operator assignment
            // @param : a const reference to another image of equal size
            // @returns a reference to itself after processing
            BinaryImage& operator|=(const BinaryImage&);

            // @brief element-wise binary XOR operator assignment
            // @param : a const reference to another image of equal size
            // @returns a reference to itself after processing
            BinaryImage& operator^=(const BinaryImage&);

        protected:
            // @override c.f. Image<...>::get_pixel in image.hpp
            virtual bool  get_pixel(long x, long y) const;
            virtual bool& get_pixel(long x, long y);

        private:
            sf::Vector2<long> _size;
            Eigen::Matrix<bool, Eigen::Dynamic, Eigen::Dynamic> _value;

    };
}