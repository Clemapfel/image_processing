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
            ImageRegion(const ImageSegment& segment, Image_t& image);
            void create_from(const ImageSegment& segment, Image_t& image);
            const std::vector<Vector2ui>& get_boundary_polygon() const;

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
            std::vector<Vector2ui> _boundary_polygon;

            Vector2ui _x_bounds, _y_bounds;
        };

    template<typename Image_t>
    ImageRegion<Image_t>::ImageRegion(const ImageSegment& segment, Image_t& image)
    {
        create_from(segment, image);
    }

    template<typename Image_t>
    void ImageRegion<Image_t>::create_from(const ImageSegment& segment, Image_t& image)
    {
        _elements.clear();
        ImageSegment temp_boundary;
        ImageSegment candidate_boundary;

        unsigned int min_x = image.get_size().x(), max_x = 0;
        unsigned int min_y = image.get_size().y(), max_y = 0;
        for (auto& px : segment)
        {
            size_t n_unconnected = 0;
            for (long i = -1; i <= +1; ++i)
                for (long j = -1; j <= +1; ++j)
                    if (not (i == 0 and j == 0) and segment.find(Vector2ui(px.x() + i, px.y() + j)) == segment.end())
                        n_unconnected++;

            if (n_unconnected > 1)
            {
                _elements.emplace(Element{px, image(px.x(), px.y()), true});
                temp_boundary.insert(px);
            }
            else if (n_unconnected == 1)
            {
                candidate_boundary.insert(px);
            }
            else
            {
                _elements.emplace(Element{px, image(px.x(), px.y()), false});
            }

            min_x = std::min<unsigned int>(min_x, px.x());
            max_x = std::max<unsigned int>(max_x, px.x());
            min_y = std::min<unsigned int>(min_y, px.y());
            max_y = std::max<unsigned int>(max_y, px.y());
        }

        _x_bounds = {min_x, max_x};
        _y_bounds = {min_y, max_y};

        std::vector<Vector2ui> boundary;
        std::vector<uint8_t> directions;
        boundary.reserve(temp_boundary.size());
        directions.reserve(temp_boundary.size());

        auto direction_to_px = [&](Vector2ui c, uint8_t direction) -> Vector2ui
        {

        };

        auto push_if_neighbour = [&](Vector2ui c, uint8_t direction) -> bool
        {
            direction = direction % 8;
            int x_offset, y_offset;
            switch (direction)
            {
                case 0: // WEST
                    x_offset = -1;
                    y_offset = 0;
                    break;

                case 1: // SOUTH WEST
                    x_offset = -1;
                    y_offset = +1;
                    break;

                case 2: // SOUTH
                    x_offset = 0;
                    y_offset = +1;
                    break;

                case 3: // SOUTH EAST
                    x_offset = +1;
                    y_offset = +1;
                    break;

                case 4: // EAST
                    x_offset = +1;
                    y_offset = 0;
                    break;

                case 5: // NORTH EAST
                    x_offset = +1;
                    y_offset = -1;
                    break;

                case 6: // NORTH
                    x_offset = 0;
                    y_offset = -1;
                    break;

                case 7: // NORTH_WEST
                    x_offset = -1;
                    y_offset = -1;
                    break;

                default:
                    assert(false);
            }

            auto to_check = Vector2ui(c.x() + x_offset, c.y() + y_offset);
            if (temp_boundary.find(to_check) != temp_boundary.end())
            {
                boundary.push_back(to_check);
                directions.push_back(direction);
                temp_boundary.erase(to_check);
                return true;
            }
            return false;
        };

        boundary.push_back(*temp_boundary.begin());
        temp_boundary.erase(boundary.back());
        directions.push_back(0);

        while (temp_boundary.size() > 0)
        {
            auto current = boundary.back();
            auto current_direction = directions.back();

            bool found = false;
            for (size_t direction = current_direction, n = 0; n < 8; ++direction, ++n)
            {
                if (push_if_neighbour(current, direction))
                {
                    found = true;
                    break;
                }
            }

            if (not found)
            {
                for (long i = -1; i <= +1; ++i)
                {
                    for (long j = -1; j <= +1; ++j)
                    {
                        if (candidate_boundary.find(Vector2ui(current.x() + i, current.y() + j)) !=
                            candidate_boundary.end())
                        {
                            candidate_boundary.erase(Vector2ui(current.x() + i, current.y() + j));
                            temp_boundary.insert(Vector2ui(current.x() + i, current.y() + j));
                            goto next;
                        }
                    }
                }

                for (size_t i = boundary.size() - 1; i > 0; --i)
                {
                    current = boundary.at(i);
                    current_direction = directions.at(i);

                    for (size_t direction = current_direction - 1, n = 0; n < 8; ++direction, ++n)
                        if (push_if_neighbour(current, direction))
                            goto next;
                }

                if (not found)
                    break; //assert(false && "region not 8-connected");

                next:;
            }
        }

        auto turn_type = [&](size_t i_a, size_t i_b) -> int
        {
            auto point_a = boundary.at(i_a),
                 point_b = boundary.at(i_b);

            // warp point
            if (abs(int(point_a.x()) - int(point_b.x())) > 1 or
                abs(int(point_a.y()) - int(point_b.y())) > 1)
                return 0;

            auto dir_a = directions.at(i_a),
                 dir_b = directions.at(i_b);

            if (dir_b > dir_a or (dir_a == 7 and dir_b == 0))
                return -1; // left-hand turn
            else if (dir_b < dir_a or (dir_a == 0 and dir_b == 7))
                return +1; // right-hand turn
            else
                return 0; // colinear
        };

        _boundary_polygon.clear();

        for (size_t i = 0; i < boundary.size()-1; ++i)
            if (turn_type(i, i+1) != 0)
                _boundary_polygon.push_back(boundary.at(i));

        _boundary_polygon = boundary;
    }

    template<typename Image_t>
    const std::vector<Vector2ui>& ImageRegion<Image_t>::get_boundary_polygon() const
    {
        return _boundary_polygon;
    }


}