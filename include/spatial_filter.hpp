// 
// Copyright 2021 Clemens Cords
// Created on 29.07.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <Dense>


namespace crisp
{
    template<typename Value_t>
    class SpatialFilter
    {
        public:
            impl Kernel = Eigen::Matrix<Value_t, Eigen::Dynamic, Eigen::Dynamic>;

            SpatialFilter(long rows, long cols);

            template<typename Other_t>
            explicit operator SpatialFilter<Other_t>();

            template<typename OtherValue_t>
            [[nodiscard]] SpatialFilter<OtherValue_t> cast_to() const;

            void clear_and_resize(long rows, long cols);

            Value_t& operator()(long x, long y);
            Value_t operator()(long x, long y) const;

            // if true, will divide result of correlating kernel with pixel by the sum of kernel elements
            void set_normalization_enabled(bool);

            template<typename Image_t>
            void correlate_with(Image_t&);

            template<typename Image_t>
            void convolute_with(Image_t&);

            void set_kernel(Kernel&&);

            static Kernel&& identity();
            static Kernel&& gaussian_box();
            static Kernel&& gaussian();
            static Kernel&& mean();
            static Kernel&& median();
            static Kernel&& max();
            static Kernel&& min();

        private:
            bool _normalization_enabled = true;
            Value_t _sum_of_elements;
            Kernel _kernel;
    };

    template<typename Value_t>
    SpatialFilter<Value_t>::SpatialFilter(long rows, long cols)
    {
        _kernel.resize(rows, cols);
        _kernel.setConstant(Value_t(1.f));
        _sum_of_elements = rows * cols;
    }

    template<typename Value_t>
    template<typename Other_t>
    SpatialFilter<Other_t> SpatialFilter<Value_t>::cast_to() const
    {
        Eigen::Matrix<Other_t, Eigen::Dynamic, Eigen::Dynamic> new_kernel;
        new_kernel.resize(_kernel.rows(), _kernel.cols());

        for (long i = 0; i < _kernel.rows(); ++i)
            for (long j = 0; j < _kernel.cols(); ++j)
                new_kernel(i, j) = static_cast<Other_t>(_kernel(i, j));

        auto out = SpatialFilter<Other_t>(_kernel.rows(), _kernel.cols());
        out.set_kernel(new_kernel);
        return out;
    }

    template<typename Value_t>
    template<typename Other_t>
    SpatialFilter<Value_t>::operator SpatialFilter<Other_t>()
    {
        return (*this).template cast_to<Other_t>();
    }

    template<typename Value_t>
    void SpatialFilter<Value_t>::clear_and_resize(long width, long height)
    {
        _kernel.resize(width, height);
        _kernel.setConstant(Value_t(1.f));
        _sum_of_elements = width * height;
    }

    template<typename Value_t>
    Value_t SpatialFilter<Value_t>::operator()(long x, long y) const
    {
        return _kernel(x, y);
    }

    template<typename Value_t>
    Value_t& SpatialFilter<Value_t>::operator()(long x, long y)
    {
        return _kernel(x, y);
    }

    template<typename Value_t>
    void SpatialFilter<Value_t>::set_normalization_enabled(bool b)
    {
        _normalization_enabled = b;
    }

    template<typename Value_t>
    void SpatialFilter<Value_t>::set_kernel(Kernel&& kernel)
    {
        assert(kernel.cols() % 2 == 1 and kernel.rows() % 2 == 1 && "kernel has to be of odd dimensions in order to have a centered origin");
        _kernel = kernel;
    }

    template<typename Value_t>
    template<typename Image_t>
    void SpatialFilter<Value_t>::correlate_with(Image_t& image)
    {
        static_assert(std::is_same_v<Value_t, typename Image_t::value_t>, "Please cast the kernel/filter to a different value type via .cast_to<...>() before correlating it with an image of different value type");

        Value_t sum_of_elements = 0;
        for (long i = 0; i < _kernel.rows(); ++i)
            for (long j = 0; j < _kernel.cols(); ++j)
                sum_of_elements += _kernel(i, j);

        Eigen::Matrix<Value_t, Eigen::Dynamic, Eigen::Dynamic> result;
        result.resize(image.get_size().x, image.get_size().y);

        long a = (_kernel.cols() - 1) / 2;
        long b = (_kernel.rows() - 1) / 2;

        for (long x = 0; x < image.get_size().x; ++x)
        {
            for (long y = 0; y < image.get_size().y; ++y)
            {
                Value_t current_sum = 0;
                for (long s = -a; s <= a; ++s)
                    for (long t = -b; t <= b; ++t)
                        current_sum += _kernel(s + a, t + b) * image(x + a, y + b);

                result(x, y) = _normalization_enabled ? current_sum / sum_of_elements : current_sum;
            }
        }

        for (long x = 0; x < image.get_size().x; ++x)
            for (long y = 0; y < image.get_size().y; ++y)
                image(x, y) = result(x, y);
    }

    template<typename Value_t>
    template<typename Image_t>
    void SpatialFilter<Value_t>::convolute_with(Image_t& image)
    {
        static_assert(std::is_same_v<Value_t, typename Image_t::value_t>, "Please cast the kernel/filter to a different value type via .cast_to<...>() before convoluting it with an image of different value type");

        Value_t sum_of_elements = 0;
        for (long i = 0; i < _kernel.cols(); ++i)
            for (long j = 0; j < _kernel.rows(); ++j)
                sum_of_elements += _kernel(i, j);

        Eigen::Matrix<Value_t, Eigen::Dynamic, Eigen::Dynamic> result;
        result.resize(image.get_size().x, image.get_size().y);

        long a = (_kernel.cols() - 1) / 2;
        long b = (_kernel.rows() - 1) / 2;

        for (long x = 0; x < image.get_size().x; ++x)
        {
            for (long y = 0; y < image.get_size().y; ++y)
            {
                Value_t current_sum = 0;
                for (long s = -a; s <= a; ++s)
                    for (long t = -b; t <= b; ++t)
                        current_sum += _kernel(s + a, t + b) * image(x - a, y - b); // rotates kernel by 180

                result(x, y) = _normalization_enabled ? current_sum / sum_of_elements : current_sum;
            }
        }

        for (long x = 0; x < image.get_size().x; ++x)
            for (long y = 0; y < image.get_size().y; ++y)
                image(x, y) = result(x, y);
    }
}

