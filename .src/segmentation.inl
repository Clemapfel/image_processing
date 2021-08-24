// 
// Copyright 2021 Clemens Cords
// Created on 18.08.21 by clem (mail@clemens-cords.com)
//

#include <unordered_set>
#include <deque>
#include <cassert>

namespace crisp::Segmentation
{
    std::vector<ImageSegment> decompose_into_segments(const BinaryImage& image, size_t min_region_size)
    {
        std::vector<ImageSegment> segments;

        BinaryImage seen;
        seen.create(image.get_size().x(), image.get_size().y(), false);
        bool color = false;

        for (long x = 0; x < seen.get_size().x(); ++x)
        {
            for (long y = 0; y < seen.get_size().y(); ++y)
            {
                if (seen(x, y))
                    continue;

                if (not seen(x, y))
                {
                    color = image(x, y);

                    segments.emplace_back();
                    std::deque<Vector2ui> to_add;
                    to_add.emplace_front(x, y);

                    while (not to_add.empty())
                    {
                        auto current = to_add.front();
                        for (long i : {-1, 1, 0, 0})
                        {
                            for (long j : {0, 0, -1, 1})
                            {
                                if (current.x() + i < 0 or current.x() + i >= seen.get_size().x() or
                                    current.y() + j < 0 or current.y() + j >= seen.get_size().y() or
                                    seen(current.x() + i, current.y() + j))
                                    continue;

                                if (image(current.x() + i, current.y() + j) == color)
                                {
                                    to_add.emplace_back(current.x() + i, current.y() + j);
                                    seen(current.x() + i, current.y() + j) = true;
                                }
                            }
                        }

                        segments.back().add(to_add.front());
                        to_add.pop_front();
                    }

                    if (segments.back().size() < min_region_size)
                        segments.erase(segments.end() - 1);

                    color = false;
                }
            }
        }

        return segments;
    }

    std::vector<ImageSegment> decompose_into_segments(const GrayScaleImage& image, size_t min_region_size)
    {
        std::vector<ImageSegment> segments;

        BinaryImage seen;
        seen.create(image.get_size().x(), image.get_size().y(), false);
        float color = -1;

        for (long x = 0; x < seen.get_size().x(); ++x)
        {
            for (long y = 0; y < seen.get_size().y(); ++y)
            {
                if (seen(x, y))
                    continue;

                if (not seen(x, y))
                {
                    color = floor(image(x, y) * 255);

                    segments.emplace_back();
                    std::deque<Vector2ui> to_add;
                    to_add.emplace_front(x, y);

                    while (not to_add.empty())
                    {
                        auto current = to_add.front();
                        for (long i : {-1, 1, 0, 0})
                        {
                            for (long j : {0, 0, -1, 1})
                            {
                                if (current.x() + i < 0 or current.x() + i >= seen.get_size().x() or
                                    current.y() + j < 0 or current.y() + j >= seen.get_size().y() or
                                    seen(current.x() + i, current.y() + j))
                                    continue;

                                if (floor(image(current.x() + i, current.y() + j) * 255) == color)
                                {
                                    to_add.emplace_back(current.x() + i, current.y() + j);
                                    seen(current.x() + i, current.y() + j) = true;
                                }
                            }
                        }

                        segments.back().add(to_add.front());
                        to_add.pop_front();
                    }

                    if (segments.back().size() < min_region_size)
                        segments.erase(segments.end() - 1);

                    color = -1;
                }
            }
        }

        return segments;
    }

    std::vector<ImageSegment> decompose_into_segments(const ColorImage& image, size_t min_region_size)
    {
        std::vector<ImageSegment> segments;

        BinaryImage seen;
        seen.create(image.get_size().x(), image.get_size().y(), false);
        Color color = Color(-1, -1, -1);

        for (long x = 0; x < seen.get_size().x(); ++x)
        {
            for (long y = 0; y < seen.get_size().y(); ++y)
            {
                if (seen(x, y))
                    continue;

                if (not seen(x, y))
                {
                    color = image(x, y);
                    color.red() = floor(255 * color.red());
                    color.green() = floor(255 * color.green());
                    color.blue() = floor(255 * color.blue());

                    segments.emplace_back();
                    std::deque<Vector2ui> to_add;
                    to_add.emplace_front(x, y);

                    while (not to_add.empty())
                    {
                        auto current = to_add.front();
                        for (long i : {-1, 1, 0, 0})
                        {
                            for (long j : {0, 0, -1, 1})
                            {
                                if (current.x() + i < 0 or current.x() + i >= seen.get_size().x() or
                                    current.y() + j < 0 or current.y() + j >= seen.get_size().y() or
                                    seen(current.x() + i, current.y() + j))
                                    continue;

                                auto current_color = image(current.x() + i, current.y() + j);

                                if (floor(current_color.red() * 255) == color.red() and
                                    floor(current_color.green() * 255) == color.green() and
                                    floor(current_color.blue() * 255) == color.blue())
                                {
                                    to_add.emplace_back(current.x() + i, current.y() + j);
                                    seen(current.x() + i, current.y() + j) = true;
                                }
                            }
                        }

                        segments.back().add(to_add.front());
                        to_add.pop_front();
                    }

                    if (segments.back().size() < min_region_size)
                        segments.erase(segments.end() - 1);

                    color = -1;
                }
            }
        }

        return segments;
    }


