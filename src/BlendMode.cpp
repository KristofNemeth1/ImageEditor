#include "BlendMode.hpp"

namespace BlendMode
{
    float Clamp01(float value)
    {
        return std::clamp(value, 0.0f, 1.0f);
    }

    Pixel NormalPixel(
        const Pixel& top,
        const Pixel& bottom,
        float opacity)
    {
        return bottom * (1.0f - opacity) +
               top * opacity;
    }

    Pixel MultiplyPixel(
        const Pixel& top,
        const Pixel& bottom,
        float opacity)
    {
        Pixel blended = top * bottom;

        return NormalPixel(
            blended,
            bottom,
            opacity);
    }

    Pixel AddPixel(
        const Pixel& top,
        const Pixel& bottom,
        float opacity)
    {
        Pixel blended(
            Clamp01(top.GetR() + bottom.GetR()),
            Clamp01(top.GetG() + bottom.GetG()),
            Clamp01(top.GetB() + bottom.GetB()),
            Clamp01(top.GetA() + bottom.GetA()));

        return NormalPixel(
            blended,
            bottom,
            opacity);
    }

    Pixel SubtractPixel(
        const Pixel& top,
        const Pixel& bottom,
        float opacity)
    {
        Pixel blended(
            Clamp01(bottom.GetR() - top.GetR()),
            Clamp01(bottom.GetG() - top.GetG()),
            Clamp01(bottom.GetB() - top.GetB()),
            Clamp01(bottom.GetA() - top.GetA()));

        return NormalPixel(
            blended,
            bottom,
            opacity);
    }

    Pixel DividePixel(
        const Pixel& top,
        const Pixel& bottom,
        float opacity)
    {
        constexpr float epsilon = 0.00001f;

        Pixel blended(
            Clamp01(bottom.GetR() /
                     std::max(top.GetR(), epsilon)),

            Clamp01(bottom.GetG() /
                     std::max(top.GetG(), epsilon)),

            Clamp01(bottom.GetB() /
                     std::max(top.GetB(), epsilon)),

            bottom.GetA());

        return NormalPixel(
            blended,
            bottom,
            opacity);
    }

    Pixel NormalLogic::operator()(
        const Pixel& top,
        const Pixel& bottom,
        float opacity) const
    {
        return NormalPixel(
            top,
            bottom,
            opacity);
    }

    Pixel MultiplyLogic::operator()(
        const Pixel& top,
        const Pixel& bottom,
        float opacity) const
    {
        return MultiplyPixel(
            top,
            bottom,
            opacity);
    }

    Pixel DivideLogic::operator()(
        const Pixel& top,
        const Pixel& bottom,
        float opacity) const
    {
        return DividePixel(
            top,
            bottom,
            opacity);
    }

    Pixel AddLogic::operator()(
        const Pixel& top,
        const Pixel& bottom,
        float opacity) const
    {
        return AddPixel(
            top,
            bottom,
            opacity);
    }

    Pixel SubtractLogic::operator()(
        const Pixel& top,
        const Pixel& bottom,
        float opacity) const
    {
        return SubtractPixel(
            top,
            bottom,
            opacity);
    }
}