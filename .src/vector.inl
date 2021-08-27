//
// Copyright 2021 Clemens Cords
// Created on 25.07.21 by clem (mail@clemens-cords.com)
//

#include <vector.hpp>

namespace crisp
{

    // ### Vector ##########################################################
    template<typename T, size_t N>
    Vector<T, N>::Iterator::Iterator(Vector<T, N>* holder, size_t i)
        : _holder(holder), _i(i)
    {}

    template<typename T, size_t N>
    bool Vector<T, N>::Iterator::operator==(Vector<T, N>::Iterator& other) const
    {
        return _i == other._i;
    }

    template<typename T, size_t N>
    bool Vector<T, N>::Iterator::operator!=(Vector<T, N>::Iterator& other) const
    {
        return _i != other._i;
    }

    template<typename T, size_t N>
    typename Vector<T, N>::Iterator& Vector<T, N>::Iterator::operator++()
    {
        if (_i < N)
            _i++;

        return *this;
    }

    template<typename T, size_t N>
    typename Vector<T, N>::Iterator& Vector<T, N>::Iterator::operator--()
    {
        if (_i > 0)
            _i--;

        return *this;
    }

    template<typename T, size_t N>
    void Vector<T, N>::Iterator::operator=(T new_value)
    {
        _holder->at(_i) = new_value;
    }

    template<typename T, size_t N>
    T& Vector<T, N>::Iterator::operator*() const
    {
        return _holder->at(_i);
    }

    template<typename T, size_t N>
    Vector<T, N>::Iterator::operator T() const
    {
        return _holder->at(_i);
    }

    // ### Vector ##########################################################
    template<typename T, size_t N>
    Vector<T, N>::Vector()
        : Eigen::Array<T, 1, N>()
    {
        Eigen::Array<T, 1, N>::setConstant(0);
    }

    template<typename T, size_t N>
    Vector<T, N>::Vector(std::initializer_list<T> list)
    {
        assert(list.size() == N);

        size_t i = 0;
        for (auto it = list.begin(); i < N and it != list.end(); it += 1, ++i)
            Eigen::Array<T, 1, N>::operator()(0, i) = *it;
    }

    template<typename T, size_t N>
    T & Vector<T, N>::operator[](size_t i)
    {
        return Eigen::Array<T, 1, N>::operator()(0, i);
    }

    template<typename T, size_t N>
    T & Vector<T, N>::at(size_t i)
    {
        if (i >= N)
            throw std::out_of_range("Index " + std::to_string(i) + " out of range for vector of size " + std::to_string(N));

        return Eigen::Array<T, 1, N>::operator()(0, i);;
    }

    template<typename T, size_t N>
    T Vector<T, N>::at(size_t i) const
    {
        if (i >= N)
            throw std::out_of_range("Index " + std::to_string(i) + " out of range for vector of size " + std::to_string(N));

        return Eigen::Array<T, 1, N>::operator()(0, i);;
    }

    template<typename T, size_t N>
    auto Vector<T, N>::begin()
    {
        return Vector<T, N>::Iterator(this, 0);
    }

    template<typename T, size_t N>
    auto Vector<T, N>::end()
    {
        return Vector<T, N>::Iterator(this, N);
    }

    template<typename T, size_t N>
    Vector<T, N> Vector<T, N>::operator+(const Vector<T, N>& other) const
    {
        auto out = *this;
        out += other;
        return out;
    }

    template<typename T, size_t N>
    Vector<T, N> Vector<T, N>::operator-(const Vector<T, N>& other) const
    {
        auto out = *this;
        out *= other;
        return out;
    }

    template<typename T, size_t N>
    Vector<T, N> Vector<T, N>::operator+(T scalar) const
    {
        auto out = *this;
        for (size_t i = 0; i < N; ++i)
            out.at(i) += scalar;
        return out;
    }

    template<typename T, size_t N>
    Vector<T, N> Vector<T, N>::operator-(T scalar) const
    {
        auto out = *this;
        for (size_t i = 0; i < N; ++i)
            out.at(i) -= scalar;
        return out;
    }

    template<typename T, size_t N>
    Vector<T, N> Vector<T, N>::operator*(T scalar) const
    {
        auto out = *this;
        for (size_t i = 0; i < N; ++i)
            out.at(i) *= scalar;
        return out;
    }

    template<typename T, size_t N>
    Vector<T, N> Vector<T, N>::operator/(T scalar) const
    {
        auto out = *this;
        for (size_t i = 0; i < N; ++i)
            out.at(i) /= scalar;
        return out;
    }

    template<typename T, size_t N>
    Vector<T, N>& Vector<T, N>::operator+=(T scalar)
    {
        for (size_t i = 0; i < N; ++i)
            this->at(i) += scalar;
        return *this;
    }

    template<typename T, size_t N>
    Vector<T, N>& Vector<T, N>::operator-=(T scalar)
    {
        for (size_t i = 0; i < N; ++i)
            this->at(i) -= scalar;
        return *this;
    }

    template<typename T, size_t N>
    bool Vector<T, N>::operator==(const Vector<T, N>& other) const
    {
        for (size_t i = 0; i < N; ++i)
            if (this->at(i) != other.at(i))
                return false;

        return true;
    }

    template<typename T, size_t N>
    bool Vector<T, N>::operator!=(const Vector<T, N>& other) const
    {
        for (size_t i = 0; i < N; ++i)
            if (this->at(i) == other.at(i))
                return false;

        return true;
    }

    template<typename T>
    Vector2<T>::Vector2(T x, T y)
        : Vector<T, 2>({x, y})
    {}

    template<typename T>
    Vector2<T>::Vector2(Vector<T, 2> super)
        : Vector<T, 2>(super)
    {}

    template<typename T>
    Vector3<T>::Vector3(T x, T y, T z)
        : Vector<T, 3>({x, y, z})
    {}

    template<typename T>
    Vector3<T>::Vector3(Vector<T, 3> super)
        : Vector<T, 3>(super)
    {}

    template<typename T>
    Vector4<T>::Vector4(T x, T y, T z, T w)
        : Vector<T, 4>({x, y, z, w})
    {}

    template<typename T>
    Vector4<T>::Vector4(Vector<T, 4> super)
        : Vector<T, 4>(super)
    {}

    template<typename T, size_t N>
    Vector<T, N> clamp(Vector<T, N> min, Vector<T, N> max, Vector<T, N> value)
    {
        for (size_t i = 0; i < N; ++i)
        {
            if (value.at(i) < min.at(i))
                value[i] = min.at(i);
            else if (value.at(i) > max.at(i))
                value[i] = max.at(i);
        }

        return value;
    }

    template<typename T>
    T clamp(T min, T max, T value)
    {
        if (value < min)
            value = min;
        else if (value > max)
            value = max;

        return value;
    }

    template<typename T, size_t N>
    T min(Vector<T, N> vector)
    {
        T current_min = std::numeric_limits<T>::max();

        for (size_t i = 0; i < N; ++i)
            current_min = std::min(vector[i], current_min);

        return current_min;
    }

    template<typename T, size_t N>
    T max(Vector<T, N> vector)
    {
        T current_max = std::numeric_limits<T>::min();

        for (size_t i = 0; i < N; ++i)
            current_max = std::max(vector[i], current_max);

        return current_max;
    }
    
    template<typename T, size_t N>
    double dist(const Vector<T, N>& a, const Vector<T, N>& b)
    {
        double sum = T(0);

        for (size_t i = 0; i < N; ++i)
            sum += double((a.at(i) - b.at(i)) * (a.at(i) - b.at(i)));

        return sqrt(sum);
    }
}