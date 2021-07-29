// 
// Copyright 2021 Clemens Cords
// Created on 28.07.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <image.hpp>
#include <binary_image.hpp>
#include <grayscale_image.hpp>

namespace crisp
{
    struct ImageHandler
    {
        template<typename Int_t = uint8_t>
        [[nodiscard]] static BinaryImage get_nths_bitplane(GrayScaleImage&, Int_t which_bit);
    };

    template<typename Int_t>
    BinaryImage ImageHandler::get_nths_bitplane(GrayScaleImage& image, Int_t which_bit)
    {
        //static_assert(std::is_integral_v<Int_t>, "bitmask cannot be non-integral type");
        //static_assert(which_bit < std::numeric_limits<Int_t>::digits, "n-th bit cannot be large than total number of base-radix");

        BinaryImage out;
        out.create(image.get_size().x, image.get_size().y);

        const Int_t bitmask = Int_t(1) << (std::numeric_limits<Int_t>::digits - which_bit - 1);

        size_t n_false = 0, n_true = 0;
        for (long x = 0; x < image.get_size().x; ++x)
        {
            for (long y = 0; y < image.get_size().y; ++y)
            {
                float value_f = image(x, y);

                auto value_i = static_cast<Int_t>(value_f * std::numeric_limits<Int_t>::max());
                value_i = value_i & bitmask;
                value_i = value_i >> (std::numeric_limits<Int_t>::digits - which_bit - 1);

                out(x, y) = (value_i == Int_t(1));
            }
        }

        out.update_image();
        return out;
    }
}

#include ".src/image_handler.inl"