// 
// Copyright 2021 Clemens Cords
// Created on 11.08.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <SFML/Graphics/Image.hpp>

#include <image.hpp>
#include <grayscale_image.hpp>


namespace crisp
{
    class ColorImage : public Image<Color>
    {
        public:
            ColorImage() = default;
            
            void create(long width, long height, Color = Color(0, 0, 0, 1)) override;
            void create_from_file(std::string path);
            
            GrayScaleImage get_red_plane() const;
            void set_red_plane(const GrayScaleImage&);
            
            GrayScaleImage get_green_plane() const;
            void set_green_plane(const GrayScaleImage&);
            
            GrayScaleImage get_blue_plane() const;
            void set_blue_plane(const GrayScaleImage&);

            GrayScaleImage get_hue_plane() const;
            void set_hue_plane(const GrayScaleImage&);

            GrayScaleImage get_saturiation_plane() const;
            void set_saturation_plane(const GrayScaleImage&);

            GrayScaleImage get_value_plane() const;
            void set_value_plane(const GrayScaleImage&);

            GrayScaleImage get_lightness_plane() const;
            void set_lightness_plane(const GrayScaleImage&);

            GrayScaleImage get_alpha_plane() const;
            void set_alpha_plane(const GrayScaleImage&);

            [[nodiscard]] sf::Vector2<long> get_size() const override;

        protected:
            Color  get_pixel(long x, long y) const override;
            Color& get_pixel(long x, long y) override;

        private:
            sf::Vector2<long> _size;
            Eigen::Matrix<Color, Eigen::Dynamic, Eigen::Dynamic> _rgba;
    };

    void ColorImage::create(long width, long height, Color color)
    {
        _rgba.resize(width, height);
        _rgba.setConstant(color);
    }

    void ColorImage::create_from_file(std::string path)
    {
        sf::Image image;
        image.loadFromFile(path);

        _size.x = image.getSize().x;
        _size.y = image.getSize().y;
        _rgba.resize(image.getSize().x, image.getSize().y);

        for (long x = 0; x < image.getSize().x; ++x)
        {
            for (long y = 0; y < image.getSize().y; ++y)
            {
                auto color = image.getPixel(x, y);
                _rgba(x, y) = Color(color.r / 255.f, color.g / 255.f, color.b / 255.f, color.a / 255.f);
            }
        }
    }

    Color & ColorImage::get_pixel(long x, long y)
    {
        return _rgba(x, y);
    }

    Color ColorImage::get_pixel(long x, long y) const
    {
        return _rgba(x, y);
    }

    sf::Vector2<long> ColorImage::get_size() const
    {
        return {_rgba.rows(), _rgba.cols()};
    }

    GrayScaleImage ColorImage::get_red_plane() const
    {
        auto out = GrayScaleImage();
        out.create(_rgba.rows(), _rgba.cols());

        for (long x = 0; x < _rgba.rows(); ++x)
            for (long y = 0; y < _rgba.cols(); ++y)
                out(x, y) = _rgba(x, y).red();

        return out;
    }

    void ColorImage::set_red_plane(const GrayScaleImage& plane)
    {
        for (long x = 0; x < _rgba.rows(); ++x)
            for (long y = 0; y < _rgba.cols(); ++y)
                _rgba(x, y).red() = plane(x, y);
    }

    GrayScaleImage ColorImage::get_blue_plane() const
    {
        auto out = GrayScaleImage();
        out.create(_rgba.rows(), _rgba.cols());

        for (long x = 0; x < _rgba.rows(); ++x)
            for (long y = 0; y < _rgba.cols(); ++y)
                out(x, y) = _rgba(x, y).blue();

        return out;
    }

    void ColorImage::set_blue_plane(const GrayScaleImage& plane)
    {
        for (long x = 0; x < _rgba.rows(); ++x)
            for (long y = 0; y < _rgba.cols(); ++y)
                _rgba(x, y).blue() = plane(x, y);
    }

    GrayScaleImage ColorImage::get_green_plane() const
    {
        auto out = GrayScaleImage();
        out.create(_rgba.rows(), _rgba.cols());

        for (long x = 0; x < _rgba.rows(); ++x)
            for (long y = 0; y < _rgba.cols(); ++y)
                out(x, y) = _rgba(x, y).green();

        return out;
    }

