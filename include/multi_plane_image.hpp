// 
// Copyright 2021 Clemens Cords
// Created on 19.08.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <image.hpp>
#include <grayscale_image.hpp>
#include <vector.hpp>

namespace crisp
{
    // n-dimensional image for arbitrary formats
    template<typename VectorValue_t, size_t N>
    class MultiPlaneImage : public Image<Vector<VectorValue_t, N>>
    {
        public:
            using Value_t = Vector<VectorValue_t, N>>;

            MultiPlaneImage() = default;

            // @brief create the image
            // @param width: the x-dimension of the image
            // @param height: the y-dimension of the image
            // @param init: the initial value of each pixel
            void create(long width, long height, Value_t init = Value_t(0));

            // @brief access the nth vector component of each pixel as a grayscale image
            // @param n: the planes index
            // @returns: GrayScaleImage of the components
            GrayScaleImage get_nth_plane(size_t n);

            // @brief set the nth vector component of each pixel
            // @param : GrayScaleImage holding the values
            void set_nth_plane(const GrayScaleImage&);

        protected:
            Vector2ui _size;
            Eigen::Matrix<Value_t, Eigen::Dynamic, Eigen::Dynamic> _values;

        private:
            Value_t get_pixel(long x, long y) const override;
            Value_t& get_pixel(long x, long y) override;
    };

    template<typename VectorValue_t, size_t N>
    void MultiPlaneImage<VectorValue_t, N>::create(long width, long height, MultiPlaneImage::Value_t init)
    {
        _values.resize(width, height);
        _values.setConstant(init);
       _size.x() = width;
        _size.y() = height;
    }

    template<typename VectorValue_t, size_t N>
    GrayScaleImage MultiPlaneImage<VectorValue_t, N>::get_nth_plane(size_t n)
    {
        assert(n < N);

        GrayScaleImage out;

        for (long x = 0; x <_size.x(); ++x)
            for (long y = 0; y < _size.y(); ++y)
                out(x, y) = _values(x, y).at(n);

        return out;
    }

    template<typename VectorValue_t, size_t N>
    void MultiPlaneImage<VectorValue_t, N>::set_nth_plane(const GrayScaleImage& in)
    {
        assert(in.get_size() == _size);

        for (long x = 0; x <_size.x(); ++x)
            for (long y = 0; y < _size.y(); ++y)
                _values(x, y).at(n) = in(x, y);
    }

    template<typename VectorValue_t, size_t N>
    typename MultiPlaneImage<VectorValue_t, N>::Value_t MultiPlaneImage<VectorValue_t, N>::get_pixel(long x, long y) const
    {
        return _values(x, y);
    }

    template<typename VectorValue_t, size_t N>
    typename MultiPlaneImage<VectorValue_t, N>::Value_t& MultiPlaneImage<VectorValue_t, N>::get_pixel(long x, long y)
    {
        return _values(x, y);
    }

}