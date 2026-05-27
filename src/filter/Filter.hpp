#pragma once

#include "../Image.hpp"

#pragma once

#include "../Image.hpp"
#include "../Pixel.hpp"

#include <algorithm>
#include <utility>

namespace Filter
{
    struct InverseLogic
    {
        Image operator()(Image img) const
        {
            for (Pixel& pixel : img.pixels)
            {
                pixel.SetR(1.0f - pixel.GetR());
                pixel.SetG(1.0f - pixel.GetG());
                pixel.SetB(1.0f - pixel.GetB());
            }

            return img;
        }
    };

    struct GrayscaleLogic
    {
        Image operator()(Image img) const
        {
            for (Pixel& pixel : img.pixels)
            {
                const float gray =
                    0.299f * pixel.GetR() +
                    0.587f * pixel.GetG() +
                    0.114f * pixel.GetB();

                pixel.SetR(gray);
                pixel.SetG(gray);
                pixel.SetB(gray);
            }

            return img;
        }
    };

    struct BrightnessLogic
    {
        float amount;

        explicit BrightnessLogic(float amount)
            : amount(amount)
        {
        }

        Image operator()(Image img) const
        {
            for (Pixel& pixel : img.pixels)
            {
                pixel.SetR(std::clamp(pixel.GetR() + amount, 0.0f, 1.0f));
                pixel.SetG(std::clamp(pixel.GetG() + amount, 0.0f, 1.0f));
                pixel.SetB(std::clamp(pixel.GetB() + amount, 0.0f, 1.0f));
            }

            return img;
        }
    };

    struct ContrastLogic
    {
        float factor;

        explicit ContrastLogic(float factor)
            : factor(factor)
        {
        }

        Image operator()(Image img) const
        {
            for (Pixel& pixel : img.pixels)
            {
                pixel.SetR(std::clamp((pixel.GetR() - 0.5f) * factor + 0.5f, 0.0f, 1.0f));
                pixel.SetG(std::clamp((pixel.GetG() - 0.5f) * factor + 0.5f, 0.0f, 1.0f));
                pixel.SetB(std::clamp((pixel.GetB() - 0.5f) * factor + 0.5f, 0.0f, 1.0f));
            }

            return img;
        }
    };

    struct ThresholdLogic
    {
        float threshold;

        explicit ThresholdLogic(float threshold)
            : threshold(threshold)
        {
        }

        Image operator()(Image img) const
        {
            for (Pixel& pixel : img.pixels)
            {
                const float gray =
                    0.299f * pixel.GetR() +
                    0.587f * pixel.GetG() +
                    0.114f * pixel.GetB();

                const float value = gray >= threshold ? 1.0f : 0.0f;

                pixel.SetR(value);
                pixel.SetG(value);
                pixel.SetB(value);
            }

            return img;
        }
    };

    struct ResizeLogic
    {
        int newWidth;
        int newHeight;

        ResizeLogic(int newWidth, int newHeight)
            : newWidth(newWidth),
              newHeight(newHeight)
        {
        }

        Image operator()(const Image& img) const
        {
            Image result(newWidth, newHeight);

            for (int y = 0; y < newHeight; ++y)
            {
                for (int x = 0; x < newWidth; ++x)
                {
                    const int srcX = x * img.sizeX / newWidth;
                    const int srcY = y * img.sizeY / newHeight;

                    result.pixels[y * newWidth + x] =
                        img.pixels[srcY * img.sizeX + srcX];
                }
            }

            return result;
        }
    };

    struct BlurLogic
    {
        int radius;

        explicit BlurLogic(int radius)
            : radius(radius)
        {
        }

        Image operator()(const Image& img) const
        {
            Image result = img;

            for (int y = 0; y < img.sizeY; ++y)
            {
                for (int x = 0; x < img.sizeX; ++x)
                {
                    float r = 0.0f;
                    float g = 0.0f;
                    float b = 0.0f;
                    float a = 0.0f;
                    int count = 0;

                    for (int dy = -radius; dy <= radius; ++dy)
                    {
                        for (int dx = -radius; dx <= radius; ++dx)
                        {
                            const int nx = x + dx;
                            const int ny = y + dy;

                            if (nx < 0 || ny < 0 || nx >= img.sizeX || ny >= img.sizeY)
                                continue;

                            const Pixel& p = img.pixels[ny * img.sizeX + nx];

                            r += p.GetR();
                            g += p.GetG();
                            b += p.GetB();
                            a += p.GetA();
                            ++count;
                        }
                    }

                    Pixel& out = result.pixels[y * img.sizeX + x];

                    out.SetR(r / count);
                    out.SetG(g / count);
                    out.SetB(b / count);
                    out.SetA(a / count);
                }
            }

            return result;
        }
    };

    template <typename T, typename Logic>
	class FilterOp
	{
	private:
		T source;
		Logic logic;

	public:
		template <typename... Args>
		explicit FilterOp(T source, Args&&... args)
			: source(std::move(source)),
			logic(std::forward<Args>(args)...)
		{
		}

		Image render() const
		{
			return logic(source.render());
		}
	};

	template <typename T>
	struct Inverse : FilterOp<T, InverseLogic>
	{
		using FilterOp<T, InverseLogic>::FilterOp;
	};

	template <typename T>
	struct Grayscale : FilterOp<T, GrayscaleLogic>
	{
		using FilterOp<T, GrayscaleLogic>::FilterOp;
	};

	template <typename T>
	struct Brightness : FilterOp<T, BrightnessLogic>
	{
		using FilterOp<T, BrightnessLogic>::FilterOp;
	};

	template <typename T>
	struct Contrast : FilterOp<T, ContrastLogic>
	{
		using FilterOp<T, ContrastLogic>::FilterOp;
	};

	template <typename T>
	struct Threshold : FilterOp<T, ThresholdLogic>
	{
		using FilterOp<T, ThresholdLogic>::FilterOp;
	};

	template <typename T>
	struct Resize : FilterOp<T, ResizeLogic>
	{
		using FilterOp<T, ResizeLogic>::FilterOp;
	};

	template <typename T>
	struct Blur : FilterOp<T, BlurLogic>
	{
		using FilterOp<T, BlurLogic>::FilterOp;
	};

	template <typename T>
	Inverse(T) -> Inverse<T>;

	template <typename T>
	Grayscale(T) -> Grayscale<T>;

	template <typename T>
	Brightness(T, float) -> Brightness<T>;

	template <typename T>
	Contrast(T, float) -> Contrast<T>;

	template <typename T>
	Threshold(T, float) -> Threshold<T>;

	template <typename T>
	Resize(T, int, int) -> Resize<T>;

	template <typename T>
	Blur(T, int) -> Blur<T>;
}