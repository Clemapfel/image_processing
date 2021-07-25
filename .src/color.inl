//
// Created by clem on 25.07.21.
//

#include <cassert>
#include <type_traits>
#include <vector.hpp>

namespace todo
{
    template<typename T1, typename T2>
    T2&& convert(T1 from)
    {
        /*
        static constexpr auto is_color_model = []<class T>(T dummy)
        {
            return std::is_same_v<T, Color> or std::is_same_v<T, RGB> or std::is_same_v<T, HSV> or std::is_same_v<T, CMYK> or std::is_same_v<T, GrayScale>;
        };

        static_assert(is_color_model(T1()) and is_color_model(T2()), "argument is not a valid color representation");

        // RGB to CMYK
        if (std::is_same_v<T1, RGB> and std::is_same_v<T2, CMYK>)
        {
            CMYK out;
            out.c = 1 - from.r;
            out.m = 1 - from.g;
            out.y = 1 - from.b;

            auto vec = Vector3u8(out.c, out.m, out.y);
            uint8_t k = min(vec);
        }


        // HSV to RGB
        if (std::is_same_v<T1, HSV> and std::is_same_v<T2, RGB>)
        {
        }
        // RGB to HSV
        else if (std::is_same_v<T1, RGB> and std::is_same_v<T2, HSV>)
        {

        }
        // to grey scale
        else if (std::is_same_v<T2, GrayScale>)
        {
            if (std::is_same_v<T1, RGB>)
            {
                // convert to hsv
                // lower v to 0
            }
            else if (std::is_same_v<T1, HSV>)
            {
                // lower v to 0
                // convert to rgb
            }
        }
        else 
            assert(true && "TODO");
            */
    }
}