// 
// Copyright 2021 Clemens Cords
// Created on 31.07.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <optional>
#include <vector>

#include <SFML/System/Vector2.hpp>
#include <Dense>

namespace crisp
{
    // morphological transforms using a flat or non-flat structuring element
    template<typename StructuringElementValue_t = bool>
    class MorphologicalTransform
    {
        public:
            class StructuringElement;

            MorphologicalTransform();

            // @brief save a structuring element to be used in any operations
            // @param : the structuring element with the same value type specific for the transform
            void set_structuring_element(StructuringElement);

            // @brief erode an image using the current structuring element
            // @param image: the image to be eroded, the image is modified directly
            //
            // @note eroding a binary image with a non-flat (grayscale) structuring element is discouraged
            // @complexity: o(n*m*p*q) where n, m are the images dimensions, p, q the structuring elements dimensions
            template<typename Image_t>
            void erode(Image_t& image);

            // @brief geodesically erode an image with a mask, the mask can be thought of as specifying an area
            //        representing the "core" of an image such that any amount of erosion passes will not reduce the image
            //        to less than the "core
            // @param image: the image to be eroded, the image is modified directly
            // @param mask: the mask use
            //
            // @complexity: o(n*m*p*q) where n, m are the images dimensions, p, q the structuring elements dimensions
            // @note geodiscally eroding a binary image with a non-flat (grayscale) structuring element is discouraged
            template<typename Image_t>
            void erode(Image_t& image, const Image_t& mask);

            // @brief dilate an image using the current structuring element
            // @param image: the image to be dilated, modified directly
            //
            // @complexity: o(n*m*p*q) where n, m are the images dimensions, p, q the structuring elements dimensions
            // @note dilating a binary image with a non-flat (grayscale) structuring element is discouraged
            template<typename Image_t>
            void dilate(Image_t& image);

            // @brief geodesically dilate an image with a mask, the mask can be though of as representing the maximum
            //        bounds a shape can be dilated to, no amount of dilation passes will increase the sizes volume
            //        to more than the mask
            // @param image: the image to be dilated, modified directly
            // @param mask: the mask used
            //
            // @complexity: o(n*m*p*q) where n, m are the images dimensions, p, q the structuring elements dimensions
            // @note geodesically dilating a binary image with a non-flat (grayscale) structuring element is discouraged
            template<typename Image_t>
            void dilate(Image_t& image, const Image_t& mask);

            // @brief open an image, equivalent to dilate(erode(image))
            // @param image: the image to be opened
            //
            // @complexity: o(2*n*m*p*q) where n, m are the images dimensions, p, q the structuring elements dimensions
            template<typename Image_t>
            void open(Image_t& image);

            // @brief open an image, equivalent to erode(dilate(image))
            // @param image: the image to be closed
            //
            // @complexity: o(2*n*m*p*q) where n, m are the images dimensions, p, q the structuring elements dimensions
            template<typename Image_t>
            void close(Image_t& image);

            // @brief hit-or-miss transform an image, it can be conceptualized as finding all locations of the structure
            //        elements template in the image
            // @param image: the image to be transformed, will result in a image where all values are either 1 if the template
            //               matched, 0 otherwise
            //
            // @complexity: o(n*m*(p*q/2)) where n, m are the images dimensions, p, q the structuring elements dimensions
            template<typename Image_t>
            void hit_or_miss_transform(Image_t& image);

            // structuring element used in transforms, matrix of std::optional<Value_t>
            class StructuringElement
            {
                using Value_t = StructuringElementValue_t;
                // bool represents a flat structuring element with possible values {false, true, "don't care"}
                // float represents a non-flat structuring element with possible values [0-1] or "don't care"
                    
                public:
                    // @brief initialize the element with nrow*ncol "don't care" values (internally: std::nullopt)
                    // @param nrows: number of rows
                    // @param ncols: number of columns
                    // @returns flat structuring element by value
                    static StructuringElement all_dont_care(long nrows, long ncols);

                    // @brief initialize the element with all "foreground" values (internally: Value_t(1.f))
                    // @param nrows: number of rows
                    // @param ncols: number of columns
                    // @returns flat structuring element by value
                    static StructuringElement all_foreground(long nrows, long ncols);

                    // @brief initialize the element with all "background" values (internally: Value_t(0.f))
                    // @param nrows: number of rows
                    // @param ncols: number of columns
                    // @returns flat structuring element by value
                    static StructuringElement all_background(long nrows, long ncols);

                    // @brief initialize the element so that two lines originating at (x = 0, y = (dimensions-1) / 2) and (x = (dimensions - 1) / 2), y = 0) cross in the center point
                    // @param dimensions: the number of rows and columns, has to be odd
                    // @returns flat structuring element by value
                    static StructuringElement cross(long dimensions);

                    // @brief initialize the element so that every pixel is a foreground value
                    // @param dimensions: the number of rows and columns, can be odd or even
                    // @notes this function is equivalent to called all_foreground(dimensions, dimensions)
                    // @returns flat structuring element by value
                    static StructuringElement square(long dimensions);

                    // @brief initialize the element as a 45° rhombus (lozenge) such that values inside the rhombus are in the foreground
                    // @param dimensions: the height and width of the element and rhombus
                    // @returns flat structuring element by value
                    static StructuringElement diamond(long dimensions);

                    // @brief initialize the element as a circle
                    // @param dimensions: twice the radius of the circle
                    // @returns flat structuring element by value
                    static StructuringElement circle(long dimensions);