    void ColorImage::set_green_plane(const GrayScaleImage& plane)
    {
        for (long x = 0; x < _rgba.rows(); ++x)
            for (long y = 0; y < _rgba.cols(); ++y)
                _rgba(x, y).green() = plane(x, y);
    }

    GrayScaleImage ColorImage::get_alpha_plane() const
    {
        auto out = GrayScaleImage();
        out.create(_rgba.rows(), _rgba.cols());

        for (long x = 0; x < _rgba.rows(); ++x)
            for (long y = 0; y < _rgba.cols(); ++y)
                out(x, y) = _rgba(x, y).alpha();

        return out;
    }

    void ColorImage::set_alpha_plane(const GrayScaleImage& plane)
    {
        for (long x = 0; x < _rgba.rows(); ++x)
            for (long y = 0; y < _rgba.cols(); ++y)
                _rgba(x, y).alpha() = plane(x, y);
    }

    GrayScaleImage ColorImage::get_hue_plane() const
    {
        auto out = GrayScaleImage();
        out.create(_rgba.rows(), _rgba.cols());

        for (long x = 0; x < _rgba.rows(); ++x)
            for (long y = 0; y < _rgba.cols(); ++y)
                out(x, y) = _rgba(x, y).as_hsv().h;

        return out;
    }

    void ColorImage::set_hue_plane(const GrayScaleImage& plane)
    {
        for (long x = 0; x < _rgba.rows(); ++x)
        {
            for (long y = 0; y < _rgba.cols(); ++y)
            {
                auto as_hsv = _rgba(x, y).as_hsv();
                as_hsv.h = plane(x, y);
                _rgba(x, y) = Color(as_hsv);
            }
        }
    }

    GrayScaleImage ColorImage::get_saturiation_plane() const
    {
        auto out = GrayScaleImage();
        out.create(_rgba.rows(), _rgba.cols());

        for (long x = 0; x < _rgba.rows(); ++x)
            for (long y = 0; y < _rgba.cols(); ++y)
                out(x, y) = _rgba(x, y).as_hsv().s;

        return out;
    }

    void ColorImage::set_saturation_plane(const GrayScaleImage& plane)
    {
        for (long x = 0; x < _rgba.rows(); ++x)
        {
            for (long y = 0; y < _rgba.cols(); ++y)
            {
                auto as_hsv = _rgba(x, y).as_hsv();
                as_hsv.s = plane(x, y);
                _rgba(x, y) = Color(as_hsv);
            }
        }
    }

    GrayScaleImage ColorImage::get_value_plane() const
    {
        auto out = GrayScaleImage();
        out.create(_rgba.rows(), _rgba.cols());

        for (long x = 0; x < _rgba.rows(); ++x)
            for (long y = 0; y < _rgba.cols(); ++y)
                out(x, y) = _rgba(x, y).as_hsv().v;

        return out;
    }

    void ColorImage::set_value_plane(const GrayScaleImage& plane)
    {
        for (long x = 0; x < _rgba.rows(); ++x)
        {
            for (long y = 0; y < _rgba.cols(); ++y)
            {
                auto as_hsv = _rgba(x, y).as_hsv();
                as_hsv.v = plane(x, y);
                _rgba(x, y) = Color(as_hsv);
            }
        }
    }

    GrayScaleImage ColorImage::get_lightness_plane() const
    {
        auto out = GrayScaleImage();
        out.create(_rgba.rows(), _rgba.cols());

        for (long x = 0; x < _rgba.rows(); ++x)
            for (long y = 0; y < _rgba.cols(); ++y)
                out(x, y) = _rgba(x, y).as_hsl().l;

        return out;
    }

    void ColorImage::set_lightness_plane(const GrayScaleImage& plane)
    {
        for (long x = 0; x < _rgba.rows(); ++x)
        {
            for (long y = 0; y < _rgba.cols(); ++y)
            {
                auto as_hsl = _rgba(x, y).as_hsl();
                as_hsl.l = plane(x, y);
                _rgba(x, y) = Color(as_hsl);
            }
        }
    }

}