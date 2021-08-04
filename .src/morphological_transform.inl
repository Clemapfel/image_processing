// 
// Copyright 2021 Clemens Cords
// Created on 01.08.21 by clem (mail@clemens-cords.com)
//

#pragma once

namespace crisp
{
    // ## TRANSFORM ####################################################

    template<typename Value_t>
    void MorphologicalTransform<Value_t>::set_structuring_element(MorphologicalTransform::StructuringElement element)
    {
        _structuring_element = element;
    }

    namespace detail
    {
        template<typename Image_t>
        void flat_erode_aux(Image_t& image, Image_t& result, StructuringElement<bool> se)
        {
            long n = se.get_size().x,
                 m = se.get_size().y;

            auto origin = se.get_origin();

            for (long x = 0; x < image.get_size().x; ++x)
            {
                for (long y = 0; y < image.get_size().y; ++y)
                {
                    // BINARY IMAGE
                    if (std::is_same_v<typename Image_t::value_t, bool>)
                    {
                        bool missed = false;
                        for (int a = -origin.x; a < n - origin.x; ++a)
                        {
                            for (int b = -origin.y; b < m - origin.y; ++b)
                            {
                                if (se.is_foreground(a + origin.x, b + origin.y) !=
                                    image.get_pixel_or_padding(x + a, y + b))
                                {
                                    missed = true;
                                    goto skip;
                                }
                            }
                        }

                        skip:
                        if (missed)
                            result(x, y) = false;
                    }
                    // GRAYSCALE IMAGE
                    else
                    {
                        using ImageValue_t = typename Image_t::value_t;
                        ImageValue_t min = image(x, y);

                        for (int a = -origin.x; a < n - origin.x; ++a)
                        {
                            for (int b = -origin.y; b < m - origin.y; ++b)
                            {
                                if (se.is_foreground(a + origin.x, b + origin.y))
                                {
                                   min = std::min(image.get_pixel_or_padding(x + a, y + b), min);
                                }
                            }
                        }

                        result(x, y) = min;
                    }
                }
            }
        }

        template<typename Image_t>
        void flat_dilate_aux(Image_t& image, Image_t& result, StructuringElement<bool> se)
        {
            long n = se.get_size().x,
                 m = se.get_size().y;

            auto origin = se.get_origin();

            for (long x = 0; x < image.get_size().x; ++x)
            {
                for (long y = 0; y < image.get_size().y; ++y)
                {
                    if (std::is_same_v<typename Image_t::value_t, bool>)
                    {
                        bool found = false;
                        for (int a = -origin.x; a < n - origin.x; ++a)
                        {
                            for (int b = -origin.y; b < m - origin.y; ++b)
                            {
                                if (se.is_foreground(a + origin.x, b + origin.y) ==
                                    image.get_pixel_or_padding(x + a, y + b))
                                {
                                    found = true;
                                    goto skip;
                                }
                            }
                        }

                        skip:
                        if (found)
                            result(x, y) = true;
                    }
                    else
                    {
                        using ImageValue_t = typename Image_t::value_t;
                        ImageValue_t max = image(x, y);

                        for (int a = -origin.x; a < n - origin.x; ++a)
                        {
                            for (int b = -origin.y; b < m - origin.y; ++b)
                            {
                                if (se.is_foreground(a + origin.x, b + origin.y))
                                {
                                   max = std::max(image.get_pixel_or_padding(x + a, y + b), max);
                                }
                            }
                        }

                        result(x, y) = max;
                    }
                }
            }
        }
    }

    template<typename Value_t>
    template<typename Image_t>
    void MorphologicalTransform<Value_t>::erode(Image_t& image)
    {
        Image_t result;
        result.create(image.get_size().x, image.get_size().y, typename Image_t::value_t(true));

        if (std::is_same_v<Value_t, bool>)
            detail::flat_erode_aux<Image_t>(image, result, _structuring_element);

        for (long i = 0; i < image.get_size().x; ++i)
            for (long j = 0; j < image.get_size().y; ++j)
                image(i, j) = result(i, j);
    }

