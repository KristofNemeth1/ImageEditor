#include "Filter.hpp"
#include <cstdlib>
#include <cmath>

/*
Filter& Filter::operator=(const Filter& filter)
{
	if (&filter != this)
	{
		isActive = filter.isActive;
	}
	return *this;
}

std::ostream& operator<<(std::ostream& os, const Filter& filter) 
{
	os << "filter: " << filter.GetName() << std::endl;
	return os;
}

Filter::~Filter() 
{

}

void GrayScale::Apply(Image& img, const Image& original) const 
{
	for (int x = 0; x < img.GetX(); x++)
	{
		for (int y = 0; y < img.GetY(); y++)
		{
			uchar res = (original.pixels[x][y].GetR() + original.pixels[x][y].GetB() + original.pixels[x][y].GetG()) / 3;
			img.pixels[x][y] = Pixel(res, res, res);
		}
	}
}

void Invert::Apply(Image& img, const Image& original) const
{
	for (int x = 0; x < img.GetX(); x++)
	{
		for (int y = 0; y < img.GetY(); y++)
		{
			
			img.pixels[x][y] = Pixel(
				std::abs(original.pixels[x][y].GetR() - 255),
				std::abs(original.pixels[x][y].GetG() - 255),
				std::abs(original.pixels[x][y].GetB() - 255)		
			);
		}
	}
}

void Saturate::Apply(Image& img, const Image& original) const
{
	for (int x = 0; x < img.GetX(); x++)
	{
		for (int y = 0; y < img.GetY(); y++)
		{
			int r = original.pixels[x][y].GetR();
			int g = original.pixels[x][y].GetG();
			int b = original.pixels[x][y].GetB();

			double gray = 0.2989 * r + 0.5870 * g + 0.1140 * b;
			r = (int)( - gray * value + r * (1 + value));
			g = (int)( - gray * value + g * (1 + value));
			b = (int)( - gray * value + b * (1 + value));

			if (r > 255) r = 255;
			if (g > 255) g = 255;
			if (b > 255) b = 255;
			if (r < 0) r = 0;
			if (g < 0) g = 0;
			if (b < 0) b = 0;

			Pixel res = Pixel(r, g, b);
			img.pixels[x][y] = res;
		}
	}
}

void Brightness::Apply(Image& img, const Image& original) const 
{
	for (int x = 0; x < img.GetX(); x++)
	{
		for (int y = 0; y < img.GetY(); y++)
		{
			int r = original.pixels[x][y].GetR();
			int g = original.pixels[x][y].GetG();
			int b = original.pixels[x][y].GetB();

			r *= (int)std::pow(2, value);
			g *= (int)std::pow(2, value);
			b *= (int)std::pow(2, value);

			if (r > 255) r = 255;
			if (g > 255) g = 255;
			if (b > 255) b = 255;
			if (r < 0) r = 0;
			if (g < 0) g = 0;
			if (b < 0) b = 0;

			Pixel res = Pixel(r, g, b);

			img.pixels[x][y] = res;
		}
	}
}

void Color::Apply(Image& img, const Image& original, const Pixel p) const
{
	for (int x = 0; x < img.GetX(); x++)
	{
		for (int y = 0; y < img.GetY(); y++)
		{
			img.pixels[x][y] = Pixel(original.pixels[x][y].GetR() * p.GetR(), original.pixels[x][y].GetR() * p.GetR(), original.pixels[x][y].GetR() * p.GetR());
		}
	}
}


void BetterGrayScale::Apply(Image& img, const Image& original) const
{
	for (int x = 0; x < img.GetX(); x++)
	{
		for (int y = 0; y < img.GetY(); y++)
		{
			int res = (int)((0.3 * original.pixels[x][y].GetR() + 0.59 * original.pixels[x][y].GetB() + 0.11 * original.pixels[x][y].GetG()) / 3);
			img.pixels[x][y] = Pixel(res, res, res);
		}
	}
}*/