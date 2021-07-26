//
// Copyright 2021 Clemens Cords
// Created on 25.07.21 by clem (mail@clemens-cords.com)
//

#include <cassert>
#include <type_traits>
#include <vector.hpp>

namespace crisp
{
    // mix
    template<typename Color_t>
    Color_t mix(Color_t a, Color_t b, float weight)
    {
        static_assert(true, "mix template parameter is not a valid color representation");
    }

    template<>
    Color mix(Color a, Color b, float weight_f)
    {
         auto weight = static_cast<uint8_t>(weight_f * uint8_t(255));
         weight = clamp<uint8_t>(0, 255, weight);

         uint8_t af = uint8_t(255) - weight;
         uint8_t bf = weight;

         Color out;
         out.r = a.r * af + b.r * bf;
         out.g = a.g * af + b.g * bf;
         out.b = a.b * af + b.g * bf;
         out.a = a.a * af + b.a * bf;

         return out;
    }

    template<>
    RGB mix(RGB a, RGB b, float weight_f)
    {
         auto weight = static_cast<uint8_t>(weight_f * uint8_t(255));
         weight = clamp<uint8_t>(0, 255, weight);

         uint8_t af = uint8_t(255) - weight;
         uint8_t bf = weight;

         RGB out;
         out.r = a.r * af + b.r * bf;
         out.g = a.g * af + b.g * bf;
         out.b = a.b * af + b.g * bf;
         out.a = a.a * af + b.a * bf;

         return out;
    }

    template<>
    HSV mix(HSV a, HSV b, float weight_f)
    {
         auto weight = static_cast<uint8_t>(weight_f * uint8_t(255));
         weight = clamp<uint8_t>(0, 255, weight);

         uint8_t af = uint8_t(255) - weight;
         uint8_t bf = weight;

         HSV out;
         out.h = a.h * af + b.h * bf;
         out.s = a.s * af + b.s * bf;
         out.v = a.v * af + b.v * bf;
         out.a = a.a * af + b.a * bf;

         return out;
    }

    template<>
    HSL mix(HSL a, HSL b, float weight_f)
    {
         auto weight = static_cast<uint8_t>(weight_f * uint8_t(255));
         weight = clamp<uint8_t>(0, 255, weight);

         uint8_t af = uint8_t(255) - weight;
         uint8_t bf = weight;

         HSL out;
         out.h = a.h * af + b.h * bf;
         out.s = a.s * af + b.s * bf;
         out.l = a.l * af + b.l * bf;
         out.a = a.a * af + b.a * bf;

         return out;
    }

    template<>
    GrayScale mix(GrayScale a, GrayScale b, float weight_f)
    {
         auto weight = static_cast<uint8_t>(weight_f * uint8_t(255));
         weight = clamp<uint8_t>(0, 255, weight);

         uint8_t af = uint8_t(255) - weight;
         uint8_t bf = weight;

         GrayScale out;
         out.v = a.v * af + b.v * bf;
         out.a = a.a * af + b.a * bf;

         return out;
    }

    // convert to
    template<typename T>
    T convert_to(T from)
    {
        return from;
    }

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
    HSV convert_to(Color from)
    {
        auto as_rgb = RGB{from.r, from.g, from.b, from.a};
        return convert_to<HSV>(as_rgb);
    }

    template<>
    HSL convert_to(Color from)
    {
        auto as_rgb = RGB{from.r, from.g, from.b, from.a};
        return convert_to<HSL>(as_rgb);
    }

    template<>
    RGB convert_to(Color from)
    {
        return RGB{from.r, from.g, from.b, from.a};
    }

    template<>
    GrayScale convert_to(Color from)
    {
        uint8_t average = (from.r + from.g + from.b) / 3;
        return GrayScale{average, from.a};
    }

    // from RGB
    template<>
    HSV convert_to(RGB from)
    {
        float r = from.r / uint8_t(255),
              g = from.g / uint8_t(255),
              b = from.b / uint8_t(255);

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
        out.h = h * uint8_t(255);
        out.s = s * uint8_t(255);
        out.v = v * uint8_t(255);
        out.a = from.a;

        return out;
    }

    template<>
    HSL convert_to(RGB from)
    {
        auto as_hsv = convert_to<HSV>(from);
        return convert_to<HSL>(as_hsv);
    }

    template<>
    Color convert_to(RGB from)
    {
        return Color(from.r, from.g, from.b, from.a);
    }

    template<>
    GrayScale convert_to(RGB from)
    {
        uint8_t average = (from.r + from.g + from.b) / 3;
        return GrayScale{average, from.a};
    }

    // from HSV
    template<>
    RGB convert_to(HSV from)
    {
        float h = from.h / uint8_t(255),
              s = from.s / uint8_t(255),
              v = from.v / uint8_t(255);

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
        out.r = r * uint8_t(255);
        out.g = g * uint8_t(255);
        out.b = b * uint8_t(255);
        out.a = from.a;

        return out;
    }

    template<>
    HSL convert_to(HSV from)
    {
        float hsv_s = from.s / uint8_t(255),
              hsv_v = from.v / uint8_t(255);

        float hsl_l = hsv_v * (1 - hsv_s / 2),
              hsl_s;

        if (hsl_l == 0 or hsl_l == 1)
            hsl_s = 0;
        else
            hsv_s = (hsv_v - hsl_l) / std::min(hsl_l, 1.f - hsl_l);

        HSL out;
        out.h = from.h;
        out.s = hsl_s * uint8_t(255);
        out.l = hsl_l * uint8_t(255);

        return out;
    }

    template<>
    Color convert_to(HSV from)
    {
        auto as_rgb = convert_to<RGB>(from);
        return Color(as_rgb.r, as_rgb.g, as_rgb.b, from.a);
    }

    template<>
    GrayScale convert_to(HSV from)
    {
        return GrayScale{from.v, from.a};
    }

    // from HSL
    template<>
    Color convert_to(HSL from)
    {
        auto as_hsv = convert_to<HSV>(from);
        auto as_rgb = convert_to<RGB>(as_hsv);
        return Color(as_hsv.r, as_hsv.g, as_hsv.b, from.a);
    }

    template<>
    RGB convert_to(HSL from)
    {
        auto as_hsv = convert_to<HSV>(from);
        return convert_to<RGB>(as_hsv);
    }

    template<>
    HSV convert_to(HSL from)
    {
        float hsl_l = from.l / uint8_t(255),
              hsl_s = from.s / uint8_t(255);

        float hsv_v = hsl_l + hsl_s * std::minmin(hsl_l, 1.f - hsl_l);
        float hsv_s = (hsv_v != 0) ? 2 * (1.f - hsl_l / hsv_v) : 0;

        HSV out;
        out.h = from.h;
        out.s = hsv_s * uint8_t(255);
        out.v = hsv_v * uint8_t(255);

        return out;
    }

    template<>
    GrayScale convert_to(HSL from)
    {
        return GrayScale{from.l, from.a};
    }

    // from GreyScale
    template<>
    Color convert_to(GrayScale from)
    {
        return Color(from.v, from.v, from.v, from.a);
    }

    template<>
    RGB convert_to(GrayScale from)
    {
        return RGB{from.v, from.v, from.v, from.a};
    }

    template<>
    HSV convert_to(GrayScale from)
    {
        return HSV{0, 0, from.v, from.a};
    }

    template<>
    HSL convert_to(GrayScale from)
    {
        return HSL{0, 0, from.v, from.a};
    }
}