    template<typename Value_t>
    template<typename Image_t>
    void MorphologicalTransform<Value_t>::erode(Image_t& image, const Image_t& mask)
    {
        Image_t result;
        result.create(image.get_size().x, image.get_size().y, typename Image_t::value_t(true));

        if (std::is_same_v<Value_t, bool>)
            detail::flat_erode_aux<Image_t>(image, result, _structuring_element);

        for (long i = 0; i < image.get_size().x; ++i)
            for (long j = 0; j < image.get_size().y; ++j)
                image(i, j) = std::max(result(i, j), mask(i, j));
    }

    template<typename Value_t>
    template<typename Image_t>
    void MorphologicalTransform<Value_t>::dilate(Image_t& image)
    {
        Image_t result;
        result.create(image.get_size().x, image.get_size().y, typename Image_t::value_t(false));

        if (std::is_same_v<Value_t, bool>)
            detail::flat_dilate_aux<Image_t>(image, result, _structuring_element);

        for (long i = 0; i < image.get_size().x; ++i)
            for (long j = 0; j < image.get_size().y; ++j)
                image(i, j) = result(i, j);
    }

    template<typename Value_t>
    template<typename Image_t>
    void MorphologicalTransform<Value_t>::dilate(Image_t& image, const Image_t& mask)
    {
        Image_t result;
        result.create(image.get_size().x, image.get_size().y, typename Image_t::value_t(false));

        if (std::is_same_v<Value_t, bool>)
            detail::flat_dilate_aux<Image_t>(image, result, _structuring_element);

        for (long i = 0; i < image.get_size().x; ++i)
            for (long j = 0; j < image.get_size().y; ++j)
                image(i, j) = std::min(result(i, j), mask(i, j));
    }

    template<typename Value_t>
    template<typename Image_t>
    void MorphologicalTransform<Value_t>::open(Image_t& image)
    {
        erode(image);
        dilate(image);
    }

    template<typename Value_t>
    template<typename Image_t>
    void MorphologicalTransform<Value_t>::close(Image_t& image)
    {
        dilate(image);
        erode(image);
    }

    template<typename Value_t>
    template<typename Image_t>
    void MorphologicalTransform<Value_t>::hit_or_miss_transform(Image_t& image)
    {
        if (not std::is_same_v<Value_t, bool> and std::is_same_v<typename Image_t::value_t, bool>)
            std::cerr << "[WARNING] hit-or-miss template matching a binary image with a non-flat structuring element is discouraged as all non-zero elements of the structuring element will be treated as true (white) during comparison" << std::endl;

        long n = _structuring_element.get_size().x,
             m = _structuring_element.get_size().y;

        auto origin = _structuring_element.get_origin();

        Image_t result;
        result.create(image.get_size().x, image.get_size().y, Value_t(0.f));

        for (long x = 0; x < image.get_size().x; ++x)
        {
            for (long y = 0; y < image.get_size().y; ++y)
            {
                for (int a = -origin.x; a < n - origin.x; ++a)
                {
                    for (int b = -origin.y; b < m - origin.y; ++b)
                    {
                        if (_structuring_element.is_dont_care(a + origin.x, b + origin.y))
                            continue;

                        if (image.get_pixel_or_padding(x + a, y + b) !=
                            _structuring_element.get_value(a + origin.x, b + origin.y))
                        {
                            goto no_hit;
                        }
                    }
                }

                // match found
                result(x, y) = Value_t(true);

                no_hit:;
            }
        }

        for (long i = 0; i < image.get_size().x; ++i)
            for (long j = 0; j < image.get_size().y; ++j)
                image(i, j) = result(i, j);
    }

    // ### STRUCTURING ELEMENT #########################################

    template<typename Value_t>
    MorphologicalTransform<Value_t>::StructuringElement::StructuringElement(long nrows, long ncols)
    {
        clear_and_resize(nrows, ncols);
    }

