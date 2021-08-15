// 
// Copyright 2021 Clemens Cords
// Created on 29.07.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <Dense>

namespace crisp
{
    template<typename Value_t>
    using Kernel = Eigen::Matrix<Value_t, Eigen::Dynamic, Eigen::Dynamic>;

    // @brief convolute one kernel k1 with another kernel k2, useful for combining them
    // @param image: k2, what would traditionally be an image
    // @param right: k1, the kernel the convolution applies
    // @returns resulting kernel
    template<typename Value_t>
    Kernel<Value_t> convolute(Kernel<Value_t> image, Kernel<Value_t> right);

    // @brief attempt to seperate a filter kernel k of size m*n into 2 vectors u, v such that u * v = k
    // @param original [in]: the kernel of arbitrary size to be seperated
    // @param out_left  [out]: pointer that u will be written to, m*1 matrix, nullptr if k is not seperable
    // @param out_right [out]: pointer that v will be written to, 1*n matrix, nullptr if k is not seperable
    // @returns true if seperation was succesfull, false otherwise
    //
    // @note the solution is arrived at numerically, a mean error: mean(abs(k - u*v)) = ~0.005f is expected
    template<typename Value_t>
    bool seperate(const Kernel<Value_t>& original, Kernel<Value_t>* out_left, Kernel<Value_t>* out_right);

    template<typename Value_t>
    void normalize(Kernel<Value_t>&);

    template<typename Value_t>
    void rotate(Kernel<Value_t>&, size_t n_90_degree);

    // Filters in the spatial (2d image) domain
    template<typename Image_t, typename Value_t = typename Image_t::value_t>
    class SpatialFilter
    {
        public:
            using EvaluationFunction_t = std::function<typename Image_t::value_t(const Image_t&, long, long, const Kernel<Value_t>&)>;

            SpatialFilter();

            // @brief apply filter to an image
            // @param : Image of the appropraite type specific as the template parameter
            void apply_to(Image_t&);

            // @brief set the filters kernel, a kernel is always square, has a odd-numbered dimensions and has it's origin at it's center
            // @param : the kernel
            // @note when not initialized, the kernel is the identity matrix [1] of size 1x1
            void set_kernel(Kernel<Value_t>);

            // @brief get size of kernel
            sf::Vector2<long> get_size() const;

            // @brief set the filters evaluation function, this governs how the convluted values are added up
            // @param : the function, must be bindable to std::function<typename Image_t::value_t(const Image_t& image, long pixel_x, long pixel_y, const Kernel<Value_t>& kernel)>;
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

            // ### KERNELS ##############################################################
            
            // @brief the identity kernel, a 1 at the center 0 everywhere else
            // @param dimension: the width and height of the kernel
            static Kernel<Value_t> identity(long dimensions);

            // @brief a kernel where every element is 0
            // @param dimension: the width and height of the kernel
            static Kernel<Value_t> one(long dimensions);

            // @brief a kernel where every element is 1
            // @param dimension: the width and height of the kernel
            static Kernel<Value_t> zero(long dimensions);

            // @brief a kernel where every element is initialized to the specified value
            // @param dimension: the width and height of the kernel
            static Kernel<Value_t> box(long dimensions, Value_t value);

            // @brief a kernel where every element is (1 / size), useful for blurring
            // @param dimension: the width and height of the kernel
            // @note convoluting with a normalized box kernel is equivalent to calculating the mean with a kernel of only 1s
            static Kernel<Value_t> normalized_box(long dimension);

            // @brief a rotationally symmetrical gaussian kernel. Normalized so that the sum of all elements is 1
            // @param dimension: the width and height of the kernel
            static Kernel<Value_t> gaussian(long dimension);

            // @brief kernel implementing the discrete laplacian in all 4 (8) directions, useful for edge detection
            // @param diagonal_edges: should the kernel also consider top-left, top-right, bottom-left and bottom-right directional derivatives
            static Kernel<Value_t> laplacian_first_derivative(bool diagonal_edges = true);
            static Kernel<Value_t> laplacian_second_derivative(bool diagonal_edges = true);

            enum LineDirection : int {HORIZONTAL, PLUS_45, VERTICAL, MINUS_45};

            static Kernel<Value_t> line_detection(LineDirection);

            // x-gradient: f(x, y) - f(x+1, y), y-gradient: f(x, y) - f(x, y+1)
            enum GradientDirection : int {X_DIRECTION = 0, Y_DIRECTION = 1};

            // @returns a 2x1 matrix for x-direction, a 1x2 matrix for y-direction
            static Kernel<Value_t> simple_gradient(GradientDirection);
            static Kernel<Value_t> simple_gradient_x();
            static Kernel<Value_t> simple_gradient_y();

            static Kernel<Value_t> roberts(GradientDirection);
            static Kernel<Value_t> roberts_x();
            static Kernel<Value_t> roberts_y();

            static Kernel<Value_t> prewitt(GradientDirection);
            static Kernel<Value_t> prewitt_x();
            static Kernel<Value_t> prewitt_y();

            static Kernel<Value_t> sobel(GradientDirection);
            static Kernel<Value_t> sobel_x();
            static Kernel<Value_t> sobel_y();

            enum KirschCompassDirection : int {NORTH, NORTH_WEST, WEST, SOUTH_WEST, SOUTH, SOUTH_EAST, EAST, NORTH_EAST};

            static Kernel<Value_t> kirsch_compass(KirschCompassDirection);
            static Kernel<Value_t> kirsch_compass_n();
            static Kernel<Value_t> kirsch_compass_nw();
            static Kernel<Value_t> kirsch_compass_w();
            static Kernel<Value_t> kirsch_compass_sw();
            static Kernel<Value_t> kirsch_compass_s();
            static Kernel<Value_t> kirsch_compass_se();
            static Kernel<Value_t> kirsch_compass_e();
            static Kernel<Value_t> kirsch_compass_ne();

            static Kernel<Value_t> laplacian_of_gaussian(long dimensions);

            // ### KERNELS ##############################################################

            // @brief applies the kernel by convolving it with the image such that pixel = kernel(s, t) * image(x + s, y + t)
            // @param normalize: should the resulting pixel be normalized back into the range [0, 1]
            // @note it's easier to image convolution has having a n*n square, aligning the center of the square with each pixel and sliding it over the image
            static auto&& convolution();

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

        private:
            Kernel<Value_t> _kernel;
            EvaluationFunction_t _eval;
    };

    using GrayScaleFilter = SpatialFilter<GrayScaleImage, GrayScaleImage::value_t>;
    //using BinaryFilter = SpatialFilter<BinaryImage, BinaryImage::value_t>;
}

#include ".src/spatial_filter.inl"

