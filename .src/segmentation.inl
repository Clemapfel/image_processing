// 
// Copyright 2021 Clemens Cords
// Created on 18.08.21 by clem (mail@clemens-cords.com)
//

namespace crisp::Segmentation
{
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
            left_n = 0; left_sum = 0; right_n = 0; right_sum = 0;

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

        auto update = [&](size_t x, size_t y, size_t i)
        {
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

                ++ left;
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

        auto color_distance = [&](const Color& a, const Color& b) -> float
        {
            float out = 0;
            out += (a.x() - b.x()) * (a.x() - b.x());
            out += (a.y() - b.y()) * (a.y() - b.y());
            out += (a.z() - b.z()) * (a.z() - b.z());

            return out;
        };

        auto xy_distance = [&](const Vector2f& a, const Vector2f& b)
        {
            float out = 0;
            out += (a.x() - b.x()) * (a.x() - b.x());
            out += (a.y() - b.y()) * (a.y() - b.y());

            return out / (2*spacing*spacing);
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

                clusters.emplace(cluster_i, Cluster{Vector2f(new_x, new_y), image(new_x, new_y), Vector2f(new_x, new_y), 1});
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
                    if (center.x() + i < 0 or center.x() + i >= image.get_size().x() or center.y() + j < 0 or center.y() + j >= image.get_size().y())
                        continue;

                    n_pixels_visited += 1;

                    auto& current = out(center.x() + i, center.y() +j);
                    int old_i = current.x();
                    if (old_i == cluster_i)
                        continue;

                    auto pos = Vector2ui(center.x() + i, center.y() + j);
                    auto color = image(pos.x(), pos.y());
                    auto color_dist = color_distance(mean_color, color);
                    auto xy_dist = xy_distance(mean_pos, Vector2f(pos.x(), pos.y())) / (2*spacing);
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
            auto& next = clusters.at(i+1).final_color;

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

        auto color_distance = [&](float a, float b) -> float
        {
            return (a - b) * (a - b) * 3;
        };

        auto xy_distance = [&](const Vector2f& a, const Vector2f& b)
        {
            float out = 0;
            out += (a.x() - b.x()) * (a.x() - b.x());
            out += (a.y() - b.y()) * (a.y() - b.y());

            return out / (2*spacing*spacing);
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

                clusters.emplace(cluster_i, Cluster{Vector2f(new_x, new_y), image(new_x, new_y), Vector2f(new_x, new_y), 1});
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
                    if (center.x() + i < 0 or center.x() + i >= image.get_size().x() or center.y() + j < 0 or center.y() + j >= image.get_size().y())
                        continue;

                    n_pixels_visited += 1;

                    int old_i = out(center.x() + i, center.y() +j);
                    if (old_i == cluster_i)
                        continue;

                    auto pos = Vector2ui(center.x() + i, center.y() + j);
                    auto color = image(pos.x(), pos.y());
                    auto color_dist = color_distance(mean_color, color);
                    auto xy_dist = xy_distance(mean_pos, Vector2f(pos.x(), pos.y())) / (2*spacing);
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
            auto& next = clusters.at(i+1).final_color;

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

    ColorImage region_growing_clustering(const ColorImage& image, std::vector<Vector2ui> seeds)
    {
        // citations:
        // https://ieeexplore.ieee.org/document/295913
        // https://sci-hub.se/https://www.sciencedirect.com/science/article/abs/pii/S0262885605000673

        // iterate through regions in order of size, smallest first
        // if other region with same mean encountered, merge both
        // find all neighbor pixels of region, order by color-distance

        struct Region
        {
            size_t n;
            Color color_sum;
            std::vector<Vector2ui> potential_neighbours;
            bool converged = false;
            Color final_color = Color(-1, -1, -1);  // only used in post-processing
        };

        std::map<size_t, Region> regions;

        size_t k = 1;
        for (auto& seed : seeds)
        {
            regions.emplace(k, Region{1, image(seed.x(), seed.y()), std::vector<Vector2ui>()});
            for (int i = -1; i <= 1; ++i)
            {
                for (int j = -1; j <= 1; ++j)
                {
                    if (seed.x() + i < 0 or seed.x() + i >= image.get_size().x() or
                        seed.y() + j < 0 or seed.y() + j >= image.get_size().y())
                        continue;

                    regions.at(k).potential_neighbours.emplace_back(seed.x() + i, seed.y() + j);
                }
            }
        }

        auto color_distance = [](const Color& a, const Color& b) -> float {};
        float threshold = 0.5;

        ColorImage out; // .r is region label
        out.create(image.get_size().x(), image.get_size().y(), Color(0, -1, -1));

        size_t n_converged = 0;
        while (n_converged < regions.size())
        {
            for (auto& pair : regions)
            {
                size_t region_i = pair.first;
                auto mean_color = pair.second.color_sum;
                mean_color /= pair.second.n;

                auto to_test = pair.second.potential_neighbours;
                pair.second.potential_neighbours.clear();

                for (auto& point : to_test)
                {
                    auto new_color = image(point.x(), point.y());
                    if (color_distance(new_color, mean_color) < threshold)
                    {
                        out(point.x(), point.y()).x() = region_i;
                        pair.second.n += 1;
                        pair.second.color_sum += new_color;

                        for (int i = -1; i <= 1; ++i)
                        {
                            for (int j = -1; j <= 1; ++j)
                            {
                                if (point.x() + i < 0 or point.x() + i >= image.get_size().x() or
                                    point.y() + j < 0 or point.y() + j >= image.get_size().y())
                                    continue;

                                if (out(point.x(), point.y()).x() == region_i)
                                    continue;

                                if (out(point.x(), point.y()).x() != 0) // no merge for now
                                    continue;

                                pair.second.potential_neighbours.emplace_back(point.x() + i, point.y() + j);
                            }
                        }
                    }
                }
            }
        }

        for (auto& px : out)
        {
            if (px.x() == 0)
            {
                px = Color(0, 0, 0);
                continue;
            }

            auto& region = regions.at(px.x());

            if (region.final_color == Color(-1, -1, -1))
            {
                region.final_color = region.color_sum;
                region.color_sum /= n;
            }

            px = region.final_color;
        }

        return out;
    }

    ColorImage region_growing_clustering(const ColorImage& image, size_t n_seeds)
    {
        float spacing = sqrt(image.get_size().x() * image.get_size().y() / n_seeds);

        std::random_device device;
        std::mt19937 engine;
        engine.seed(device());

        std::vector<Vector2ui> seeds;

        for (long x = ceil(spacing / 2); x < image.get_size().x() - 1; x += spacing)
        {
            auto x_dist = std::uniform_int_distribution<int>(x - spacing/2, x + spacing/2);
            for (long y = ceil(spacing / 2); y < image.get_size().y() - 1; y += spacing)
            {
                auto y_dist = std::uniform_int_distribution<int>(y - spacing/2, y + spacing/2);
                seeds.emplace_back(x_dist(engine), y_dist(engine));
            }
        }

        return region_growing_clustering(image, seeds);
    }
}
