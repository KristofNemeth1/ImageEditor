#include "Image.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>

namespace {

void validateChannelCount(int channelCount)
{
	if (channelCount < 1 || channelCount > 4) {
		throw std::invalid_argument("Image channel count must be between 1 and 4.");
	}
}

int normalizeDesiredChannels(int desiredChannels)
{
	if (desiredChannels == 0) {
		return 0;
	}

	validateChannelCount(desiredChannels);
	return desiredChannels;
}

std::size_t checkedPixelCount(int width, int height)
{
	if (width < 0 || height < 0) {
		throw std::invalid_argument("Image dimensions cannot be negative.");
	}

	const std::size_t w = static_cast<std::size_t>(width);
	const std::size_t h = static_cast<std::size_t>(height);
	if (w != 0 && h > std::numeric_limits<std::size_t>::max() / w) {
		throw std::overflow_error("Image dimensions are too large.");
	}

	return w * h;
}

std::size_t checkedRowBytes(int width, int channelCount)
{
	validateChannelCount(channelCount);

	const std::size_t w = static_cast<std::size_t>(width);
	const std::size_t c = static_cast<std::size_t>(channelCount);
	if (w != 0 && c > std::numeric_limits<std::size_t>::max() / w) {
		throw std::overflow_error("Image row is too large.");
	}

	return w * c;
}

std::size_t checkedRequiredBytes(int height, std::size_t rowStrideBytes, std::size_t packedRowBytes)
{
	if (height <= 0) {
		return 0;
	}

	const std::size_t rowsBeforeLast = static_cast<std::size_t>(height - 1);
	if (rowsBeforeLast != 0 && rowStrideBytes > std::numeric_limits<std::size_t>::max() / rowsBeforeLast) {
		throw std::overflow_error("Image buffer stride is too large.");
	}

	const std::size_t bytesBeforeLastRow = rowStrideBytes * rowsBeforeLast;
	if (packedRowBytes > std::numeric_limits<std::size_t>::max() - bytesBeforeLastRow) {
		throw std::overflow_error("Image buffer is too large.");
	}

	return bytesBeforeLastRow + packedRowBytes;
}

int checkedStbStride(int width, int channelCount)
{
	const std::size_t stride = checkedRowBytes(width, channelCount);
	if (stride > static_cast<std::size_t>(std::numeric_limits<int>::max())) {
		throw std::overflow_error("Image row is too large for stb.");
	}

	return static_cast<int>(stride);
}

void validateImageShape(int width, int height, int channelCount)
{
	validateChannelCount(channelCount);
	checkedPixelCount(width, height);
}

float byteToFloat(unsigned char value)
{
	return static_cast<float>(value) / 255.0f;
}

float normalizeChannel(float value)
{
	if (!std::isfinite(value)) {
		return 0.0f;
	}

	if (value > 1.0f) {
		value /= 255.0f;
	}

	return std::clamp(value, 0.0f, 1.0f);
}

unsigned char channelToByte(float value)
{
	return static_cast<unsigned char>(std::lround(normalizeChannel(value) * 255.0f));
}

void pixelToRawBytes(const Pixel& pixel, int channelCount, unsigned char* output)
{
	switch (channelCount) {
	case 1:
		output[0] = channelToByte(pixel.GetR());
		break;
	case 2:
		output[0] = channelToByte(pixel.GetR());
		output[1] = channelToByte(pixel.GetA());
		break;
	case 3:
		output[0] = channelToByte(pixel.GetR());
		output[1] = channelToByte(pixel.GetG());
		output[2] = channelToByte(pixel.GetB());
		break;
	case 4:
		output[0] = channelToByte(pixel.GetR());
		output[1] = channelToByte(pixel.GetG());
		output[2] = channelToByte(pixel.GetB());
		output[3] = channelToByte(pixel.GetA());
		break;
	default:
		validateChannelCount(channelCount);
	}
}

std::string lowerExtension(const std::string& fileName)
{
	const std::size_t dotPosition = fileName.find_last_of('.');
	if (dotPosition == std::string::npos) {
		return {};
	}

	std::string extension = fileName.substr(dotPosition + 1);
	std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char value) {
		return static_cast<char>(std::tolower(value));
	});
	return extension;
}

Image::Format formatFromExtension(const std::string& fileName)
{
	const std::string extension = lowerExtension(fileName);

	if (extension == "png") {
		return Image::Format::Png;
	}
	if (extension == "jpg" || extension == "jpeg") {
		return Image::Format::Jpg;
	}
	if (extension == "bmp") {
		return Image::Format::Bmp;
	}
	if (extension == "tga") {
		return Image::Format::Tga;
	}

	throw std::invalid_argument("Unsupported image export extension: " + extension);
}

