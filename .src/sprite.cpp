// 
// Copyright 2021 Clemens Cords
// Created on 29.07.21 by clem (mail@clemens-cords.com)
//

#include <SFML/Graphics/RenderTarget.hpp>

#include <sprite.hpp>
#include <vector.hpp>

namespace crisp
{
    void Sprite::align_topleft_with(Vector2f top_left)
    {
        _top_left_pos.x = top_left.at(0);
        _top_left_pos.y = top_left.at(1);
    }

    void Sprite::align_center_with(Vector2f center)
    {
        _top_left_pos.x = center.at(0) - _texture.getSize().x * 0.5;
        _top_left_pos.y = center.at(1) - _texture.getSize().y * 0.5;
    }

    void Sprite::zoom(float factor, bool smooth)
    {
        _zoom_factor = factor;
        if (smooth != _texture.isSmooth())
            _texture.setSmooth(smooth);
    }

    void Sprite::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(_top_left_pos.x, _top_left_pos.y);
        states.transform.scale(_zoom_factor, _zoom_factor);
        states.texture = &_texture;
        target.draw(_sprite, states);
    }

}
