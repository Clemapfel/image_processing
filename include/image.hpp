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
    class Image : public sf::Drawable
    {
        class Iterator;

        public:
            virtual Value_t & operator()(long x, long y) = 0;
            virtual sf::Vector2<long> get_size() const = 0;

            Iterator begin();
            Iterator end();

            // render
            void align_topleft_with(Vector2f);
            void align_center_with(Vector2f);
            void zoom(float factor, bool smooth = false);
            void update_image();

        protected:
            sf::Image _image;
            sf::Texture _texture;

        private:
            // render
            void draw(sf::RenderTarget& target, sf::RenderStates states) const override final;
            sf::Sprite _sprite;

            float _zoom_factor = 1;

            sf::Vector2f _center_pos;

            // iterator
            struct Iterator
            {
                public:
                    Iterator(Image<Value_t>*, size_t x, size_t y);

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
                    Image<Value_t>* _data;
                    sf::Vector2<long> _size;
                    long _x, _y = 0;
            };
    };
}

#include ".src/image.inl"