    BinaryImage basic_threshold(const GrayScaleImage& image)
    {
        const float convergence_treshold = 1 / 255.f;

        auto histogram = Histogram<uint8_t>();
        histogram.create_from(image);

        uint8_t old_threshold = histogram.median() * 255;
        uint8_t new_threshold = 0;
        float total_sum = 0,
                left_sum = 0,
                right_sum = 0;

        size_t total_n = 0,
                left_n = 0,
                right_n = 0;

        float left_mean, right_mean;

        for (uint8_t i = 0; i < uint8_t(255); ++i)
        {
            float value = histogram.at(i) * (i / 255.f);
            if (i < old_threshold)
            {
                left_n += histogram.at(i);
                left_sum += value;
            }
            else
            {
                right_n += histogram.at(i);
                right_sum += value;
            }
        }

        left_mean = left_sum / left_n;
        right_mean = right_sum / right_n;
        new_threshold = (left_mean + 0.5 * (abs(right_mean - left_mean))) * 255;

        // TODO: optimize by only adding/subtracting necessary parts from left/right sum
        while (std::abs<float>((old_threshold / 255.f) - (new_threshold / 255.f)) > convergence_treshold)
        {
            old_threshold = new_threshold;
            left_n = 0;
            left_sum = 0;
            right_n = 0;
            right_sum = 0;

            for (uint8_t i = 0; i < uint8_t(255); ++i)
            {
                float value = histogram.at(i) * (i / 255.f);
                if (i < old_threshold)
                {
                    left_n += histogram.at(i);
                    left_sum += value;
                }
                else
                {
                    right_n += histogram.at(i);
                    right_sum += value;
                }
            }

            left_mean = left_sum / left_n;
            right_mean = right_sum / right_n;
            new_threshold = (left_mean + 0.5 * (abs(right_mean - left_mean))) * 255;
        }

        auto out = BinaryImage();
        out.create(image.get_size().x(), image.get_size().y());

        for (long x = 0; x < image.get_size().x(); ++x)
            for (long y = 0; y < image.get_size().y(); ++y)
                out(x, y) = image(x, y) > (new_threshold / 255.f);

        return out;
    }

    BinaryImage otsu_threshold(const GrayScaleImage& image)
    {
        auto histogram = Histogram<uint8_t>();
        histogram.create_from(image);

        std::map<uint8_t, std::pair<float, float>> threshold_to_sums;   // k: {cumulative_sum, intensity_sum}
        float mn = image.get_size().x() * image.get_size().y();
        float global_mean = 0;

        for (uint8_t k = 0; k < 255; ++k)
        {
            float cumulative_sum = 0;
            float intensity_sum = 0;
            size_t n = 0;
            for (uint8_t i = 0; i <= k; ++i)
            {
                float p_i = histogram.at(i) / mn;
                cumulative_sum += p_i;
                intensity_sum += p_i * i;
                n += histogram.at(i);

                if (k == 254)
                    global_mean += p_i * i;
            }
            threshold_to_sums.emplace(k, std::make_pair(cumulative_sum, intensity_sum));
        }

        uint8_t max_k = 0;
        float max_sigma = 0;
        for (auto& pair : threshold_to_sums)
        {
            auto p_i = pair.second.first;
            auto local_mean = pair.second.second;
            auto sigma = pow(global_mean * p_i - local_mean, 2) / (p_i * (1 - p_i));

            if (sigma > max_sigma)
            {
                max_sigma = sigma;
                max_k = pair.first;
            }
        }

        float result = max_k / 255.f;

        auto out = BinaryImage();
        out.create(image.get_size().x(), image.get_size().y());

        for (long x = 0; x < image.get_size().x(); ++x)
            for (long y = 0; y < image.get_size().y(); ++y)
                out(x, y) = image(x, y) > result;

        return out;
    }

