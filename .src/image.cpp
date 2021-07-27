// 
// Copyright 2021 Clemens Cords
// Created on 27.07.21 by clem (mail@clemens-cords.com)
//

#include <stdexcept>

#include <SFML/Graphics/RenderTarget.hpp>

#include <image.hpp>

namespace crisp
{
    Image::Image()
        : _shape(sf::TriangleFan, 4)
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
}