    template<typename Value_t>
    sf::Vector2<long> MorphologicalTransform<Value_t>::StructuringElement::get_size() const
    {
        return {_matrix.rows(), _matrix.cols()};
    }

    template<typename Value_t>
    sf::Vector2<long> MorphologicalTransform<Value_t>::StructuringElement::get_origin() const
    {
        return _origin;
    }

    template<typename Value_t>
    void MorphologicalTransform<Value_t>::StructuringElement::clear_and_resize(long nrows, long ncols)
    {
        _matrix.resize(nrows, ncols);
        _matrix.setConstant(std::nullopt);
        _origin = {(nrows - (nrows % 2 == 1 ? 1 : 0)) / 2, (ncols - (ncols % 2 == 1 ? 1 : 0)) / 2};
    }

    template<typename Value_t>
    void MorphologicalTransform<Value_t>::StructuringElement::set_value(long row_i, long col_i, Value_t value)
    {
        _matrix(row_i, col_i) = value;
    }

    template<typename Value_t>
    void MorphologicalTransform<Value_t>::StructuringElement::set_foreground(long row_i, long col_i)
    {
        _matrix(row_i, col_i) = Value_t(1.f);
    }

    template<typename Value_t>
    void MorphologicalTransform<Value_t>::StructuringElement::set_background(long row_i, long col_i)
    {
        _matrix(row_i, col_i) = Value_t(0);
    }

    template<typename Value_t>
    void MorphologicalTransform<Value_t>::StructuringElement::set_dont_care(long row_i, long col_i)
    {
        _matrix(row_i, col_i) = std::nullopt;
    }

    template<typename Value_t>
    void MorphologicalTransform<Value_t>::StructuringElement::invert()
    {
        for (long x = 0; x < _matrix.rows(); ++x)
        {
            for (long y = 0; y < _matrix.cols(); ++y)
            {
                std::optional<Value_t> value = _matrix(x, y);
                if (value.has_value())
                    continue;
                else
                {
                    auto value = _matrix(x, y).value();
                    _matrix(x, y) = Value_t(Value_t(1.0f) - float(value));
                }

            }
        }
    }

    template<typename Value_t>
    void MorphologicalTransform<Value_t>::StructuringElement::set_origin(long row_i, long col_i)
    {
        if (row_i < 0 or row_i >= _matrix.size() or col_i < 0 or col_i >= _matrix.front().size())
            throw std::invalid_argument("origin at [" + std::to_string(row_i) + ", " + std::to_string(col_i) + "] would be outside the bounds of this " + std::to_string(_matrix.rows()) + "x" + std::to_string(_matrix.cols()) + " structuring element");
        _origin = {row_i, col_i};
    }

    template<typename Value_t>
    void MorphologicalTransform<Value_t>::StructuringElement::reset_origin_to_center()
    {
        set_origin(floor(_matrix.rows() / 2.f), floor(_matrix.cols() / 2.f));
    }

    template<typename Value_t>
    std::optional<Value_t> MorphologicalTransform<Value_t>::StructuringElement::get_value(long x, long y) const
    {
        return _matrix(x, y);
    }

    template<typename Value_t>
    bool MorphologicalTransform<Value_t>::StructuringElement::is_foreground(long x, long y) const
    {
        return _matrix(x, y) > Value_t(0.f);
    }

    template<typename Value_t>
    bool MorphologicalTransform<Value_t>::StructuringElement::is_background(long x, long y) const
    {
        return _matrix(x, y) == Value_t(0.f);
    }

    template<typename Value_t>
    bool MorphologicalTransform<Value_t>::StructuringElement::is_dont_care(long x, long y) const
    {
        return not _matrix(x, y).has_value();
    }

    template<typename Value_t>
    typename MorphologicalTransform<Value_t>::StructuringElement
    MorphologicalTransform<Value_t>::StructuringElement::all_dont_care(long nrows, long ncols)
    {
        auto out = MorphologicalTransform<Value_t>::StructuringElement(nrows, ncols);
        out._matrix.setConstant(std::nullopt);
        return out;
    }

