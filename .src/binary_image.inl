// 
// Copyright 2021 Clemens Cords
// Created on 28.07.21 by clem (mail@clemens-cords.com)
//

namespace crisp
{
    inline void BinaryImage::create(long width, long height, bool init)
    {
       _size.x() = width;
        _size.y() = height;
        _value.resize(width, height);
        _value.setConstant(init);
    }

    inline void BinaryImage::create_from_file(std::string path, float cutoff = 0.5)
    {
        assert(cutoff > 0 and cutoff < 1 && "specifying a cutoff outside of (0, 1) will create a picture that is all a single color, if you want that behavior please use create(long, long)");

        sf::Image image;
        image.loadFromFile(path);

       _size.x() = image.getSize().x;
        _size.y() = image.getSize().y;
        _value.resize(image.getSize().x, image.getSize().y);

        for (long x = 0; x < image.getSize().x; ++x)
        {
            for (long y = 0; y < image.getSize().y; ++y)
            {
                auto color = image.getPixel(x, y);
                float value = (color.r / 255.f + color.g / 255.f + color.b / 255.f) / 3;
                _value(x, y) = value > cutoff;
            }
        }
    }

    inline bool & BinaryImage::get_pixel(long x, long y)
    {
        return _value(x, y);
    }

    inline bool BinaryImage::get_pixel(long x, long y) const
    {
        return _value(x, y);
    }

    inline Vector2ui BinaryImage::get_size() const
    {
        return _size;
    }

    inline BinaryImage BinaryImage::operator!() const
    {
        BinaryImage out;
        out.create(get_size().x(), get_size().y());

        for (long x = 0; x < get_size().x(); ++x)
            for (long y = 0; y < get_size().y(); ++y)
                out(x, y) = not operator()(x, y);

        return out;
    }

    inline BinaryImage BinaryImage::operator&(const BinaryImage& other) const
    {
        BinaryImage out;
        out.create(get_size().x(), get_size().y());

        for (long x = 0; x < get_size().x(); ++x)
            for (long y = 0; y < get_size().y(); ++y)
                out(x, y) = (*this)(x, y) & other(x, y);

        return out;
    }

    inline BinaryImage BinaryImage::operator|(const BinaryImage& other) const
    {
        BinaryImage out;
        out.create(get_size().x(), get_size().y());

        for (long x = 0; x < get_size().x(); ++x)
            for (long y = 0; y < get_size().y(); ++y)
                out(x, y) = (*this)(x, y) | other(x, y);

        return out;
    }

    inline BinaryImage BinaryImage::operator^(const BinaryImage& other) const
    {
        BinaryImage out;
        out.create(get_size().x(), get_size().y());

        for (long x = 0; x < get_size().x(); ++x)
            for (long y = 0; y < get_size().y(); ++y)
                out(x, y) = (*this)(x, y) ^ other(x, y);

        return out;
    }

    inline BinaryImage& BinaryImage::invert()
    {
        for (bool& b : (*this))
            b = not b;

        return *this;
    }

    inline BinaryImage& BinaryImage::operator&=(const BinaryImage& other)
    {
        for (long x = 0; x < get_size().x(); ++x)
            for (long y = 0; y < get_size().y(); ++y)
                (*this)(x, y) = (*this)(x, y) & other(x, y);

        return *this;
    }

    inline BinaryImage& BinaryImage::operator|=(const BinaryImage& other)
    {
        for (long x = 0; x < get_size().x(); ++x)
            for (long y = 0; y < get_size().y(); ++y)
                (*this)(x, y) = (*this)(x, y) | other(x, y);

        return *this;
    }

    inline BinaryImage& BinaryImage::operator^=(const BinaryImage& other)
    {
        for (long x = 0; x < get_size().x(); ++x)
            for (long y = 0; y < get_size().y(); ++y)
                (*this)(x, y) = (*this)(x, y) ^ other(x, y);

        return *this;
    }
}

