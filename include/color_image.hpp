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
    // image whos pixels are color vectors
    class ColorImage : public Image<Color>
    {
        public:
            ColorImage() = default;
            
            // @brief create an image of specified size and solid color
            // @param width: the x-dimension of the image
            // @param height: the y-dimension of the image
            // @param : color all pixels will be assigned to, rgba(0, 0, 0, 1) by default
            void create(long width, long height, Color = Color(0, 0, 0)) override;
            
            // @brief create an image from a file
            // @param path: the absolute path to the file
            void create_from_file(std::string path);
            
            // @brief access all red components
            // @returns grayscale image with intensities equal to the values of the components
            GrayScaleImage get_red_plane() const;
            
            // @brief set all red components
            // @param : the grayscale image
            void set_red_plane(const GrayScaleImage&);
            
            // @brief access all green components
            // @returns grayscale image with intensities equal to the values of the components
            GrayScaleImage get_green_plane() const;

            // @brief set all green components
            // @param : the grayscale image
            void set_green_plane(const GrayScaleImage&);
            
            // @brief access all blue components
            // @returns grayscale image with intensities equal to the values of the components
            GrayScaleImage get_blue_plane() const;

            // @brief set all blue components
            // @param : the grayscale image
            void set_blue_plane(const GrayScaleImage&);

            // @brief access all hue components
            // @returns grayscale image with intensities equal to the values of the components
            GrayScaleImage get_hue_plane() const;

            // @brief set all hue components
            // @param : the grayscale image
            void set_hue_plane(const GrayScaleImage&);

            // @brief access all saturiation components
            // @returns grayscale image with intensities equal to the values of the components
            GrayScaleImage get_saturiation_plane() const;

            // @brief set all saturation components
            // @param : the grayscale image
            void set_saturation_plane(const GrayScaleImage&);

            // @brief access all hsv value components
            // @returns grayscale image with intensities equal to the values of the components
            GrayScaleImage get_value_plane() const;

            // @brief set all hsv value components
            // @param : the grayscale image
            void set_value_plane(const GrayScaleImage&);

            // @brief access all hsl lightness components
            // @returns grayscale image with intensities equal to the values of the components
            GrayScaleImage get_lightness_plane() const;

            // @brief set all hsl lightness components
            // @param : the grayscale image
            void set_lightness_plane(const GrayScaleImage&);

            // @override, c.f. Image<...>::get_size in image.hpp
            [[nodiscard]] Vector2ui get_size() const override;

        private:
            Color  get_pixel(long x, long y) const override;
            Color& get_pixel(long x, long y) override;

            Vector2ui _size;
            Eigen::Matrix<Color, Eigen::Dynamic, Eigen::Dynamic> _rgb;
    };

    void ColorImage::create(long width, long height, Color color)
    {
        _rgb.resize(width, height);
        _rgb.setConstant(color);
    }

    void ColorImage::create_from_file(std::string path)
    {
        sf::Image image;
        image.loadFromFile(path);

       _size.x() = image.getSize().x;
        _size.y() = image.getSize().y;
        _rgb.resize(image.getSize().x, image.getSize().y);

        for (long x = 0; x < image.getSize().x; ++x)
        {
            for (long y = 0; y < image.getSize().y; ++y)
            {
                auto color = image.getPixel(x, y);
                _rgb(x, y) = Color(color.r / 255.f, color.g / 255.f, color.b / 255.f);
            }
        }
    }

    Color & ColorImage::get_pixel(long x, long y)
    {
        return _rgb(x, y);
    }

    Color ColorImage::get_pixel(long x, long y) const
    {
        return _rgb(x, y);
    }

    Vector2ui ColorImage::get_size() const
    {
        return Vector2ui{static_cast<unsigned long>(_rgb.rows()), static_cast<unsigned long>(_rgb.cols())};
    }

    GrayScaleImage ColorImage::get_red_plane() const
    {
        auto out = GrayScaleImage();
        out.create(_rgb.rows(), _rgb.cols());

        for (long x = 0; x < _rgb.rows(); ++x)
            for (long y = 0; y < _rgb.cols(); ++y)
                out(x, y) = _rgb(x, y).red();

        return out;
    }

    void ColorImage::set_red_plane(const GrayScaleImage& plane)
    {
        for (long x = 0; x < _rgb.rows(); ++x)
            for (long y = 0; y < _rgb.cols(); ++y)
                _rgb(x, y).red() = plane(x, y);
    }

    GrayScaleImage ColorImage::get_blue_plane() const
    {
        auto out = GrayScaleImage();
        out.create(_rgb.rows(), _rgb.cols());

        for (long x = 0; x < _rgb.rows(); ++x)
            for (long y = 0; y < _rgb.cols(); ++y)
                out(x, y) = _rgb(x, y).blue();

        return out;
    }

    void ColorImage::set_blue_plane(const GrayScaleImage& plane)
    {
        for (long x = 0; x < _rgb.rows(); ++x)
            for (long y = 0; y < _rgb.cols(); ++y)
                _rgb(x, y).blue() = plane(x, y);
    }

    GrayScaleImage ColorImage::get_green_plane() const
    {
        auto out = GrayScaleImage();
        out.create(_rgb.rows(), _rgb.cols());

        for (long x = 0; x < _rgb.rows(); ++x)
            for (long y = 0; y < _rgb.cols(); ++y)
                out(x, y) = _rgb(x, y).green();

        return out;
    }

    void ColorImage::set_green_plane(const GrayScaleImage& plane)
    {
        for (long x = 0; x < _rgb.rows(); ++x)
            for (long y = 0; y < _rgb.cols(); ++y)
                _rgb(x, y).green() = plane(x, y);
    }

    GrayScaleImage ColorImage::get_hue_plane() const
    {
        auto out = GrayScaleImage();
        out.create(_rgb.rows(), _rgb.cols());

        for (long x = 0; x < _rgb.rows(); ++x)
            for (long y = 0; y < _rgb.cols(); ++y)
                out(x, y) = _rgb(x, y).as_hsv().h;

        return out;
    }

    void ColorImage::set_hue_plane(const GrayScaleImage& plane)
    {
        for (long x = 0; x < _rgb.rows(); ++x)
        {
            for (long y = 0; y < _rgb.cols(); ++y)
            {
                auto as_hsv = _rgb(x, y).as_hsv();
                as_hsv.h = plane(x, y);
                _rgb(x, y) = Color(as_hsv);
            }
        }
    }

    GrayScaleImage ColorImage::get_saturiation_plane() const
    {
        auto out = GrayScaleImage();
        out.create(_rgb.rows(), _rgb.cols());

        for (long x = 0; x < _rgb.rows(); ++x)
            for (long y = 0; y < _rgb.cols(); ++y)
                out(x, y) = _rgb(x, y).as_hsv().s;

        return out;
    }

    void ColorImage::set_saturation_plane(const GrayScaleImage& plane)
    {
        for (long x = 0; x < _rgb.rows(); ++x)
        {
            for (long y = 0; y < _rgb.cols(); ++y)
            {
                auto as_hsv = _rgb(x, y).as_hsv();
                as_hsv.s = plane(x, y);
                _rgb(x, y) = Color(as_hsv);
            }
        }
    }

    GrayScaleImage ColorImage::get_value_plane() const
    {
        auto out = GrayScaleImage();
        out.create(_rgb.rows(), _rgb.cols());

        for (long x = 0; x < _rgb.rows(); ++x)
            for (long y = 0; y < _rgb.cols(); ++y)
                out(x, y) = _rgb(x, y).as_hsv().v;

        return out;
    }

    void ColorImage::set_value_plane(const GrayScaleImage& plane)
    {
        for (long x = 0; x < _rgb.rows(); ++x)
        {
            for (long y = 0; y < _rgb.cols(); ++y)
            {
                auto as_hsv = _rgb(x, y).as_hsv();
                as_hsv.v = plane(x, y);
                _rgb(x, y) = Color(as_hsv);
            }
        }
    }

    GrayScaleImage ColorImage::get_lightness_plane() const
    {
        auto out = GrayScaleImage();
        out.create(_rgb.rows(), _rgb.cols());

        for (long x = 0; x < _rgb.rows(); ++x)
            for (long y = 0; y < _rgb.cols(); ++y)
                out(x, y) = _rgb(x, y).as_hsl().l;

        return out;
    }

    void ColorImage::set_lightness_plane(const GrayScaleImage& plane)
    {
        for (long x = 0; x < _rgb.rows(); ++x)
        {
            for (long y = 0; y < _rgb.cols(); ++y)
            {
                auto as_hsl = _rgb(x, y).as_hsl();
                as_hsl.l = plane(x, y);
                _rgb(x, y) = Color(as_hsl);
            }
        }
    }

}