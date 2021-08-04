// 
// Copyright 2021 Clemens Cords
// Created on 04.08.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <set>

#include <image.hpp>
#include <vector.hpp>

namespace crisp
{
    template<typename Value_t>
    class ImageSegment
    {
        template<typename>
        class Iterator;

        public:
            ImageSegment() = default;

            void create(Image<Value_t>* original, std::vector<sf::Vector2<long>> coords);

            Image<Value_t>& to_new_image() const;

            auto begin();
            auto end();

        private:
            Image<Value_t>* _image;

            sf::Vector2<long> _x_bounds,
                              _y_bounds;

            struct PixelCoordCompare
            {
                bool operator()(const sf::Vector2<long>& a, const sf::Vector2<long>& b)
                {
                    return a.x != b.x ? a.x < b.x : a.y <= b.y;
                }
            };
            std::set<sf::Vector2<long>, PixelCoordCompare> _pixels;

            template<typename SetIterator_t>
            struct Iterator
            {
                public:
                    Iterator(Image<Value_t>*, SetIterator_t);

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
                    explicit operator Value_t() const;

                    void operator=(Value_t new_value);

                private:
                    Image<Value_t>* _image;
                    SetIterator_t _it;
            };
    };

    template<typename Value_t>
    void ImageSegment<Value_t>::create(Image <Value_t>* original, std::vector<sf::Vector2<long>> coords)
    {
        _image = original;
        _x_bounds.x = std::numeric_limits<long>::max();
        _x_bounds.y = long(0);
        _y_bounds = _x_bounds;

        for (auto& coord : coords)
        {
            if (_x_bounds.x < coord.x)
                _x_bounds.x = coord.x;

            if (_x_bounds.y > coord.x)
                _x_bounds.y = coord.x;

            if (_y_bounds.x < coord.y)
                _y_bounds.x = coord.y;

            if (_y_bounds.y > coord.y)
                _y_bounds.y = coord.y;

            _pixels.insert(coord);
        }
    }

    template<typename Value_t>
    Image<Value_t>& ImageSegment<Value_t>::to_new_image() const
    {
        Image<Value_t> out;
        out.create(_x_bounds.y - _x_bounds.x, _y_bounds.y - _y_bounds.x, Value_t(0));

        for (auto& coord : _pixels)
            out(coord.x - _x_bounds.x, coord.y - _y_bounds.y) = _image->operator()(coord.x, coord.y);

        return out;
    }

    template<typename Value_t>
    auto ImageSegment<Value_t>::begin()
    {
        return Iterator(_image, _pixels.begin());
    }

    template<typename Value_t>
    auto ImageSegment<Value_t>::end()
    {
        return Iterator(_image, _pixels.end());
    }

    template<typename Value_t>
    template<typename SetIterator_t>
    ImageSegment<Value_t>::Iterator<SetIterator_t>::Iterator(Image<Value_t>* image, SetIterator_t it)
        : _image(image), _it(it)
    {}

    template<typename Value_t>
    template<typename SetIterator_t>
    bool ImageSegment<Value_t>::Iterator<SetIterator_t>::operator==(ImageSegment::Iterator<SetIterator_t>& other) const
    {
        return other._it == _it;
    }

    template<typename Value_t>
    template<typename SetIterator_t>
    bool ImageSegment<Value_t>::Iterator<SetIterator_t>::operator!=(ImageSegment::Iterator<SetIterator_t>& other) const
    {
        return other._it != _it;
    }

    template<typename Value_t>
    template<typename SetIterator_t>
    typename ImageSegment<Value_t>::template Iterator<SetIterator_t> & ImageSegment<Value_t>::Iterator<SetIterator_t>::operator++()
    {
        _it++;
    }

    template<typename Value_t>
    template<typename SetIterator_t>
    typename ImageSegment<Value_t>::template Iterator<SetIterator_t> & ImageSegment<Value_t>::Iterator<SetIterator_t>::operator--()
    {
        _it--;
    }

    template<typename Value_t>
    template<typename SetIterator_t>
    Value_t& ImageSegment<Value_t>::Iterator<SetIterator_t>::operator*() const
    {
        return _image->operator()((*_it).x, (*_it).y);
    }

    template<typename Value_t>
    template<typename SetIterator_t>
    ImageSegment<Value_t>::Iterator<SetIterator_t>::operator Value_t() const
    {
        return _image->operator()((*_it).x, (*_it).y);
    }

    template<typename Value_t>
    template<typename SetIterator_t>
    void ImageSegment<Value_t>::Iterator<SetIterator_t>::operator=(Value_t new_value)
    {
        _image->operator()((*_it).x, (*_it).y) = new_value;
    }

    using BinaryImageSegment = ImageSegment<bool>;
    using GrayScaleImageSegment = ImageSegment<float>;
}