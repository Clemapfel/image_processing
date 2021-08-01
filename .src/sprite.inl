// 
// Copyright 2021 Clemens Cords
// Created on 29.07.21 by clem (mail@clemens-cords.com)
//

#pragma once

namespace crisp
{
    template<typename Value_t>
    void Sprite::load_from(const Image<Value_t>& image)
    {
        sf::Image temp;
        temp.create(image.get_size().x, image.get_size().y);

        for (long x = 0; x < image.get_size().x; ++x)
        {
            for (long y = 0; y < image.get_size().y; ++y)
            {
                float value_f = float(image(x, y));
                value_f = clamp(0.f, 1.f, value_f);
                auto value = static_cast<uint8_t>(value_f * uint8_t(255));
                temp.setPixel(x, y, sf::Color(value, value, value, 255));
            }
        }

        _texture.loadFromImage(temp);
        _sprite.setTextureRect(sf::IntRect(0, 0, temp.getSize().x, temp.getSize().y));
        _sprite.setTexture(_texture);
        _sprite.setPosition(0, 0);
    }
}