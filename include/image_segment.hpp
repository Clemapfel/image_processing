// 
// Copyright 2021 Clemens Cords
// Created on 04.08.21 by clem (mail@clemens-cords.com)
//

#pragma once

#include <set>

#include <image.hpp>
#include <vector.hpp>

namespace crisp
{
    // a set of unique pixel coordinates
    namespace detail
    {
        struct PixelCoordCompare
        {
            bool operator()(const Vector2ui& a, const Vector2ui& b) const
            {
                return a.x() != b.x() ? a.x() < b.x() : a.y() <= b.y();
            }
        };
    }

    using ImageSegment = std::set<Vector2ui, detail::PixelCoordCompare>;
}
