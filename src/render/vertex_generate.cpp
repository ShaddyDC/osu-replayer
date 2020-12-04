#include "vertex_generate.h"

#include <Magnum/Math/Intersection.h>
#include <Magnum/Math/Matrix3.h>

inline float signed_area(const Magnum::Vector2 a, const Magnum::Vector2 b, const Magnum::Vector2 c)
{
    return (b.x() - a.x()) * (c.y() - a.y()) - (c.x() - a.x()) * (b.y() - a.y());
}

inline bool counter_clockwise(const Magnum::Vector2 a, const Magnum::Vector2 b, const Magnum::Vector2 c)
{
    return signed_area(a, b, c) > 0;
}

std::vector<Slider_vert> vertex_generate(const Slider_segment& slider, const float width)
{
    using namespace Magnum::Math;
    using namespace Magnum::Math::Literals;

    constexpr const auto angle_diff = Magnum::Rad{10._degf};

    constexpr const auto color_left = 1.f;
    constexpr const auto color_right = 0.f;

    constexpr auto depth_border = 1.f;
    constexpr auto depth_mid = 0.1f;

    std::vector<Slider_vert> output;


    // First segment
    {
        const auto a = slider[0];
        const auto b = lerp(slider[0], slider[1], 0.5f);

        const auto direction = (b - a).normalized();
        const auto side = direction.perpendicular();

        auto color_mid = Magnum::Math::lerp(color_left, color_right, 0.5f);

        output.emplace_back(Magnum::Vector3{a - width * side, depth_border}, color_left);
        output.emplace_back(Magnum::Vector3{a, depth_mid}, color_mid);
        output.emplace_back(Magnum::Vector3{b - width * side, depth_border}, color_left);

        output.emplace_back(Magnum::Vector3{b, depth_mid}, color_mid);
        output.emplace_back(Magnum::Vector3{a, depth_mid}, color_mid);
        output.emplace_back(Magnum::Vector3{b - width * side, depth_border}, color_left);


        output.emplace_back(Magnum::Vector3{a + width * side, depth_border}, color_right);
        output.emplace_back(Magnum::Vector3{a, depth_mid}, color_mid);
        output.emplace_back(Magnum::Vector3{b + width * side, depth_border}, color_right);

        output.emplace_back(Magnum::Vector3{b, depth_mid}, color_mid);
        output.emplace_back(Magnum::Vector3{a, depth_mid}, color_mid);
        output.emplace_back(Magnum::Vector3{b + width * side, depth_border}, color_right);
    }

    // Last Segment
    {
        const auto a = lerp(slider[slider.size() - 2], slider[slider.size() - 1], 0.5f);
        const auto b = slider[slider.size() - 1];

        const auto direction = (b - a).normalized();
        const auto side = direction.perpendicular();

        auto color_mid = Magnum::Math::lerp(color_left, color_right, 0.5f);

        output.emplace_back(Magnum::Vector3{a - width * side, depth_border}, color_left);
        output.emplace_back(Magnum::Vector3{a, depth_mid}, color_mid);
        output.emplace_back(Magnum::Vector3{b - width * side, depth_border}, color_left);

        output.emplace_back(Magnum::Vector3{b, depth_mid}, color_mid);
        output.emplace_back(Magnum::Vector3{a, depth_mid}, color_mid);
        output.emplace_back(Magnum::Vector3{b - width * side, depth_border}, color_left);


        output.emplace_back(Magnum::Vector3{a + width * side, depth_border}, color_right);
        output.emplace_back(Magnum::Vector3{a, depth_mid}, color_mid);
        output.emplace_back(Magnum::Vector3{b + width * side, depth_border}, color_right);

        output.emplace_back(Magnum::Vector3{b, depth_mid}, color_mid);
        output.emplace_back(Magnum::Vector3{a, depth_mid}, color_mid);
        output.emplace_back(Magnum::Vector3{b + width * side, depth_border}, color_right);

        for(auto angle = 0._degf; angle < 180._degf; angle += Magnum::Deg{angle_diff}) {
            output.emplace_back(Magnum::Vector3{b, depth_mid}, color_mid);
            output.emplace_back(Magnum::Vector3{
                                        b + width * Magnum::Matrix2x2{Magnum::Matrix3::rotation(-angle)} * side, depth_border},
                                color_left);
            output.emplace_back(Magnum::Vector3{
                                        b + width * Magnum::Matrix2x2{Magnum::Matrix3::rotation(-(angle + Magnum::Deg{angle_diff}))} *
                                                        side,
                                        depth_border},
                                color_left);
        }
    }

    for(std::size_t i = 2; i < slider.size(); ++i) {
        const auto a = lerp(slider[i - 2], slider[i - 1], 0.5);
        const auto b = slider[i - 1];
        const auto c = lerp(slider[i - 1], slider[i], 0.5);

        const auto direction_a = (b - a).normalized();
        const auto direction_c = (c - b).normalized();
        auto side_a = direction_a.perpendicular();
        auto side_c = direction_c.perpendicular();

        auto color_out = color_left;
        auto color_in = color_right;

        if(counter_clockwise(a, b, c)) {
            side_a = -side_a;
            side_c = -side_c;
            std::swap(color_out, color_in);
        }
        const auto color_mid = Magnum::Math::lerp(color_in, color_out, 0.5f);

        const auto [t, u] = Magnum::Math::Intersection::lineSegmentLineSegment(
                a - width * side_a, direction_a, c - width * side_c,
                direction_c);
        if(!std::isfinite(t) || !std::isfinite(u) || std::isnan(t) || std::isnan(u)) continue;//no intersection or whatever //Todo: Handle straight lines

        const auto inner = a - width * side_a + t * direction_a;


        // Slider consists of two incoming line segments and the outer arc (E)
        // The line segments are each split into two halves (A/B,C/D) each to allow setting the depth buffer to 1 in the middle and 0 at the borders
        // This allows to use the depth buffer to handle overlap

        // --------------
        //_____D_____|_E_|
        //     C    /|   |
        // ________/ |   |
        //        |A | B |
        //        |  |   |

        // A
        output.emplace_back(Magnum::Vector3{a - width * side_a, depth_border}, color_in);
        output.emplace_back(Magnum::Vector3{a, depth_mid}, color_mid);
        output.emplace_back(Magnum::Vector3{inner, depth_border}, color_in);

        output.emplace_back(Magnum::Vector3{b, depth_mid}, color_mid);
        output.emplace_back(Magnum::Vector3{a, depth_mid}, color_mid);
        output.emplace_back(Magnum::Vector3{inner, depth_border}, color_in);

        // B
        output.emplace_back(Magnum::Vector3{b, depth_mid}, color_mid);
        output.emplace_back(Magnum::Vector3{a, depth_mid}, color_mid);
        output.emplace_back(Magnum::Vector3{a + width * side_a, depth_border}, color_out);

        output.emplace_back(Magnum::Vector3{b, depth_mid}, color_mid);
        output.emplace_back(Magnum::Vector3{b + width * side_a, depth_border}, color_out);
        output.emplace_back(Magnum::Vector3{a + width * side_a, depth_border}, color_out);

        // C
        output.emplace_back(Magnum::Vector3{c - width * side_c, depth_border}, color_in);
        output.emplace_back(Magnum::Vector3{c, depth_mid}, color_mid);
        output.emplace_back(Magnum::Vector3{inner, depth_border}, color_in);

        output.emplace_back(Magnum::Vector3{b, depth_mid}, color_mid);
        output.emplace_back(Magnum::Vector3{c, depth_mid}, color_mid);
        output.emplace_back(Magnum::Vector3{inner, depth_border}, color_in);

        // D
        output.emplace_back(Magnum::Vector3{b, depth_mid}, color_mid);
        output.emplace_back(Magnum::Vector3{c, depth_mid}, color_mid);
        output.emplace_back(Magnum::Vector3{c + width * side_c, depth_border}, color_out);

        output.emplace_back(Magnum::Vector3{b, depth_mid}, color_mid);
        output.emplace_back(Magnum::Vector3{b + width * side_c, depth_border}, color_out);
        output.emplace_back(Magnum::Vector3{c + width * side_c, depth_border}, color_out);

        // Arc (E)
        if(counter_clockwise(a, b, c)) {
            const auto total_angle = angle(side_c, side_a);
            for(auto angle = Magnum::Rad{0._degf}; angle < total_angle; angle += angle_diff) {
                output.emplace_back(Magnum::Vector3{b, depth_mid}, color_mid);
                output.emplace_back(Magnum::Vector3{
                                            b + width * Magnum::Matrix2x2{Magnum::Matrix3::rotation(-angle)} * side_c, depth_border},
                                    color_out);
                output.emplace_back(Magnum::Vector3{
                                            b + width * Magnum::Matrix2x2{Magnum::Matrix3::rotation(-(angle + angle_diff))} *
                                                            side_c,
                                            depth_border},
                                    color_out);
            }
        } else {
            const auto total_angle = angle(side_a, side_c);
            for(auto angle = Magnum::Rad{0._degf}; angle < total_angle; angle += angle_diff) {
                output.emplace_back(Magnum::Vector3{b, depth_mid}, color_mid);
                output.emplace_back(Magnum::Vector3{
                                            b + width * Magnum::Matrix2x2{Magnum::Matrix3::rotation(-angle)} * side_a, depth_border},
                                    color_out);
                output.emplace_back(Magnum::Vector3{
                                            b + width * Magnum::Matrix2x2{Magnum::Matrix3::rotation(-(angle + angle_diff))} *
                                                            side_a,
                                            depth_border},
                                    color_out);
            }
        }
    }
    return output;
}

