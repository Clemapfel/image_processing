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
#include <spatial_filter.hpp>

namespace crisp
{
    // renderable entity images need to be bound to do draw them or save them
    class Sprite : public sf::Drawable
    {
        public:
            Sprite() = default;

            // @brief construct a sprite from an image
            // @param image: the image to be rendered
            // @complexity: (m*n)^2 operations, m*n*4 space in ram
            template<typename Value_t>
            void create_from(const Image<Value_t>& image);

            // @brief construct a sprite from a histogram
            // @param histogram: histogram to be rendered
            // @param image_height: maximum height of the sprite in pixels, histogram counts will be scaled accordingly
            // @param show mean: if true, shows mean as a red bar
            template<typename Value_t>
            void create_from(const Histogram<Value_t>& histogram, long image_height, bool show_mean = true);

            // @brief construct a sprite from a frequency domain filter
            // @param filter: the filter to be visualized, normalizes the intensities and treats the filters as a grayscale image
            void create_from(const FrequencyDomainFilter& filter);

            // @brief construct a sprite from a spatial domain filter
            // @param filter: the filter to be visualized, normalizes the intensities and treats the filters as a grayscale image
            template<typename Image_t>
            void create_from(const SpatialFilter<Image_t>& filter);

            // @brief get the size of the sprite, including scaling
            // @returns vector where .x is the width, .y the height of the sprite
            sf::Vector2f get_size() const;

            // @brief get the top left position of the sprite relative to the render window origin
            // @returns vector where .x is the x-coordinate, .y the y-coordinate of the top left origin
            sf::Vector2f get_topleft() const;

            // @brief get the center position of the sprite relative to the render window origin
            // @returns vector where .x is the x-coordinate, .y the y-coordinate of the center
            //
            // @note equivalent to {get_topleft() + get_size()*0.5}
            sf::Vector2f get_center() const;

            // @brief set the sprites position by specifying the new top left
            // @param : the point, relative to the render windows origin
            void align_topleft_with(Vector2f);

            // @brief set the sprites position by specifiny the new center
            // @param : the center, relative to teh render windows origin
            void align_center_with(Vector2f);

            // @brief scale the sprite for rendering
            // @param factor: the factor by which the sprite is scaled, factors < 1 or non multiples of 2 will cause distortion
            // @param smooth: should the sprite be smoothed after scaling, nearest-neighbor otherwise
            void scale(float factor, bool smooth = false);

            // @brief save the sprite to a file
            // @param path: the absolute path to the resulting file
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