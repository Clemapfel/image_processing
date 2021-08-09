// 
// Copyright 2021 Clemens Cords
// Created on 29.07.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <image.hpp>
#include <vector.hpp>
#include <histogram.hpp>
#include <frequency_domain_filter.hpp>

namespace crisp
{
    // renderable entity images need to be bound to do draw them or save them
    class Sprite : public sf::Drawable
    {
        public:
            Sprite() = default;

            // @complexity: (m*n)^2 operations, m*n*4 space in ram
            template<typename Value_t>
            void load_from(const Image<Value_t>& image);

            template<typename Value_t>
            void load_from(const Histogram<Value_t>& histogram, long image_height, bool show_mean = true);

            void load_from(const FrequencyDomainFilter& filter);

            sf::Vector2f get_size() const;
            sf::Vector2f get_topleft() const;
            sf::Vector2f get_center() const;

            // render
            void align_topleft_with(Vector2f);
            void align_center_with(Vector2f);
            void zoom(float factor, bool smooth = false);

            // to file
            bool save_to_file(std::string path) const;

        protected:
            sf::Sprite _sprite;
            sf::Texture _texture;

            void draw(sf::RenderTarget& target, sf::RenderStates states) const override final;

            float _zoom_factor = 1;
            sf::Vector2f _top_left_pos = {0, 0};
    };
}

#include ".src/sprite.inl"