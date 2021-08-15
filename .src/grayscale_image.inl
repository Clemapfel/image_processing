// 
// Copyright 2021 Clemens Cords
// Created on 27.07.21 by clem (mail@clemens-cords.com)
//

#include <stdexcept>

namespace crisp
{
    inline GrayScaleImage::GrayScaleImage()
    {}

    inline float & GrayScaleImage::get_pixel(long x, long y)
    {
        return _value(x, y);
    }

    inline float GrayScaleImage::get_pixel(long x, long y) const
    {
        return _value(x, y);
    }

    inline void GrayScaleImage::create(long width, long height, float init)
    {
        _size.x = width;
        _size.y = height;
        _value.resize(static_cast<Eigen::Index>(width), static_cast<Eigen::Index>(height));

        if (init >= 0)
            _value.setConstant(init);
    }

    inline void GrayScaleImage::create_from_file(std::string path)
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

    inline sf::Vector2<long> GrayScaleImage::get_size() const
    {
        return _size;
    }
}

