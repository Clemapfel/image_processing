// 
// Copyright 2021 Clemens Cords
// Created on 27.07.21 by clem (mail@clemens-cords.com)
//

#include <stdexcept>

#include <SFML/Graphics/RenderTarget.hpp>

#include <image.hpp>

namespace crisp
{
    // ### Image ##################################################

    GrayScaleImage::GrayScaleImage()
    {
        _sprite.setPosition(0, 0);
    }

    float & GrayScaleImage::operator()(long x, long y)
    {
        return _value(x, y);
    }

    void GrayScaleImage::create(long width, long height, BitDepth depth)
    {
        _size.x = width;
        _size.y = height;

        _center_pos = {width * 0.5f, height * 0.5f};

        _image.create(width, height);
        _texture.loadFromImage(_image);
        _sprite.setTexture(_texture);

        _value.resize(static_cast<Eigen::Index>(width), static_cast<Eigen::Index>(height));
        _bit_depth = depth;
    }

    void GrayScaleImage::create_from_file(std::string path, BitDepth depth)
    {
        _texture.loadFromFile(path);
        _image = _texture.copyToImage();

        _size.x = _image.getSize().x;
        _size.y = _image.getSize().y;
        _value.resize(_image.getSize().x, _image.getSize().y);

        for (long x = 0; x < _image.getSize().x; ++x)
        {
            for (long y = 0; y < _image.getSize().y; ++y)
            {
                auto color = _image.getPixel(x, y);
                float value = color.r / 255.f + color.g / 255.f + color.b / 255.f;
                _value(x, y) = value / 3;
            }
        }

        update_image();

        _bit_depth = depth;
    }

    void GrayScaleImage::update_image()
    {
        for (long x = 0; x < _size.x; ++x)
        {
            for (long y = 0; y < _size.y; ++y)
            {
                uint8_t value = static_cast<uint8_t>(_value(x, y) * uint8_t(255));
                _image.setPixel(x, y, sf::Color(value, value, value, 255));
            }
        }

        _texture.loadFromImage(_image);
        _sprite.setTexture(_texture);
    }

    void GrayScaleImage::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(_center_pos.x - 0.5f * _size.x, _center_pos.y - 0.5f * _size.y);
        states.transform.scale(_zoom_factor, _zoom_factor);
        target.draw(_sprite, states);
    }

    void GrayScaleImage::align_topleft_with(Vector2f top_left)
    {
        _center_pos.x = top_left.at(0) + _size.x * 0.5;
        _center_pos.y = top_left.at(1) + _size.y * 0.5;
    }

    void GrayScaleImage::align_center_with(Vector2f center)
    {
        _center_pos.x = center.at(0);
        _center_pos.y = center.at(1);
    }

    void GrayScaleImage::zoom(float factor, bool smooth)
    {
        _zoom_factor = factor;
        if (smooth != _texture.isSmooth())
            _texture.setSmooth(smooth);
    }

    GrayScaleImage::Iterator GrayScaleImage::begin()
    {
        return GrayScaleImage::Iterator(&_value, 0, 0);
    }

    GrayScaleImage::Iterator GrayScaleImage::end()
    {
        return GrayScaleImage::Iterator(&_value, _size.x, _size.y);
    }

    // ### Iterator ##################################################

    GrayScaleImage::Iterator::Iterator(Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic>* data, size_t x, size_t y)
        : _data(data), _x(x), _y(y), _size(_data->rows(), _data->cols())
    {}

    bool GrayScaleImage::Iterator::operator==(Iterator& other) const
    {
        return _data == other._data and _x == other._x and _y == other._y;
    }

    bool GrayScaleImage::Iterator::operator!=(Iterator& other) const
    {
        return not operator==(other);
    }

    GrayScaleImage::Iterator & GrayScaleImage::Iterator::operator++()
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

    GrayScaleImage::Iterator & GrayScaleImage::Iterator::operator--()
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

    float & GrayScaleImage::Iterator::operator*() const
    {
        return _data->operator()(_x, _y);
    }

    void GrayScaleImage::Iterator::operator=(float new_value)
    {
        _data->operator()(_x, _y) = new_value;
    }


    /*
    Image::Image()
    {}

    void Image::create(size_t width, size_t height, BitDepth bit_depth, bool sRGB)
    {
        _size.x = static_cast<float>(width);
        _size.y = static_cast<float>(height);

        _bit_depth = bit_depth;
        _is_srgb = sRGB;
        
        _shape[0].position = {0,0};
        _shape[0].color = sf::Color::White;
        _shape[0].texCoords = {0,0};
    
        _shape[1].position = {_size.x, 0};
        _shape[1].color = sf::Color::White;
        _shape[1].texCoords = {_size.x,0};
    
        _shape[2].position = {_size.x, _size.y};
        _shape[2].color = sf::Color::White;
        _shape[2].texCoords = {_size.x,_size.y};
    
        _shape[3].position = {0, _size.y};
        _shape[3].color = sf::Color::White;
        _shape[3].texCoords = {0,_size.y};

        _center_pos = {_size.x * 0.5f, _size.y * 0.5f};
    }

    void Image::create_from_file(std::string texture_path, BitDepth bit_depth, bool sRGB)
    {
        if (not _texture.loadFromFile(texture_path))
            throw std::invalid_argument("cannot access texture at path \"" + texture_path + "\".");

        _size.x = static_cast<float>(_texture.getSize().x);
        _size.y = static_cast<float>(_texture.getSize().y);

        _bit_depth = bit_depth;
        _is_srgb = sRGB;

        _texture.setSrgb(_is_srgb);

        _shape[0].position = {0,0};
        _shape[0].color = sf::Color::White;
        _shape[0].texCoords = {0,0};

        _shape[1].position = {_size.x, 0};
        _shape[1].color = sf::Color::White;
        _shape[1].texCoords = {_size.x,0};

        _shape[2].position = {_size.x, _size.y};
        _shape[2].color = sf::Color::White;
        _shape[2].texCoords = {_size.x,_size.y};

        _shape[3].position = {0, _size.y};
        _shape[3].color = sf::Color::White;
        _shape[3].texCoords = {0,_size.y};
    }

    void Image::set_smoothing_enabled(bool enabled)
    {
        _texture.setSmooth(enabled);
    }

    bool Image::is_smoothing_enabled() const
    {
        return _texture.isSmooth();
    }

    void Image::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.texture = &_texture;
        states.transform = sf::Transform();
        states.transform.translate({_center_pos.x - _size.x * 0.5f, _center_pos.y - _size.y * 0.5f});
        target.draw(_shape, states);
    }

    void Image::align_topleft_with(Vector2f top_left)
    {
        _center_pos.x = top_left.at(0) + _size.x * 0.5;
        _center_pos.y = top_left.at(1) + _size.y * 0.5;
    }

    void Image::align_center_with(Vector2f center)
    {
        _center_pos.x = center.at(0);
        _center_pos.y = center.at(1);
    }
     */
}

