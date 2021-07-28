// 
// Copyright 2021 Clemens Cords
// Created on 28.07.21 by clem (mail@clemens-cords.com)
//

#pragma once

namespace crisp
{
    template<typename Value_t>
    void Image<Value_t>::update_image()
    {
        for (long x = 0; x < get_size().x; ++x)
        {
            for (long y = 0; y < get_size().y; ++y)
            {
                auto value = static_cast<uint8_t>(operator()(x, y) * uint8_t(255));
                _image.setPixel(x, y, sf::Color(value, value, value, 255));
            }
        }

        _texture.loadFromImage(_image);
        _sprite.setTexture(_texture);
        _sprite.setPosition(0, 0);
    }

    template<typename Value_t>
    void Image<Value_t>::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(_center_pos.x - 0.5f * get_size().x, _center_pos.y - 0.5f * get_size().y);
        states.transform.scale(_zoom_factor, _zoom_factor);
        target.draw(_sprite, states);
    }

    template<typename Value_t>
    void Image<Value_t>::align_topleft_with(Vector2f top_left)
    {
        _center_pos.x = top_left.at(0) + get_size().x * 0.5;
        _center_pos.y = top_left.at(1) + get_size().y * 0.5;
    }

    template<typename Value_t>
    void Image<Value_t>::align_center_with(Vector2f center)
    {
        _center_pos.x = center.at(0);
        _center_pos.y = center.at(1);
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
    float & Image<Value_t>::Iterator::operator*() const
    {
        return _data->operator()(_x, _y);
    }

    template<typename Value_t>
    void Image<Value_t>::Iterator::operator=(float new_value)
    {
        _data->operator()(_x, _y) = new_value;
    }
}