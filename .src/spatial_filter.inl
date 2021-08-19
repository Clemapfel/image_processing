// 
// Copyright 2021 Clemens Cords
// Created on 30.07.21 by clem (mail@clemens-cords.com)
//

#pragma once

namespace crisp
{

    template<typename Value_t>
    Eigen::Matrix<Value_t, Eigen::Dynamic, Eigen::Dynamic>
    convolute(Eigen::Matrix<Value_t, Eigen::Dynamic, Eigen::Dynamic> image,
              Eigen::Matrix<Value_t, Eigen::Dynamic, Eigen::Dynamic> right)
    {
        Vector2ui size = {image.rows(), image.cols()};

        assert(image.cols() >= right.cols() and image.rows() >= right.rows());

        Eigen::Matrix<Value_t, Eigen::Dynamic, Eigen::Dynamic> result;
        result.resize(size.x(), size.y());

        long a = floor(right.rows() / 2.f);
        long b = floor(right.cols() / 2.f);

        for (long i = 0; i < size.x(); ++i)
        {
            for (long j = 0; j < size.y(); ++j)
            {
                Value_t current_sum = 0;
                for (long s = -a; s <= a; ++s)
                {
                    for (long t = -b; t <= b; ++t)
                    {
                        if (i + s < 0 or j + t < 0 or i + s >= image.rows() or j + t >= image.cols())
                            continue;

                        auto test = right(a + s, b + t);
                        auto test2 = image(i + s, j + t);
                        current_sum += test * test2;
                    }
                }
                result(i, j) = current_sum;
            }
        }

        return result;
    }

    template<typename Value_t>
    bool seperate(const Kernel<Value_t>& original, Kernel<Value_t>* out_left, Kernel<Value_t>* out_right)
    {
        auto svd = Eigen::JacobiSVD<Kernel<Value_t>, Eigen::ColPivHouseholderQRPreconditioner>(original, Eigen::ComputeThinU | Eigen::ComputeThinV);

        auto s = svd.singularValues()(0);

        auto singular_sum = 0;
        for(size_t i = 0; i < svd.singularValues().size(); ++i)
            singular_sum += svd.singularValues()(i);

        if (abs(s - singular_sum) < 0.005)
        {
            out_left = nullptr;
            out_right = nullptr;
            return false;
        }

        auto U = svd.matrixU();
        auto u = Eigen::Vector3d();
        for (size_t i = 0; i < U.rows(); ++i)
            u(i) = U(i, 0) * s;

        auto V = svd.matrixV();
        auto v = Eigen::Vector3d();
        for (size_t i = 0; i < V.cols(); ++i)
            v(i) = V(i, 0);

        *out_left = u;
        *out_right = v.transpose();
        return true;
    }

    template<typename Value_t>
    void normalize(Kernel<Value_t>& kernel)
    {
        Value_t sum = 0;
        for (long i = 0; i < kernel.rows(); ++i)
            for (long j = 0; j < kernel.rows(); ++j)
                sum += kernel(i, j);

        for (long i = 0; i < kernel.rows(); ++i)
            for (long j = 0; j < kernel.rows(); ++j)
                kernel(i, j) /= sum;
    }
    
    template<typename Value_t>
    void rotate(Kernel<Value_t>& kernel, size_t n)
    {
        n = n % 4;
        if (n == 0 or n == 4)
            return;

        long size = kernel.cols();

        while (n > 0)
        {
            for (long x = 0; x < size / 2; ++x)
            {
                for (long y = x; y < size - x - 1; ++y)
                {
                    Value_t temp = kernel(x, y);
                    kernel[x][y] = kernel[y][size - 1 - x];
                    kernel[y][size - 1 - x] = kernel[size - 1 - x][size - 1 - y];
                    kernel[size - 1 - x][size - 1 - y] = kernel[size - 1 - y][x];
                    kernel[size - 1 - y][x] = temp;
                }
            }
            n -= 1;
        }
    }

    template<typename Image_t, typename Value_t>
    SpatialFilter<Image_t, Value_t>::SpatialFilter()
        : _kernel(identity(1)), _eval(convolution())
    {}

    /*
    template<typename Image_t, typename Value_t>
    template<typename Other_t>
    SpatialFilter<Other_t> SpatialFilter<Image_t, Value_t>::cast_to() const
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

    template<typename Image_t, typename Value_t>
    template<typename Other_t>
    SpatialFilter<Image_t, Value_t>::operator SpatialFilter<Other_t>()
    {
        return (*this).template cast_to<Other_t>();
    }
     */

