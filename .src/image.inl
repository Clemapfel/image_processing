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