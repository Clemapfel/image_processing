// 
// Copyright 2021 Clemens Cords
// Created on 29.07.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <Dense>


namespace crisp
{
    // Filters in the spatial (2d image) domain
    template<typename Image_t, typename Value_t = typename Image_t::value_t>
    class SpatialFilter
    {
        public:
            using Kernel_t = Eigen::Matrix<typename Image_t::value_t, Eigen::Dynamic, Eigen::Dynamic>;
            using EvaluationFunction_t = std::function<typename Image_t::value_t(const Image_t&, long, long, const Kernel_t&)>;

            SpatialFilter();

            // @brief apply filter to an image
            // @param : Image of the appropraite type specific as the template parameter
            void apply_to(Image_t&);

            // @brief set the filters kernel, a kernel is always square, has a odd-numbered dimensions and has it's origin at it's center
            // @param : the kernel
            // @note when not initialized, the kernel is the identity matrix [1] of size 1x1
            void set_kernel(Kernel_t&&);

            // @brief set the filters evaluation function, this governs how the convluted values are added up
            // @param : the function, must be bindable to std::function<typename Image_t::value_t(const Image_t& image, long pixel_x, long pixel_y, const Kernel_t& kernel)>;
            // @note when not initialized, the evaluation function is the standard convolution
            void set_evaluation_function(EvaluationFunction_t&&);

            // @brief access the kernels value at the specified position
            // @param x: the row index of the value
            // @param y: the col index of the value
            // @returns writable reference to the value
            Value_t& operator()(long x, long y);

            // @brief access the kernels value at the specified position without being able to modify the kernel
            // @param x: the row index of the value
            // @param y: the col index of the value
            // @returns copy of the value
            Value_t operator()(long x, long y) const;

            // @brief rotates the kernels matrix by a multiple of 90°
            // @param n_90_degree_rotatios: the factor n so that the kernel will be rotated by n*90°
            void rotate_kernel(size_t n_90_degree_rotations);

            // pre defined kernels, set with SpatialFilter.set_kernel()
            struct Kernel
            {
                // @brief the identity kernel, a 1 at the center 0 everywhere else
                // @param dimension: the width and height of the kernel
                static Kernel_t identity(long dimensions);

                // @brief a kernel where every element is 0
                // @param dimension: the width and height of the kernel
                static Kernel_t one(long dimensions);

                // @brief a kernel where every element is 1
                // @param dimension: the width and height of the kernel
                static Kernel_t zero(long dimensions);

                // @brief a kernel where every element is initialized to the specified value
                // @param dimension: the width and height of the kernel
                static Kernel_t box(long dimensions, Value_t value);

                // @brief a kernel where every element is (1 / size), useful for blurring
                // @param dimension: the width and height of the kernel
                // @note convoluting with a normalized box kernel is equivalent to calculating the mean with a kernel of only 1s
                static Kernel_t normalized_box(long dimension);

                // @brief a rotationally symmetrical gaussian kernel. Normalized so that the sum of all elements is 1
                // @param dimension: the width and height of the kernel
                static Kernel_t gaussian(long dimension);

                // @brief kernel implementing the discrete laplacian in all 4 (8) directions, useful for edge detection
                // @param diagonal_edges: should the kernel also consider top-left, top-right, bottom-left and bottom-right directional derivatives
                static Kernel_t isotropic_laplacian_3x3(bool diagonal_edges = true);
            };

            // pre defined evaluation functions, set with SpatialFilter.set_evaluation_function()
            struct EvaluationFunction
            {
                // @brief applies the kernel by convolving it with the image such that pixel = kernel(s, t) * image(x + s, y + t)
                // @param normalize: should the resulting pixel be normalized back into the range [0, 1]
                // @note it's easier to image convolution has having a n*n square, aligning the center of the square with each pixel and sliding it over the image
                static auto&& convolution(bool normalize = true);

                // @brief applies the kernel by returning the mean of it's weighted sum. This is equivalent to convolution
                static auto&& mean();

                // @brief applies the kernel by returning the median of it's weighted sum
                // @note useful for filtering salt-and-pepper-noise in combinationg with an all-1s kernel
                static auto&& median();

                // @brief applies the kernel by returning the minimum of it's weighted sum
                static auto&& min();

                // @brief applies the kernel by returning the maximum of it's weighted sum
                static auto&& max();

                // @brief applies the kernel by returning the nth k-quantile of it's weighted sum. For example n = 1 k = 4 will return the first quartile
                // @notes specifying n <= 0 is equivalent to min(), n >= k is equivalent to max()
                static auto&& n_ths_k_quantile(size_t n, size_t k);
            };

        private:
            Kernel_t _kernel;
            EvaluationFunction_t _eval;
    };

    using GrayScaleFilter = SpatialFilter<GrayScaleImage, GrayScaleImage::value_t>;
    using BinaryFilter = SpatialFilter<BinaryImage, BinaryImage::value_t>;
}

#include ".src/spatial_filter.inl"

