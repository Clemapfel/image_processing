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
    // a set of pixel belonging to an image
    class ImageSegment
    {
        struct PixelCoordCompare
        {
            bool operator()(const Vector2ui& a, const Vector2ui& b) const
            {
                return a.x() != b.x() ? a.x() < b.x() : a.y() <= b.y();
            }
        };
        using Set_t = std::set<Vector2ui, PixelCoordCompare>;

        public:
            ImageSegment() = default;

            void create(std::vector<Vector2ui> coords);
            void add(Vector2ui);
            void remove(Vector2ui);
            void clear();
            size_t size();

            // @brief paste the transform onto a new image of size equal to the minimum bounding box of segments the pixel set
            // @returns an image of an arbitrary type
            // @note All pixels in that rectangle that do not correspond to the original image are substituted with 0
            template<typename NewImage_t>
            NewImage_t to_new_image() const;

            // @brief get an iterator to the first pixel in the segment
            // @returns bidirectional non-const iterator
            auto begin();
            auto begin() const;

            // @brief get an iterator to the past-the-end element of the segment
            // @returns bidirectional non-const iterator
            auto end();
            auto end() const;

        private:
            Vector2ui _x_bounds,
                      _y_bounds;

            Set_t _pixels;

            /*
            template<typename SetIterator_t>
            struct Iterator
            {
                public:
                    Iterator(Image_t*, SetIterator_t);

                    using iterator_category = std::bidirectional_iterator_tag;
                    using value_type = typename Image_t::value_t;
                    using difference_type = int;
                    using pointer = typename Image_t::value_t*;
                    using reference = typename Image_t::value_t&;

                    bool operator==(Iterator& other) const;
                    bool operator!=(Iterator& other) const;

                    Iterator& operator++();
                    Iterator& operator--();

                    typename Image_t::value_t& operator*() const;
                    explicit operator typename Image_t::value_t() const;

                    void operator=(typename Image_t::value_t new_value);

                private:
                    Image_t* _image;
                    SetIterator_t _it;
            };
            */
    };

    void ImageSegment::create(std::vector<Vector2ui> coords)
    {
        _pixels.clear();
        for (auto& c : coords)
            _pixels.insert(c);
    }

    void ImageSegment::add(Vector2ui px)
    {
        _pixels.insert(px);
    }

    void ImageSegment::remove(Vector2ui px)
    {
        _pixels.erase(px);
    }

    void ImageSegment::clear()
    {
        _pixels.clear();
    }

    size_t ImageSegment::size()
    {
        return _pixels.size();
    }

    auto ImageSegment::begin()
    {
        return _pixels.begin();
    }

    auto ImageSegment::end()
    {
        return _pixels.end();
    }

    auto ImageSegment::begin() const
    {
        return _pixels.begin();
    }

    auto ImageSegment::end() const
    {
        return _pixels.end();
    }
}

//#include <.src/image_segment.inl>