// 
// Copyright 2021 Clemens Cords
// Created on 29.07.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <SFML/System/Vector2.hpp>

#include <image.hpp>

#include <functional>
#include <vector>


namespace crisp
{
    // a functor that only operates on a subset of pixel
    template<typename Image_t>
    class SubimageView
    {
        class Iterator;

        public:
            static inline const auto all_values = [](long x, long y, typename Image_t::value_t value) -> bool {return true;};

            void create_from(Image_t& image, std::function<bool(long, long, typename Image_t::value_t)>&& which = all_values)
            {
                for (long x = 0; x < image.get_size().x; ++x)
                    for (long y = 0; y < image.get_size().y; ++y)
                        if (which(x, y, image(x, y)))
                            _value_pos.push_back({x, y});

                _image = &image;
            }

            Iterator begin() {return Iterator(_image, &_value_pos, 0);}
            Iterator end() {return Iterator(_image, &_value_pos, _value_pos.size());}

        private:
            Image_t* _image;
            std::vector<sf::Vector2<long>> _value_pos;

            struct Iterator
            {
                using Value_t = typename Image_t::value_t;

                Iterator(Image_t* image, std::vector<sf::Vector2<long>>*, size_t i);

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
                    Image_t* _image;
                    std::vector<sf::Vector2<long>>* _value_pos;
                    size_t _i;
            };
    };

    template<typename Image_t>
    SubimageView<Image_t>::Iterator::Iterator(Image_t* image, std::vector<sf::Vector2<long>>* value, size_t i)
        : _image(image), _value_pos(value), _i(i)
    {}

    template<typename Image_t>
    bool SubimageView<Image_t>::Iterator::operator==(Iterator& other) const
    {
        return _value_pos == other._value_pos and _i == other._i;
    }

    template<typename Image_t>
    bool SubimageView<Image_t>::Iterator::operator!=(Iterator& other) const
    {
        return not (*this == other);
    }

    template<typename Image_t>
    typename SubimageView<Image_t>::Iterator & SubimageView<Image_t>::Iterator::operator++()
    {
        if (_i < _value_pos->size())
            _i++;

        return *this;
    }

    template<typename Image_t>
    typename SubimageView<Image_t>::Iterator & SubimageView<Image_t>::Iterator::operator--()
    {
        if (_i > 0)
            _i--;

        return *this;
    }

    template<typename Image_t>
    typename Image_t::value_t & SubimageView<Image_t>::Iterator::operator*() const
    {
        auto pos = _value_pos->at(_i);
        return _image->operator()(pos.x, pos.y);
    }

    template<typename Image_t>
    typename SubimageView<Image_t>::Iterator & SubimageView<Image_t>::Iterator::operator=(typename Image_t::value_t value)
    {
        auto pos = _value_pos->at(_i);
        _image->operator()(pos.x, pos.y) = value;

        return *this;
    }


}