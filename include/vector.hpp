//
// Copyright 2021 Clemens Cords
// Created on 25.07.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <utility>
#include <Dense>

namespace crisp
{
    // forward delcaration
    template<typename, size_t>
    class Vector;

    // @brief clamps input element-wise
    // @param min: lower bounds
    // @param max: upper bounds
    // @param value: value to be clamped
    // @returns: value in [min, max]
    template<typename T, size_t N>
    Vector<T, N> clamp(Vector<T, N> min, Vector<T, N> max, Vector<T, N> value);

    // @override clamp override for scalars
    template<typename T>
    T clamp(T min, T max, T value);

    // @brief returns element-wise mixture equal to a[i] * weight + b[i] * (1-weight)
    // @param a: first vector
    // @param b: second vector,
    // @param weight: float in [0, 1] where 0 corresponds to 100% a, 0% b and vice-versa
    // @returns: Vector of element-wise mixtures
    // @note: weight is silently clamped to [0, 1] if it's value falls outside of that interval
    template<typename T, size_t N>
    Vector<T, N> mix(Vector<T, N> a, Vector<T, N> b, float weight);

    // @override mix override for scalars
    template<typename T>
    T mix(T a, T b, float weight);

    // @brief returns minimum value in set of all elements of vector
    template<typename T, size_t N>
    T min(Vector<T, N> vector);

    // @brief returns maximum value in set of all elements of vector
    template<typename T, size_t N>
    T max(Vector<T, N> vector);

    // @brief: wrapper for Eigen::Vector that offers std::vector-like style convenience
    template<typename T, size_t N>
    class Vector : protected Eigen::Array<T, 1, N>
    {
        using Super_t = Eigen::Array<T, 1, N>;
        
        public:
            Vector();

            // @brief: initialize the vector with an initializer list
            // @note: for n = 2, 3, 4 an additional constructor is declared that takes exactly n arguments
            Vector(std::initializer_list<T> list);

            // @brief: access element of the vector without bounds checking
            // @param i: index of the element
            // @returns: writable reference to the element
            T& operator[](size_t i);

            // @brief: access element of the vector with bounds checking
            // @param i: index of the element
            // @returns: writable reference to the element
            T& at(size_t i);

            // @brief: const access element of the vector with bounds checking
            // @param i: index of the element
            // @returns: value of the element
            T at(size_t i) const;
            
            // @brief perform element-wise addition
            // @param : another vector of the same size
            // @returns vector of values after operation
            using Super_t::operator+;

            // @brief perform element-wise subtraction
            // @param : another vector of the same size
            // @returns vector of values after operation
            using Super_t::operator-;

            // @brief perform element-wise inner product
            // @param : another vector of the same size
            // @returns vector of values after operation
            using Super_t::operator*;

            // @brief perform element-wise inner division
            // @param : another vector of the same size
            // @returns vector of values after operation
            using Super_t::operator/;

            // @brief addition assignment
            // @param : another vector of the same size
            // @returns reference to self after assignment
            using Super_t::operator+=;

            // @brief subtraction assignment
            // @param : another vector of the same size
            // @returns reference to self after assignment
            using Super_t::operator-=;

            // @brief inner product assignment
            // @param : another vector of the same size
            // @returns reference to self after assignment
            using Super_t::operator*=;

            // @brief inner division assignment
            // @param : another vector of the same size
            // @returns reference to self after assignment
            using Super_t::operator/=;

            // @brief boolean element-wise comparison
            // @param : another vector of the same size
            // @returns true if all components of both colors are equal, false otherwise
            bool operator==(const Vector<T, N>& other) const;

            // @brief boolean element-wise comparison
            // @param : another vector of the same size
            // @returns false if all components of both colors are equal, true otherwise
            bool operator!=(const Vector<T, N>& other) const;
            
            // @returns: bi-directional iterator to the first element
            auto begin();

            // @returns: bi-directional iterator pointing to past-the-end element
            auto end();
            
        protected:
            using Super_t::x;
            using Super_t::y;
            using Super_t::z;
            using Super_t::w;

        private:
            // iterator class
            struct Iterator
            {
                public:
                    Iterator(Vector<T, N>* holder, size_t i);

                    using iterator_category = std::bidirectional_iterator_tag;
                    using value_type = T;
                    using difference_type = int;
                    using pointer = T*;
                    using reference = T&;

                    bool operator==(Iterator& other) const;
                    bool operator!=(Iterator& other) const;

                    Iterator& operator++();
                    Iterator& operator--();

                    T& operator*() const;
                    explicit operator T() const;

                    void operator=(T new_value);

                private:
                    Vector<T, N>* _holder;
                    size_t _i = 0;
            };
    };

    // class specialization for n = 2
    template<typename T>
    struct Vector2 : public Vector<T, 2>
    {
        Vector2() = default;
        Vector2(T x, T y);
        
        using Vector<T, 2>::x;
        using Vector<T, 2>::y;
    };

    // class specialization for n = 3
    template<typename T>
    struct Vector3 : public Vector<T, 3>
    {
        Vector3() = default;
        Vector3(T x, T y, T z);
        
        using Vector<T, 3>::x;
        using Vector<T, 3>::y;
        using Vector<T, 3>::z;
    };

    // class specialization for n = 4
    template<typename T>
    struct Vector4 : public Vector<T, 4>
    {
        Vector4() = default;
        Vector4(T x, T y, T z, T w);
        
        using Vector<T, 4>::x;
        using Vector<T, 4>::y;
        using Vector<T, 4>::z;
        using Vector<T, 4>::w;
    };

    // typedefs for commonly used Ts
    using Vector2f = Vector2<float>;
    using Vector2i = Vector2<int>;
    using Vector2ui = Vector2<unsigned long>;
    using Vector2ui8 = Vector2<uint8_t>;

    using Vector3f = Vector3<float>;
    using Vector3i = Vector3<int>;
    using Vector3ui = Vector3<unsigned long>;
    using Vector3ui8 = Vector3<uint8_t>;
    
    using Vector4f = Vector4<float>;
    using Vector4i = Vector4<int>;
    using Vector4ui = Vector4<unsigned long>;
    using Vector4ui8 = Vector4<uint8_t>;
}

#include ".src/vector.inl"
