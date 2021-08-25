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
    // @brief extracts all 4-connected regions of identical value from an image, unless discarded all pixels will be allocated to exactly on segment
    // @param image: image containing the regions
    // @param min_region_size: threshold n such that all regions with number of pixels less than n will be removed
    // @returns vector of 4-connected regions
    //
    // @complexity: O(k*m*n) where k, number of pairwise different values in image, m,n size of the image
    std::vector<ImageSegment> decompose_into_segments(const BinaryImage& image, size_t min_region_size = 2);
    std::vector<ImageSegment> decompose_into_segments(const GrayScaleImage& image, size_t min_region_size = 2);
    std::vector<ImageSegment> decompose_into_segments(const ColorImage& image, size_t min_region_size = 2);

    // @brief extracts all 4-connected regions of specified allowed value from an image
    // @param image: image containing the regions
    // @param allowed_value: values that are allocated to regions
    // @param min_region_size: threshold n such that all regions with number of pixels less than n will be removed
    // @returns vector of 4-connected regions
    //
    // @complexity: O(k*m*n) where k, number of pairwise different allowed values and m, n size of the image
    std::vector<ImageSegment> decompose_into_segments(const BinaryImage& image, std::vector<bool> allowed_values, size_t min_region_size = 2);
    std::vector<ImageSegment> decompose_into_segments(const GrayScaleImage& image, std::vector<float> allowed_values, size_t min_region_size = 2);
    std::vector<ImageSegment> decompose_into_segments(const ColorImage& image, std::vector<Color> allowed_values, size_t min_region_size = 2);

    // @brief threshold grayscale image with manually specified threshold
    // @param image: the image to be thresholded
    // @returns binary image where a pixel is false (black) if it is equal to below the threshold, white otherwise
    //
    // @complexity amortized o(m*n) where m,n size of image
    [[nodiscard]] BinaryImage manual_threshold(const GrayScaleImage& image, float threshold);

    // @brief recursively modify threshold until convergence is achieved, simple but decent
    // @param image: the image to be thresholded
    // @returns binary image where a pixel is false (black) if it is equal to below the threshold, white otherwise
    //
    // @complexity O(k * log(k) + m*n) where k is the number of pairwise different intensities in the image
    [[nodiscard]] BinaryImage basic_threshold(const GrayScaleImage& image);

    // @brief applies otsu method to arrive at a mean that optimizes the between-class variance
    // @param image: the image to be thresholded
    // @returns a binary image where a pixel is false (black) if it is equal to below the threshold, white otherwise
    //
    // @complexity amortized o(255 * k + m*n) where k is the number of pairwise different intensities
    [[nodiscard]] BinaryImage otsu_threshold(const GrayScaleImage& image);

    // @brief iterate through the image in a spiraling pattern, calculating the local mean based on the pixels previously visited
    //        tends to be more resistant to non-uniform lighting
    // @param image: the image to be thresholded
    // @param constant: a constant modifying how many pixels are taken into account when calculating the local mean
    // @returns binary image where a pixel is false (black) if it is equal to below the threshold, white otherwise
    //
    // @complexity: amortized o(constant * m * n) where m, n the size of the image
    [[nodiscard]] BinaryImage variable_threshold(const GrayScaleImage& image, float constant = 0.25);

    // @brief compute the local threshold based on the local neighborhood of each pixel. Slow but most resistant to non-uniform lighting
    // @param image: image to be thresholded
    // @param neighborhood_size: the number of pixels sampled in the neighborhood of each pixel
    // @returns binary image where a pixel is false (black) if it is equal to below the threshold, white otherwise
    //
    // @complexity: amortized o(8 * neighborhood_size * m * n), where m,n size of the image
    [[nodiscard]] BinaryImage neighborhood_threshold(const GrayScaleImage& image, size_t neighborhood_size = 10);

    // @brief cluster an image into n superpixels
    // @param : image to be segmented
    // @param n_superpixel: the number of superpixels, may be merge during post-processing
    // @returns segmented image of corresponding type
    //
    // @complexity: amortized o(2 * N * m*n) where m, n size of image, N number of super pixel
    [[nodiscard]] ColorImage superpixel_clustering(const ColorImage&, size_t n_superpixel);
    [[nodiscard]] GrayScaleImage superpixel_clustering(const GrayScaleImage&, size_t n_superpixel);

    // @brief segment an image by succesively growing regions from a seed
    // @param image: image to be segmented
    // @param seeds: vector of seeds from which to start the growing. See overloads below for automated seed selections
    // @param add_upper_threshold: threshold T1 such that when a region encounters a pixel, if the color-distance between the pixel and the region is smaller than T1, it is added to the cluster
    // @param merge_upper_threshold: threshold T2 such that when a region encounters another region, if the color-distance between the two regions mean colors is samller than T2, the are merged
    // @returns segmented image of the corresponding type
    //
    // @note T1, T2 in [0, 1] where 0 means no values will be added/merged, 1 means all values will be added/merged
    // @complexity: O(N * m*n) where m, n size of image, N number of seeds. Runtime may decrease drastically based on number of merges
    [[nodiscard]] ColorImage region_growing_clustering(const ColorImage& image, std::vector<Vector2ui> seeds, float add_upper_threshold, float merge_upper_threshold);
    [[nodiscard]] GrayScaleImage region_growing_clustering(const GrayScaleImage& image, std::vector<Vector2ui> seeds, float add_upper_threshold, float merge_upper_threshold);

    // @overload: see
    // @brief segment an image by region growing by specifying a binary image that is treated as the set of seeds
    // @param image: image to be segmented
    // @param seed_image: binary image where each true (white) pixel corresponds to one seed position
    // @param add_upper_threshold: threshold T1 such that when a region encounters a pixel, if the color-distance between the pixel and the region is smaller than T1, it is added to the cluster
    // @param merge_upper_threshold: threshold T2 such that when a region encounters another region, if the color-distance between the two regions mean colors is samller than T2, the are merged
    // @returns segmented image of the corresponding type
    template<typename Image_t>
    [[nodiscard]] Image_t region_growing_clustering(const Image_t& image, BinaryImage seed_image, float add_upper_threshold, float merge_upper_threshold);

    // @brief segment image by k-means clustering with a fixed number of k. No merging will take place
    // @param image: image to be segmented
    // @param n_clusters: number of clusters
    // @param max_n_iterations: maximum number of iterations unless convergence is achieved earlier
    //                          default: infinite, recommended: more than 2, less than 20
    // @returns segmented image of corresponding type
    //
    // @complexity: amortized o(m*n + N*k*m*n) where k number of clusters, N max number of iterations, m, n size of image
    // @note for a detail explanation of how the heuristic for the initial cluster centers work, see: TODO
    [[nodiscard]] GrayScaleImage k_means_clustering(const GrayScaleImage& image, size_t n_clusters, size_t max_n_iterations = -1);
    [[nodiscard]] ColorImage k_means_clustering(const ColorImage& image, size_t n_clusters, size_t max_n_iterations = -1);
}

#include ".src/segmentation.inl"