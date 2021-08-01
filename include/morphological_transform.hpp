// 
// Copyright 2021 Clemens Cords
// Created on 31.07.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <optional>
#include <vector>

#include <SFML/System/Vector2.hpp>
#include <Dense>

namespace crisp
{
    template<typename StructuringElementValue_t = bool>
    class MorphologicalTransform
    {
        public:
            class StructuringElement;

            MorphologicalTransform();
            void set_structuring_element(StructuringElement);

            template<typename Image_t>
            void erode(Image_t& image);

            template<typename Image_t>
            void dilate(Image_t& image);

            template<typename Image_t>
            void open(Image_t& image);

            template<typename Image_t>
            void close(Image_t& image);

            template<typename Image_t>
            void hit_or_miss_transform(Image_t& image);

            class StructuringElement
            {
                public:
                    static StructuringElement all_dont_care(long nrows, long ncols);
                    static StructuringElement all_foreground(long nrows, long ncols);
                    static StructuringElement all_background(long nrows, long ncols);

                    static StructuringElement cross(long dimensions);
                    static StructuringElement square(long dimensions);
                    static StructuringElement diamond(long dimensions);
                    static StructuringElement circle(long dimensions);

                    static StructuringElement square_pyramid(long dimensions);
                    static StructuringElement diamond_pyramid(long dimensions);
                    static StructuringElement hemisphere(long dimensions);

                    StructuringElement(long nrows, long ncols);
                    sf::Vector2<long> get_size() const;
                    sf::Vector2<long> get_origin() const;

                    void clear_and_resize(long nrows, long ncols);
                    void set_offset(long row_i, long col_i, StructuringElementValue_t);
                    void set_foreground(long row_i, long col_i);
                    void set_background(long row_i, long col_i);
                    void set_dont_care(long row_i, long col_i);

                    void reset_origin_to_center();
                    void set_origin(long row_i, long col_i);
                    void invert();

                    std::optional<StructuringElementValue_t> get_value(long x, long y) const;
                    bool is_foreground(long x, long y) const;
                    bool is_background(long x, long y) const;
                    bool is_dont_care(long x, long y) const;
                    StructuringElementValue_t get_offset(long x, long y) const;

                private:
                    sf::Vector2<long> _origin;
                    Eigen::Matrix<std::optional<StructuringElementValue_t>, Eigen::Dynamic, Eigen::Dynamic> _matrix;
            };

        private:
            StructuringElement _structuring_element;
    };

    template<typename T>
    using StructuringElement = typename MorphologicalTransform<T>::StructuringElement;

    using FlatStructuringElement = typename MorphologicalTransform<bool>::StructuringElement;
    using NonFlatStructuringElement = typename MorphologicalTransform<bool>::StructuringElement;

    template<typename Value_t>
    void MorphologicalTransform<Value_t>::set_structuring_element(MorphologicalTransform::StructuringElement element)
    {
        _structuring_element = element;
    }

    template<typename Value_t>
    template<typename Image_t>
    void MorphologicalTransform<Value_t>::dilate(Image_t& image)
    {
        long n = _structuring_element.get_size().x,
             m = _structuring_element.get_size().y;

        auto origin = _structuring_element.get_origin();

        Image_t result;
        result.create(image.get_size().x, image.get_size().y);

        for (long x = 0; x < image.get_size().x; ++x)
        {
            for (long y = 0; y < image.get_size().y; ++y)
            {
                for (int a = -origin.x; a < n - origin.x; ++a)
                {
                    for (int b = -origin.y; b < m - origin.y; ++b)
                    {
                        bool updated = false;
                        typename Image_t::value_t max = std::numeric_limits<typename Image_t::value_t>::min();

                        if (_structuring_element.is_dont_care(a + origin.x, b + origin.y))
                            continue;
                        else if (_structuring_element.is_foreground(a + origin.x, b + origin.y))
                        {
                            if (image.get_pixel_or_padding(x + a, y + b) > max)
                            {
                                max = image.get_pixel_or_padding(x + a, y + b);
                                updated = true;
                            }
                        }

                        if (updated)
                            result(x, y) = max;
                    }
                }

            }
        }

        for (long i = 0; i < image.get_size().x; ++i)
            for (long j = 0; j < image.get_size().y; ++j)
                image(i, j) = result(i, j);
    }

    template<typename Value_t>
    template<typename Image_t>
    void MorphologicalTransform<Value_t>::erode(Image_t& image)
    {
        long n = _structuring_element.get_size().x,
             m = _structuring_element.get_size().y;

        auto origin = _structuring_element.get_origin();

        Image_t result;
        result.create(image.get_size().x, image.get_size().y, Value_t(1.f));

        for (long x = 0; x < image.get_size().x; ++x)
        {
            for (long y = 0; y < image.get_size().y; ++y)
            {
                for (int a = -origin.x; a < n - origin.x; ++a)
                {
                    for (int b = -origin.y; b < m - origin.y; ++b)
                    {
                        bool updated = false;
                        typename Image_t::value_t min = Value_t(1);

                        if (_structuring_element.is_dont_care(a + origin.x, b + origin.y))
                            continue;
                        else if (_structuring_element.is_foreground(a + origin.x, b + origin.y))
                        {
                            if (float(image.get_pixel_or_padding(x + a, y + b)) < float(min))
                            {
                                min = image.get_pixel_or_padding(x + a, y + b);
                                updated = true;
                            }
                        }

                        if (updated)
                            result(x, y) = min;
                    }
                }

            }
        }

        for (long i = 0; i < image.get_size().x; ++i)
            for (long j = 0; j < image.get_size().y; ++j)
                image(i, j) = result(i, j);
    }
}

#include ".src/morphological_transform.inl"