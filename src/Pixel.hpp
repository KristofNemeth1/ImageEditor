#pragma once

#pragma once

class Pixel
{
protected:
    float A;
    float R;
    float G;
    float B;

public:
    Pixel(float r = 1.0f,
          float g = 1.0f,
          float b = 1.0f,
          float a = 1.0f);

    Pixel(const Pixel& other);

    ~Pixel() = default;

    float GetA() const;
    float GetR() const;
    float GetG() const;
    float GetB() const;

    void SetA(float a);
    void SetR(float r);
    void SetG(float g);
    void SetB(float b);

    Pixel operator+(const Pixel& other) const;
    Pixel operator-(const Pixel& other) const;
    Pixel operator*(const Pixel& other) const;

    Pixel operator*(float scalar) const;
    Pixel operator/(float scalar) const;

    Pixel& operator=(const Pixel& other);

    Pixel& operator+=(const Pixel& other);
    Pixel& operator-=(const Pixel& other);
    Pixel& operator*=(const Pixel& other);

    Pixel& operator*=(float scalar);
    Pixel& operator/=(float scalar);
};

Pixel operator*(float scalar, const Pixel& pixel);