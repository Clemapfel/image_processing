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

            void set_structuring_element(StructuringElement&&);

            template<typename Image_t>
            void erode(Image_t& image);

            template<typename Image_t>
            void dilate(Image_t& image);

            template<typename Image_t>
            void open(Image_t& image);

            template<typename Image_t>
            void close(Image_t& image);

            template<typename Image_t>
            void hit_or_miss_transform(Image_t& image);

            class StructuringElement
            {
                public:
                    static StructuringElement&& all_dont_care(long nrows, long ncols);
                    static StructuringElement&& all_foreground(long nrows, long ncols);
                    static StructuringElement&& all_background(long nrows, long ncols);

                    static StructuringElement&& cross(long dimensions);
                    static StructuringElement&& square(long dimensions);
                    static StructuringElement&& diamond(long dimensions);
                    static StructuringElement&& circle(long dimensions);

                    static StructuringElement&& square_pyramid(long dimensions);
                    static StructuringElement&& diamond_pyramid(long dimensions);
                    static StructuringElement&& hemisphere(long dimensions);

                    StructuringElement(long nrows, long ncols);

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
                    float get_offset(long x, long y) const;

                private:
                    sf::Vector2<long> _origin;
                    sf::Vector2<long> _size;
                    Eigen::Matrix<std::optional<StructuringElementValue_t>, Eigen::Dynamic, Eigen::Dynamic> _matrix;
            };

        private:
            StructuringElement _structuring_element;
    };

    using FlatMorphologicalTransform = MorphologicalTransform<bool>;
    using NonFlatMorphologicalTransform = MorphologicalTransform<float>;

    template<typename Value_t>
    void MorphologicalTransform<Value_t>::set_structuring_element(MorphologicalTransform::StructuringElement&& element)
    {
        _structuring_element = element;
    }


}

#include ".src/morphological_transform.inl"