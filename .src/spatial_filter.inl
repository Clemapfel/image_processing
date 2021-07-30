// 
// Copyright 2021 Clemens Cords
// Created on 30.07.21 by clem (mail@clemens-cords.com)
//

#pragma once

namespace crisp
{
    template<typename Image_t>
    SpatialFilter<Image_t>::SpatialFilter()
        : _kernel(Kernel::identity(3)), _eval(EvaluationFunction::correlation(true))
    {}

    template<typename Image_t>
    template<typename Other_t>
    SpatialFilter<Other_t> SpatialFilter<Image_t>::cast_to() const
    {
        Eigen::Matrix<Other_t, Eigen::Dynamic, Eigen::Dynamic> new_kernel;
        new_kernel.resize(_kernel.rows(), _kernel.cols());

        for (long i = 0; i < _kernel.rows(); ++i)
            for (long j = 0; j < _kernel.cols(); ++j)
                new_kernel(i, j) = static_cast<typename Other_t::value_t>(_kernel(i, j));

        auto out = SpatialFilter<Other_t>(_kernel.rows(), _kernel.cols());
        out.set_kernel(new_kernel);
        return out;
    }

    template<typename Image_t>
    template<typename Other_t>
    SpatialFilter<Image_t>::operator SpatialFilter<Other_t>()
    {
        return (*this).template cast_to<Other_t>();
    }

    template<typename Image_t>
    typename SpatialFilter<Image_t>::Value_t SpatialFilter<Image_t>::operator()(long x, long y) const
    {
        return _kernel(x, y);
    }

    template<typename Image_t>
    typename SpatialFilter<Image_t>::Value_t& SpatialFilter<Image_t>::operator()(long x, long y)
    {
        return _kernel(x, y);
    }

    template<typename Image_t>
    void SpatialFilter<Image_t>::set_kernel(Kernel_t&& kernel)
    {
        assert(kernel.cols() % 2 == 1 and kernel.rows() % 2 == 1 && "kernel has to be of odd dimensions in order to have a centered origin");
        _kernel = kernel;
    }

    template<typename Image_t>
    void SpatialFilter<Image_t>::set_evaluation_function(EvaluationFunction_t&& eval)
    {
        _eval = eval;
    }

    template<typename Image_t>
    void SpatialFilter<Image_t>::apply_to(Image_t& image)
    {
        Value_t sum_of_elements = 0;
        for (long i = 0; i < _kernel.rows(); ++i)
            for (long j = 0; j < _kernel.cols(); ++j)
                sum_of_elements += _kernel(i, j);

        Eigen::Matrix<Value_t, Eigen::Dynamic, Eigen::Dynamic> result;
        result.resize(image.get_size().x, image.get_size().y);

        for (long x = 0; x < image.get_size().x; ++x)
            for (long y = 0; y < image.get_size().y; ++y)
                result(x, y) = _eval(image, x, y, _kernel);

        for (long x = 0; x < image.get_size().x; ++x)
            for (long y = 0; y < image.get_size().y; ++y)
                image(x, y) = result(x, y);
    }

    // ### KERNELS ###########################################################################

    template<typename Image_t>
    typename SpatialFilter<Image_t>::Kernel_t && SpatialFilter<Image_t>::Kernel::identity(long dimensions)
    {
        Kernel_t out;
        out.resize(dimensions, dimensions);
        out.setConstant(Value_t(0));
        out((dimensions - 1) / 2, (dimensions - 1) / 2) = Value_t(1);
        return std::move(out);
    }

    template<typename Image_t>
    typename SpatialFilter<Image_t>::Kernel_t && SpatialFilter<Image_t>::Kernel::zero(long dimensions)
    {
        Kernel_t out;
        out.resize(dimensions, dimensions);
        out.setConstant(Value_t(0));
        return std::move(out);
    }

