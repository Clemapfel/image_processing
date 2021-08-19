// 
// Copyright 2021 Clemens Cords
// Created on 19.08.21 by clem (mail@clemens-cords.com)
//

namespace crisp
{
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

    template<typename VectorValue_t, size_t N>
    ColorImage
    MultiPlaneImage<VectorValue_t, N>::to_rgb_image(size_t red_plane_i, size_t green_plane_i, size_t blue_plane_i)
    {
        ColorImage out;
        out.create(_size.x(), _size.y());

        for (long x = 0; x <_size.x(); ++x)
        {
            for (long y = 0; y < _size.y(); ++y)
            {
                auto value = _values(x, y);
                out(x, y) = Color(value.at(red_plane_i), value.at(green_plane_i), value.at(blue_plane_i));
            }
        }

        return out;
    }
}