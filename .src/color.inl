//
// Copyright 2021 Clemens Cords
// Created on 25.07.21 by clem (mail@clemens-cords.com)
//

#include <cassert>
#include <iostream>

namespace crisp
{
    // mix
    template<typename Color_t>
    inline Color_t mix(Color_t a, Color_t b, float weight)
    {
        static_assert(true, "mix template parameter is not a valid color representation");
    }

    template<>
    inline Color mix(Color a, Color b, float weight)
    {
         weight = clamp<float>(0, 1, weight);

         float af = 1 - weight;
         float bf = weight;

         Color out;
         out.red() = a.red() * af + b.red() * bf;
         out.green() = a.green() * af + b.green() * bf;
         out.blue() = a.blue() * af + b.green() * bf;

         return out;
    }

    template<>
    inline RGB mix(RGB a, RGB b, float weight)
    {
         weight = clamp<float>(0, 1, weight);

         float af = 1 - weight;
         float bf = weight;

         RGB out;
         out.r = a.r * af + b.r * bf;
         out.g = a.g * af + b.g * bf;
         out.b = a.b * af + b.g * bf;

         return out;
    }

    template<>
    inline HSV mix(HSV a, HSV b, float weight)
    {
         weight = clamp<float>(0, 1, weight);

         float af = 1 - weight;
         float bf = weight;

         HSV out;
         out.h = a.h * af + b.h * bf;
         out.s = a.s * af + b.s * bf;
         out.v = a.v * af + b.v * bf;

         return out;
    }

    template<>
    inline HSL mix(HSL a, HSL b, float weight)
    {
         weight = clamp<float>(0, 1, weight);

         float af = 1 - weight;
         float bf = weight;

         HSL out;
         out.h = a.h * af + b.h * bf;
         out.s = a.s * af + b.s * bf;
         out.l = a.l * af + b.l * bf;

         return out;
    }

    template<>
    inline GrayScale mix(GrayScale a, GrayScale b, float weight)
    {
         weight = clamp<float>(0, 1, weight);

         float af = 1 - weight;
         float bf = weight;

         GrayScale out;
         out.v = a.v * af + b.v * bf;

         return out;
    }

    // convert to
    template<typename T1, typename T2>
    T2 convert_to(T1 from)
    {
        // will only be called with incorrect use
        static_assert(true, "convert_to template parameter and/or argument is not a valid color representation");
    }

    template<typename T>
    inline T convert_to(T from)
    {
        return from;
    }

    template<typename T>
    T convert_to(T from);

    template<typename T>
    T convert_to(Color from);

    template<typename T>
    T convert_to(RGB from);

    template<typename T>
    T convert_to(HSV from);

    template<typename T>
    T convert_to(HSL from);

    template<typename T>
    T convert_to(GrayScale from);

    // forward declarations
    template<>
    HSV convert_to(RGB);
    template<>
    HSL convert_to(RGB);

    template<>
    RGB convert_to(HSV);
    template<>
    HSL convert_to(HSV);

    template<>
    RGB convert_to(HSL);
    template<>
    HSV convert_to(HSL);

    // from Color
    template<>
    inline HSV convert_to(Color from)
    {
        auto as_rgb = RGB{from.red(), from.green(), from.blue()};
        return convert_to<HSV>(as_rgb);
    }

    template<>
    inline HSL convert_to(Color from)
    {
        auto as_rgb = RGB{from.red(), from.green(), from.blue()};
        return convert_to<HSL>(as_rgb);
    }

    template<>
    inline RGB convert_to(Color from)
    {
        return RGB{from.red(), from.green(), from.blue()};
    }

    template<>
    inline GrayScale convert_to(Color from)
    {
        float average = (from.red() + from.green() + from.blue()) / 3;
        return GrayScale{average};
    }

    // from RGB
    template<>
    inline HSV convert_to(RGB from)
    {
        float r = from.r,
              g = from.g,
              b = from.b;

        float h, s, v;

        auto min = from.r < from.g ? from.r : from.g;
        min = min  < from.b ? min  : from.b;

        auto max = from.r > from.g ? from.r : from.g;
        max = max  > from.b ? max  : from.b;

        auto delta = max - min;

        if (delta > 0)
        {
            if (max == r)
                h = 60 * (fmod(((g - b) / delta), 6));

            else if (max == g)
                h = 60 * (((b - r) / delta) + 2);

            else if (max == b)
                h = 60 * (((r - g) / delta) + 4);

            if (max > 0)
                s = delta / max;
            else
                s = 0;

            v = max;
        }
        else
        {
            h = 0;
            s = 0;
            v = max;
        }

        if (h < 0)
            h += 360;

        HSV out;

        out.h = h / 360.f;
        out.s = s;
        out.v = v;

        return out;
    }

    template<>
    inline HSL convert_to(RGB from)
    {
        auto as_hsv = convert_to<HSV>(from);
        return convert_to<HSL>(as_hsv);
    }

    template<>
    inline Color convert_to(RGB from)
    {
        return Color(from.r, from.g, from.b);
    }

    template<>
    inline GrayScale convert_to(RGB from)
    {
        float average = (from.r + from.g + from.b) / 3;
        return GrayScale{average};
    }

