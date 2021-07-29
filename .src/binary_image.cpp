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

    BinaryImage BinaryImage::operator!() const
    {
        BinaryImage out;
        out.create(get_size().x, get_size().y);

        for (long x = 0; x < get_size().x; ++x)
            for (long y = 0; y < get_size().y; ++y)
                out(x, y) = not operator()(x, y);

        return out;
    }

    BinaryImage BinaryImage::operator&(const BinaryImage& other) const
    {
        BinaryImage out;
        out.create(get_size().x, get_size().y);

        for (long x = 0; x < get_size().x; ++x)
            for (long y = 0; y < get_size().y; ++y)
                out(x, y) = (*this)(x, y) & other(x, y);

        return out;
    }

    BinaryImage BinaryImage::operator|(const BinaryImage& other) const
    {
        BinaryImage out;
        out.create(get_size().x, get_size().y);

        for (long x = 0; x < get_size().x; ++x)
            for (long y = 0; y < get_size().y; ++y)
                out(x, y) = (*this)(x, y) | other(x, y);

        return out;
    }

    BinaryImage BinaryImage::operator^(const BinaryImage& other) const
    {
        BinaryImage out;
        out.create(get_size().x, get_size().y);

        for (long x = 0; x < get_size().x; ++x)
            for (long y = 0; y < get_size().y; ++y)
                out(x, y) = (*this)(x, y) ^ other(x, y);

        return out;
    }

    BinaryImage& BinaryImage::invert()
    {
        for (bool& b : (*this))
            b = not b;

        return *this;
    }

    BinaryImage& BinaryImage::operator&=(const BinaryImage& other)
    {
        for (long x = 0; x < get_size().x; ++x)
            for (long y = 0; y < get_size().y; ++y)
                (*this)(x, y) = (*this)(x, y) & other(x, y);

        return *this;
    }

    BinaryImage& BinaryImage::operator|=(const BinaryImage& other)
    {
        for (long x = 0; x < get_size().x; ++x)
            for (long y = 0; y < get_size().y; ++y)
                (*this)(x, y) = (*this)(x, y) | other(x, y);

        return *this;
    }

    BinaryImage& BinaryImage::operator^=(const BinaryImage& other)
    {
        for (long x = 0; x < get_size().x; ++x)
            for (long y = 0; y < get_size().y; ++y)
                (*this)(x, y) = (*this)(x, y) ^ other(x, y);

        return *this;
    }
}