    BinaryImage variable_threshold(const GrayScaleImage& image, float constant)
    {
        BinaryImage out;
        out.create(image.get_size().x(), image.get_size().y());

        std::list<float> tail;
        float current_sum = 0;

        size_t tail_length = constant * std::min(image.get_size().x(), image.get_size().y());
        size_t k = 0;

        auto update = [&](size_t x, size_t y, size_t i) {
            out(x, y) = image(x, y) > (current_sum / tail.size());

            if (i > tail_length)
            {
                current_sum -= tail.front();
                tail.erase(tail.begin());
            }

            tail.emplace_back(image(x, y));
            current_sum += tail.back();
        };

        int top = 0, bottom = image.get_size().x() - 1, left = 0, right = image.get_size().y() - 1;
        int direction = 1;

        while (top <= bottom and left <= right)
        {
            if (direction == 1)
            {
                for (int i = left; i <= right; ++i, k++)
                    update(top, i, k);

                ++top;
                direction = 2;
            }

            else if (direction == 2)
            {
                for (int i = top; i <= bottom; ++i, k++)
                    update(i, right, k);

                --right;
                direction = 3;
            }

            else if (direction == 3)
            {
                for (int i = right; i >= left; --i, k++)
                    update(bottom, i, k);

                --bottom;
                direction = 4;
            }
            else if (direction == 4)
            {
                for (int i = bottom; i >= top; --i)
                    update(i, left, k);

                ++left;
                direction = 1;
            }
        }

        return out;
    }

    // @complexity: amortized m*n*8*neighborhood_size, where m,n size of the image
    BinaryImage neighborhood_threshold(const GrayScaleImage& image, size_t neighborhood_size)
    {
        auto out = BinaryImage();
        out.create(image.get_size().x(), image.get_size().y());

        const int spread = 7;
        const int half_spread = spread * 0.5;

        for (long x = 0; x < image.get_size().x(); ++x)
        {
            for (long y = 0; y < image.get_size().y(); ++y)
            {
                float mean = 0;
                size_t count = 0;
                for (long i = 1; i <= spread * neighborhood_size; i += spread)
                {
                    mean += image.get_pixel_or_padding(x - i, y);
                    mean += image.get_pixel_or_padding(x + i, y);
                    mean += image.get_pixel_or_padding(x, y + i);
                    mean += image.get_pixel_or_padding(x, y - i);

                    mean += image.get_pixel_or_padding(x - i - half_spread, y - i - half_spread);
                    mean += image.get_pixel_or_padding(x + i - half_spread, y - i - half_spread);
                    mean += image.get_pixel_or_padding(x - i - half_spread, y + i - half_spread);
                    mean += image.get_pixel_or_padding(x + i - half_spread, y + i - half_spread);
                }
                mean /= (8 * neighborhood_size);
                out(x, y) = image(x, y) > mean;
            }
        }

        return out;
    }

