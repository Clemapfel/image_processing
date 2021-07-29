// 
// Copyright 2021 Clemens Cords
// Created on 28.07.21 by clem (mail@clemens-cords.com)
//

#include <binary_image.hpp>

namespace crisp
{
    void BinaryImage::create(long width, long height)
    {
        _size.x = width;
        _size.y = height;
        _value.resize(width, height);

        for (long x = 0; x < width; ++x)
            for (long y = 0; y < height; ++y)
                _value(x, y) = false;
    }

    bool & BinaryImage::operator()(long x, long y)
    {
        return _value(x, y);
    }

    bool BinaryImage::operator()(long x, long y) const
    {
        return _value(x, y);
    }

    sf::Vector2<long> BinaryImage::get_size() const
    {
        return _size;
    }
}

