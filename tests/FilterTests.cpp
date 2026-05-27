#include "filter/Filter.hpp"

#include <gtest/gtest.h>

namespace
{
    constexpr float Epsilon = 0.000001f;

    struct TestSource
    {
        Image image;

        Image render() const
        {
            return image;
        }
    };

    Image MakeTestImage()
    {
        return Image(
            {
                Pixel(1.0f, 0.0f, 0.0f), // Red
                Pixel(0.0f, 1.0f, 0.0f), // Green
                Pixel(0.0f, 0.0f, 1.0f), // Blue
                Pixel(1.0f, 1.0f, 1.0f)  // White
            },
            2,
            2,
            3
        );
    }

    void ExpectPixelNear(
        const Pixel& pixel,
        float r,
        float g,
        float b,
        float a = 1.0f)
    {
        EXPECT_NEAR(pixel.GetR(), r, Epsilon);
        EXPECT_NEAR(pixel.GetG(), g, Epsilon);
        EXPECT_NEAR(pixel.GetB(), b, Epsilon);
        EXPECT_NEAR(pixel.GetA(), a, Epsilon);
    }

    TestSource MakeSource()
    {
        return TestSource{MakeTestImage()};
    }
}

TEST(FilterTests, InverseInvertsRgbAndKeepsAlpha)
{
    const Image result = Filter::Inverse(MakeSource()).render();

    ASSERT_EQ(result.GetX(), 2);
    ASSERT_EQ(result.GetY(), 2);
    ASSERT_EQ(result.pixels.size(), 4u);

    ExpectPixelNear(result.pixels[0], 0.0f, 1.0f, 1.0f);
    ExpectPixelNear(result.pixels[1], 1.0f, 0.0f, 1.0f);
    ExpectPixelNear(result.pixels[2], 1.0f, 1.0f, 0.0f);
    ExpectPixelNear(result.pixels[3], 0.0f, 0.0f, 0.0f);
}

TEST(FilterTests, GrayscaleUsesWeightedLuminance)
{
    const Image result = Filter::Grayscale(MakeSource()).render();

    ASSERT_EQ(result.pixels.size(), 4u);

    ExpectPixelNear(result.pixels[0], 0.299f, 0.299f, 0.299f);
    ExpectPixelNear(result.pixels[1], 0.587f, 0.587f, 0.587f);
    ExpectPixelNear(result.pixels[2], 0.114f, 0.114f, 0.114f);
    ExpectPixelNear(result.pixels[3], 1.0f, 1.0f, 1.0f);
}

TEST(FilterTests, BrightnessOffsetsRgbAndClampsToUnitRange)
{
    const Image result = Filter::Brightness(MakeSource(), 0.25f).render();

    ASSERT_EQ(result.pixels.size(), 4u);

    ExpectPixelNear(result.pixels[0], 1.0f, 0.25f, 0.25f);
    ExpectPixelNear(result.pixels[1], 0.25f, 1.0f, 0.25f);
    ExpectPixelNear(result.pixels[2], 0.25f, 0.25f, 1.0f);
    ExpectPixelNear(result.pixels[3], 1.0f, 1.0f, 1.0f);
}

TEST(FilterTests, ContrastScalesRgbAroundHalf)
{
    const Image result = Filter::Contrast(MakeSource(), 0.5f).render();

    ASSERT_EQ(result.pixels.size(), 4u);

    ExpectPixelNear(result.pixels[0], 0.75f, 0.25f, 0.25f);
    ExpectPixelNear(result.pixels[1], 0.25f, 0.75f, 0.25f);
    ExpectPixelNear(result.pixels[2], 0.25f, 0.25f, 0.75f);
    ExpectPixelNear(result.pixels[3], 0.75f, 0.75f, 0.75f);
}

TEST(FilterTests, ThresholdUsesWeightedLuminance)
{
    const Image result = Filter::Threshold(MakeSource(), 0.5f).render();

    ASSERT_EQ(result.pixels.size(), 4u);

    ExpectPixelNear(result.pixels[0], 0.0f, 0.0f, 0.0f);
    ExpectPixelNear(result.pixels[1], 1.0f, 1.0f, 1.0f);
    ExpectPixelNear(result.pixels[2], 0.0f, 0.0f, 0.0f);
    ExpectPixelNear(result.pixels[3], 1.0f, 1.0f, 1.0f);
}

TEST(FilterTests, ResizeUsesNearestNeighborSampling)
{
    const Image result = Filter::Resize(MakeSource(), 4, 4).render();

    ASSERT_EQ(result.GetX(), 4);
    ASSERT_EQ(result.GetY(), 4);
    ASSERT_EQ(result.pixels.size(), 16u);

    ExpectPixelNear(result.At(0, 0), 1.0f, 0.0f, 0.0f);
    ExpectPixelNear(result.At(2, 0), 0.0f, 1.0f, 0.0f);
    ExpectPixelNear(result.At(0, 2), 0.0f, 0.0f, 1.0f);
    ExpectPixelNear(result.At(2, 2), 1.0f, 1.0f, 1.0f);
}

TEST(FilterTests, BlurAveragesNeighboringPixels)
{
    const Image result = Filter::Blur(MakeSource(), 1).render();

    ASSERT_EQ(result.GetX(), 2);
    ASSERT_EQ(result.GetY(), 2);
    ASSERT_EQ(result.pixels.size(), 4u);

    for (const Pixel& pixel : result.pixels)
    {
        ExpectPixelNear(pixel, 0.5f, 0.5f, 0.5f);
    }
}
