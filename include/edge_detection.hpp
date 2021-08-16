// 
// Copyright 2021 Clemens Cords
// Created on 12.08.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <binary_image.hpp>
#include <grayscale_image.hpp>
#include <spatial_filter.hpp>

// triggers edge detection function to measure a fitting threshold from the image
#define MEASURE_THRESHOLD -1.f

namespace crisp::EdgeDetection
{
    // @brief detects edges via computing the simplest form of the gradient, then thresholding the resulting image. Innacurate but fast
    // @param original: the image to be transformed
    // @param threshold: threshold for the gradient, range [0, 1] where 1 excludes all possible edges, 0 includes all
    // @returns binary image with edges
    BinaryImage threshold_gradient(const GrayScaleImage& original, float threshold = MEASURE_THRESHOLD);

    // @brief detects edges via computing the sobel gradient, then thresholding the resulting image. Moderatly fast and moderately accurate
    // @param original: the image to be transformed
    // @param threshold: threshold for the gradient, range [0, 1] where 1 excludes all possible edges, 0 includes all
    // @returns binary image with edges
    BinaryImage threshold_sobel(const GrayScaleImage& original, float threshold = MEASURE_THRESHOLD);

    // @brief detecs edge via the canny-algorithm. Slow but accurate
    // @param original: the image to be transformed
    // @param lower_threshold: the lower threshold for gradient magnitude, range [0, 1]
    // @param upper_threshold: the upper threshold for gradient magnitude, range [0, 1]
    // @returns binary image of thresholds
    BinaryImage canny(const GrayScaleImage& original, float lower_threshold = MEASURE_THRESHOLD, float upper_threshold = MEASURE_THRESHOLD);
}

#include ".src/edge_detection.inl"