    ColorImage superpixel_clustering(const ColorImage& image, size_t n_superpixel)
    {
        const size_t n_pixels = image.get_size().x() * image.get_size().y();
        auto spacing = sqrt(n_pixels / double(n_superpixel));

        auto color_distance = [&](const Color& a, const Color& b) -> float {
            float out = 0;
            out += (a.x() - b.x()) * (a.x() - b.x());
            out += (a.y() - b.y()) * (a.y() - b.y());
            out += (a.z() - b.z()) * (a.z() - b.z());

            return out;
        };

        auto xy_distance = [&](const Vector2f& a, const Vector2f& b) {
            float out = 0;
            out += (a.x() - b.x()) * (a.x() - b.x());
            out += (a.y() - b.y()) * (a.y() - b.y());

            return out / (2 * spacing * spacing);
        };

        struct Cluster
        {
            Vector2f center;
            Color color_sum;
            Vector2f xy_sum;
            size_t n;

            Color final_color; // unused until post-processing
        };

        std::map<size_t, Cluster> clusters;

        for (long x = spacing / 2, cluster_i = 1; x < image.get_size().x(); x += spacing)
            for (long y = spacing / 2; y < image.get_size().y(); y += spacing, cluster_i++)
            {
                long new_x = x,
                        new_y = y;


                float max_gradient = 0;
                for (int i = -1; i < 1; ++i)
                {
                    for (int j = -1; j < 1; ++j)
                    {
                        float x_gradient = color_distance(image(x + i + 0, y + j), image(x + i + 1, y + j));
                        float y_gradient = color_distance(image(x + i, y + j + 0), image(x + i, y + j + 1));

                        float gradient = abs(x_gradient) + abs(y_gradient);

                        if (gradient > max_gradient)
                        {
                            max_gradient = gradient;
                            new_x = x + i;
                            new_y = y + j;
                        }
                    }
                }

                clusters.emplace(cluster_i,
                                 Cluster{Vector2f(new_x, new_y), image(new_x, new_y), Vector2f(new_x, new_y), 1});
            }

        // use color image as labeling space [-1, 0, 1, ... centers.size()-1] to save memory and allocation time
        // r = label
        // g = distance
        // b = 0
        ColorImage out;
        out.create(image.get_size().x(), image.get_size().y(), Color(0, std::numeric_limits<float>::infinity(), 0));

        size_t n_pixels_visited = 0;
        size_t n_assigned = 0;

        for (auto& pair : clusters)
        {
            size_t cluster_i = pair.first;
            Color mean_color = pair.second.color_sum;
            mean_color /= pair.second.n;

            Vector2f mean_pos = pair.second.xy_sum;
            mean_pos /= pair.second.n;
            auto& center = mean_pos;

            for (int i = -spacing; i < +spacing; ++i)
            {
                for (int j = -spacing; j < +spacing; ++j)
                {
                    if (center.x() + i < 0 or center.x() + i >= image.get_size().x() or center.y() + j < 0 or
                        center.y() + j >= image.get_size().y())
                        continue;

                    n_pixels_visited += 1;

                    auto& current = out(center.x() + i, center.y() + j);
                    int old_i = current.x();
                    if (old_i == cluster_i)
                        continue;

                    auto pos = Vector2ui(center.x() + i, center.y() + j);
                    auto color = image(pos.x(), pos.y());
                    auto color_dist = color_distance(mean_color, color);
                    auto xy_dist = xy_distance(mean_pos, Vector2f(pos.x(), pos.y())) / (2 * spacing);
                    auto new_dist = color_dist + xy_dist;
                    auto old_dist = current.y();

                    if (new_dist < old_dist)
                    {
                        out(pos.x(), pos.y()).x() = cluster_i;
                        out(pos.x(), pos.y()).y() = new_dist;

                        pair.second.n += 1;
                        pair.second.color_sum += color;
                        pair.second.xy_sum += Vector2f(pos.x(), pos.y());

                        if (old_i != 0)
                        {
                            auto& cluster = clusters.at(old_i);
                            cluster.color_sum -= color;
                            cluster.xy_sum += Vector2f(pos.x(), pos.y());
                            cluster.n -= 1;
                        }

                        n_assigned += 1;
                        mean_color = pair.second.color_sum;
                        mean_color /= pair.second.n;
                    }
                }
            }
        }

        //std::cout << n_pixels_visited << " / " << n_pixels << std::endl;  // O(N*spacing*spacing + n*m)

        for (auto& c : clusters)
        {
            auto color = c.second.color_sum;
            color /= c.second.n;
            c.second.final_color = color;
        }

        float threshold = 1 / 255.f;
        for (size_t i = 1; i < clusters.size(); ++i)
        {
            auto& current = clusters.at(i).final_color;
            auto& next = clusters.at(i + 1).final_color;

            if (color_distance(current, next) < threshold)
            {
                current += next;
                current /= 2;
                next = current;
            }
        }

        for (auto& px : out)
            px = clusters.at(px.x()).final_color;

        return out;
    }

