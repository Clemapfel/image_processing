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

            virtual Value_t & operator()(long x, long y) = 0;
            virtual Value_t operator()(long x, long y) const = 0;
            virtual sf::Vector2<long> get_size() const = 0;

            Iterator begin();
            Iterator end();

        private:
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