                    // @brief initialize the element as a 3d square pyramid with it's peak value at the center
                    // @param dimensions: size of the square
                    // @param max_intensity: intensity at the outer edges of the element, default 0
                    // @param min_intensity: intensity at the peak (center) of the element, default 1
                    // @returns non-flat structuring elementy by value
                    // @notes for Value_t = bool this function is equivalent to calling square(dimensions)
                    static StructuringElement square_pyramid(long dimensions, Value_t min_intensity = Value_t(0.f), Value_t max_intensity = Value_t(1.f));

                    // @brief initialize the element as a 3d diamond pyramid
                    // @param dimensions: the size of the square structuring element
                    // @param min_intensity: the lowest intensity present at the outside of the pyramid, 0 by default
                    // @param max_intensity: the highest intensity present at the tip of the pyramid, 1 by default
                    static StructuringElement diamond_pyramid(long dimensions, Value_t min_intensity = Value_t(0.f), Value_t max_intensity = Value_t(1.f));

                    // @brief initialize the element as a 3d cone
                    // @param dimensions: the size of the square structuring element
                    // @param min_intensity: the lowest intensity present at the outside of the cone, 0 by default
                    // @param max_intensity: the highest intensity present at the tip of the cone, 1 by default
                    static StructuringElement cone(long dimensions, Value_t min_intensity = Value_t(0.f), Value_t max_intensity = Value_t(1.f));

                    // @brief initialize the element as a 3d sphere
                    // @param dimensions: the size of the square structuring element
                    // @param min_intensity: the lowest intensity at the outside of the surface of the sphere
                    // @param max_intensity: the highest intensity near the center of the surface of the sphere
                    static StructuringElement hemisphere(long dimensions, Value_t min_intensity = Value_t(0.f), Value_t max_intensity = Value_t(1.f));

                    // @brief initialize the structuring element
                    // @param nrows: the x-dimension of the structuring element
                    // @param ncols: the y-dimension of the structuring element
                    StructuringElement(long nrows, long ncols);

                    // @brief get the structuring elements dimensions
                    // @returns a vector of two longs where .x is the number of rows, .y the number of columns
                    sf::Vector2<long> get_size() const;

                    // @brief get the origin of the structuring element, centered by default
                    // @returns vector of two longs where .x is the x-coordinate, .y is the y-coordinate of the center
                    sf::Vector2<long> get_origin() const;

                    // @brief clear all values and resize the element
                    // @param nrows: the x-dimensions of the element
                    // @param ncols: the y-dimensions of the element
                    void clear_and_resize(long nrows, long ncols);

                    // @brief set the value of a specific index, any non-zero value will be treated as "foreground"
                    // @param row_i: the x-index of the element to be changed
                    // @param col_i: the y-index of the element to be changed
                    // @param : the value the element will be changed to
                    void set_value(long row_i, long col_i, StructuringElementValue_t);

                    // @brief set the value of a specified element to 1
                    // @param row_i: the x-index of the element to be changed
                    // @param col_i: the y-index of the element to be changed
                    void set_foreground(long row_i, long col_i);

                    // @brief set the value of a specified element to 0
                    // @param row_i: the x-index of the element to be changed
                    // @param col_i: the y-index of the element to be changed
                    void set_background(long row_i, long col_i);

                    // @brief set the value of a specified element to "don't care" (std::nullopt)
                    // @param row_i: the x-index of the element to be changed
                    // @param col_i: the y-index of the element to be changed
                    void set_dont_care(long row_i, long col_i);

                    // @brief reset the origin to center, equivalent to set_origin(floor(nrows / 2), floor(ncols / 2))
                    void reset_origin_to_center();

                    // @brief set the origin to the specified position
                    // @param row_i: the x-index of the new origin
                    // @param col_i: the y-index of the new origin
                    void set_origin(long row_i, long col_i);

                    // @brief invert all element not set to "don't care". For flat structuring elements all values x will be set to "not x", for non-flat structuring elements all x will be set to "1 - x"
                    void invert();

                    // @brief get the value (offset) of an element
                    // @param row_i: the x-index of the element to be changed
                    // @param col_i: the y-index of the element to be changed
                    // @returns an optional, not valid representing a "don't care" element
                    std::optional<StructuringElementValue_t> get_value(long row_i, long col_i) const;

                    // @brief equivalent to get_value(x, y) != 1
                    // @param row_i: the x-index
                    // @param col_i: the y-index
                    // @returns true if the value is neither zero nor "don't care", false otherwise
                    bool is_foreground(long x, long y) const;

                    // @brief equivalent to get_value(x, y) == 0
                    // @param row_i: the x-index
                    // @param col_i: the y-index
                    // @returns true if the value is 0, false otherwise
                    bool is_background(long x, long y) const;

                    // @brief equivalent to get_value(x, y) == std::nullopt
                    // @param row_i: the x-index
                    // @param col_i: the y-index
                    // @returns true if the value is "don't care", false otherwise
                    bool is_dont_care(long x, long y) const;

                private:
                    sf::Vector2<long> _origin;
                    Eigen::Matrix<std::optional<StructuringElementValue_t>, Eigen::Dynamic, Eigen::Dynamic> _matrix;
            };

        private:
            StructuringElement _structuring_element;
    };

    // typedefs for convenience
    template<typename T>
    using StructuringElement = typename MorphologicalTransform<T>::StructuringElement;

    using FlatStructuringElement = typename MorphologicalTransform<bool>::StructuringElement;
    using NonFlatStructuringElement = typename MorphologicalTransform<float>::StructuringElement;
}

#include ".src/morphological_transform.inl"