int clampJpegQuality(int quality)
{
	return std::clamp(quality, 1, 100);
}

std::string loadFailureMessage(const std::string& source)
{
	const char* reason = stbi_failure_reason();
	if (reason == nullptr) {
		return "Failed to load image from " + source + ".";
	}

	return "Failed to load image from " + source + ": " + reason;
}

void appendWriteData(void* context, void* data, int size)
{
	if (size <= 0) {
		return;
	}

	auto* output = static_cast<std::vector<unsigned char>*>(context);
	auto* first = static_cast<unsigned char*>(data);
	output->insert(output->end(), first, first + size);
}

void requireExportable(const Image& image)
{
	const std::size_t expectedPixels = checkedPixelCount(image.GetX(), image.GetY());
	validateChannelCount(image.GetChannels());

	if (image.GetX() == 0 || image.GetY() == 0 || expectedPixels == 0) {
		throw std::runtime_error("Cannot export an empty image.");
	}
	if (image.pixels.size() != expectedPixels) {
		throw std::runtime_error("Image pixel storage does not match image dimensions.");
	}
}

} // namespace

Image::Image(int sizeX, int sizeY, int channels)
	: sizeX(sizeX), sizeY(sizeY), channels(channels)
{
	validateImageShape(sizeX, sizeY, channels);
	pixels.resize(checkedPixelCount(sizeX, sizeY));
}

Image::Image(const Image& img)
	: sizeX(img.sizeX), sizeY(img.sizeY), channels(img.channels), pixels(img.pixels)
{
}

Image::Image(const char* FileName)
	: Image()
{
	if (FileName == nullptr) {
		throw std::invalid_argument("Image file name cannot be null.");
	}

	LoadFromFile(FileName);
}

Image::Image(const std::string& fileName)
	: Image()
{
	LoadFromFile(fileName);
}

Image::Image(const unsigned char* encodedData, std::size_t dataSize, int desiredChannels)
	: Image()
{
	LoadFromEncodedMemory(encodedData, dataSize, desiredChannels);
}

std::ostream& operator<<(std::ostream& os, const Image& img)
{
	os << "x: " << img.GetX() << " y: " << img.GetY() << " channels: " << img.GetChannels();
	return os;
}

Image::~Image() = default;

Image& Image::operator=(const Image& img)
{
	if (this != &img) {
		sizeX = img.sizeX;
		sizeY = img.sizeY;
		channels = img.channels;
		pixels = img.pixels;
	}

	return *this;
}

Pixel& Image::At(int x, int y)
{
	if (x < 0 || y < 0 || x >= sizeX || y >= sizeY) {
		throw std::out_of_range("Pixel coordinates are outside the image.");
	}

	return pixels[static_cast<std::size_t>(y) * static_cast<std::size_t>(sizeX) + static_cast<std::size_t>(x)];
}

const Pixel& Image::At(int x, int y) const
{
	if (x < 0 || y < 0 || x >= sizeX || y >= sizeY) {
		throw std::out_of_range("Pixel coordinates are outside the image.");
	}

	return pixels[static_cast<std::size_t>(y) * static_cast<std::size_t>(sizeX) + static_cast<std::size_t>(x)];
}

Image Image::FromFile(const std::string& fileName, int desiredChannels)
{
	Image image;
	image.LoadFromFile(fileName, desiredChannels);
	return image;
}

Image Image::FromMemory(const unsigned char* encodedData, std::size_t dataSize, int desiredChannels)
{
	return FromEncodedMemory(encodedData, dataSize, desiredChannels);
}

Image Image::FromEncodedMemory(const unsigned char* encodedData, std::size_t dataSize, int desiredChannels)
{
	Image image;
	image.LoadFromEncodedMemory(encodedData, dataSize, desiredChannels);
	return image;
}

Image Image::FromRawMemory(
	const unsigned char* rawPixels,
	int width,
	int height,
	int channels,
	std::size_t rowStrideBytes)
{
	Image image;
	image.LoadFromRawMemory(rawPixels, width, height, channels, rowStrideBytes);
	return image;
}