    template<typename Image_t, typename Value_t>
    Value_t SpatialFilter<Image_t, Value_t>::operator()(long x, long y) const
    {
        return _kernel(x, y);
    }

    template<typename Image_t, typename Value_t>
    Value_t& SpatialFilter<Image_t, Value_t>::operator()(long x, long y)
    {
        return _kernel(x, y);
    }

    template<typename Image_t, typename Value_t>
    void SpatialFilter<Image_t, Value_t>::set_kernel(Kernel<Value_t> kernel)
    {
        _kernel = kernel;
    }

    template<typename Image_t, typename Value_t>
    Vector2ui SpatialFilter<Image_t, Value_t>::get_size() const
    {
        return {_kernel.rows(), _kernel.cols()};
    }

    template<typename Image_t, typename Value_t>
    void SpatialFilter<Image_t, Value_t>::set_evaluation_function(EvaluationFunction_t&& eval)
    {
        _eval = eval;
    }

    template<typename Image_t, typename Value_t>
    void SpatialFilter<Image_t, Value_t>::apply_to(Image_t& image)
    {
        Value_t sum_of_elements = 0;
        for (long i = 0; i < _kernel.rows(); ++i)
            for (long j = 0; j < _kernel.cols(); ++j)
                sum_of_elements += _kernel(i, j);

        Eigen::Matrix<Value_t, Eigen::Dynamic, Eigen::Dynamic> result;
        result.resize(image.get_size().x(), image.get_size().y());

        for (long x = 0; x < image.get_size().x(); ++x)
            for (long y = 0; y < image.get_size().y(); ++y)
                result(x, y) = _eval(image, x, y, _kernel);

        for (long x = 0; x < image.get_size().x(); ++x)
            for (long y = 0; y < image.get_size().y(); ++y)
                image(x, y) = result(x, y);
    }

    // ### KERNELS ###########################################################################

    template<typename Image_t, typename Value_t>
    Kernel<Value_t> SpatialFilter<Image_t, Value_t>::identity(long dimensions)
    {
        assert(dimensions != 0);
        Kernel<Value_t> out;
        out.resize(dimensions, dimensions);
        out.setConstant(Value_t(0));
        out((dimensions - 1) / 2, (dimensions - 1) / 2) = Value_t(1);
        return out;
    }

    template<typename Image_t, typename Value_t>
    Kernel<Value_t> SpatialFilter<Image_t, Value_t>::zero(long dimensions)
    {
        assert(dimensions != 0);
        Kernel<Value_t> out;
        out.resize(dimensions, dimensions);
        out.setConstant(Value_t(0));
        return out;
    }

    template<typename Image_t, typename Value_t>
    Kernel<Value_t> SpatialFilter<Image_t, Value_t>::one(long dimensions)
    {
        assert(dimensions != 0);
        Kernel<Value_t> out;
        out.resize(dimensions, dimensions);
        out.setConstant(Value_t(1));
        return out;
    }

    template<typename Image_t, typename Value_t>
    Kernel<Value_t> SpatialFilter<Image_t, Value_t>::box(long dimensions, Value_t value)
    {
        assert(dimensions != 0);
        Kernel<Value_t> out;
        out.resize(dimensions, dimensions);
        out.setConstant(Value_t(value));
        return out;
    }

    template<typename Image_t, typename Value_t>
    Kernel<Value_t> SpatialFilter<Image_t, Value_t>::normalized_box(long dimension)
    {
        assert(dimension != 0);
        return box(dimension, Value_t(1.f / float(dimension)));
    }

    template<typename Image_t, typename Value_t>
    Kernel<Value_t> SpatialFilter<Image_t, Value_t>::gaussian(long dimension)
    {
        assert(dimension != 0);
        Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> matrix;
        matrix.resize(dimension, dimension);

        auto square = [](double value) {return value * value;};

        double sum = 0;
        double sigma_sq = double(dimension);
        double gauss_factor = 1.f / sqrt(2 * M_PI + sigma_sq);
        double center = ceil(double(dimension) / 2.f);

        for (long x = 0; x < dimension; ++x)
        {
            for (long y = 0; y < dimension; ++y)
            {
                double length = sqrt(square(x - center) + square(y - center));
                matrix(x, y) = gauss_factor * exp(-1 * length / (2 * sigma_sq));
                sum += matrix(x, y);
            }
        }

        Kernel<Value_t> out;
        out.resize(dimension, dimension);

        // normalize and convert
        for (long x = 0; x < dimension; ++x)
            for (long y = 0; y < dimension; ++y)
                out(x, y) = static_cast<Value_t>(matrix(x, y) / sum);

        return out;
    }

