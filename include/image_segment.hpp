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
    // a set of unique pixel coordinates, usually restrained to all being in a 4-connected cluster
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

            // @brief create the segment from a vector of pixel coordinates
            // @param coords: vector of coordinates
            void create(std::vector<Vector2ui> coords);

            // @brief add pixel coordinate to the segment
            void add(Vector2ui);

            // @brief remove a pixel coordinate from the segment
            // @param : coordinate to be removed
            void remove(Vector2ui);

            // @brief remove all pixel coordinates in the the segment
            void clear();

            // @brief get number of pixel coordinates in segment
            // @returns number of coordinates, 0 if empty
            size_t size();

            // @brief paste the transform onto a new image of size equal to the minimum bounding box of segments the pixel set
            // @returns an image of an arbitrary type
            // @note All pixels in that rectangle that do not correspond to the original image are substituted with 0
            template<typename NewImage_t>
            NewImage_t to_new_image() const;

            // @brief get an iterator to the first pixel in the segment
            // @returns bidirectional (const) iterator
            auto begin();
            auto begin() const;

            // @brief get an iterator to the past-the-end element of the segment
            // @returns bidirectional (const) iterator
            auto end();
            auto end() const;

        private:
            Vector2ui _x_bounds,
                      _y_bounds;

            Set_t _pixels;
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