bool Image::LoadFromFile(const std::string& fileName, int desiredChannels)
{
	if (fileName.empty()) {
		throw std::invalid_argument("Image file name cannot be empty.");
	}

	const int requestedChannels = normalizeDesiredChannels(desiredChannels);
	int width = 0;
	int height = 0;
	int fileChannels = 0;

	std::unique_ptr<stbi_uc, decltype(&stbi_image_free)> decoded(
		stbi_load(fileName.c_str(), &width, &height, &fileChannels, requestedChannels),
		stbi_image_free
	);

	if (!decoded) {
		throw std::runtime_error(loadFailureMessage(fileName));
	}

	const int finalChannels = requestedChannels == 0 ? fileChannels : requestedChannels;
	return LoadFromRawMemory(decoded.get(), width, height, finalChannels);
}

bool Image::LoadFromMemory(const unsigned char* encodedData, std::size_t dataSize, int desiredChannels)
{
	return LoadFromEncodedMemory(encodedData, dataSize, desiredChannels);
}

bool Image::LoadFromEncodedMemory(const unsigned char* encodedData, std::size_t dataSize, int desiredChannels)
{
	if (encodedData == nullptr || dataSize == 0) {
		throw std::invalid_argument("Encoded image memory cannot be null or empty.");
	}
	if (dataSize > static_cast<std::size_t>(std::numeric_limits<int>::max())) {
		throw std::invalid_argument("Encoded image memory is too large for stb_image.");
	}

	const int requestedChannels = normalizeDesiredChannels(desiredChannels);
	int width = 0;
	int height = 0;
	int fileChannels = 0;

	std::unique_ptr<stbi_uc, decltype(&stbi_image_free)> decoded(
		stbi_load_from_memory(
			encodedData,
			static_cast<int>(dataSize),
			&width,
			&height,
			&fileChannels,
			requestedChannels
		),
		stbi_image_free
	);

	if (!decoded) {
		throw std::runtime_error(loadFailureMessage("memory"));
	}

	const int finalChannels = requestedChannels == 0 ? fileChannels : requestedChannels;
	return LoadFromRawMemory(decoded.get(), width, height, finalChannels);
}

bool Image::LoadFromRawMemory(
	const unsigned char* rawPixels,
	int width,
	int height,
	int channelCount,
	std::size_t rowStrideBytes)
{
	validateImageShape(width, height, channelCount);

	const std::size_t pixelCount = checkedPixelCount(width, height);
	const std::size_t packedRowBytes = checkedRowBytes(width, channelCount);
	if (rowStrideBytes == 0) {
		rowStrideBytes = packedRowBytes;
	}
	if (rowStrideBytes < packedRowBytes && height > 0) {
		throw std::invalid_argument("Input row stride is smaller than the packed row size.");
	}
	checkedRequiredBytes(height, rowStrideBytes, packedRowBytes);
	if (pixelCount > 0 && rawPixels == nullptr) {
		throw std::invalid_argument("Raw image memory cannot be null.");
	}

	std::vector<Pixel> loadedPixels(pixelCount);

	for (int y = 0; y < height; ++y) {
		const unsigned char* row = rawPixels + static_cast<std::size_t>(y) * rowStrideBytes;
		for (int x = 0; x < width; ++x) {
			const unsigned char* source = row + static_cast<std::size_t>(x) * static_cast<std::size_t>(channelCount);
			Pixel pixel;

			switch (channelCount) {
			case 1:
				pixel = Pixel(byteToFloat(source[0]), byteToFloat(source[0]), byteToFloat(source[0]), 1.0f);
				break;
			case 2:
				pixel = Pixel(byteToFloat(source[0]), byteToFloat(source[0]), byteToFloat(source[0]), byteToFloat(source[1]));
				break;
			case 3:
				pixel = Pixel(byteToFloat(source[0]), byteToFloat(source[1]), byteToFloat(source[2]), 1.0f);
				break;
			case 4:
				pixel = Pixel(byteToFloat(source[0]), byteToFloat(source[1]), byteToFloat(source[2]), byteToFloat(source[3]));
				break;
			default:
				validateChannelCount(channelCount);
			}

			loadedPixels[static_cast<std::size_t>(y) * static_cast<std::size_t>(width) + static_cast<std::size_t>(x)] = pixel;
		}
	}

	sizeX = width;
	sizeY = height;
	channels = channelCount;
	pixels = std::move(loadedPixels);

	return true;
}

std::vector<unsigned char> Image::ToRawMemory() const
{
	const std::size_t packedRowBytes = checkedRowBytes(sizeX, channels);
	const std::size_t requiredBytes = checkedRequiredBytes(sizeY, packedRowBytes, packedRowBytes);

	std::vector<unsigned char> raw(requiredBytes);
	CopyToRawMemory(raw.data(), raw.size());
	return raw;
}

