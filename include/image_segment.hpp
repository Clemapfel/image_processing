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
    template<typename Value_t>
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

            // @brief create the segment from an image and a vector of xy-coordinates
            // @param original: pointer to an image, must be kept in memory for the segment to stay valid
            // @param coords: vector of pixel coordinates, doublets will be ignored
            void create(Image<Value_t>* original, std::vector<Vector2ui> coords);

            // @brief paste the transform onto a new image of size equal to the minimum bounding box of segments the pixel set
            // @returns an image of an arbitrary type
            // @note All pixels in that rectangle that do not correspond to the original image are substituted with 0
            template<typename Image_t>
            Image_t to_new_image() const;

            // @brief return a range with all coordinates of the pixels in the segment. The coordinates are relative to the original images origin
            // @returns an std::set holding sf::vectors with 2 longs, vectors have member .x and .y
            const Set_t& get_original_coordinates() const;

            // @brief get an iterator to the first pixel in the segment
            // @returns bidirectional non-const iterator
            auto begin();

            // @brief get an iterator to the past-the-end element of the segment
            // @returns bidirectional non-const iterator
            auto end();

        private:
            Image<Value_t>* _image;

            Vector2ui _x_bounds,
                              _y_bounds;

            Set_t _pixels;

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

    using BinaryImageSegment = ImageSegment<bool>;
    using GrayScaleImageSegment = ImageSegment<float>;
}

#include <.src/image_segment.inl>