// 
// Copyright 2021 Clemens Cords
// Created on 29.08.21 by clem (mail@clemens-cords.com)
//

namespace crisp
{
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

        _boundary.reserve(temp_boundary.size());
        _boundary_directions.reserve(temp_boundary.size());

        auto direction_to_px = [&](Vector2ui c, uint8_t direction) -> Vector2ui
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

            return Vector2ui(c.x() + x_offset, c.y() + y_offset);
        };

        auto top_left = *temp_boundary.begin();
        temp_boundary.erase(top_left);
        _boundary.push_back(top_left);
        _boundary_directions.push_back(0);

        size_t current_i = 0;
        bool finished_maybe = false;
        do
        {
            auto current = _boundary.at(current_i);
            auto current_direction = _boundary_directions.at(current_i);

            bool found = false;

            // check strong candidates
            for (int direction = current_direction - 1, n = 0; n < 8; ++direction, ++n)
            {
                auto to_check = direction_to_px(current, direction);

                if (to_check.x() < 0 or to_check.x() > image.get_size().x() or to_check.y() < 0 or to_check.y() > image.get_size().y())
                    continue;

                if (to_check == top_left)
                    finished_maybe = true;

                if (temp_boundary.find(to_check) != temp_boundary.end())
                {
                    _boundary.push_back(to_check);
                    _boundary_directions.push_back(direction);
                    temp_boundary.erase(to_check);
                    found = true;
                    break;
                }
            }

            if (found)
            {
                current_i = _boundary.size() - 1;
                continue;
            }
            else if (finished_maybe)
                break;

            // check weak candidates
            for (int direction = current_direction - 1, n = 0; n < 8; ++direction, ++n)
            {
                auto to_check = direction_to_px(current, direction);

                if (to_check.x() < 0 or to_check.x() > image.get_size().x() or to_check.y() < 0 or to_check.y() > image.get_size().y())
                    continue;

                if (candidate_boundary.find(to_check) != candidate_boundary.end())
                {
                    _boundary.push_back(to_check);
                    _boundary_directions.push_back(direction);
                    candidate_boundary.erase(to_check);
                    found = true;
                    break;
                }
            }

            if (found)
            {
                current_i = _boundary.size() - 1;
                continue;
            }
            else
                current_i--;

        } while (current_i > 0);

        auto turn_type = [&](size_t i_a, size_t i_b) -> int
        {
            auto point_a = _boundary.at(i_a),
                 point_b = _boundary.at(i_b);

            // warp point
            if (abs(int(point_a.x()) - int(point_b.x())) > 1 or
                abs(int(point_a.y()) - int(point_b.y())) > 1)
                return 0;

            auto dir_a = _boundary_directions.at(i_a),
                 dir_b = _boundary_directions.at(i_b);

            if (dir_b > dir_a or (dir_a == 7 and dir_b == 0))
                return -1; // left-hand turn
            else if (dir_b < dir_a or (dir_a == 0 and dir_b == 7))
                return +1; // right-hand turn
            else
                return 0; // colinear
        };

        _boundary_polygon.clear();

        Vector2f mean_pos = Vector2f(0, 0);

        for (size_t i = 0; i < _boundary.size() - 1; ++i)
        {
            mean_pos += Vector2f(_boundary.at(i).x(), _boundary.at(i).y());

            if (turn_type(i, i+1) != 0)
                _boundary_polygon.push_back(_boundary.at(i));
        }

        _centroid = mean_pos / float(_boundary.size());
        std::cout << "centroid: " << _centroid << std::endl;


        //_boundary_polygon = boundary;
    }

    template<typename Image_t>
    std::vector<float> ImageRegion<Image_t>::farthest_point_signature() const
    {
        // El-ghazal, A., Basir, O., & Belkasim, S. (2009). Farthest point distance: A new shape signature for Fourier descriptors. Signal Processing: Image Communication, 24(7), 572–586. doi:10.1016/j.image.2009.04.00
        auto value = [&](Vector2ui a)
        {
            // explicit casts for readability
            int a_x = a.x();
            int a_y = a.y();
            int c_x = _centroid.x();
            int c_y = _centroid.y();
            float a_to_centroid = sqrt((a_x - c_x) * (a_x - c_x) + (a_y - c_y) * (a_y - c_y));

            Vector2ui farthest_point = a;
            float max_dist = 0;
            for (const auto& point : _boundary)
            {
                int p_x = point.x();
                int p_y = point.y();
                float dist = sqrt((a_x - p_x) * (a_x - p_x) + (a_y - p_y) * (a_y - p_y));
                if (dist > max_dist)
                {
                    farthest_point = point;
                    max_dist = max_dist;
                }
            }

            int fp_x = farthest_point.x();
            int fp_y = farthest_point.y();
            float fp_to_centroid = sqrt((fp_x - c_x) * (fp_x - c_x) + (fp_y - c_y) * (fp_y - c_y));

            return a_to_centroid + fp_to_centroid;
        };

        std::vector<float> signature;
        signature.reserve(_boundary.size());

        for (auto& px : _boundary)
            signature.push_back(value(px));

        return signature;
    }

    template<typename Image_t>
    std::vector<float> ImageRegion<Image_t>::radial_distance_signature() const
    {
        auto value = [&](Vector2ui a)
        {
            int a_x = a.x();
            int a_y = a.y();
            int c_x = _centroid.x();
            int c_y = _centroid.y();
            return sqrt((a_x - c_x) * (a_x - c_x) + (a_y - c_y) * (a_y - c_y));
        };

        std::vector<float> signature;
        signature.reserve(_boundary.size());

        for (auto& px : _boundary)
            signature.push_back(value(px));

        return signature;
    }

    template<typename Image_t>
    std::vector<std::complex<float>> ImageRegion<Image_t>::complex_coordinate_signature() const
    {
        std::vector<std::complex<float>> signature;
        signature.reserve(_boundary.size());

        for (auto& px : _boundary)
            signature.emplace_back(px.x() - _centroid.x(), px.y() - _centroid.y());

        return signature;
    }

    template<typename Image_t>
    std::vector<float> ImageRegion<Image_t>::slope_chain_code_signature() const
    {
        std::vector<float> signature;
        for (size_t i = 0; i < _boundary_polygon.size() - 2; ++i)
        {
            auto a = _boundary_polygon.at(i);
            auto b = _boundary_polygon.at(i + 1);
            signature.push_back(atan2(b.y() - a.y(), (b.x() - a.x())));
        }

        return signature;
    }
}