    template<typename Image_t, typename Value_t>
    Kernel<Value_t> SpatialFilter<Image_t, Value_t>::laplacian_first_derivative(bool diagonal_edges)
    {
        Kernel<Value_t> out;
        out.resize(3, 3);

        out(0, 0) = diagonal_edges ? -1 : 0;
        out(0, 1) = -1;
        out(0, 2) = diagonal_edges ? -1 : 0;
        out(1, 0) = -1;
        out(1, 1) = diagonal_edges ? 8 : 4;
        out(1, 2) = -1;
        out(2, 0) = diagonal_edges ? -1 : 0;
        out(2, 1) = -1;
        out(2, 2) = diagonal_edges ? -1 : 0;

        return out;
    }

    template<typename Image_t, typename Value_t>
    Kernel<Value_t> SpatialFilter<Image_t, Value_t>::laplacian_second_derivative(bool diagonal_edges)
    {
        Kernel<Value_t> out;
        out.resize(3, 3);

        out(0, 0) = diagonal_edges ? -9 : 0;
        out(0, 1) = -1;
        out(0, 2) = diagonal_edges ? -9 : 0;
        out(1, 0) = -1;
        out(1, 1) = diagonal_edges ? 72 : 4;
        out(1, 2) = -1;
        out(2, 0) = diagonal_edges ? -9 : 0;
        out(2, 1) = -1;
        out(2, 2) = diagonal_edges ? -9 : 0;

        return out;
    }

    template<typename Image_t, typename Value_t>
    Kernel<Value_t> SpatialFilter<Image_t, Value_t>::line_detection(LineDirection direction)
    {
        Kernel<Value_t> out;
        out.resize(3, 3);

        switch (direction)
        {
            case HORIZONTAL:
                out << -1, -1, -1,
                        2,  2,  2,
                       -1, -1, -1;
                break;

            case PLUS_45:
                out <<  2, -1, -1,
                       -1,  2, -1,
                       -1, -1,  2;
                break;

            case VERTICAL:
                out << -1,  2, -1,
                       -1,  2, -1,
                       -1,  2, -1;
                break;

            case MINUS_45:
                out << -1, -1,  2,
                       -1,  2, -1,
                        2, -1,  2;
                break;
        }

        return out;
    }


    template<typename Image_t, typename Value_t>
    Kernel<Value_t> SpatialFilter<Image_t, Value_t>::simple_gradient(GradientDirection direction)
    {
        Kernel<Value_t> out;

        if (direction == X_DIRECTION)
        {
            out.resize(1, 2);
            out(0, 0) = -1;
            out(0, 1) = 1;
        }
        else
        {
            out.resize(2, 1);
            out(0, 0) = -1;
            out(1, 0) = 1;
        }

        return out;
    }

    template<typename Image_t, typename Value_t>
    Kernel<Value_t> SpatialFilter<Image_t, Value_t>::simple_gradient_x()
    {
        return simple_gradient(GradientDirection::X_DIRECTION);
    }

    template<typename Image_t, typename Value_t>
    Kernel<Value_t> SpatialFilter<Image_t, Value_t>::simple_gradient_y()
    {
        return simple_gradient(GradientDirection::Y_DIRECTION);
    }


    template<typename Image_t, typename Value_t>
    Kernel<Value_t> SpatialFilter<Image_t, Value_t>::roberts(GradientDirection direction)
    {
        Kernel<Value_t> out;
        out.resize(2, 2);

        if (direction == X_DIRECTION)
        {
            out(0, 0) = -1;
            out(0, 1) = 0;
            out(1, 1) = 1;
            out(1, 0) = 0;
        }
        else
        {
            out(0, 0) = 1;
            out(0, 1) = 0;
            out(1, 1) = -1;
            out(1, 0) = 0;
        }

        return out;
    }

    template<typename Image_t, typename Value_t>
    Kernel<Value_t> SpatialFilter<Image_t, Value_t>::roberts_x()
    {
        return roberts_x(GradientDirection::X_DIRECTION);
    }

    template<typename Image_t, typename Value_t>
    Kernel<Value_t> SpatialFilter<Image_t, Value_t>::roberts_y()
    {
        return roberts_y(GradientDirection::Y_DIRECTION);
    }