    template<typename Value_t>
    typename MorphologicalTransform<Value_t>::StructuringElement
    MorphologicalTransform<Value_t>::StructuringElement::all_foreground(long nrows, long ncols)
    {
        auto out = MorphologicalTransform<Value_t>::StructuringElement(nrows, ncols);
        out._matrix.setConstant(Value_t(1.f));
        return out;
    }

    template<typename Value_t>
    typename MorphologicalTransform<Value_t>::StructuringElement
    MorphologicalTransform<Value_t>::StructuringElement::all_background(long nrows, long ncols)
    {
        auto out = MorphologicalTransform<Value_t>::StructuringElement(nrows, ncols);
        out._matrix.setConstant(Value_t(0.f));
        return out;
    }

    template<typename Value_t>
    MorphologicalTransform<Value_t>::MorphologicalTransform()
        : _structuring_element(MorphologicalTransform<Value_t>::StructuringElement::all_dont_care(1, 1))
    {}

    template<typename Value_t>
    typename MorphologicalTransform<Value_t>::StructuringElement
    MorphologicalTransform<Value_t>::StructuringElement::square(long dimensions)
    {
        return MorphologicalTransform<Value_t>::StructuringElement::all_foreground(dimensions, dimensions);
    }

    template<typename Value_t>
    typename MorphologicalTransform<Value_t>::StructuringElement
    MorphologicalTransform<Value_t>::StructuringElement::cross(long dimensions)
    {
        assert(dimensions % 2 == 1 && "dimensions have to be odd for the structuring element to be rotationally symmetrical");
        auto out = all_dont_care(dimensions, dimensions);

        for (size_t col_i = 0; col_i < out._matrix.cols(); ++col_i)
            out._matrix((out._matrix.rows() - 1) / 2, col_i) = Value_t(1.f);

        for (size_t row_i = 0; row_i < out._matrix.rows(); ++row_i)
            out._matrix((row_i, out._matrix.cols() - 1) / 2) = Value_t(1.f);

        return out;
    }

    template<typename Value_t>
    typename MorphologicalTransform<Value_t>::StructuringElement
    MorphologicalTransform<Value_t>::StructuringElement::diamond(long dimensions)
    {
        assert(dimensions % 2 == 1 && "dimensions have to be odd for the structuring element to be rotationally symmetrical");
        auto out = all_dont_care(dimensions, dimensions);

        if (not std::is_floating_point_v<Value_t>)
            return out;

        size_t offset = 0;

        long half = ((dimensions - 1) / 2);
        while (offset < half)
        {
            for (long x = half, y = offset; x < dimensions and y <= half; x++, y++)
            {
                out._matrix(x, y) = Value_t(1);
                out._matrix(dimensions - x - 1, y) = Value_t(1);
                out._matrix(x, dimensions - y - 1) = Value_t(1);
                out._matrix(dimensions - x - 1, dimensions - y - 1) = Value_t(1);
            }

            offset += 1;
        }

        return out;
    }

    template<typename Value_t>
    typename MorphologicalTransform<Value_t>::StructuringElement
    MorphologicalTransform<Value_t>::StructuringElement::circle(long dimensions)
    {
        assert(dimensions % 2 == 1 && "dimensions have to be odd for the structuring element to be rotationally symmetrical");
        auto out = all_dont_care(dimensions, dimensions);
        
        long radius = (dimensions - 1) / 2;

        auto mark = [dimensions, r = radius](long x, long y) -> bool {return (sqrt((x-r)*(x-r) + (y-r)*(y-r))) < r;};

        for (long i = 0; i < dimensions; ++i)
            for (long j = 0; j < dimensions; ++j)
                if (mark(i, j))
                    out.set_foreground(i, j);

        return out;
    }

