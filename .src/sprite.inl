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

    template<typename Value_t>
    void Sprite::load_from(const Histogram <Value_t>& histogram, long height)
    {
        auto& counts = histogram.get_counts();
        size_t max = 0;
        for (const auto& pair : counts)
            max = std::max(max, pair.second);

        size_t step = ceil( float(max) / (0.85 * height));

        size_t n_buckets = std::numeric_limits<Value_t>::max();

        if (n_buckets > 1920)
            std::cerr << "[Warning] rendering a histogram with " << n_buckets << " different values to a texture will take up a large amount of memory" << std::endl;

        sf::Image temp;
        temp.create(n_buckets, height, sf::Color::Black);

        for (size_t x = 0; x < n_buckets; ++x)
        {
            for (size_t y = 1; y * step < histogram.at(x); ++y)
            {
                temp.setPixel(x, temp.getSize().y - y, sf::Color::White);
            }
        }

        _texture.loadFromImage(temp);
        _sprite.setTextureRect(sf::IntRect(0, 0, temp.getSize().x, temp.getSize().y));
        _sprite.setTexture(_texture);
        _sprite.setPosition(0, 0);
    }
}