    template<typename Image_t>
    typename SpatialFilter<Image_t>::Kernel_t && SpatialFilter<Image_t>::Kernel::one(long dimensions)
    {
        Kernel_t out;
        out.resize(dimensions, dimensions);
        out.setConstant(Value_t(1));
        return std::move(out);
    }

    template<typename Image_t>
    typename SpatialFilter<Image_t>::Kernel_t && SpatialFilter<Image_t>::Kernel::box(long dimensions, Value_t value)
    {
        Kernel_t out;
        out.resize(dimensions, dimensions);
        out.setConstant(Value_t(value));
        return std::move(out);
    }

    template<typename Image_t>
    typename SpatialFilter<Image_t>::Kernel_t && SpatialFilter<Image_t>::Kernel::normalized_box(long dimension)
    {
        return box(dimension, Value_t(1.f / float(dimension)));
    }

    template<typename Image_t>
    typename SpatialFilter<Image_t>::Kernel_t && SpatialFilter<Image_t>::Kernel::gaussian(long dimension)
    {
        Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> matrix;

        auto square = [](double value) {return value * value;};

        double sum = 0;
        double sigma_sq = dimension;
        double gauss_factor = 1.f / sqrt(2 * M_PI + sigma_sq);
        double center = ceil(dimension / 2.f);

        for (long x = 0; x < dimension; ++x)
        {
            for (long y = 0; y < dimension; ++y)
            {
                double length = sqrt(square(x - center) + square(y - center));
                matrix(x, y) = gauss_factor * exp(-1 * length / (2 * sigma_sq));
                sum += matrix(x, y);
            }
        }

        Kernel_t out;

        // normalize and convert
        for (long x = 0; x < dimension; ++x)
            for (long y = 0; y < dimension; ++y)
                out(x, y) = static_cast<Image_t>(matrix(x, y) / sum);

        return std::move(out);
    }

    template<typename Image_t>
    typename SpatialFilter<Image_t>::Kernel_t && SpatialFilter<Image_t>::Kernel::isotropic_laplacian_3x3(bool diagonal_edges)
    {
        Kernel_t out;

        out(0, 0) = diagonal_edges ? -1 : 0;
        out(0, 1) = -1;
        out(0, 2) = diagonal_edges ? -1 : 0;
        out(1, 0) = -1;
        out(1, 1) = diagonal_edges ? 8 : 4;
        out(1, 2) = -1;
        out(2, 0) = diagonal_edges ? -1 : 0;
        out(2, 1) = -1;
        out(2, 2) = diagonal_edges ? -1 : 0;

        return std::move(out);
    }

    // ### EVAL FUNCTIONS ####################################################################

    template<typename Image_t>
    typename SpatialFilter<Image_t>::EvaluationFunction_t && SpatialFilter<Image_t>::EvaluationFunction::correlation(bool normalize)
    {
        return [normalize](Image_t image, long x, long y, Kernel_t kernel) -> Value_t
        {
            long a = (kernel.cols() - 1) / 2;
            long b = (kernel.rows() - 1) / 2;

            Value_t sum_of_elements = 1;
            if (normalize)
            {
                sum_of_elements = 0;
                for (long i = 0; i < kernel.rows(); ++i)
                    for (long j = 0; j < kernel.cols(); ++j)
                        sum_of_elements += kernel(i, j);
            }

            Value_t current_sum = 0;
            for (long s = -a; s <= a; ++s)
                for (long t = -b; t <= b; ++t)
                    current_sum += kernel(s + a, t + b) * image(x + a, y + b);

            return current_sum / sum_of_elements;
        };
    }