    // from HSV
    template<>
    inline RGB convert_to(HSV from)
    {
        from.h *= 360;
        float c = from.v * from.s;
        float h_2 = from.h / 60;
        float x = c * (1 - std::fabs(std::fmod(h_2, 2) - 1));

        RGB out;

        if (0 <= h_2 and h_2 < 1)
        {
            out = RGB{c, x, 0};
        }
        else if (1 <= h_2 and h_2 < 2)
        {
            out = RGB{x, c, 0};
        }
        else if (2 <= h_2 and h_2 < 3)
        {
            out = RGB{0, c, x};
        }
        else if (3 <= h_2 and h_2 < 4)
        {
            out = RGB{0, x, c};
        }
        else if (4 <= h_2 and h_2 < 5)
        {
            out = RGB{x, 0, c};
        }
        else if (5 <= h_2 and h_2 <= 6)
        {
            out = RGB{c, 0, x};
        }

        auto m = from.v - c;
        out.r += m;
        out.g += m;
        out.b += m;

        return out;
    }

    template<>
    inline HSL convert_to(HSV from)
    {
        float hsv_s = from.s,
              hsv_v = from.v;

        float hsl_l = hsv_v * (1 - hsv_s / 2),
              hsl_s;

        if (hsl_l == 0 or hsl_l == 1)
            hsl_s = 0;
        else
            hsv_s = (hsv_v - hsl_l) / std::min(hsl_l, 1.f - hsl_l);

        HSL out;
        out.h = from.h;
        out.s = hsl_s;
        out.l = hsl_l;

        return out;
    }

    template<>
    inline Color convert_to(HSV from)
    {
        auto as_rgb = convert_to<RGB>(from);
        return Color(as_rgb.r, as_rgb.g, as_rgb.b);
    }

    template<>
    inline GrayScale convert_to(HSV from)
    {
        return GrayScale{from.v};
    }

    // from HSL
    template<>
    inline Color convert_to(HSL from)
    {
        auto as_hsv = convert_to<HSV>(from);
        auto as_rgb = convert_to<RGB>(as_hsv);
        return Color(as_rgb.r, as_rgb.g, as_rgb.b);
    }

    template<>
    inline RGB convert_to(HSL from)
    {
        auto as_hsv = convert_to<HSV>(from);
        return convert_to<RGB>(as_hsv);
    }

    template<>
    inline HSV convert_to(HSL from)
    {
        //s*=l<.5?l:1-l;return[h,2*s/(l+s),l+s]

        from.s *= from.l < 0.5 ? from.l : 1 - from.l;

        HSV out;
        out.h = from.h;
        out.s = 2 * from.s / (from.l + from.s);
        out.v = from.l + from.s;

        return out;
    }

    template<>
    inline GrayScale convert_to(HSL from)
    {
        return GrayScale{from.l};
    }

    // from GreyScale
    template<>
    inline Color convert_to(GrayScale from)
    {
        return Color(from.v, from.v, from.v);
    }

    template<>
    inline RGB convert_to(GrayScale from)
    {
        return RGB{from.v, from.v, from.v};
    }

    template<>
    inline HSV convert_to(GrayScale from)
    {
        return HSV{0, 0, from.v};
    }

    template<>
    inline HSL convert_to(GrayScale from)
    {
        return HSL{0, 0, from.v};
    }

    inline Color::Color()
        : Vector3f(0, 0, 0)
    {}

    inline Color::Color(float value)
        : Vector3f(value, value, value)
    {}

    inline Color::Color(float red, float green, float blue)
        : Vector3f(red, green, blue)
    {}

    inline Color::Color(RGB rgb)
        : Vector3f(rgb.r, rgb.g, rgb.b)
    {}

    inline Color::Color(HSV hsv)
        : Color()
    {
        auto asrgb = convert_to<RGB>(hsv);
        red() = asrgb.r;
        green() = asrgb.g;
        blue() = asrgb.b;
    }

    inline Color::Color(HSL hsl)
        : Color()
    {
        auto asrgb = convert_to<RGB>(hsl);
        red() = asrgb.r;
        green() = asrgb.g;
        blue() = asrgb.b;
    }

    inline Color::Color(GrayScale grayscale)
        : Color()
    {
        red() = grayscale.v;
        green() = grayscale.v;
        blue() = grayscale.v;
    }

    float & Color::red()
    {
        return operator()(0, 0);
    }

    float Color::red() const
    {
        return operator()(0, 0);
    }

    float & Color::green()
    {
        return operator()(0, 1);
    }

    float Color::green() const
    {
        return operator()(0, 1);
    }

    float & Color::blue()
    {
        return operator()(0, 2);
    }

    float Color::blue() const
    {
        return operator()(0, 2);
    }

    inline RGB Color::as_rgb() const
    {
        return convert_to<RGB>(*this);
    }

    inline HSV Color::as_hsv() const
    {
        return convert_to<HSV>(*this);
    }

    inline HSL Color::as_hsl() const
    {
        return convert_to<HSL>(*this);
    }

    inline GrayScale Color::as_grayscale() const
    {
        return convert_to<GrayScale>(*this);
    }

    inline Color & Color::operator=(RGB rgb)
    {
        red() = rgb.r;
        green() = rgb.g;
        blue() = rgb.b;
        return *this;
    }

    inline Color & Color::operator=(HSV hsv)
    {
        auto asrgb = convert_to<RGB>(hsv);
        red() = asrgb.r;
        green() = asrgb.g;
        blue() = asrgb.b;
        return *this;
    }

    inline Color & Color::operator=(HSL hsl)
    {
        auto asrgb = convert_to<RGB>(hsl);
        red() = asrgb.r;
        green() = asrgb.g;
        blue() = asrgb.b;
        return *this;
    }

    inline Color & Color::operator=(GrayScale grayscale)
    {
        red() = grayscale.v;
        green() = grayscale.v;
        blue() = grayscale.v;
        return *this;
    }

    inline Color Color::invert() const
    {
        Color out;
        out.red() = 1 - red();
        out.green() = 1 - green();
        out.blue() = 1 - blue();
        return out;
    }
}