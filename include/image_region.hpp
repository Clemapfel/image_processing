// 
// Copyright 2021 Clemens Cords
// Created on 26.08.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <image_segment.hpp>
#include <vector.hpp>

namespace crisp
{
    // a 4-connected region of an image, unlike ImageSegment, each pixel in the region has more than just a spacial dimension
    template<typename Image_t>
    class ImageRegion
    {
        using Value_t = typename Image_t::value_t;

        public:
            ImageRegion() = default;
            void create_from(const ImageSegment& segment, const Image_t& image);

        private:
            struct Element
            {
                Vector2ui position;
                Value_t value;
                bool is_boundary;
            };

            struct ElementCompare
            {
                bool operator()(const Element& a, const Element& b) const
                {
                    return a.position.x() != b.position.x() ? a.position.x() < b.position.x() : a.position.y() <= b.position.y();
                }
            };

            std::set<Element, ElementCompare> _elements;
            std::vector<Vector2ui> _boundary;

            Vector2ui _x_bounds, _y_bounds;
        };

    template<typename Image_t>
    void ImageRegion<Image_t>::create_from(const ImageSegment& segment, const Image_t& image)
    {
        _elements.clear();
        ImageSegment temp_boundary;

        unsigned int min_x = image.get_size().x(), max_x = 0;
        unsigned int min_y = image.get_size().y(), max_y = 0;
        for (auto& px : segment)
        {
            size_t n_unconnected = 0;
            for (long i = -1; i <= +1; ++i)
                for (long j = -1; j <= +1; ++j)
                    if (not (i == 0 and j == 0) and segment.find(Vector2ui(px.x() + i, px.y() + j)) == segment.end())
                        n_unconnected++;

            _elements.emplace(Element{px, image(px.x(), px.y()), n_unconnected > 1});

            if (n_unconnected > 1)
                temp_boundary.insert(px);

            min_x = std::min<unsigned int>(min_x, px.x());
            max_x = std::max<unsigned int>(max_x, px.x());
            min_y = std::min<unsigned int>(min_y, px.y());
            max_y = std::max<unsigned int>(max_y, px.y());
        }

        _x_bounds = {min_x, max_x};
        _y_bounds = {min_y, max_y};

        _boundary.clear();
        _boundary.reserve(temp_boundary.size());
        _boundary.push_back(*temp_boundary.begin());
        temp_boundary.erase(_boundary.front());

        auto push_if_neighbour = [&](Vector2ui c, int x_offset, int y_offset) -> bool
        {
            auto to_check = Vector2ui(c.x() + x_offset, c.y() + y_offset);
            if (temp_boundary.find(to_check) != temp_boundary.end())
            {
                _boundary.push_back(to_check);
                temp_boundary.erase(to_check);
                return true;
            }
            return false;
        };

        // counterclockwise order boundary to aid in MPP
        while (temp_boundary.size() > 0)
        {
            auto current = _boundary.back();
            if (push_if_neighbour(current, -1, 0))
                continue;
            else if (push_if_neighbour(current, 0, -1))
                continue;
            else if (push_if_neighbour(current, +1, 0))
                continue;
            else if (push_if_neighbour(current, 0, +1))
                continue;
            else
            {
                bool found = false;
                for (size_t i = _boundary.size() - 1; i > 0; --i)
                {
                    auto back_current = _boundary.at(i);
                    if (push_if_neighbour(current, -1, 0))
                        found = true;
                    else if (push_if_neighbour(current, 0, -1))
                        found = true;
                    else if (push_if_neighbour(current, +1, 0))
                        found = true;
                    else if (push_if_neighbour(current, 0, +1))
                        found = true;
                    else
                        continue;

                    if (found)
                        break;
                }

                assert(found && "region not 4-connected");
            }
        }
    }

    //https://sci-hub.se/https://ieeexplore.ieee.org/document/5008948
}