    template<typename Value_t>
    typename MorphologicalTransform<Value_t>::StructuringElement
    MorphologicalTransform<Value_t>::StructuringElement::square_pyramid(long dimensions, Value_t min, Value_t max)
    {
        auto out = all_dont_care(dimensions, dimensions);

        if (not std::is_floating_point_v<Value_t>)
            return out;

        const Value_t step = (max - min) / ((dimensions - 1) / 2);
        Value_t current_color = Value_t(min);
        size_t offset = 0;

        while (offset < ((dimensions - 1) / 2))
        {
            for (long x = offset; x < dimensions - offset; ++x)
            {
                out._matrix(x, offset) = current_color;
                out._matrix(x, dimensions - offset - 1) = current_color;
            }

            for (long y = offset; y < dimensions - offset; ++y)
            {
                out._matrix(offset, y) = current_color;
                out._matrix(dimensions - offset - 1, y) = current_color;
            }

            current_color += step;
            offset += 1;
        }

        return out;
    }

    template<typename Value_t>
    typename MorphologicalTransform<Value_t>::StructuringElement
    MorphologicalTransform<Value_t>::StructuringElement::diamond_pyramid(long dimensions, Value_t min, Value_t max)
    {
        auto out = all_dont_care(dimensions, dimensions);

        if (not std::is_floating_point_v<Value_t>)
            return out;

        const Value_t step = (max - min) / ((dimensions - 1) / 2);
        Value_t current_color = Value_t(min);
        size_t offset = 0;

        long half = ((dimensions - 1) / 2);
        while (offset < half)
        {
            for (long x = half, y = offset; x < dimensions and y <= half; x++, y++)
            {
                out._matrix(x, y) = current_color;
                out._matrix(dimensions - x - 1, y) = current_color;
                out._matrix(x, dimensions - y - 1) = current_color;
                out._matrix(dimensions - x - 1, dimensions - y - 1) = current_color;
            }
            
            offset += 1;
            current_color += step;
        }

        return out;
    }

    template<typename Value_t>
    typename MorphologicalTransform<Value_t>::StructuringElement
    MorphologicalTransform<Value_t>::StructuringElement::cone(long dimensions, Value_t min, Value_t max)
    {
        if (not std::is_floating_point_v<Value_t>)
            return circle(dimensions, dimensions);

        assert(dimensions % 2 == 1 && "dimensions have to be odd for the structuring element to be rotationally symmetrical");
        auto out = all_dont_care(dimensions, dimensions);

        long radius = (dimensions - 1) / 2;
        auto dist = [dimensions, r = radius](long x, long y) -> float {return sqrt((x-r)*(x-r) + (y-r)*(y-r));};

        const Value_t step = (max - min) / ((dimensions - 1) / 2);
        Value_t current_color = Value_t(min);
        size_t offset = 0;

        for (long i = 0; i < dimensions; ++i)
            for (long j = 0; j < dimensions; ++j)
                if (dist(i, j) <= radius)
                    out.set_offset(i, j, (1 - (dist(i, j) / radius)) * (max - min) + min);
                else
                    out.set_background(i, j);

        return out;
    }

    template<typename Value_t>
    typename MorphologicalTransform<Value_t>::StructuringElement
    MorphologicalTransform<Value_t>::StructuringElement::hemisphere(long dimensions, Value_t min, Value_t max)
    {
        // sqrt(radius*radius + x*x)
        if (not std::is_floating_point_v<Value_t>)
            return circle(dimensions);

        assert(dimensions % 2 == 1 && "dimensions have to be odd for the structuring element to be rotationally symmetrical");
        auto out = all_dont_care(dimensions, dimensions);

        long radius = (dimensions - 1) / 2;
        auto dist = [dimensions, r = radius](long x, long y) -> float {return sqrt((x-r)*(x-r) + (y-r)*(y-r));};

        const Value_t step = (max - min) / ((dimensions - 1) / 2);
        Value_t current_color = Value_t(min);
        size_t offset = 0;

        for (long i = 0; i < dimensions; ++i)
        {
            for (long j = 0; j < dimensions; ++j)
            {
                if (dist(i, j) <= radius)
                {
                    Value_t v = dist(i, j) / radius;
                    out.set_offset(i, j, sqrt(1 - v * v));
                }
                else
                    out.set_background(i, j);
            }
        }

        return out;
    }
}