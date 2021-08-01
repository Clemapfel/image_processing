// 
// Copyright 2021 Clemens Cords
// Created on 01.08.21 by clem (mail@clemens-cords.com)
//

#pragma once

namespace crisp
{
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
    void MorphologicalTransform<Value_t>::StructuringElement::set_offset(long row_i, long col_i, Value_t value)
    {
        _matrix(row_i, col_i) = value;
    }

    template<typename Value_t>
    Value_t MorphologicalTransform<Value_t>::StructuringElement::get_offset(long row_i, long col_i) const
    {
        return _matrix(row_i, col_i).value_or(Value_t(0.f));
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
    MorphologicalTransform<Value_t>::StructuringElement::cross(long dimensions)
    {
        assert(dimensions % 2 == 1 && "dimensions have to be odd for the structuring element to be rotationally symmetrical");
        auto out = MorphologicalTransform<Value_t>::StructuringElement(dimensions, dimensions);

        out._matrix.setConstant(Value_t(0));

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
        auto out = MorphologicalTransform<Value_t>::StructuringElement(dimensions, dimensions);

        long half_nrows = (dimensions - 1) / 2;
        long half_ncols = (dimensions - 1) / 2;

        long row_i = 0;
        int col_offset = 0;

        while (row_i <= half_nrows)
        {
            for (int offset = -col_offset; offset < col_offset; ++offset)
            {
                out._matrix(row_i, half_ncols + offset) = 1;
                out._matrix(dimensions - row_i, half_ncols + offset) = 1;
            }

            row_i += 1;
            col_offset += 1;
        }

        return out;
    }

    template<typename Value_t>
    typename MorphologicalTransform<Value_t>::StructuringElement
    MorphologicalTransform<Value_t>::StructuringElement::circle(long dimensinos)
    {
        //
    }

    template<typename Value_t>
    typename MorphologicalTransform<Value_t>::StructuringElement
    MorphologicalTransform<Value_t>::StructuringElement::square_pyramid(long dimensinos)
    {
        //
    }

    template<typename Value_t>
    typename MorphologicalTransform<Value_t>::StructuringElement
    MorphologicalTransform<Value_t>::StructuringElement::diamond_pyramid(long dimensinos)
    {
        //
    }

    template<typename Value_t>
    typename MorphologicalTransform<Value_t>::StructuringElement
    MorphologicalTransform<Value_t>::StructuringElement::hemisphere(long dimensinos)
    {
        //
    }

    template<typename Value_t>
    typename MorphologicalTransform<Value_t>::StructuringElement
    MorphologicalTransform<Value_t>::StructuringElement::square(long dimensions)
    {
        return MorphologicalTransform<Value_t>::StructuringElement::all_foreground(dimensions, dimensions);
    }
}