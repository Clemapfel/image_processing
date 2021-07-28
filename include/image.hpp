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
    template<typename Value_t>
    class Image : protected sf::Drawable
    {
        public:
            Value_t operator()(long x, long y) = 0;

    };

    // an image living in ram, treated by the CPU
    class GrayScaleImage : public sf::Drawable
    {
        friend class RenderWindow;
        class Iterator;

        public:
            GrayScaleImage();
            enum BitDepth {BIT_8, BIT_16, BIT_32};

            void create(long width, long height, BitDepth = BIT_8);
            void create_from_file(std::string path, BitDepth = BIT_8);

            float& operator()(long x, long y);

            // transformations that only apply to rendering
            void align_topleft_with(Vector2f);
            void align_center_with(Vector2f);
            void zoom(float factor, bool smooth = false);

            void update_image();

            Iterator begin();
            Iterator end();

        protected:
            void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

        private:
            BitDepth _bit_depth = BIT_8;
            sf::Vector2<long> _size;
            Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> _value;

            // render
            sf::Image _image;
            sf::Texture _texture;
            sf::Sprite _sprite;

            float _zoom_factor = 1;

            sf::Vector2f _center_pos;

            // iterator
            struct Iterator
            {
                public:
                    Iterator(Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic>* data, size_t x, size_t y);

                    using iterator_category = std::bidirectional_iterator_tag;
                    using value_type = float;
                    using difference_type = int;
                    using pointer = float*;
                    using reference = float&;

                    bool operator==(Iterator& other) const;
                    bool operator!=(Iterator& other) const;

                    Iterator& operator++();
                    Iterator& operator--();

                    float& operator*() const;
                    explicit operator float() const;

                    void operator=(float new_value);

                private:
                    Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic>* _data;

                    sf::Vector2<long> _size;
                    long _x, _y = 0;
            };
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