void Image::CopyToRawMemory(unsigned char* output, std::size_t outputSize, std::size_t rowStrideBytes) const
{
	validateImageShape(sizeX, sizeY, channels);

	const std::size_t pixelCount = checkedPixelCount(sizeX, sizeY);
	if (pixels.size() != pixelCount) {
		throw std::runtime_error("Image pixel storage does not match image dimensions.");
	}

	const std::size_t packedRowBytes = checkedRowBytes(sizeX, channels);
	if (rowStrideBytes == 0) {
		rowStrideBytes = packedRowBytes;
	}
	if (rowStrideBytes < packedRowBytes && sizeY > 0) {
		throw std::invalid_argument("Output row stride is smaller than the packed row size.");
	}

	const std::size_t requiredBytes = checkedRequiredBytes(sizeY, rowStrideBytes, packedRowBytes);

	if (requiredBytes > 0 && output == nullptr) {
		throw std::invalid_argument("Output memory cannot be null.");
	}
	if (outputSize < requiredBytes) {
		throw std::invalid_argument("Output buffer is too small for image pixels.");
	}

	for (int y = 0; y < sizeY; ++y) {
		unsigned char* row = output + static_cast<std::size_t>(y) * rowStrideBytes;
		for (int x = 0; x < sizeX; ++x) {
			pixelToRawBytes(
				pixels[static_cast<std::size_t>(y) * static_cast<std::size_t>(sizeX) + static_cast<std::size_t>(x)],
				channels,
				row + static_cast<std::size_t>(x) * static_cast<std::size_t>(channels)
			);
		}
	}
}

std::vector<unsigned char> Image::ExportToMemory(Format format, int quality) const
{
	requireExportable(*this);

	if (format == Format::Auto) {
		format = Format::Png;
	}

	std::vector<unsigned char> raw = ToRawMemory();
	std::vector<unsigned char> encoded;
	const int stride = checkedStbStride(sizeX, channels);

	int result = 0;
	switch (format) {
	case Format::Png:
		result = stbi_write_png_to_func(appendWriteData, &encoded, sizeX, sizeY, channels, raw.data(), stride);
		break;
	case Format::Jpg:
		result = stbi_write_jpg_to_func(appendWriteData, &encoded, sizeX, sizeY, channels, raw.data(), clampJpegQuality(quality));
		break;
	case Format::Bmp:
		result = stbi_write_bmp_to_func(appendWriteData, &encoded, sizeX, sizeY, channels, raw.data());
		break;
	case Format::Tga:
		result = stbi_write_tga_to_func(appendWriteData, &encoded, sizeX, sizeY, channels, raw.data());
		break;
	case Format::Auto:
		break;
	}

	if (result == 0) {
		throw std::runtime_error("Failed to export image to memory.");
	}

	return encoded;
}

void Image::Clear()
{
	sizeX = 0;
	sizeY = 0;
	channels = 0;
	pixels.clear();
}

void Image::Normalize()
{
	for (Pixel& pixel : pixels) {
		pixel.SetR(normalizeChannel(pixel.GetR()));
		pixel.SetG(normalizeChannel(pixel.GetG()));
		pixel.SetB(normalizeChannel(pixel.GetB()));
		pixel.SetA(normalizeChannel(pixel.GetA()));
	}
}

void Image::Export(const char* FileName, Format format, int quality) const
{
	if (FileName == nullptr) {
		throw std::invalid_argument("Image file name cannot be null.");
	}

	Export(std::string(FileName), format, quality);
}

void Image::Export(const std::string& fileName, Format format, int quality) const
{
	if (fileName.empty()) {
		throw std::invalid_argument("Image file name cannot be empty.");
	}

	requireExportable(*this);

	if (format == Format::Auto) {
		format = formatFromExtension(fileName);
	}

	std::vector<unsigned char> raw = ToRawMemory();
	const int stride = checkedStbStride(sizeX, channels);

	int result = 0;
	switch (format) {
	case Format::Png:
		result = stbi_write_png(fileName.c_str(), sizeX, sizeY, channels, raw.data(), stride);
		break;
	case Format::Jpg:
		result = stbi_write_jpg(fileName.c_str(), sizeX, sizeY, channels, raw.data(), clampJpegQuality(quality));
		break;
	case Format::Bmp:
		result = stbi_write_bmp(fileName.c_str(), sizeX, sizeY, channels, raw.data());
		break;
	case Format::Tga:
		result = stbi_write_tga(fileName.c_str(), sizeX, sizeY, channels, raw.data());
		break;
	case Format::Auto:
		break;
	}

	if (result == 0) {
		throw std::runtime_error("Failed to export image to " + fileName + ".");
	}
}
