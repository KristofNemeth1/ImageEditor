#pragma once

#include "Pixel.hpp"

#include <cstddef>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

class Image
{
protected:


public:
	std::vector<Pixel> pixels;
	int sizeX;
	int sizeY;
	int channels;

	enum class Format {
		Auto,
		Png,
		Jpg,
		Bmp,
		Tga
	};

	Image(int sizeX = 0, int sizeY = 0, int channels = 3);
	Image(const Image& img);
	Image(Image&& img) noexcept = default;
	Image(const char* FileName);
	explicit Image(const std::string& fileName);
	Image(const unsigned char* encodedData, std::size_t dataSize, int desiredChannels = 0);

	void debug_print(const std::string& name) const {
        std::endl(std::cout << "Rendering Layer: " << name);
    }

	int GetX() const { return sizeX; }
	int GetY() const { return sizeY; }
	int GetChannels() const { return channels; }

	Image& operator=(const Image& img);
	Image& operator=(Image&& img) noexcept = default;

	Pixel& At(int x, int y);
	const Pixel& At(int x, int y) const;

	static Image FromFile(const std::string& fileName, int desiredChannels = 0);
	static Image FromMemory(const unsigned char* encodedData, std::size_t dataSize, int desiredChannels = 0);
	static Image FromEncodedMemory(const unsigned char* encodedData, std::size_t dataSize, int desiredChannels = 0);
	static Image FromRawMemory(
		const unsigned char* rawPixels,
		int width,
		int height,
		int channels,
		std::size_t rowStrideBytes = 0
	);

	bool LoadFromFile(const std::string& fileName, int desiredChannels = 0);
	bool LoadFromMemory(const unsigned char* encodedData, std::size_t dataSize, int desiredChannels = 0);
	bool LoadFromEncodedMemory(const unsigned char* encodedData, std::size_t dataSize, int desiredChannels = 0);
	bool LoadFromRawMemory(
		const unsigned char* rawPixels,
		int width,
		int height,
		int channels,
		std::size_t rowStrideBytes = 0
	);

	std::vector<unsigned char> ToRawMemory() const;
	void CopyToRawMemory(unsigned char* output, std::size_t outputSize, std::size_t rowStrideBytes = 0) const;
	std::vector<unsigned char> ExportToMemory(Format format = Format::Png, int quality = 100) const;

	void Clear();
	void Normalize();
	void Export(const char* FileName, Format format = Format::Auto, int quality = 100) const;
	void Export(const std::string& fileName, Format format = Format::Auto, int quality = 100) const;
	~Image();
};

class SourceImage {
private:
    std::string name;
public:
    SourceImage(std::string name) : name(std::move(name)) {}

    Image render() const {
        Image img(name);
        img.debug_print(name);
        return img;
    }
};

std::ostream& operator<<(std::ostream& os, const Image& img);


