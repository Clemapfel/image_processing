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
    // enum that governs what values indices out of bounds will return
    enum PaddingType
    {
        // let image be [1, 2, 3, 4] (size 4*1), then padding has the form:
        ZERO = 0,   // ...0, 0, [1, 2, 3, 4], 0, 0, 0...
        ONE = 1,    // ...1, 1, [1, 2, 3, 4], 1, 1, 1...
        REPEAT,     // ...3, 4, [1, 2, 3, 4], 1, 2, 3...
        MIRROR,     // ...2, 1, [1, 2, 3, 4], 4, 3, 2...
        STRETCH,     // ...1, 1, [1, 2, 3, 4], 4, 4, 4...    // default
        // only x-direction padding shown, analogous in y-direction
    };

    // an image that lives in ram and is operated upon by the cpu
    template<typename Value_t>
    class Image
    {
        class Iterator;
        class ConstIterator;

        public:
            // the value type of the images matrix
            using value_t = Value_t;

            // @brief access a specific pixel
            // @param x: the row index
            // @param y: the column index
            // @returns a reference to the value, can be written to
            virtual Value_t & operator()(long x, long y);

            // @brief access a specific pixel in a const context
            // @param x: the row index
            // @param y: the column index
            // @returns a copy of the value
            virtual Value_t operator()(long x, long y) const;

            // @brief explicit overload for operator(long, long) const for clarity
            // @param x: the row index
            // @param y: the column index
            // @returns a copy of the value
            virtual Value_t get_pixel_or_padding(int x, int y) const;

            // @brief get the images size
            // @return vector v such that v.x = #rows, v.y = #cols
            [[nodiscard]] virtual sf::Vector2<long> get_size() const = 0;

            // @brief resize an image and clear each pixel
            // @param width: number of rows
            // @param height: number of columns
            virtual void create(long width, long height, Value_t init) = 0;

            // @brief set the type of padding, see above
            // @param : enum constant of the padding type
            void set_padding_type(PaddingType);

            // @brief get the type of padding
            // @returns enum constant of the padding type
            PaddingType get_padding_type() const;

            // @brief get an iterator to the first pixel
            // @returns iterator pointing to (0, 0)
            // @note the iterator will advance left to right, top to bottom
            Iterator begin();
            ConstIterator begin() const;

            // @brief get an iterator to the past-the-end element
            // @returns iterator pointing to (#rows, #cols)
            Iterator end();
            ConstIterator end() const;

        protected:
            // pure virtual function to access pixels, does not need to account for padding
            virtual Value_t  get_pixel(long x, long y) const = 0;
            virtual Value_t& get_pixel(long x, long y) = 0;

        private:
            PaddingType _padding_type = PaddingType::STRETCH;
            Value_t get_pixel_out_of_bounds(int x, int y) const;

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

            struct ConstIterator
            {
                public:
                    ConstIterator(const Image<Value_t>*, size_t x, size_t y);

                    using iterator_category = std::bidirectional_iterator_tag;
                    using value_type = const Value_t;
                    using difference_type = int;
                    using pointer = const Value_t*;
                    using reference = const Value_t&;

                    bool operator==(ConstIterator& other) const;
                    bool operator!=(ConstIterator& other) const;

                    ConstIterator& operator++();
                    ConstIterator& operator--();

                    const Value_t& operator*() const;

                private:
                    const Image<Value_t>* _data;
                    sf::Vector2<long> _size;
                    long _x, _y = 0;
            };
    };
}

#include ".src/image.inl"