    template<typename Image_t>
    typename SpatialFilter<Image_t>::EvaluationFunction_t && SpatialFilter<Image_t>::EvaluationFunction::convolution(bool normalize)
    {
        return [normalize](Image_t image, long x, long y, Kernel_t kernel) -> Value_t
        {
            long a = (kernel.cols() - 1) / 2;
            long b = (kernel.rows() - 1) / 2;

            Value_t sum_of_elements = 1;
            if (normalize)
            {
                sum_of_elements = 0;
                for (long i = 0; i < kernel.rows(); ++i)
                    for (long j = 0; j < kernel.cols(); ++j)
                        sum_of_elements += kernel(i, j);
            }

            Value_t current_sum = 0;
            for (long s = -a; s <= a; ++s)
                for (long t = -b; t <= b; ++t)
                    current_sum += kernel(s + a, t + b) * image(x - a, y - b); // - rotates kernel

            return current_sum / sum_of_elements;
        };
    }

    template<typename Image_t>
    typename SpatialFilter<Image_t>::EvaluationFunction_t && SpatialFilter<Image_t>::EvaluationFunction::min()
    {
        return [](Image_t image, long x, long y, Kernel_t kernel) -> Value_t
        {
            long a = (kernel.cols() - 1) / 2;
            long b = (kernel.rows() - 1) / 2;

            Value_t current = std::numeric_limits<Value_t>::max();

            for (long s = -a; s <= a; ++s)
                for (long t = -b; t <= b; ++t)
                    current = std::min<Value_t>(kernel(s + a, t + b) * image(x + a, y + b), current);

            return current;
        };
    }

    template<typename Image_t>
    typename SpatialFilter<Image_t>::EvaluationFunction_t && SpatialFilter<Image_t>::EvaluationFunction::max()
    {
        return [](Image_t image, long x, long y, Kernel_t kernel) -> Value_t
        {
            long a = (kernel.cols() - 1) / 2;
            long b = (kernel.rows() - 1) / 2;

            Value_t current = std::numeric_limits<Value_t>::min();

            for (long s = -a; s <= a; ++s)
                for (long t = -b; t <= b; ++t)
                    current = std::max<Value_t>(kernel(s + a, t + b) * image(x + a, y + b), current);

            return current;
        };
    }

    template<typename Image_t>
    typename SpatialFilter<Image_t>::EvaluationFunction_t && SpatialFilter<Image_t>::EvaluationFunction::mean()
    {
        return correlation(true);
    }

    template<typename Image_t>
    typename SpatialFilter<Image_t>::EvaluationFunction_t && SpatialFilter<Image_t>::EvaluationFunction::median()
    {
        return [](Image_t image, long x, long y, Kernel_t kernel) -> Value_t
        {
            long a = (kernel.cols() - 1) / 2;
            long b = (kernel.rows() - 1) / 2;

            std::vector<Value_t> values;
            values.reserve(kernel.rows()*kernel.cols());

            for (long s = -a; s <= a; ++s)
                for (long t = -b; t <= b; ++t)
                    values.push_back(kernel(s + a, t + b) * image(x + a, y + b));

            assert(values.size() % 2 == 1 && "Kernel needs to have odd dimensions");
            return values.at(values.size() / 2);
        };
    }

    template<typename Image_t>
    typename SpatialFilter<Image_t>::EvaluationFunction_t && SpatialFilter<Image_t>::EvaluationFunction::n_ths_k_quantile(
            size_t n, size_t k)
    {
        return [n, k](Image_t image, long x, long y, Kernel_t kernel) -> Value_t
        {
            assert(kernel.rows() % 2 == 1 and kernel.cols() % 2 == 1 && "Kernel needs to have odd dimensions");

            long a = (kernel.cols() - 1) / 2;
            long b = (kernel.rows() - 1) / 2;

            std::vector<Value_t> values;
            values.reserve(kernel.rows()*kernel.cols());

            for (long s = -a; s <= a; ++s)
                for (long t = -b; t <= b; ++t)
                    values.push_back(kernel(s + a, t + b) * image(x + a, y + b));

            if (n >= k)
                return values.back();
            else if (n <= 0)
                return values.front();
            else
                return values.at(size_t(values.size() * (float(n) / float(k))));
        };
    }
}