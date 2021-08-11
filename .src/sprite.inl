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
    void Sprite::load_from(const Histogram <Value_t>& histogram, long height, bool show_mean)
    {
        auto& counts = histogram.get_counts();
        size_t n_buckets = std::numeric_limits<Value_t>::max();

        if (counts.empty())
        {
            sf::Image img;
            img.create(n_buckets, height, sf::Color::Black);

            _texture.loadFromImage(img);
            _sprite.setTextureRect(sf::IntRect(0, 0, img.getSize().x, img.getSize().y));
            _sprite.setTexture(_texture);
            _sprite.setPosition(0, 0);
            return;
        }

        float mean = 0;
        float n = 0;
        size_t max = 0;
        for (const auto& pair : counts)
        {
            max = std::max(max, pair.second);

            if (show_mean)
            {
                mean += pair.first * pair.second;
                n += pair.second;
            }
        }

        if (show_mean)
            mean /= n;

        size_t step = ceil( float(max) / (0.85 * height));

        if (n_buckets > 1920)
            std::cerr << "[Warning] rendering a histogram with " << n_buckets << " different values to a texture will take up a large amount of memory" << std::endl;

        sf::Image temp;
        temp.create(n_buckets, height, sf::Color::Black);

        auto mean_x = size_t(round(mean));
        for (size_t x = 0; x < n_buckets; ++x)
        {
            if (show_mean and x == mean_x)
                for (size_t y = 0; y < temp.getSize().y; ++y)
                    temp.setPixel(x, y, sf::Color::Red);

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

    inline void Sprite::load_from(const FrequencyDomainFilter& filter)
    {
        sf::Image temp;
        temp.create(filter.get_size().x, filter.get_size().y);

        const auto& values = filter.get_values();

        size_t i = 0;
        for (long x = 0; x < temp.getSize().x; ++x)
        {
            for (long y = 0; y < temp.getSize().y; ++y, ++i)
            {
                float value = filter.get_values().at(i);
                temp.setPixel(x, y, sf::Color(value * 255.f, value * 255.f, value * 255.f, 255));
            }
        }

        _texture.loadFromImage(temp);
        _sprite.setTextureRect(sf::IntRect(0, 0, temp.getSize().x, temp.getSize().y));
        _sprite.setTexture(_texture);
        _sprite.setPosition(0, 0);
    }

    inline sf::Vector2f Sprite::get_size() const
    {
        return {_texture.getSize().x * _sprite.getScale().x, _texture.getSize().y * _sprite.getScale().y};
    }

    inline sf::Vector2f Sprite::get_topleft() const
    {
        return _top_left_pos;
    }

    inline sf::Vector2f Sprite::get_center() const
    {
        auto size = get_size();
        return {_top_left_pos.x + size.x * 0.5f, _top_left_pos.y + size.y * 0.5f};
    }

        inline void Sprite::align_topleft_with(Vector2f top_left)
    {
        _top_left_pos.x = round(top_left.at(0));
        _top_left_pos.y = round(top_left.at(1));
    }

    inline void Sprite::align_center_with(Vector2f center)
    {
        _top_left_pos.x = round(center.at(0) - _texture.getSize().x * 0.5);
        _top_left_pos.y = round(center.at(1) - _texture.getSize().y * 0.5);
    }

    inline void Sprite::zoom(float factor, bool smooth)
    {
        _zoom_factor = factor;
        if (smooth != _texture.isSmooth())
            _texture.setSmooth(smooth);
    }

    inline void Sprite::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(_top_left_pos.x, _top_left_pos.y);
        states.transform.scale(_zoom_factor, _zoom_factor);
        states.texture = &_texture;
        target.draw(_sprite, states);
    }

    inline bool Sprite::save_to_file(std::string path) const
    {
        sf::Image temp = _texture.copyToImage();
        auto res = temp.saveToFile(path);
        if (res)
            std::cerr << "[LOG] succesfully saved " << path << std::endl;
        else
            std::cerr << "[LOG] could not save " << path << std::endl;

        return res;
    }
}