    template<typename Image_t, typename Value_t>
    Kernel<Value_t> SpatialFilter<Image_t, Value_t>::prewitt(GradientDirection direction)
    {
        Kernel<Value_t> out;
        out.resize(3, 3);

        if (direction == X_DIRECTION)
        {
            out << -1, -1, -1,
                    0,  0,  0,
                    1,  1,  1 ;
        }
        else
        {
            out << -1,  0,  1,
                   -1,  0,  1,
                   -1,  0,  1 ;
        }

        return out;
    }

    template<typename Image_t, typename Value_t>
    Kernel<Value_t> SpatialFilter<Image_t, Value_t>::prewitt_x()
    {
        return prewitt(GradientDirection::X_DIRECTION);
    }

    template<typename Image_t, typename Value_t>
    Kernel<Value_t> SpatialFilter<Image_t, Value_t>::prewitt_y()
    {
        return prewitt(GradientDirection::Y_DIRECTION);
    }

    template<typename Image_t, typename Value_t>
    Kernel<Value_t> SpatialFilter<Image_t, Value_t>::sobel(GradientDirection direction)
    {
        Kernel<Value_t> out;
        out.resize(3, 3);

        if (direction == X_DIRECTION)
        {
            out << -1, -2, -1,
                    0,  0,  0,
                    1,  2,  1 ;
        }
        else
        {
            out << -1,  0,  1,
                   -2,  0,  2,
                   -1,  0,  1 ;
        }

        return out;
    }

    template<typename Image_t, typename Value_t>
    Kernel<Value_t> SpatialFilter<Image_t, Value_t>::sobel_x()
    {
        return sobel(GradientDirection::X_DIRECTION);
    }

    template<typename Image_t, typename Value_t>
    Kernel<Value_t> SpatialFilter<Image_t, Value_t>::sobel_y()
    {
        return sobel(GradientDirection::Y_DIRECTION);
    }

    template<typename Image_t, typename Value_t>
    Kernel<Value_t> SpatialFilter<Image_t, Value_t>::kirsch_compass(KirschCompassDirection direction)
    {
        Kernel<Value_t> out;
        out.resize(3, 3);

        switch (direction)
        {
            case NORTH:
                out << -3, -3,  5,
                       -3,  0,  5,
                       -3, -3,  5;
                break;

            case NORTH_WEST:
                out << -3,  5,  5,
                       -3,  0,  5,
                       -3, -3, -3;
                break;

            case WEST:
                out <<  5,  5,  5,
                       -3,  0, -3,
                       -3, -3, -3;
                break;

            case SOUTH_WEST:
                out <<  5,  5, -3,
                        5,  0, -3,
                       -3, -3, -3;
                break;

            case SOUTH:
                out <<  5, -3, -3,
                        5,  0, -3,
                        5, -3, -3;
                break;

            case SOUTH_EAST:
                out << -3, -3, -3,
                        5,  0, -3,
                        5,  5, -3;
                break;

            case EAST:
                out << -3, -3, -3,
                       -3,  0, -3,
                        5,  5,  5;
                break;

            case NORTH_EAST:
                out << -3, -3, -3,
                       -3,  0,  5,
                       -3,  5,  5;
                break;
        }

        return out;
    }

    template<typename Image_t, typename Value_t>
    Kernel<Value_t> SpatialFilter<Image_t, Value_t>::kirsch_compass_n()
    {
        return kirsch_compass(KirschCompassDirection::NORTH);
    }

    template<typename Image_t, typename Value_t>
    Kernel<Value_t> SpatialFilter<Image_t, Value_t>::kirsch_compass_nw()
    {
        return kirsch_compass(KirschCompassDirection::NORTH_WEST);
    }

    template<typename Image_t, typename Value_t>
    Kernel<Value_t> SpatialFilter<Image_t, Value_t>::kirsch_compass_w()
    {
        return kirsch_compass(KirschCompassDirection::WEST);
    }

    template<typename Image_t, typename Value_t>
    Kernel<Value_t> SpatialFilter<Image_t, Value_t>::kirsch_compass_sw()
    {
        return kirsch_compass(KirschCompassDirection::SOUTH_WEST);
    }

    template<typename Image_t, typename Value_t>
    Kernel<Value_t> SpatialFilter<Image_t, Value_t>::kirsch_compass_s()
    {
        return kirsch_compass(KirschCompassDirection::SOUTH);
    }

    template<typename Image_t, typename Value_t>
    Kernel<Value_t> SpatialFilter<Image_t, Value_t>::kirsch_compass_se()
    {
        return kirsch_compass(KirschCompassDirection::SOUTH_EAST);
    }

