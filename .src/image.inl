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
    typename Image<Value_t>::ConstIterator Image<Value_t>::begin() const
    {
        return Image::ConstIterator(this, 0, 0);
    }

    template<typename Value_t>
    typename Image<Value_t>::Iterator Image<Value_t>::end()
    {
        return Image::Iterator(this, get_size().x() , get_size().y());
    }

    template<typename Value_t>
    typename Image<Value_t>::ConstIterator Image<Value_t>::end() const
    {
        return Image::ConstIterator(this, get_size().x(), get_size().y());
    }

    template<typename Value_t>
    PaddingType Image<Value_t>::get_padding_type() const
    {
        return _padding_type;
    }

    template<typename Value_t>
    void Image<Value_t>::set_padding_type(PaddingType type)
    {
        _padding_type = type;
    }

    template<typename Value_t>
    Value_t Image<Value_t>::get_pixel_or_padding(int x, int y) const
    {
        return get_pixel_out_of_bounds(x, y);
    }

    template<typename Value_t>
    Value_t& Image<Value_t>::operator()(long x, long y)
    {
        if (x < 0 or x >= get_size().x() or y < 0 or y >= get_size().y())
            throw std::out_of_range("index outside of image bounds, please use get_pixel_or_padding() to access padding");

        return get_pixel(x, y);
    }

    template<typename Value_t>
    Value_t Image<Value_t>::operator()(long x, long y) const
    {
        if (x < 0 or x >= get_size().x() or y < 0 or y >= get_size().y())
            throw std::out_of_range("index outside of image bounds, please use get_pixel_or_padding() to access padding");

        return get_pixel(x, y);
    }

    template<typename Value_t>
    Value_t Image<Value_t>::get_pixel_out_of_bounds(int x, int y) const
    {
        if (x >= 0 and x < get_size().x() and y >= 0 and y < get_size().y())
            return get_pixel(x, y);

        switch (_padding_type)
        {
            case ZERO:
                return Value_t(0);
            case ONE:
                return Value_t(1);
            case REPEAT:
            {
                int x_mod = x % int(get_size().x());
                int y_mod = y % int(get_size().y());

                if (x_mod < 0)
                    x_mod += get_size().x();

                if (y_mod < 0)
                    y_mod += get_size().y();

                return get_pixel(x_mod, y_mod);
            }
            case MIRROR:
            {
                int new_x = x % (get_size().x() - 1);
                if (x < 0)
                    new_x = abs(new_x);
                else if (x >= get_size().x())
                    new_x = get_size().x() - 1 - new_x;

                int new_y = y % (get_size().y() - 1);
                if (y < 0)
                    new_y = abs(new_y);
                else if (y >= get_size().y())
                    new_y = get_size().y() - 1 - new_y;

                return get_pixel(new_x, new_y);
            }
            case STRETCH:
            {
                int new_x = x;
                if (x < 0)
                    new_x = 0;
                if (x >= get_size().x())
                    new_x = get_size().x() - 1;

                int new_y = y;
                if (y < 0)
                    new_y = 0;
                if (y >= get_size().y())
                    new_y = get_size().y() - 1;

                return get_pixel(new_x, new_y);
            }
            default:
                return Value_t(0);
        }
    }

    template<typename Value_t>
    auto Image<Value_t>::operator+(const Image<Value_t>& other) const
    {
        assert(get_size() == other.get_size());
        Image<Value_t> out = *this;

        for (long x = 0; x < other.size().x; ++x)
            for (long y = 0; y < other.size().y; ++y)
                out(x, y) += other(x, y);

        return out;
    }

    template<typename Value_t>
    auto Image<Value_t>::operator-(const Image<Value_t>& other) const
    {
        assert(get_size() == other.get_size());
        Image<Value_t> out = *this;

        for (long x = 0; x < other.size().x; ++x)
            for (long y = 0; y < other.size().y; ++y)
                out(x, y) -= other(x, y);

        return out;
    }

    template<typename Value_t>
    auto Image<Value_t>::operator*(const Image<Value_t>& other) const
    {
        assert(get_size() == other.get_size());
        Image<Value_t> out = *this;

        for (long x = 0; x < other.size().x; ++x)
            for (long y = 0; y < other.size().y; ++y)
                out(x, y) *= other(x, y);

        return out;
    }

    template<typename Value_t>
    auto Image<Value_t>::operator/(const Image<Value_t>& other) const
    {
        assert(get_size() == other.get_size());
        Image<Value_t> out = *this;

        for (long x = 0; x < other.size().x; ++x)
            for (long y = 0; y < other.size().y; ++y)
                out(x, y) /= other(x, y);

        return out;
    }

    template<typename Value_t>
    Image<Value_t>& Image<Value_t>::operator+=(const Image<Value_t>& other)
    {
        assert(get_size() == other.get_size());

        for (long x = 0; x < other.size().x; ++x)
            for (long y = 0; y < other.size().y; ++y)
                (*this)(x, y) += other(x, y);

        return *this;
    }

    template<typename Value_t>
    Image<Value_t>& Image<Value_t>::operator-=(const Image<Value_t>& other)
    {
        assert(get_size() == other.get_size());

        for (long x = 0; x < other.size().x; ++x)
            for (long y = 0; y < other.size().y; ++y)
                (*this)(x, y) -= other(x, y);

        return *this;
    }

    template<typename Value_t>
    Image<Value_t>& Image<Value_t>::operator*=(const Image<Value_t>& other)
    {
        assert(get_size() == other.get_size());

        for (long x = 0; x < other.size().x; ++x)
            for (long y = 0; y < other.size().y; ++y)
                (*this)(x, y) *= other(x, y);

        return *this;
    }

    template<typename Value_t>
    Image<Value_t>& Image<Value_t>::operator/=(const Image<Value_t>& other)
    {
        assert(get_size() == other.get_size());

        for (long x = 0; x < other.size().x; ++x)
            for (long y = 0; y < other.size().y; ++y)
                (*this)(x, y) /= other(x, y);

        return *this;
    }

    // ### Iterator ##################################################
    template<typename Value_t>
    Image<Value_t>::Iterator::Iterator(Image<Value_t>* data, size_t x, size_t y)
        : _data(data), _x(x), _y(y), _size(_data->get_size().x(), _data->get_size().y())
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
        if (_x <_size.x() - 1)
            _x++;
        else if (_y < _size.y() - 1)
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
            _x =_size.x() - 1;
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

    // const

    template<typename Value_t>
    Image<Value_t>::ConstIterator::ConstIterator(const Image<Value_t>* data, size_t x, size_t y)
        : _data(data), _x(x), _y(y), _size(_data->get_size().x(), _data->get_size().y())
    {}

    template<typename Value_t>
    bool Image<Value_t>::ConstIterator::operator==(ConstIterator& other) const
    {
        return _data == other._data and _x == other._x and _y == other._y;
    }

    template<typename Value_t>
    bool Image<Value_t>::ConstIterator::operator!=(ConstIterator& other) const
    {
        return not operator==(other);
    }

    template<typename Value_t>
    typename Image<Value_t>::ConstIterator & Image<Value_t>::ConstIterator::operator++()
    {
        if (_x <_size.x() - 1)
            _x++;
        else if (_y < _size.y() - 1)
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
    typename Image<Value_t>::ConstIterator & Image<Value_t>::ConstIterator::operator--()
    {
        if (_x > 0)
            _x--;
        else if (_y > 0)
        {
            _x =_size.x() - 1;
            _y--;
        }

        return *this;
    }

    template<typename Value_t>
    const Value_t & Image<Value_t>::ConstIterator::operator*() const
    {
        return _data->operator()(_x, _y);
    }
}