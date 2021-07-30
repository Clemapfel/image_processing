//
// Copyright 2021 Clemens Cords
// Created on 25.07.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <string>

#include <SFML/System/Vector2.hpp>

#include <vector.hpp>

namespace crisp
{
    template<typename Value_t>
    class Image
    {
        class Iterator;

        public:
            using value_t = Value_t;

            // enum that governs what values indices out of bounds will return
            enum PaddingType
            {
                // let image be [1, 2, 3, 4] (size 4*1), then padding has the form:
                ZERO = 0,   // ...0, 0, [1, 2, 3, 4], 0, 0, 0...
                ONE = 1,    // ...1, 1, [1, 2, 3, 4], 1, 1, 1...
                REPEAT,     // ...3, 4, [1, 2, 3, 4], 1, 2, 3...
                MIRROR,     // ...2, 1, [1, 2, 3, 4], 4, 3, 2...
                STRETCH     // ...1, 1, [1, 2, 3, 4], 4, 4, 4...    // default
                // only x-direction padding shown, analogous in y-direction
            };

            Value_t operator()(long x, long y) const;
            Value_t & operator()(long x, long y);

            [[nodiscard]] virtual sf::Vector2<long> get_size() const = 0;

            virtual void create(long width, long height) = 0;
            void set_padding_type(PaddingType);
            PaddingType get_padding_type() const;

            Iterator begin();
            Iterator end();

        protected:
            virtual Value_t  get_pixel(long x, long y) const = 0;
            virtual Value_t& get_pixel(long x, long y) = 0;

        private:
            PaddingType _padding_type = PaddingType::STRETCH;
            Value_t get_pixel_out_of_bounds(long x, long y) const;

            // iterator
            struct Iterator
            {
                public:
                    Iterator(Image<Value_t>*, size_t x, size_t y);

                    using iterator_category = std::bidirectional_iterator_tag;
                    using value_type = Value_t;
                    using difference_type = int;
                    using pointer = Value_t*;
                    using reference = Value_t&;

                    bool operator==(Iterator& other) const;
                    bool operator!=(Iterator& other) const;

                    Iterator& operator++();
                    Iterator& operator--();

                    Value_t& operator*() const;
                    Iterator& operator=(Value_t);

                private:
                    Image<Value_t>* _data;
                    sf::Vector2<long> _size;
                    long _x, _y = 0;
            };
    };
}

#include ".src/image.inl"