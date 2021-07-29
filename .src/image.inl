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
    void Image<Value_t>::update_image()
    {
        if (_image.getSize().x != get_size().x or _image.getSize().y != get_size().y)
            _image.create(get_size().x, get_size().y);

        for (long x = 0; x < get_size().x; ++x)
        {
            for (long y = 0; y < get_size().y; ++y)
            {
                auto value = static_cast<float>(operator()(x, y));
                assert(value >= 0 and value <= 1 && "value outside of boundaries or uninitialized");
                auto as_int = static_cast<uint8_t>(value * uint8_t(255));
                _image.setPixel(x, y, sf::Color(as_int, as_int, as_int, 255));
            }
        }

        _texture.loadFromImage(_image);
        _sprite.setTexture(_texture);
        _sprite.setPosition(0, 0);
    }

    template<typename Value_t>
    void Image<Value_t>::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(_top_left_pos.x, _top_left_pos.y);
        states.transform.scale(_zoom_factor, _zoom_factor);
        states.texture = &_texture;
        target.draw(_sprite, states);
    }

    template<typename Value_t>
    void Image<Value_t>::align_topleft_with(Vector2f top_left)
    {
        _top_left_pos.x = top_left.at(0);
        _top_left_pos.y = top_left.at(1);
    }

    template<typename Value_t>
    void Image<Value_t>::align_center_with(Vector2f center)
    {
        _top_left_pos.x = center.at(0) - get_size().x * 0.5;
        _top_left_pos.y = center.at(1) - get_size().y * 0.5;
    }

    template<typename Value_t>
    void Image<Value_t>::zoom(float factor, bool smooth)
    {
        _zoom_factor = factor;
        if (smooth != _texture.isSmooth())
            _texture.setSmooth(smooth);
    }

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