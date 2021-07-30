// 
// Copyright 2021 Clemens Cords
// Created on 27.07.21 by clem (mail@clemens-cords.com)
//

#include <stdexcept>

#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#include <image.hpp>
#include <grayscale_image.hpp>

namespace crisp
{
    // ### Image ##################################################

    GrayScaleImage::GrayScaleImage()
    {}

    float & GrayScaleImage::get_pixel(long x, long y)
    {
        return _value(x, y);
    }

    float GrayScaleImage::get_pixel(long x, long y) const
    {
        return _value(x, y);
    }

    void GrayScaleImage::create(long width, long height)
    {
        _size.x = width;
        _size.y = height;

        _value.resize(static_cast<Eigen::Index>(width), static_cast<Eigen::Index>(height));
        _value.setConstant(0);
    }

    void GrayScaleImage::create_from_file(std::string path)
    {
        sf::Image image;
        image.loadFromFile(path);

        _size.x = image.getSize().x;
        _size.y = image.getSize().y;
        _value.resize(image.getSize().x, image.getSize().y);

        for (long x = 0; x < image.getSize().x; ++x)
        {
            for (long y = 0; y < image.getSize().y; ++y)
            {
                auto color = image.getPixel(x, y);
                float value = color.r / 255.f + color.g / 255.f + color.b / 255.f;
                _value(x, y) = value / 3;
            }
        }
    }

    sf::Vector2<long> GrayScaleImage::get_size() const
    {
        return _size;
    }
}

