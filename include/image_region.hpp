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
            void create_from(const ImageSegment& segment, Image_t& image);

            // @returns vector of pixels such that the **top left** of that pixel is the sub-pixel position of the vertex
            std::vector<Vector2ui> compute_miniminum_perimeter_polygon(size_t cell_size = 1) const;

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
            
            enum class BoundaryVertexDirection : uint8_t
            {
                WEST = 0,
                SOUTH_WEST = 1,
                SOUTH = 2,
                SOUTH_EAST = 3,
                EAST = 4,
                NORTH_EAST = 5,
                NORTH = 6, 
                NORTH_WEST = 7
            };
            
            std::vector<Vector2ui> _boundary;
            std::vector<BoundaryVertexDirection> _directions;

            Vector2ui _x_bounds, _y_bounds;
        };

    template<typename Image_t>
    void ImageRegion<Image_t>::create_from(const ImageSegment& segment, Image_t& image)
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

            bool is_boundary = n_unconnected > 0;

            _elements.emplace(Element{px, image(px.x(), px.y()), is_boundary});

            if (is_boundary)
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

        std::vector<size_t> directions;
        _directions.reserve(temp_boundary.size());

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
                _boundary.push_back(to_check);
                directions.push_back(direction);
                image(_boundary.back().x(), _boundary.back().y()) = Value_t(1);
                temp_boundary.erase(to_check);
                return true;
            }
            return false;
        };

        _boundary.push_back(*temp_boundary.begin());
        image(_boundary.back().x(), _boundary.back().y()) = Value_t(1);
        temp_boundary.erase(_boundary.back());
        directions.push_back(0);

        while (temp_boundary.size() > 0)
        {
            auto current = _boundary.back();
            auto current_direction = directions.back();

            bool found = false;
            for (size_t direction = current_direction - 1, n = 0; n < 8; ++direction, ++n)
            {
                if (push_if_neighbour(current, direction))
                {
                    found = true;
                    break;
                }
            }

            if (not found)
            {
                for (size_t i = _boundary.size() - 1; i > 0; --i)
                {
                    current = _boundary.at(i);
                    current_direction = directions.at(i);

                    for (size_t direction = current_direction - 1, n = 0; n < 8; ++direction, ++n)
                        if (push_if_neighbour(current, direction))
                            goto next;
                }

                if (not found)
                    return; //assert(false && "region not 8-connected");

                next:;
            }
        }
    }

    template<typename Image_t>
    std::vector<Vector2ui> ImageRegion<Image_t>::compute_miniminum_perimeter_polygon(size_t cell_size) const
    {
        auto turn_type = [&](size_t i_a, size_t i_b) -> int
        {
            auto point_a = _boundary.at(i_a),
                 point_b = _boundary.at(i_b);

            // warp point
            if (abs(int(point_a.x()) - int(point_b.x())) > 1 or
                abs(int(point_a.y()) - int(point_b.y())) > 1)
                return 0;

            auto dir_a = _directions.at(i_a),
                 dir_b = _directions.at(i_b);

            if (dir_b > dir_a or (dir_a == 7 and dir_b == 0))
                return -1; // left-hand turn
            else if (dir_b < dir_a or (dir_a == 0 and dir_b == 7))
                return +1; // right-hand turn
            else
                return 0; // colinear
        };

        std::vector<Vector2ui> vertices;
        std::vector<bool> right_or_left;

        for (size_t i = 1; i < _boundary.size(); ++i)
        {
            auto s = turn_type(i-1, i);

            if (s != 0)
            {
                vertices.push_back(_boundary.at(i));
                right_or_left.push_back(s > 0);
            }
        }

        // MPP algorithm from "Digital Image Processing" (Gonzales, Woods, 2017), 4th edition, pg. 824
        auto sign = [](Vector2ui a, Vector2ui b, Vector2ui c) -> int
        {
            Eigen::Matrix<int, 3, 3> matrix;
            matrix << a.x(), a.y(), 1,
                      b.x(), b.y(), 1,
                      c.x(), c.y(), 1;

            auto det = matrix.determinant();

            if (det > 0)
                return +1;
            else if (det < 0)
                return -1;
            else
                return 0;
        };

        std::vector<Vector2ui> out; // final vertices
        out.push_back(_boundary.at(0) + Vector2ui(1, 1));

        size_t white_crawler = 0;
        size_t blue_crawler = 0;
        size_t current = 0;

        // convex = left
        while (current < vertices.size())
        {
            auto white_sign = sign(out.back(), vertices.at(white_crawler), vertices.at(current));
            auto blue_sign = sign(out.back(), vertices.at(blue_crawler), vertices.at(current));

            if (white_sign > 0)
            {
                out.push_back(vertices.at(white_crawler));
                blue_crawler = white_crawler;
                current = white_crawler + 1;
            }
            else if (blue_sign < 0)
            {
                out.push_back(vertices.at(blue_crawler));
                white_crawler = blue_crawler;
                current = blue_crawler + 1;
            }
            else if (white_sign <= 0 and blue_sign >= 0)
            {
                if (not right_or_left.at(current))
                    white_crawler = current;
                else
                    blue_crawler = current;
            }
        }

        return out;
    }
}