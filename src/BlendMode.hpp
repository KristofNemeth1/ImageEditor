#pragma once

#include "Image.hpp"
#include "Pixel.hpp"

#include <algorithm>
#include <stdexcept>
#include <utility>

namespace BlendMode
{
    struct NormalLogic
    {
        Pixel operator()(const Pixel& top, const Pixel& bottom, float opacity) const;
    };

    struct MultiplyLogic
    {
        Pixel operator()(const Pixel& top, const Pixel& bottom, float opacity) const;
    };

    struct DivideLogic
    {
        Pixel operator()(const Pixel& top, const Pixel& bottom, float opacity) const;
    };

    struct AddLogic
    {
        Pixel operator()(const Pixel& top, const Pixel& bottom, float opacity) const;
    };

    struct SubtractLogic
    {
        Pixel operator()(const Pixel& top, const Pixel& bottom, float opacity) const;
    };

    template <typename T1, typename T2, typename Op>
    class BlendOp
    {
    private:
        T1 top;
        T2 bottom;
        float opacity;
        Op op;

    public:
        BlendOp(T1 top, T2 bottom, float opacity = 1.0f)
            : top(std::move(top)),
              bottom(std::move(bottom)),
              opacity(opacity)
        {
        }

        Image render() const
        {
            Image topImg = top.render();
            Image bottomImg = bottom.render();

            const int blendWidth = std::min(topImg.sizeX, bottomImg.sizeX);
            const int blendHeight = std::min(topImg.sizeY, bottomImg.sizeY);

            for (int y = 0; y < blendHeight; ++y)
            {
                for (int x = 0; x < blendWidth; ++x)
                {
                    const size_t index = y * bottomImg.sizeX + x;

                    bottomImg.pixels[index] =
                        op(topImg.pixels[y * topImg.sizeX + x],
                        bottomImg.pixels[index],
                        opacity);
                }
            }

            return bottomImg;
        }
    };

    template <typename T1, typename T2>
    using Normal = BlendOp<T1, T2, NormalLogic>;

    template <typename T1, typename T2>
    using Divide = BlendOp<T1, T2, DivideLogic>;

    template <typename T1, typename T2>
    using Subtract = BlendOp<T1, T2, SubtractLogic>;

    // Syntactic Sugar / Clean Typenames using CTAD (C++17) or helper functions
    template <typename T1, typename T2>
    struct Multiply : BlendOp<T1, T2, MultiplyLogic> {
        using BlendOp<T1, T2, MultiplyLogic>::BlendOp;
    };

    template <typename T1, typename T2>
    struct Add : BlendOp<T1, T2, AddLogic> {
        using BlendOp<T1, T2, AddLogic>::BlendOp;
    };
    
    template <typename T1, typename T2>
    Multiply(T1, T2, int) -> Multiply<T1, T2>;

    template <typename T1, typename T2>
    Add(T1, T2, int) -> Add<T1, T2>;
}
