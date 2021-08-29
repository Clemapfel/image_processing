// 
// Copyright 2021 Clemens Cords
// Created on 26.08.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <image_segment.hpp>
#include <vector.hpp>

namespace crisp
{
    // a 4-connected region of an image, unlike ImageSegment, each pixel in the region has more than just a spacial dimension
    template<typename Image_t>
    class ImageRegion
    {
        using Value_t = typename Image_t::value_t;
        class Iterator;
        class ConstIterator;

        public:
            ImageRegion() = default;
            ImageRegion(const ImageSegment& segment, Image_t& image);
            void create_from(const ImageSegment& segment, Image_t& image);

            const std::vector<Vector2ui>& get_boundary();
            const std::vector<Vector2ui>& get_boundary_polygon();

            Vector2ui get_centroid() const;

            Iterator begin();
            ConstIterator begin() const;

            Iterator end();
            ConstIterator end() const;

            // @complexity O(k*k) where k number of boundary points
            std::vector<float> farthest_point_signature() const;

            // @complexity amortized o(k) where k number of boundary points
            std::vector<float> radial_distance_signature() const;

            // @complexity amortized o(k) where k number of boundary points
            std::vector<std::complex<float>> complex_coordinate_signature() const;

            // @complexity amortized o(2k) where k number of boundary points
            std::vector<float> slope_chain_code_signature() const;

            const std::pair<Vector2ui, Vector2ui>& get_major_axis();
            const std::pair<Vector2ui, Vector2ui>& get_minor_axis();
            std::array<Vector2ui, 4> get_bounding_box();
            std::array<Vector2ui, 4> get_axis_aligned_bounding_box();

            float get_perimeter() const;
            float get_area() const;
            float get_compactness() const;
            float get_circularity() const;
            float get_eccentricity() const;

            // how many non-boundary pixels have neighbours that are not in M, range [0, 1]
            float get_porosity() const;

            size_t get_n_holes(); 

            GrayScaleImage get_covariance_matrix() const;
            float get_nth_order_moment_invariant(size_t order);

        private:
            struct Element
            {
                Vector2ui position;
                Value_t value;
            };

            struct ElementCompare
            {
                bool operator()(const Element& a, const Element& b) const
                {
                    return a.position.x() != b.position.x() ? a.position.x() < b.position.x() : a.position.y() < b.position.y();
                }
            };

            std::set<Element, ElementCompare> _elements;

            std::vector<Vector2ui> _boundary;
            std::vector<size_t> _boundary_directions;
            std::vector<Vector2ui> _boundary_polygon;

            Vector2f _centroid;
            Vector2ui _x_bounds, _y_bounds;
        };
}

#include ".src/image_region.inl"