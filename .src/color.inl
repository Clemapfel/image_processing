//
// Copyright 2021 Clemens Cords
// Created on 25.07.21 by clem (mail@clemens-cords.com)
//

#include <cassert>

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
         out.alpha() = a.alpha() * af + b.alpha() * bf;

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
         out.a = a.a * af + b.a * bf;

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
         out.a = a.a * af + b.a * bf;

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
         out.a = a.a * af + b.a * bf;

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
         out.a = a.a * af + b.a * bf;

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
        auto as_rgb = RGB{from.red(), from.green(), from.blue(), from.alpha()};
        return convert_to<HSV>(as_rgb);
    }

    template<>
    inline HSL convert_to(Color from)
    {
        auto as_rgb = RGB{from.red(), from.green(), from.blue(), from.alpha()};
        return convert_to<HSL>(as_rgb);
    }

    template<>
    inline RGB convert_to(Color from)
    {
        return RGB{from.red(), from.green(), from.blue(), from.alpha()};
    }

    template<>
    inline GrayScale convert_to(Color from)
    {
        float average = (from.red() + from.green() + from.blue()) / 3;
        return GrayScale{average, from.alpha()};
    }

    // from RGB
    template<>
    inline HSV convert_to(RGB from)
    {
        float r = from.r,
              g = from.g,
              b = from.b;

        float h, s, v;
        float max = fmax(fmax(r, g), b);
        float min = fmin(fmin(r, g), b);
        float delta = max - min;

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
            h = 360 + h;

        HSV out;
        out.h = h;
        out.s = s;
        out.v = v;
        out.a = from.a;

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
        return Color(from.r, from.g, from.b, from.a);
    }

    template<>
    inline GrayScale convert_to(RGB from)
    {
        float average = (from.r + from.g + from.b) / 3;
        return GrayScale{average, from.a};
    }

    // from HSV
    template<>
    inline RGB convert_to(HSV from)
    {
        float h = from.h,
              s = from.s,
              v = from.v;

        float c = v * s;
        float h_2 = h / 60;
        float x = c * (1 - std::fabs(std::fmod(h_2, 2) - 1));

        float r, g, b;

        if (0 <= h_2 and h_2 < 1)
        {
            r = c;
            g = x;
            b = 0;
        }
        else if (1 <= h_2 and h_2 < 2)
        {
            r = x;
            g = c;
            b = 0;
        }
        else if (2 <= h_2 and h_2 < 3)
        {
            r = 0;
            g = c;
            x = b;
        }
        else if (3 <= h_2 and h_2 < 4)
        {
            r = 0;
            g = x;
            b = c;
        }
        else if (4 <= h_2 and h_2 < 5)
        {
            r = x;
            g = 0;
            b = c;
        }
        else if (5 <= h_2 and h_2 <= 6)
        {
            r = c;
            g = 0;
            b = x;
        }

        float m = v - c;
        r += m;
        g += m;
        b += m;

        RGB out;
        out.r = r;
        out.g = g;
        out.b = b;
        out.a = from.a;

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
        return Color(as_rgb.r, as_rgb.g, as_rgb.b, from.a);
    }

    template<>
    inline GrayScale convert_to(HSV from)
    {
        return GrayScale{from.v, from.a};
    }

    // from HSL
    template<>
    inline Color convert_to(HSL from)
    {
        auto as_hsv = convert_to<HSV>(from);
        auto as_rgb = convert_to<RGB>(as_hsv);
        return Color(as_rgb.r, as_rgb.g, as_rgb.b, from.a);
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
        float hsl_l = from.l,
              hsl_s = from.s;

        float hsv_v = hsl_l + hsl_s * std::min(hsl_l, 1.f - hsl_l);
        float hsv_s = (hsv_v != 0) ? 2 * (1.f - hsl_l / hsv_v) : 0;

        HSV out;
        out.h = from.h;
        out.s = hsv_s;
        out.v = hsv_v;

        return out;
    }

    template<>
    inline GrayScale convert_to(HSL from)
    {
        return GrayScale{from.l, from.a};
    }

    // from GreyScale
    template<>
    inline Color convert_to(GrayScale from)
    {
        return Color(from.v, from.v, from.v, from.a);
    }

    template<>
    inline RGB convert_to(GrayScale from)
    {
        return RGB{from.v, from.v, from.v, from.a};
    }

    template<>
    inline HSV convert_to(GrayScale from)
    {
        return HSV{0, 0, from.v, from.a};
    }

    template<>
    inline HSL convert_to(GrayScale from)
    {
        return HSL{0, 0, from.v, from.a};
    }

    inline Color::Color()
        : Eigen::Array<float, 1, 4>(0, 0, 0, 1)
    {}

    inline Color::Color(float red, float green, float blue, float alpha)
        : Eigen::Array<float, 1, 4>(red, green, blue, alpha)
    {}

    inline Color::Color(RGB rgb)
        : Eigen::Array<float, 1, 4>(rgb.r, rgb.g, rgb.b, rgb.a)
    {}

    inline Color::Color(HSV hsv)
        : Color()
    {
        auto asrgb = convert_to<RGB>(hsv);
        red() = asrgb.r;
        green() = asrgb.g;
        blue() = asrgb.b;
        alpha() = hsv.a;
    }

    inline Color::Color(HSL hsl)
        : Color()
    {
        auto asrgb = convert_to<RGB>(hsl);
        red() = asrgb.r;
        green() = asrgb.g;
        blue() = asrgb.b;
        alpha() = hsl.a;
    }

    inline Color::Color(GrayScale grayscale)
        : Color()
    {
        red() = grayscale.v;
        green() = grayscale.v;
        blue() = grayscale.v;
        alpha() = grayscale.a;
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

    float & Color::alpha()
    {
        return operator()(0, 3);
    }

    float Color::alpha() const
    {
        return operator()(0, 3);
    }

    inline RGB Color::as_rgb()
    {
        return convert_to<RGB>(*this);
    }

    inline HSV Color::as_hsv()
    {
        return convert_to<HSV>(*this);
    }

    inline HSL Color::as_hsl()
    {
        return convert_to<HSL>(*this);
    }

    inline Vector4f Color::as_float_vec()
    {
        return Vector4f(red(), green(), blue(), alpha());
    }

    inline GrayScale Color::as_grayscale()
    {
        return convert_to<GrayScale>(*this);
    }

    inline Color & Color::operator=(RGB rgb)
    {
        red() = rgb.r;
        green() = rgb.g;
        blue() = rgb.b;
        alpha() = rgb.a;
        return *this;
    }

    inline Color & Color::operator=(HSV hsv)
    {
        auto asrgb = convert_to<RGB>(hsv);
        red() = asrgb.r;
        green() = asrgb.g;
        blue() = asrgb.b;
        alpha() = hsv.a;
        return *this;
    }

    inline Color & Color::operator=(HSL hsl)
    {
        auto asrgb = convert_to<RGB>(hsl);
        red() = asrgb.r;
        green() = asrgb.g;
        blue() = asrgb.b;
        alpha() = hsl.a;
        return *this;
    }

    inline Color & Color::operator=(GrayScale grayscale)
    {
        red() = grayscale.v;
        green() = grayscale.v;
        blue() = grayscale.v;
        alpha() = grayscale.a;
        return *this;
    }

    inline bool Color::operator==(const Color& other) const
    {
        return this->red() == other.red() and this->green() == other.green() and this->blue() == other.blue() and this->alpha() == other.alpha();
    }

    inline bool Color::operator!=(const Color& other) const
    {
        return not (*this == other);
    }

    inline Color Color::invert() const
    {
        Color out;
        out.red() = 1 - red();
        out.green() = 1 - green();
        out.blue() = 1 - blue();
        out.alpha() = alpha();
        return out;
    }
}