    GrayScaleImage superpixel_clustering(const GrayScaleImage& image, size_t n_superpixel)
    {
        const size_t n_pixels = image.get_size().x() * image.get_size().y();
        auto spacing = sqrt(n_pixels / double(n_superpixel));

        auto color_distance = [&](float a, float b) -> float {
            return (a - b) * (a - b) * 3;
        };

        auto xy_distance = [&](const Vector2f& a, const Vector2f& b) {
            float out = 0;
            out += (a.x() - b.x()) * (a.x() - b.x());
            out += (a.y() - b.y()) * (a.y() - b.y());

            return out / (2 * spacing * spacing);
        };

        struct Cluster
        {
            Vector2f center;
            float color_sum;
            Vector2f xy_sum;
            size_t n;

            float final_color; // unused until post-processing
        };

        std::map<size_t, Cluster> clusters;

        for (long x = ceil(spacing / 2), cluster_i = 1; x < image.get_size().x() - 1; x += spacing)
            for (long y = ceil(spacing / 2); y < image.get_size().y() - 1; y += spacing, cluster_i++)
            {
                long new_x = x,
                        new_y = y;

                float max_gradient = 0;
                for (int i = -1; i < 1; ++i)
                {
                    for (int j = -1; j < 1; ++j)
                    {
                        float x_gradient = color_distance(image(x + i + 0, y + j), image(x + i + 1, y + j));
                        float y_gradient = color_distance(image(x + i, y + j + 0), image(x + i, y + j + 1));

                        float gradient = abs(x_gradient) + abs(y_gradient);

                        if (gradient > max_gradient)
                        {
                            max_gradient = gradient;
                            new_x = x + i;
                            new_y = y + j;
                        }
                    }
                }

                clusters.emplace(cluster_i,
                                 Cluster{Vector2f(new_x, new_y), image(new_x, new_y), Vector2f(new_x, new_y), 1});
            }

        // use grayscale as labeling space
        GrayScaleImage out;
        out.create(image.get_size().x(), image.get_size().y());

        // matrix for distance
        Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> distances;
        distances.resize(image.get_size().x(), image.get_size().y());
        distances.setConstant(std::numeric_limits<float>::infinity());

        size_t n_pixels_visited = 0;
        size_t n_assigned = 0;

        for (auto& pair : clusters)
        {
            size_t cluster_i = pair.first;
            float mean_color = pair.second.color_sum;
            mean_color /= pair.second.n;

            Vector2f mean_pos = pair.second.xy_sum;
            mean_pos /= pair.second.n;
            auto& center = mean_pos;

            for (int i = -spacing; i < +spacing; ++i)
            {
                for (int j = -spacing; j < +spacing; ++j)
                {
                    if (center.x() + i < 0 or center.x() + i >= image.get_size().x() or center.y() + j < 0 or
                        center.y() + j >= image.get_size().y())
                        continue;

                    n_pixels_visited += 1;

                    int old_i = out(center.x() + i, center.y() + j);
                    if (old_i == cluster_i)
                        continue;

                    auto pos = Vector2ui(center.x() + i, center.y() + j);
                    auto color = image(pos.x(), pos.y());
                    auto color_dist = color_distance(mean_color, color);
                    auto xy_dist = xy_distance(mean_pos, Vector2f(pos.x(), pos.y())) / (2 * spacing);
                    auto new_dist = color_dist + xy_dist;
                    auto old_dist = distances(center.x() + i, center.y() + j);

                    if (new_dist < old_dist)
                    {
                        out(pos.x(), pos.y()) = cluster_i;
                        distances(pos.x(), pos.y()) = new_dist;

                        pair.second.n += 1;
                        pair.second.color_sum += color;
                        pair.second.xy_sum += Vector2f(pos.x(), pos.y());

                        if (old_i != 0)
                        {
                            auto& cluster = clusters.at(old_i);
                            cluster.color_sum -= color;
                            cluster.xy_sum += Vector2f(pos.x(), pos.y());
                            cluster.n -= 1;
                        }

                        n_assigned += 1;
                        mean_color = pair.second.color_sum;
                        mean_color /= pair.second.n;
                    }
                }
            }
        }

        for (auto& c : clusters)
        {
            auto color = c.second.color_sum;
            color /= c.second.n;
            c.second.final_color = color;
        }

        float threshold = 3 / 255.f;
        for (size_t i = 1; i < clusters.size(); ++i)
        {
            auto& current = clusters.at(i).final_color;
            auto& next = clusters.at(i + 1).final_color;

            if (color_distance(current, next) < threshold)
            {
                current += next;
                current /= 2;
                next = current;
            }
        }

        for (auto& px : out)
            px = clusters.at(px).final_color;

        return out;
    }

