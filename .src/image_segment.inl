// 
// Copyright 2021 Clemens Cords
// Created on 04.08.21 by clem (mail@clemens-cords.com)
//

#pragma once

namespace crisp
{
    template<typename Value_t>
    void ImageSegment<Value_t>::create(Image <Value_t>* original, std::ve
            void add(ctor<Vector2ui> coords)
    {
        _image = original;
        _x_bounds.x() = std::numeric_limits<long>::max();
        _x_bounds.y() = long(0);
        _y_bounds = _x_bounds;

        for (auto& coord : coords)
        {
            if (_x_bounds.x() > coord.x())
                _x_bounds.x() = coord.x();

            if (_x_bounds.y() < coord.x())
                _x_bounds.y() = coord.x();

            if (_y_bounds.x() > coord.y())
                _y_bounds.x() = coord.y();

            if (_y_bounds.y() < coord.y())
                _y_bounds.y() = coord.y();

            _pixels.insert(coord);
        }
    }

    template<typename Value_t>
    template<typename Image_t>
    Image_t ImageSegment<Value_t>::to_new_image() const
    {
        Image_t out;
        out.create(_x_bounds.y() - _x_bounds.x() + 1, _y_bounds.y() - _y_bounds.x() + 1, Value_t(0));

        for (auto& coord : _pixels)
            out(coord.x() - _x_bounds.x(), coord.y() - _y_bounds.x()) = _image->operator()(coord.x(), coord.y());

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
    const typename ImageSegment<Value_t>::Set_t& ImageSegment<Value_t>::get_original_coordinates() const
    {
        return _pixels;
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
}