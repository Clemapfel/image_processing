//
// Copyright 2021 Clemens Cords
// Created on 25.07.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <string>

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <vector.hpp>
#include <SFML/Graphics/Sprite.hpp>

namespace crisp
{
    // an image living in ram, treated by the CPU
    class GrayScaleImage : public sf::Drawable
    {
        friend class RenderWindow;

        public:
            GrayScaleImage();
            enum BitDepth {BIT_8, BIT_16, BIT_32};

            void create(long width, long height, BitDepth = BIT_8);
            void create_from_file(std::string path, BitDepth = BIT_8);

            void align_topleft_with(Vector2f);
            void align_center_with(Vector2f);

        protected:
            void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

        private:
            BitDepth _bit_depth = BIT_8;
            sf::Vector2<long> _size;
            Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> _value;

            // render
            void update_image();
            sf::Image _image;
            sf::Texture _texture;
            sf::Sprite _sprite;

            sf::Vector2f _center_pos;
    };


    // an image living on the graphics card
    /*
    class Image : public sf::Drawable
    {
        friend class RenderWindow;

        public:
            Image();
            enum BitDepth {BIT_8, BIT_16, BIT_32};

            void create(size_t width, size_t height, BitDepth = BIT_8, bool sRGB = false);
            void create_from_file(std::string texture_path, BitDepth = BIT_8, bool sRGB = false);

            void set_smoothing_enabled(bool enabled);
            bool is_smoothing_enabled() const;

            void align_topleft_with(Vector2f);
            void align_center_with(Vector2f);

        protected:
            void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

        private:
            BitDepth _bit_depth = BIT_8;
            bool _is_srgb = false;

            // gl primitives in local coords
            sf::Texture _texture;
            sf::VertexArray _shape;

            sf::Vector2f _size;

            // render
            sf::Vector2f _center_pos;
            sf::RectangleShape _sprite;
    };
     */
}