//
// Created by clem on 25.07.21.
//

#pragma once

#include <utility>
#include <Dense>

namespace todo
{
    // forward delcaration for easier readibility by putting the non-member functions before the class definition
    template<typename, size_t>
    class Vector;

    // @brief clamps input element-wise
    // @param min: lower bounds
    // @param max: upper bounds
    // @param value: value to be clamped
    // @returns: value in [min, max]
    template<typename T, size_t n>
    Vector<T, n> clamp(Vector<T, n> min, Vector<T, n> max, Vector<T, n> value);

    // @override clamp override for scalars
    template<typename T>
    T clamp(T min, T max, T value);

    // @brief returns element-wise mixture equal to a[i] * weight + b[i] * (1-weight)
    // @param a: first vector
    // @param b: second vector,
    // @param weight: float in [0, 1] where 0 corresponds to 100% a, 0% b and vice-versa
    // @returns: Vector of element-wise mixtures
    // @note: weight is silently clamped to [0, 1] if it's value falls outside of that interval
    template<typename T, size_t n>
    Vector<T, n> mix(Vector<T, n> a, Vector<T, n> b, float weight);

    // @override mix override for scalars
    template<typename T>
    T mix(T a, T b, float weight);

    // @brief returns minimum value in set of all elements of vector
    template<typename T, size_t n>
    T min(Vector<T, n> vector);

    // @brief returns maximum value in set of all elements of vector
    template<typename T, size_t n>
    T max(Vector<T, n> vector);

    // @brief: wrapper for Eigen::Vector that offers std::vector-like style convenience
    template<typename T, size_t n>
    class Vector : public Eigen::Matrix<T, n, 1>
    {
        public:
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

            // forward declaration of internal iterator type
            struct Iterator;

            // @returns: bi-directional iterator to the first element
            Iterator begin();

            // @returns: bi-directional iterator pointing to past-the-end element
            Iterator end();

        private:
            // iterator class
            struct Iterator
            {
                public:
                    Iterator(Vector<T, n>* holder, size_t i);

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
                    Vector<T, n>* _holder;
                    size_t _i = 0;
            };
    };

    // class specialization for n = 2
    template<typename T>
    struct Vector2 : public Vector<T, 2>
    {
        Vector2(T x, T y);
    };

    // class specialization for n = 3
    template<typename T>
    struct Vector3 : public Vector<T, 3>
    {
        Vector3(T x, T y, T z);
    };

    // class specialization for n = 4
    template<typename T>
    struct Vector4 : public Vector<T, 4>
    {
        Vector4(T x, T y, T z, T w);
    };

    // typedefs for commonly used Ts
    using Vector2f = Vector2<float>;
    using Vector2i = Vector2<int>;
    using Vector2ui = Vector2<unsigned int>;
    using Vector2u8 = Vector2<uint8_t>;

    using Vector3f = Vector3<float>;
    using Vector3i = Vector3<int>;
    using Vector3ui = Vector3<unsigned int>;
    using Vector3u8 = Vector3<uint8_t>; // for representing colors

    using Vector4f = Vector4<float>;
    using Vector4i = Vector4<int>;
    using Vector4ui = Vector4<unsigned int>;
    using Vector4u8 = Vector4<uint8_t>; // for representing colors
}

#include ".src/vector.inl"
