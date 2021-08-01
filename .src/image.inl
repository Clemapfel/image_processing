// 
// Copyright 2021 Clemens Cords
// Created on 28.07.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
#include <iostream>

namespace crisp
{
    template<typename Value_t>
    typename Image<Value_t>::Iterator Image<Value_t>::begin()
    {
        return Image::Iterator(this, 0, 0);
    }

    template<typename Value_t>
    typename Image<Value_t>::Iterator Image<Value_t>::end()
    {
        return Image::Iterator(this, get_size().x, get_size().y);
    }


    template<typename Value_t>
    typename Image<Value_t>::PaddingType Image<Value_t>::get_padding_type() const
    {
        return _padding_type;
    }

    template<typename Value_t>
    void Image<Value_t>::set_padding_type(PaddingType type)
    {
        _padding_type = type;
    }

    template<typename Value_t>
    Value_t Image<Value_t>::operator()(long x, long y) const
    {
        return get_pixel_out_of_bounds(x, y);
    }

    template<typename Value_t>
    Value_t Image<Value_t>::get_pixel_or_padding(long x, long y) const
    {
        return operator()(x, y);
    }

    template<typename Value_t>
    Value_t& Image<Value_t>::operator()(long x, long y)
    {
        if (x < 0 or x >= get_size().x or y < 0 or y >= get_size().y)
            throw std::out_of_range("index outside of image bounds, please use Value_t operator()(long, long) const to access padding");

        return get_pixel(x, y);
    }

    template<typename Value_t>
    Value_t Image<Value_t>::get_pixel_out_of_bounds(long x, long y) const
    {
        if (x >= 0 and x < get_size().x and y >= 0 and y < get_size().y)
            return get_pixel(x, y);

        switch (_padding_type)
        {
            case ZERO:
                return Value_t(0);
            case ONE:
                return Value_t(1);
            case REPEAT:
                return get_pixel(x % get_size().x, y % get_size().y);
            case MIRROR:
                return get_pixel(get_size().x - (x % get_size().x), get_size().y - (y % get_size().y));
            case STRETCH:
                return get_pixel(x < 0 ? 0 : get_size().x - 1, y < 0 ? 0 : get_size().y - 1);
        }
    }

    // ### Iterator ##################################################
    template<typename Value_t>
    Image<Value_t>::Iterator::Iterator(Image<Value_t>* data, size_t x, size_t y)
        : _data(data), _x(x), _y(y), _size(_data->get_size().x, _data->get_size().y)
    {}

    template<typename Value_t>
    bool Image<Value_t>::Iterator::operator==(Iterator& other) const
    {
        return _data == other._data and _x == other._x and _y == other._y;
    }

    template<typename Value_t>
    bool Image<Value_t>::Iterator::operator!=(Iterator& other) const
    {
        return not operator==(other);
    }

    template<typename Value_t>
    typename Image<Value_t>::Iterator & Image<Value_t>::Iterator::operator++()
    {
        if (_x < _size.x - 1)
            _x++;
        else if (_y < _size.y - 1)
        {
            _x = 0;
            _y++;
        }
        else
        {
            _x++;
            _y++;
        }

        return *this;
    }

    template<typename Value_t>
    typename Image<Value_t>::Iterator & Image<Value_t>::Iterator::operator--()
    {
        if (_x > 0)
            _x--;
        else if (_y > 0)
        {
            _x = _size.x - 1;
            _y--;
        }

        return *this;
    }

    template<typename Value_t>
    Value_t & Image<Value_t>::Iterator::operator*() const
    {
        return _data->operator()(_x, _y);
    }

    template<typename Value_t>
    typename Image<Value_t>::Iterator & Image<Value_t>::Iterator::operator=(Value_t value)
    {
        _data->operator()(_x, _y) = value;
        return *this;
    }
}