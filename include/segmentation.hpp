// 
// Copyright 2021 Clemens Cords
// Created on 13.08.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <image_segment.hpp>
#include <histogram.hpp>
#include <grayscale_image.hpp>
#include <binary_image.hpp>

#include <list>

namespace crisp::Segmentation
{
    // @brief recursively modify threshold until convergence is achieved, simple but decent
    // @param image: the image to be thresholded
    // @returns binary image where a pixel is false (black) if it is equal to below the threshold, white otherwise
    //
    // @complexity O(k * log(k)) where k is the number of pairwise different intensities in the image
    BinaryImage basic_threshold(const GrayScaleImage& image);

    // @brief applies otsu method to arrive at a mean that optimizes the between-class variance
    // @param image: the image to be thresholded
    // @returns a binary image where a pixel is false (black) if it is equal to below the threshold, white otherwise
    //
    // @complexity O(255 * k) where k is the number of pairwise different intensities
    BinaryImage otsu_threshold(const GrayScaleImage& image);

    // @brief iterate through the image in a spiraling pattern, calculating the local mean based on the pixels previously visited
    //        tends to be more resistant to non-uniform lighting
    // @param image: the image to be thresholded
    // @param constant: a constant modifying how many pixels are taken into account when calculating the local mean
    // @returns binary image where a pixel is false (black) if it is equal to below the threshold, white otherwise
    //
    // @complexity: amortized o(constant * max(m, n) * m * n) where m, n the size of the image
    BinaryImage variable_threshold(const GrayScaleImage& image, float constant = 0.25);

    // @brief compute the local threshold based on the local neighborhood of each pixel. Slow but most resistant to non-uniform lighting
    // @param image: the image to be thresholded
    // @param neighborhood_size: the number of pixels sampled in the neighborhood of each pixel
    // @returns binary image where a pixel is false (black) if it is equal to below the threshold, white otherwise
    //
    // @complexity: amortized o(8 * neighborhood_size * m * n), where m,n size of the image
    BinaryImage neighborhood_threshold(const GrayScaleImage& image, size_t neighborhood_size = 10);
}

#include ".src/segmentation.inl"