std::vector<Line_vert> line_generate(const std::vector<Magnum::Vector2>& points, const float width, const std::vector<Magnum::Color4>& colors)
{
    using namespace Magnum::Math::Literals;

    const auto multi_color = colors.size() >= points.size() - 1;
    const auto triangles_mesh = multi_color;
    const auto color_at = [multi_color, &colors](const std::size_t i) {
        if(multi_color) return colors[i];
        if(!colors.empty()) return colors[0];
        return Magnum::Color4::red();
    };

    std::vector<Line_vert> result{};

    for(std::size_t i = 2; i < points.size(); ++i) {
        // Handle first and last segment differently by not taking the mid point
        const auto a = i != 2 ? lerp(points[i - 2], points[i - 1], 0.5) : points[0];
        const auto b = points[i - 1];
        const auto c = i != points.size() - 1 ? lerp(points[i - 1], points[i], 0.5) : points[i];

        const auto direction_a = (b - a).normalized();
        const auto direction_c = (c - b).normalized();
        const auto angle = Magnum::Math::angle(direction_a, direction_c);

        auto side_a = direction_a.perpendicular();
        auto side_c = direction_c.perpendicular();

        // Make sure side_x points outwards
        if(counter_clockwise(a, b, c)) {
            side_a = -side_a;
            side_c = -side_c;
        }

        const auto [t, u] = Magnum::Math::Intersection::lineSegmentLineSegment(
                a - width / 2.f * side_a, direction_a,
                c - width / 2.f * side_c, direction_c);
        //no intersection or whatever //Todo: Handle straight lines
        if(!std::isfinite(t) || !std::isfinite(u) || std::isnan(t) || std::isnan(u)) continue;

        const auto inner = a - width / 2.f * side_a + t * direction_a;
        const auto outer = b + (b - inner);

        const auto result_emplace_if = [&result](const auto position, const auto color, const auto condition) {
            if(condition) result.emplace_back(position, color);
        };

        // For the first segment (i==2), the segment start has to be added
        if(counter_clockwise(a, b, c)) {
            if(triangles_mesh || i == 2) {
                result.emplace_back(a - width / 2.f * side_a, color_at(i - 2));
                result.emplace_back(a + width / 2.f * side_a, color_at(i - 2));
            }
            result.emplace_back(inner, color_at(i - 2));
            if(angle > Magnum::Rad{140._degf}) {//Todo: Keep inner from deviating too much
                result_emplace_if(a + width / 2.f * side_a, color_at(i - 2), triangles_mesh);
                result_emplace_if(inner, color_at(i - 2), triangles_mesh);
                result.emplace_back(b + width / 2.f * side_a, color_at(i - 2));

                result_emplace_if(b + width / 2.f * side_a, color_at(i - 2), triangles_mesh);
                result.emplace_back(inner, color_at(i - 1));
                result.emplace_back(b + width / 2.f * side_c, color_at(i - 1));

                result_emplace_if(inner, color_at(i - 1), triangles_mesh);
                result_emplace_if(b + width / 2.f * side_c, color_at(i - 1), triangles_mesh);
                result.emplace_back(c - width / 2.f * side_c, color_at(i - 1));

                result_emplace_if(b + width / 2.f * side_c, color_at(i - 1), triangles_mesh);
                result_emplace_if(c - width / 2.f * side_c, color_at(i - 1), triangles_mesh);
                result.emplace_back(c + width / 2.f * side_c, color_at(i - 1));
            } else {
                result_emplace_if(a + width / 2.f * side_a, color_at(i - 2), triangles_mesh);
                result_emplace_if(inner, color_at(i - 2), triangles_mesh);
                result.emplace_back(outer, color_at(i - 2));

                result_emplace_if(inner, color_at(i - 1), triangles_mesh);
                result_emplace_if(outer, color_at(i - 1), triangles_mesh);
                result.emplace_back(c - width / 2.f * side_c, color_at(i - 1));

                result_emplace_if(outer, color_at(i - 1), triangles_mesh);
                result_emplace_if(c - width / 2.f * side_c, color_at(i - 1), triangles_mesh);
                result.emplace_back(c + width / 2.f * side_c, color_at(i - 1));
            }
        } else {
            if(triangles_mesh || i == 2) {
                result.emplace_back(a + width / 2.f * side_a, color_at(i - 2));
                result.emplace_back(a - width / 2.f * side_a, color_at(i - 2));
            }
            if(angle > Magnum::Rad{140._degf}) {
                result.emplace_back(b + width / 2.f * side_a, color_at(i - 1));

                result_emplace_if(a - width / 2.f * side_a, color_at(i - 2), triangles_mesh);
                result_emplace_if(b + width / 2.f * side_a, color_at(i - 2), triangles_mesh);
                result.emplace_back(inner, color_at(i - 2));

                result_emplace_if(b + width / 2.f * side_a, color_at(i - 2), triangles_mesh);
                result.emplace_back(b + width / 2.f * side_c, color_at(i - 1));
                result.emplace_back(inner, color_at(i - 1));

                result_emplace_if(b + width / 2.f * side_c, color_at(i - 1), triangles_mesh);
                result_emplace_if(inner, color_at(i - 1), triangles_mesh);
                result.emplace_back(c + width / 2.f * side_c, color_at(i - 1));
            } else {
                result.emplace_back(outer, color_at(i - 2));

                result_emplace_if(a - width / 2.f * side_a, color_at(i - 2), triangles_mesh);
                result_emplace_if(outer, color_at(i - 2), triangles_mesh);
                result.emplace_back(inner, color_at(i - 2));

                result_emplace_if(outer, color_at(i - 1), triangles_mesh);
                result_emplace_if(inner, color_at(i - 1), triangles_mesh);
                result.emplace_back(c + width / 2.f * side_c, color_at(i - 1));
            }
            result_emplace_if(inner, color_at(i - 1), triangles_mesh);
            result_emplace_if(c + width / 2.f * side_c, color_at(i - 1), triangles_mesh);
            result.emplace_back(c - width / 2.f * side_c, color_at(i - 1));
        }
    }

    return result;
}