    ColorImage region_growing_clustering(
            const ColorImage& image,
            std::vector<Vector2ui> seeds,
            float add_upper_threshold,
            float merge_upper_threshold)
    {
        // citations:
        // https://ieeexplore.ieee.org/document/295913
        // https://sci-hub.se/https://www.sciencedirect.com/science/article/abs/pii/S0262885605000673

        std::cout << "[LOG] starting seeded region growing clustering with " << seeds.size() << " seeds." << std::endl;

        struct point_compare
        {
            bool operator()(const Vector2ui& a, const Vector2ui& b) const
            {
                return (a.x() == b.x()) and (a.y() == b.y());
            }
        };

        static size_t image_height = 0;
        image_height = image.get_size().y();
        struct point_hash
        {
            size_t operator()(const Vector2ui& a) const noexcept
            {
                return a.y() + image_height * a.x();
            }
        };

        using Set_t = std::unordered_set<Vector2ui, point_hash, point_compare>;

        struct Region
        {
            size_t n;
            Color color_sum;
            Set_t potential_neighbours;
            bool converged = false;
            Color final_color = Color(-1, -1, -1);  // only used in post-processing
        };

        std::map<size_t, Region> regions;
        std::map<size_t, size_t> merge_map;
        size_t n_converged = 0;

        ColorImage out; // .r is region label, .g is distance
        out.create(image.get_size().x(), image.get_size().y(), Color(0, std::numeric_limits<float>::max(), -1));

        const size_t n_before_merge = regions.size();
        size_t n_merged = 0;
        size_t k = 1;
        for (auto& seed : seeds)
        {
            regions.emplace(k, Region{1, image(seed.x(), seed.y()), Set_t()});
            out(seed.x(), seed.y()) = k;
            size_t n_neighbouring_regions = 0;
            for (int i = -1; i <= 1; ++i)
            {
                for (int j = -1; j <= 1; ++j)
                {
                    if (seed.x() + i < 0 or seed.x() + i >= image.get_size().x() or
                        seed.y() + j < 0 or seed.y() + j >= image.get_size().y())
                        continue;

                    if (i == 0 or j == 0 and out(seed.x() + i, seed.y() + j).x() != 0)
                        n_neighbouring_regions += 1;

                    regions.at(k).potential_neighbours.insert(Vector2ui(seed.x() + i, seed.y() + j));
                }
            }

            k++;
        }

        auto color_distance = [](const Color& a, const Color& b) -> float {
            return dist(a, b);
        };

        size_t n_regions = regions.size();
        while (n_converged < n_regions)
        {
            std::set<size_t> scheduled_for_deletion;
            for (auto& pair : regions)
            {
                if (pair.second.converged or scheduled_for_deletion.find(pair.first) != scheduled_for_deletion.end())
                    continue;

                size_t region_i = pair.first;
                auto mean_color = pair.second.color_sum;
                mean_color /= pair.second.n;

                auto to_test = pair.second.potential_neighbours;
                std::vector<Vector2ui> checked_this_iteration;
                for (auto& point : to_test)
                {
                    auto new_color = image(point.x(), point.y());
                    auto distance = color_distance(new_color, mean_color);
                    checked_this_iteration.push_back(point);
                    if (distance < add_upper_threshold)
                    {
                        out(point.x(), point.y()).x() = region_i;
                        out(point.x(), point.y()).y() = distance;

                        pair.second.n += 1;
                        pair.second.color_sum += new_color;

                        for (int i = -1; i <= 1; ++i)
                        {
                            for (int j = -1; j <= 1; ++j)
                            {
                                if (point.x() + i < 0 or point.x() + i >= image.get_size().x() or
                                    point.y() + j < 0 or point.y() + j >= image.get_size().y())
                                    continue;

                                auto& current = out(point.x() + i, point.y() + j);

                                while (merge_map.find(current.x()) != merge_map.end())
                                    current.x() = merge_map.at(current.x());

                                if (current.x() == region_i)
                                    continue;

                                if (current.x() != 0)
                                {
                                    current = out(point.x() + i, point.y() + j);

                                    auto old_region = regions.at(current.x());
                                    auto old_region_color = regions.at(current.x()).color_sum;
                                    old_region_color /= old_region.n;
                                    if ((old_region.n < 5 and old_region.converged) or  // swallow small stagnant
                                        (dist(old_region_color, mean_color) < merge_upper_threshold // merge if similar
                                         and merge_map.find(current.x()) ==
                                             merge_map.end())) // but not if already merged with someone else
                                    {
                                        if (merge_map.find(region_i) != merge_map.end() and
                                            merge_map.at(region_i) == current.x())
                                            continue;

                                        for (auto& px : old_region.potential_neighbours)
                                            pair.second.potential_neighbours.insert(px);

                                        pair.second.color_sum += old_region.color_sum;
                                        pair.second.n += old_region.n;

                                        old_region.converged = true;
                                        old_region.potential_neighbours.clear();
                                        old_region.color_sum = Color(0, 0, 0);
                                        old_region.n = 0;

                                        n_converged++;
                                        merge_map.insert({current.x(), region_i});
                                        scheduled_for_deletion.insert(current.x());
                                        n_merged += 1;
                                    }
                                    else
                                        continue;
                                }

                                pair.second.potential_neighbours.insert(Vector2ui(point.x() + i, point.y() + j));
                            }
                        }
                    }
                }

                for (auto& point : checked_this_iteration)
                    pair.second.potential_neighbours.erase(point);

                if (pair.second.potential_neighbours.empty())
                {
                    pair.second.converged = true;
                    n_converged++;
                }
            }

            for (auto i : scheduled_for_deletion)
                regions.erase(i);

            scheduled_for_deletion.clear();
        }

        // post process away single pixel holes in regions
        std::vector<std::pair<Vector2ui, int>> to_fix;

        for (long x = 0; x < out.get_size().x(); ++x)
        {
            for (long y = 0; y < out.get_size().y(); ++y)
            {
                auto px = out(x, y);
                if (out(x, y).x() != 0)
                    continue;

                std::unordered_map<int, size_t> count;
                for (int i : {-1, 1, 0, 0})
                {
                    for (int j : {0, 0, -1, 1})
                    {
                        if (x + i < 0 or x + i >= out.get_size().x() or y + j < 0 or y + j >= out.get_size().y())
                            continue;

                        auto current_px = out(x + i, y + j);

                        if (count.find(current_px.x()) == count.end())
                            count.insert({current_px.x(), 0});

                        count.at(current_px.x()) += 1;
                    }
                }

                size_t max_count = 0;
                int which = 0;
                for (auto& pair : count)
                    if (pair.second > max_count)
                    {
                        max_count = pair.second;
                        which = pair.first;
                    }

                if (max_count > 4)
                    out(x, y) = which;
            }
        }

        size_t not_segmented = 0;
        for (auto& px : out)
        {
            if (px.x() == 0)
            {
                px = Color(1, 0, 1);
                not_segmented += 1;
                continue;
            }

            while (merge_map.find(px.x()) != merge_map.end())
                px.x() = merge_map.at(px.x());

            auto& region = regions.at(px.x());

            if (region.final_color == Color(-1, -1, -1))
            {
                region.final_color = region.color_sum;
                region.final_color /= region.n;
            }

            px = region.final_color;
        }

        std::cout << "[LOG] done. Merged " << n_merged << " clusters resulting in a segmentation with a total of "
                  << regions.size() << " clusters" << std::endl;
        return out;
    }

