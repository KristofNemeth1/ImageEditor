#include "Pixel.hpp"

Pixel::Pixel(float r, float g, float b, float a)
    : A(a), R(r), G(g), B(b)
{
}

Pixel::Pixel(const Pixel& other)
    : A(other.A),
      R(other.R),
      G(other.G),
      B(other.B)
{
}

float Pixel::GetA() const
{
    return A;
}

float Pixel::GetR() const
{
    return R;
}

float Pixel::GetG() const
{
    return G;
}

float Pixel::GetB() const
{
    return B;
}

void Pixel::SetA(float a)
{
    A = a;
}

void Pixel::SetR(float r)
{
    R = r;
}

void Pixel::SetG(float g)
{
    G = g;
}

void Pixel::SetB(float b)
{
    B = b;
}

Pixel Pixel::operator+(const Pixel& other) const
{
    return Pixel(
        R + other.R,
        G + other.G,
        B + other.B,
        A + other.A
    );
}

Pixel Pixel::operator-(const Pixel& other) const
{
    return Pixel(
        R - other.R,
        G - other.G,
        B - other.B,
        A - other.A
    );
}

Pixel Pixel::operator*(const Pixel& other) const
{
    return Pixel(
        R * other.R,
        G * other.G,
        B * other.B,
        A * other.A
    );
}

Pixel Pixel::operator*(float scalar) const
{
    return Pixel(
        R * scalar,
        G * scalar,
        B * scalar,
        A * scalar
    );
}

Pixel Pixel::operator/(float scalar) const
{
    return Pixel(
        R / scalar,
        G / scalar,
        B / scalar,
        A / scalar
    );
}

Pixel& Pixel::operator=(const Pixel& other)
{
    if (this != &other)
    {
        A = other.A;
        R = other.R;
        G = other.G;
        B = other.B;
    }

    return *this;
}

Pixel& Pixel::operator+=(const Pixel& other)
{
    R += other.R;
    G += other.G;
    B += other.B;
    A += other.A;

    return *this;
}

Pixel& Pixel::operator-=(const Pixel& other)
{
    R -= other.R;
    G -= other.G;
    B -= other.B;
    A -= other.A;

    return *this;
}

Pixel& Pixel::operator*=(const Pixel& other)
{
    R *= other.R;
    G *= other.G;
    B *= other.B;
    A *= other.A;

    return *this;
}

Pixel& Pixel::operator*=(float scalar)
{
    R *= scalar;
    G *= scalar;
    B *= scalar;
    A *= scalar;

    return *this;
}

Pixel& Pixel::operator/=(float scalar)
{
    R /= scalar;
    G /= scalar;
    B /= scalar;
    A /= scalar;

    return *this;
}

Pixel operator*(float scalar, const Pixel& pixel)
{
    return pixel * scalar;
}