    template<typename Image_t, typename Value_t>
    Kernel<Value_t> SpatialFilter<Image_t, Value_t>::kirsch_compass_e()
    {
        return kirsch_compass(KirschCompassDirection::EAST);
    }

    template<typename Image_t, typename Value_t>
    Kernel<Value_t> SpatialFilter<Image_t, Value_t>::kirsch_compass_ne()
    {
        return kirsch_compass(KirschCompassDirection::NORTH_EAST);
    }

    template<typename Image_t, typename Value_t>
    Kernel<Value_t> SpatialFilter<Image_t, Value_t>::laplacian_of_gaussian(long dimension)
    {
        assert(dimension != 0);
        Kernel<Value_t> matrix;
        matrix.resize(dimension, dimension);

        auto square = [](double value) {return value * value;};

        double sum = 0;
        double sigma_sq = dimension * 2;
        double center = ceil(double(dimension) / 2.f);

        float min = std::numeric_limits<float>::max(),
              max = std::numeric_limits<float>::min();

        for (long x = 0; x < dimension; ++x)
        {
            for (long y = 0; y < dimension; ++y)
            {
                float x_sq = (x - center)*(x - center);
                float y_sq = (y - center)*(y - center);

                matrix(x, y) = -1*((x_sq + y_sq - 2*sigma_sq) / (sigma_sq * sigma_sq)) * exp(-(x_sq + y_sq) / (2*sigma_sq));
                sum += matrix(x, y);

                min = std::min(min, matrix(x, y));
                max = std::max(max, matrix(x, y));
            }
        }

        return matrix;
    }

    // ### EVAL FUNCTIONS ####################################################################

    template<typename Image_t, typename Value_t>
    auto && SpatialFilter<Image_t, Value_t>::convolution()
    {
        return std::move([](const Image_t& image, long x, long y, const Kernel<Value_t>& kernel) -> Value_t
        {
            long a = floor(kernel.rows() / 2);
            long b = floor(kernel.cols() / 2);

            Value_t current_sum = 0;
            for (long s = -a; s <= a; ++s)
            {
                for (long t = -b; t <= b; ++t)
                {
                    if (kernel(a + s, b + t) == 0)
                        continue;

                    auto first = kernel(a + s, b + t);
                    auto two = image.get_pixel_or_padding(x + s, y + t);
                    current_sum += first * two;
                }
            }

            return current_sum;
        });
    }

    template<typename Image_t, typename Value_t>
    auto && SpatialFilter<Image_t, Value_t>::min()
    {
        return std::move([](const Image_t& image, long x, long y, const Kernel<Value_t>& kernel) -> Value_t
        {
            long a = (kernel.cols() - 1) / 2;
            long b = (kernel.rows() - 1) / 2;

            Value_t current = std::numeric_limits<Value_t>::max();

            for (long s = -a; s <= a; ++s)
                for (long t = -b; t <= b; ++t)
                    current = std::min<Value_t>(kernel(s + a, t + b) * image(x + a, y + b), current);

            return current;
        });
    }

    template<typename Image_t, typename Value_t>
    auto && SpatialFilter<Image_t, Value_t>::max()
    {
        return std::move([](const Image_t& image, long x, long y, const Kernel<Value_t>& kernel) -> Value_t
        {
            long a = (kernel.cols() - 1) / 2;
            long b = (kernel.rows() - 1) / 2;

            Value_t current = std::numeric_limits<Value_t>::min();

            for (long s = -a; s <= a; ++s)
                for (long t = -b; t <= b; ++t)
                    current = std::max<Value_t>(kernel(s + a, t + b) * image(x + a, y + b), current);

            return current;
        });
    }

    template<typename Image_t, typename Value_t>
    auto && SpatialFilter<Image_t, Value_t>::mean()
    {
        return SpatialFilter<Image_t, Value_t>::convolution(true);
    }

    template<typename Image_t, typename Value_t>
    auto && SpatialFilter<Image_t, Value_t>::median()
    {
        return std::move([](const Image_t& image, long x, long y, const Kernel<Value_t>& kernel) -> Value_t
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
        });
    }

    template<typename Image_t, typename Value_t>
    auto && SpatialFilter<Image_t, Value_t>::n_ths_k_quantile(
            size_t n, size_t k)
    {
        return std::move([n, k](const Image_t& image, long x, long y, const Kernel<Value_t>& kernel) -> Value_t
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
        });
    }


}