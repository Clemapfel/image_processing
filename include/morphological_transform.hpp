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
    template<typename StructuringElementValue_t = bool>
    class MorphologicalTransform
    {
        public:
            class StructuringElement;

            MorphologicalTransform();
            void set_structuring_element(StructuringElement);

            template<typename Image_t>
            void erode(Image_t& image);

            template<typename Image_t>
            void erode(Image_t& image, const Image_t& mask);

            template<typename Image_t>
            void dilate(Image_t& image);

            template<typename Image_t>
            void dilate(Image_t& image, const Image_t& mask);

            template<typename Image_t>
            void open(Image_t& image);

            template<typename Image_t>
            void close(Image_t& image);

            template<typename Image_t>
            void hit_or_miss_transform(Image_t& image);

            // structuring element used in transforms, matrix of std::optional<Value_t>
            class StructuringElement
            {
                using Value_t = StructuringElementValue_t;
                    
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
                    // TODO
                    static StructuringElement diamond_pyramid(long dimensions, Value_t min_intensity = Value_t(0.f), Value_t max_intensity = Value_t(1.f));
                    static StructuringElement cone(long dimensions, Value_t min_intensity = Value_t(0.f), Value_t max_intensity = Value_t(1.f));
                    static StructuringElement hemisphere(long dimensions, Value_t min_intensity = Value_t(0.f), Value_t max_intensity = Value_t(1.f));

                    StructuringElement(long nrows, long ncols);
                    sf::Vector2<long> get_size() const;
                    sf::Vector2<long> get_origin() const;

                    void clear_and_resize(long nrows, long ncols);
                    void set_offset(long row_i, long col_i, StructuringElementValue_t);
                    void set_foreground(long row_i, long col_i);
                    void set_background(long row_i, long col_i);
                    void set_dont_care(long row_i, long col_i);

                    void reset_origin_to_center();
                    void set_origin(long row_i, long col_i);
                    void invert();

                    std::optional<StructuringElementValue_t> get_value(long x, long y) const;
                    bool is_foreground(long x, long y) const;
                    bool is_background(long x, long y) const;
                    bool is_dont_care(long x, long y) const;

                private:
                    sf::Vector2<long> _origin;
                    Eigen::Matrix<std::optional<StructuringElementValue_t>, Eigen::Dynamic, Eigen::Dynamic> _matrix;
            };

        private:
            StructuringElement _structuring_element;
    };

    template<typename T>
    using StructuringElement = typename MorphologicalTransform<T>::StructuringElement;

    using FlatStructuringElement = typename MorphologicalTransform<bool>::StructuringElement;
    using NonFlatStructuringElement = typename MorphologicalTransform<float>::StructuringElement;
}

#include ".src/morphological_transform.inl"