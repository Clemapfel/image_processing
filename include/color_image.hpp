// 
// Copyright 2021 Clemens Cords
// Created on 11.08.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <image.hpp>
#include <grayscale_image.hpp>

namespace crisp
{
    class ColorImage : public Image<Color>
    {
        public:
            ColorImage();
            
            void create(long width, long height, Color);
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
            
        private:
            sf::Vector2<long> _size;
            Eigen::Matrix<Color, Eigen::Dynamic, Eigen::Dynamic> _r, _g, _b, _a;
    };
}