    GrayScaleImage region_growing_clustering(
            const GrayScaleImage& image,
            std::vector<Vector2ui> seeds,
            float add_upper_threshold,
            float merge_upper_threshold)
    {
                // citations:
        // https://ieeexplore.ieee.org/document/295913
        // https://sci-hub.se/https://www.sciencedirect.com/science/article/abs/pii/S0262885605000673

        std::cout << "[LOG] starting seeded region growing clustering with " << seeds.size() << " seeds." << std::endl;

        struct point_compare
        {
            bool operator()(const Vector2ui& a, const Vector2ui& b) const
            {
                return (a.x() == b.x()) and (a.y() == b.y());
            }
        };

        static size_t image_height = 0;
        image_height = image.get_size().y();
        struct point_hash
        {
            size_t operator()(const Vector2ui& a) const noexcept
            {
                return a.y() + image_height * a.x();
            }
        };

        using Set_t = std::unordered_set<Vector2ui, point_hash, point_compare>;

        struct Region
        {
            size_t n;
            float color_sum;
            Set_t potential_neighbours;
            bool converged = false;
            float final_color = -1;  // only used in post-processing
        };

        std::map<size_t, Region> regions;
        std::map<size_t, size_t> merge_map;
        size_t n_converged = 0;

        GrayScaleImage out; // value is region label
        out.create(image.get_size().x(), image.get_size().y(), 0);

        const size_t n_before_merge = regions.size();
        size_t n_merged = 0;
        size_t k = 1;
        for (auto& seed : seeds)
        {
            regions.emplace(k, Region{1, image(seed.x(), seed.y()), Set_t()});
            out(seed.x(), seed.y()) = k;
            size_t n_neighbouring_regions = 0;
            for (int i = -1; i <= 1; ++i)
            {
                for (int j = -1; j <= 1; ++j)
                {
                    if (seed.x() + i < 0 or seed.x() + i >= image.get_size().x() or
                        seed.y() + j < 0 or seed.y() + j >= image.get_size().y())
                        continue;

                    if (i == 0 or j == 0 and out(seed.x() + i, seed.y() + j) != 0)
                        n_neighbouring_regions += 1;

                    regions.at(k).potential_neighbours.insert(Vector2ui(seed.x() + i, seed.y() + j));
                }
            }

            k++;
        }

        auto color_distance = [](const float& a, const float& b) -> float {
            return abs(a - b);
        };

        size_t n_regions = regions.size();
        while (n_converged < n_regions)
        {
            std::set<size_t> scheduled_for_deletion;
            for (auto& pair : regions)
            {
                if (pair.second.converged or scheduled_for_deletion.find(pair.first) != scheduled_for_deletion.end())
                    continue;

                size_t region_i = pair.first;
                auto mean_color = pair.second.color_sum;
                mean_color /= pair.second.n;

                auto to_test = pair.second.potential_neighbours;
                std::vector<Vector2ui> checked_this_iteration;
                for (auto& point : to_test)
                {
                    auto new_color = image(point.x(), point.y());
                    auto distance = color_distance(new_color, mean_color);
                    checked_this_iteration.push_back(point);
                    if (distance < add_upper_threshold)
                    {
                        out(point.x(), point.y()) = region_i;

                        pair.second.n += 1;
                        pair.second.color_sum += new_color;

                        for (int i = -1; i <= 1; ++i)
                        {
                            for (int j = -1; j <= 1; ++j)
                            {
                                if (point.x() + i < 0 or point.x() + i >= image.get_size().x() or
                                    point.y() + j < 0 or point.y() + j >= image.get_size().y())
                                    continue;

                                auto& current = out(point.x() + i, point.y() + j);

                                while (merge_map.find(current) != merge_map.end())
                                    current = merge_map.at(current);

                                if (current == region_i)
                                    continue;

                                if (current != 0)
                                {
                                    auto old_region = regions.at(current);
                                    auto old_region_color = regions.at(current).color_sum;
                                    old_region_color /= old_region.n;
                                    if ((old_region.n < 5 and old_region.converged) or  // swallow small stagnant
                                        (abs(old_region_color - mean_color) < merge_upper_threshold // merge if similar
                                         and merge_map.find(current) ==
                                             merge_map.end())) // but not if already merged with someone else
                                    {
                                        if (merge_map.find(region_i) != merge_map.end() and
                                            merge_map.at(region_i) == current)
                                            continue;

                                        for (auto& px : old_region.potential_neighbours)
                                            pair.second.potential_neighbours.insert(px);

                                        pair.second.color_sum += old_region.color_sum;
                                        pair.second.n += old_region.n;

                                        old_region.converged = true;
                                        old_region.potential_neighbours.clear();
                                        old_region.color_sum = 0;
                                        old_region.n = 0;

                                        n_converged++;
                                        merge_map.insert({current, region_i});
                                        scheduled_for_deletion.insert(current);
                                        n_merged += 1;
                                    }
                                    else
                                        continue;
                                }

                                pair.second.potential_neighbours.insert(Vector2ui(point.x() + i, point.y() + j));
                            }
                        }
                    }
                }

                for (auto& point : checked_this_iteration)
                    pair.second.potential_neighbours.erase(point);

                if (pair.second.potential_neighbours.empty())
                {
                    pair.second.converged = true;
                    n_converged++;
                }
            }

            for (auto i : scheduled_for_deletion)
                regions.erase(i);

            scheduled_for_deletion.clear();
        }

        // post process away single pixel holes in regions
        std::vector<std::pair<Vector2ui, int>> to_fix;

        for (long x = 0; x < out.get_size().x(); ++x)
        {
            for (long y = 0; y < out.get_size().y(); ++y)
            {
                auto px = out(x, y);
                if (out(x, y) != 0)
                    continue;

                std::unordered_map<int, size_t> count;
                for (int i : {-1, 1, 0, 0})
                {
                    for (int j : {0, 0, -1, 1})
                    {
                        if (x + i < 0 or x + i >= out.get_size().x() or y + j < 0 or y + j >= out.get_size().y())
                            continue;

                        auto current_px = out(x + i, y + j);

                        if (count.find(current_px) == count.end())
                            count.insert({current_px, 0});

                        count.at(current_px) += 1;
                    }
                }

                size_t max_count = 0;
                int which = 0;
                for (auto& pair : count)
                    if (pair.second > max_count)
                    {
                        max_count = pair.second;
                        which = pair.first;
                    }

                if (max_count > 4)
                    out(x, y) = which;
            }
        }

        size_t not_segmented = 0;
        for (auto& px : out)
        {
            if (px == 0)
            {
                px = -1;
                not_segmented += 1;
                continue;
            }

            while (merge_map.find(px) != merge_map.end())
                px = merge_map.at(px);

            auto& region = regions.at(px);

            if (region.final_color == -1)
            {
                region.final_color = region.color_sum;
                region.final_color /= region.n;
            }

            px = region.final_color;
        }

        std::cout << "[LOG] done. Merged " << n_merged << " clusters resulting in a segmentation with a total of "
                  << regions.size() << " clusters" << std::endl;
        return out;
    }
    
