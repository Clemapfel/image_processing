// 
// Copyright 2021 Clemens Cords
// Created on 27.07.21 by clem (mail@clemens-cords.com)
//

#include <stdexcept>

#include <SFML/Graphics/RenderTarget.hpp>

#include <image.hpp>
#include <grayscale_image.hpp>

namespace crisp
{
    // ### Image ##################################################

    GrayScaleImage::GrayScaleImage()
    {}

    float & GrayScaleImage::operator()(long x, long y)
    {
        return _value(x, y);
    }

    void GrayScaleImage::create(long width, long height)
    {
        _size.x = width;
        _size.y = height;

        align_center_with({width * 0.5f, height * 0.5f});
        _value.resize(static_cast<Eigen::Index>(width), static_cast<Eigen::Index>(height));
        update_image();
    }

    void GrayScaleImage::create_from_file(std::string path)
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
    }

    sf::Vector2<long> GrayScaleImage::get_size() const
    {
        return _size;
    }
}

