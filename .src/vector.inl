//
// Created by clem on 25.07.21.
//

namespace todo
{

    // ### Vector ##########################################################

    template<typename T, size_t n>
    Vector<T, n>::Iterator::Iterator(Vector<T, n>* holder, size_t i)
        : _holder(holder), _i(i)
    {}

    template<typename T, size_t n>
    bool Vector<T, n>::Iterator::operator==(Vector<T, n>::Iterator& other) const
    {
        return _i == other._i;
    }

    template<typename T, size_t n>
    bool Vector<T, n>::Iterator::operator!=(Vector<T, n>::Iterator& other) const
    {
        return _i != other._i;
    }

    template<typename T, size_t n>
    typename Vector<T, n>::Iterator& Vector<T, n>::Iterator::operator++()
    {
        if (_i < n)
            _i++;

        return *this;
    }

    template<typename T, size_t n>
    typename Vector<T, n>::Iterator& Vector<T, n>::Iterator::operator--()
    {
        if (_i > 0)
            _i--;

        return *this;
    }

    template<typename T, size_t n>
    void Vector<T, n>::Iterator::operator=(T new_value)
    {
        _holder->at(_i) = new_value;
    }

    template<typename T, size_t n>
    T& Vector<T, n>::Iterator::operator*() const
    {
        return _holder->at(_i);
    }

    template<typename T, size_t n>
    Vector<T, n>::Iterator::operator T() const
    {
        return _holder->at(_i);
    }

    // ### Vector ##########################################################
    template<typename T, size_t n>
    Vector<T, n>::Vector(std::initializer_list<T> list)
    {
        assert(list.size() == n);

        size_t i = 0;
        for (auto it = list.begin(); i < n and it != list.end(); it += 1, ++i)
            Eigen::Matrix<T, n, 1>::operator()(0) = *it;
    }

    template<typename T, size_t n>
    T & Vector<T, n>::operator[](size_t i)
    {
        return Eigen::Matrix<T, n, 1>::operator()(i, 0);
    }

    template<typename T, size_t n>
    T & Vector<T, n>::at(size_t i)
    {
        if (i >= n)
            throw std::out_of_range("Index " + std::to_string(i) + " out of range for vector of size " + std::to_string(n));

        return Eigen::Matrix<T, n, 1>::operator()(i, 0);;
    }

    template<typename T, size_t n>
    typename Vector<T, n>::Iterator Vector<T, n>::begin()
    {
        return Vector<T, n>::Iterator(this, 0);
    }

    template<typename T, size_t n>
    typename Vector<T, n>::Iterator Vector<T, n>::end()
    {
        return Vector<T, n>::Iterator(this, n);
    }

    template<typename T>
    Vector2<T>::Vector2(T x, T y)
        : Vector<T, 2>({x, y})
    {}

    template<typename T>
    Vector3<T>::Vector3(T x, T y, T z)
        : Vector<T, 3>({x, y, z})
    {}

    template<typename T>
    Vector4<T>::Vector4(T x, T y, T z, T w)
        : Vector<T, 4>({x, y, z, w})
    {}

    template<typename T, size_t n>
    Vector<T, n> clamp(Vector<T, n> min, Vector<T, n> max, Vector<T, n> value)
    {
        for (size_t i = 0; i < n; ++i)
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

    template<typename T, size_t n>
    T min(Vector<T, n> vector)
    {
        T current_min = std::numeric_limits<T>::max();

        for (size_t i = 0; i < n; ++i)
            current_min = std::min(vector[i], current_min);

        return current_min;
    }

    template<typename T, size_t n>
    T max(Vector<T, n> vector)
    {
        T current_max = std::numeric_limits<T>::min();

        for (size_t i = 0; i < n; ++i)
            current_max = std::max(vector[i], current_max);

        return current_max;
    }
}