    template<typename Image_t>
    Image_t region_growing_clustering(const Image_t& image, size_t n_seeds, float add_upper_threshold,
                                         float merge_upper_threshold)
    {
        float spacing = std::max(image.get_size().x(), image.get_size().y()) / sqrt(n_seeds);

        std::random_device device;
        std::mt19937 engine;
        engine.seed(device());

        std::vector<Vector2ui> seeds;
        int hs = ceil(spacing / 2);
        auto x_dist = std::uniform_int_distribution<int>(-hs + 1, +hs);
        auto y_dist = std::uniform_int_distribution<int>(-hs + 1, +hs);

        for (long x = hs; x < image.get_size().x(); x += 2 * hs)
        {
            for (long y = hs; y < image.get_size().y(); y += 2 * hs)
            {
                seeds.emplace_back(std::min<unsigned int>(x + x_dist(engine), image.get_size().x() - 1),
                                   std::min<unsigned int>(y + y_dist(engine), image.get_size().y() - 1));
            }
        }

        return region_growing_clustering(image, seeds, add_upper_threshold, merge_upper_threshold);
    }

    template<typename Image_t>
    Image_t region_growing_clustering(const Image_t& image, BinaryImage seed_image, float add_upper_threshold,
                                         float merge_upper_threshold)
    {
        std::vector<Vector2ui> seeds;

        for (long x = 0; x < seed_image.get_size().x(); ++x)
            for (long y = 0; y < seed_image.get_size().y(); ++y)
                if (seed_image(x, y))
                    seeds.push_back(Vector2ui(x, y));

        return region_growing_clustering(image, seeds, add_upper_threshold, merge_upper_threshold);
    }
}

