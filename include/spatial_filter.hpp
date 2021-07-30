// 
// Copyright 2021 Clemens Cords
// Created on 29.07.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <Dense>


namespace crisp
{
    template<typename Image_t, typename Value_t = typename Image_t::value_t>
    class SpatialFilter
    {
        public:
            using Kernel_t = Eigen::Matrix<typename Image_t::value_t, Eigen::Dynamic, Eigen::Dynamic>;
            using EvaluationFunction_t = std::function<typename Image_t::value_t(Image_t, long, long, Kernel_t)>;

            SpatialFilter();

            Value_t& operator()(long x, long y);
            Value_t operator()(long x, long y) const;

            void apply_to(Image_t&);

            void set_kernel(Kernel_t&&);
            void set_evaluation_function(EvaluationFunction_t&&);

            struct Kernel
            {
                static Kernel_t&& identity(long dimensions);
                static Kernel_t&& one(long dimensions);
                static Kernel_t&& zero(long dimensions);
                static Kernel_t&& box(long dimensions, Value_t value);
                static Kernel_t&& normalized_box(long dimension);
                static Kernel_t&& gaussian(long dimension);
                static Kernel_t&& isotropic_laplacian_3x3(bool diagonal_edges = true);
            };

            struct EvaluationFunction
            {
                // @note according to p. 158 each element of element-wise sum is kernel(s, t) * f(x + s, y + t)
                static auto&& correlation(bool normalize = true);
                static auto&& convolution(bool normalize = true);
                static auto&& mean();
                static auto&& median();
                static auto&& min();
                static auto&& max();
                static auto&& n_ths_k_quantile(size_t n, size_t k);
            };

        private:
            Kernel_t _kernel;
            EvaluationFunction_t _eval;
    };
}

#include ".src/spatial_filter.inl"

