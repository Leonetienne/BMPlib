#pragma once
#include <sstream>
#include <fstream>
#include <type_traits>
#include <iomanip>


namespace BMPlib
{
    typedef unsigned char  byte;
    typedef unsigned short byte2;
    typedef unsigned int   byte4;

    using bytestring    = std::basic_string<byte>;
    using bytestream    = std::basic_stringstream<byte>;
    using byteostream   = std::basic_ostream<byte>;
    using byteistream   = std::basic_istream<byte>;
    using byteofstream  = std::basic_ofstream<byte>;
    using byteifstream  = std::basic_ifstream<byte>;

    template<typename T>
    // Will convert (int)15 to 0F 00 00 00 and NOT TO 00 00 00 0F
    bytestring ToBytes(T t)
    {
        std::basic_stringstream<byte> toret;
        byte* bPtr = (byte*)&t;
        for (long long i = 0; i < sizeof(T); i++)
            toret << *(bPtr + i);

        return toret.str();
    }

    template<typename T>
    // Will convert 0F 00 00 00 to 15 and NOT TO 251658240
    byteistream& FromBytes(byteistream& is, T& b)
    {
        const std::size_t sizeofT = sizeof(T);
        b = 0x0;
        byte buf;

        for (std::size_t i = 0; i < sizeofT; i++)
        {
            is.read(&buf, 1);
            T bbuf = buf << (i * 8);
            b |= bbuf;
        }

        return is;
    }

    // Enable streaming streams themself
    byteostream& operator<< (byteostream& os, const bytestream& osin)
    {
        return os << osin.str();
    }

    class BMP
    {
    public:
        enum class COLOR_MODE
        {
            BW, // This is a special case, since BMP doesn't support 1 channel. It is just RGB, but with a 1-channel pixel buffer
            RGB,
            RGBA
        };

        BMP() noexcept
        {
            width = 0;
            height = 0;
            colorMode = COLOR_MODE::BW;
            sizeofPxlbfr = 0;
            numChannelsFile = 0;
            numChannelsPXBF = 0;
            pixelbfr = nullptr;
            isInitialized = false;
            return;
        }

        explicit BMP(const std::size_t& width, const std::size_t& height, const BMP::COLOR_MODE& colorMode = BMP::COLOR_MODE::RGBA)
        {
            ReInitialize(width, height, colorMode);
            return;
        }

        void ReInitialize(const std::size_t& width, const std::size_t& height, const BMP::COLOR_MODE& colorMode = BMP::COLOR_MODE::RGBA)
        {
            // w and h are unsigned so this works!
            if (width + height == 0) ThrowException("Bad image dimensions!");

            // Initialize bunch of stuff
            this->width = width;
            this->height = height;
            this->colorMode = colorMode;

            switch (colorMode)
            {
            case COLOR_MODE::BW:
                numChannelsFile = 3;
                numChannelsPXBF = 1;
                break;
            case COLOR_MODE::RGB:
                numChannelsFile = 3;
                numChannelsPXBF = 3;
                break;
            case COLOR_MODE::RGBA:
                numChannelsFile = 4;
                numChannelsPXBF = 4;
                break;
            }

            // Delete pixelbuffer if already exists
            delete pixelbfr;
            sizeofPxlbfr = sizeof(byte) * width * height * numChannelsPXBF;

            // Try to allocate memory for the pixelbuffer
            try
            {
                pixelbfr = new byte[sizeofPxlbfr];
            }
            catch (std::bad_alloc)
            {
                // too bad!
                ThrowException("Can't allocate memory for pixelbuffer!");
            }

            // Make image black
            memset(pixelbfr, 0, sizeofPxlbfr);

            isInitialized = true;
            return;
        }

        // Will convert between color modes. Set isNonColorData to true to treat pixeldata as raw values rather than color
        void ConvertTo(const BMP::COLOR_MODE& convto, bool isNonColorData = false)
        {
            if (!isInitialized)
                ThrowException("Not initialized!");

            byte* tmp_pxlbfr;
            try
            {
                tmp_pxlbfr = new byte[sizeofPxlbfr];
            }
            catch (std::bad_alloc)
            {
                // too bad!
                ThrowException("Can't allocate memory for temporary conversion pixelbuffer!");
            }

            const std::size_t numPx = width * height;
            const std::size_t oldPxlbfrSize = sizeofPxlbfr;
            memcpy(tmp_pxlbfr, pixelbfr, sizeofPxlbfr);

            switch (colorMode)
            {
            case COLOR_MODE::BW:
#pragma region CONVERT_FROM_BW
                switch (convto)
                {
                case COLOR_MODE::RGB:
                    // BW -> RGB

                    ReInitialize(width, height, COLOR_MODE::RGB);
                    for (std::size_t i = 0; i < numPx; i++)
                    {
                        pixelbfr[i * 3 + 0] = tmp_pxlbfr[i];
                        pixelbfr[i * 3 + 1] = tmp_pxlbfr[i];
                        pixelbfr[i * 3 + 2] = tmp_pxlbfr[i];
                    }

                    break;
                case COLOR_MODE::RGBA:
                    // BW -> RGBA

                    ReInitialize(width, height, COLOR_MODE::RGBA);
                    for (std::size_t i = 0; i < numPx; i++)
                    {
                        pixelbfr[i * 4 + 0] = tmp_pxlbfr[i];
                        pixelbfr[i * 4 + 1] = tmp_pxlbfr[i];
                        pixelbfr[i * 4 + 2] = tmp_pxlbfr[i];
                        pixelbfr[i * 4 + 3] = 0xFF;
                    }
                    break;

                default:
                    break;
                }

#pragma endregion
                break;

            case COLOR_MODE::RGB:
#pragma region CONVERT_FROM_RGB
                switch (convto)
                {
                case COLOR_MODE::BW:
                    // RGB -> BW

                    ReInitialize(width, height, COLOR_MODE::BW);
                    for (std::size_t i = 0; i < numPx; i++)
                    {
                        if (isNonColorData)
                            pixelbfr[i] = (byte)((tmp_pxlbfr[i * 3 + 0] + tmp_pxlbfr[i * 3 + 1] + tmp_pxlbfr[i * 3 + 2]) * 0.33333333);
                        else
                            pixelbfr[i] = (byte)(
                                tmp_pxlbfr[i * 3 + 0] * 0.3 +
                                tmp_pxlbfr[i * 3 + 1] * 0.59 +
                                tmp_pxlbfr[i * 3 + 2] * 0.11);
                    }
                    break;

                case COLOR_MODE::RGBA:
                    // RGB -> RGBA

                    ReInitialize(width, height, COLOR_MODE::RGBA);
                    for (std::size_t i = 0; i < numPx; i++)
                    {
                        pixelbfr[i * 4 + 0] = tmp_pxlbfr[i * 3 + 0];
                        pixelbfr[i * 4 + 1] = tmp_pxlbfr[i * 3 + 1];
                        pixelbfr[i * 4 + 2] = tmp_pxlbfr[i * 3 + 2];
                        pixelbfr[i * 4 + 3] = 0xFF;
                    }
                    break;

                default:
                    break;
                }
#pragma endregion
                break;
            
            case COLOR_MODE::RGBA:
#pragma region CONVERT_FROM_RGBA
                switch (convto)
                {
                case COLOR_MODE::BW:
                    // RGBA -> BW

                    ReInitialize(width, height, COLOR_MODE::BW);
                    for (std::size_t i = 0; i < numPx; i++)
                    {
                        if (isNonColorData)
                            pixelbfr[i] = (byte)((tmp_pxlbfr[i * 4 + 0] + tmp_pxlbfr[i * 4 + 1] + tmp_pxlbfr[i * 4 + 2]) * 0.33333333);
                        else
                            pixelbfr[i] = (byte)(
                                tmp_pxlbfr[i * 4 + 0] * 0.3 +
                                tmp_pxlbfr[i * 4 + 1] * 0.59 +
                                tmp_pxlbfr[i * 4 + 2] * 0.11);
                    }
                    break;

                case COLOR_MODE::RGB:
                    // RGBA -> RGB

                    ReInitialize(width, height, COLOR_MODE::RGB);
                    for (std::size_t i = 0; i < numPx; i++)
                    {
                        pixelbfr[i * 3 + 0] = tmp_pxlbfr[i * 4 + 0];
                        pixelbfr[i * 3 + 1] = tmp_pxlbfr[i * 4 + 1];
                        pixelbfr[i * 3 + 2] = tmp_pxlbfr[i * 4 + 2];
                    }
                    break;

                default:
                    break;
                }
#pragma endregion
                break;
            }

            delete[] tmp_pxlbfr;
            return;
        }

        byte* GetPixelBuffer() noexcept
        {
            return pixelbfr;
        }

        const byte* GetPixelBuffer() const noexcept
        {
            return pixelbfr;
        }

        std::size_t GetWidth() const noexcept
        {
            return width;
        }

        std::size_t GetHeight() const noexcept
        {
            return height;
        }

        COLOR_MODE GetColorMode() const noexcept
        {
            return colorMode;
        }

        bool IsInitialized() const noexcept
        {
            return isInitialized;
        }

        std::size_t CalculatePixelIndex(const std::size_t& x, const std::size_t& y) const
        {
            if ((x >= width) || (y >= height)) ThrowException("Pixel coordinates out of range!");

            return numChannelsPXBF * ((y * width) + x);
        }

        byte* GetPixel(const std::size_t& x, const std::size_t& y)
        {
            return pixelbfr + CalculatePixelIndex(x, y);
        }

        const byte* GetPixel(const std::size_t& x, const std::size_t& y) const
        {
            return pixelbfr + CalculatePixelIndex(x, y);
        }

        // Sets a pixels color
        // If using RGBA, use all
        // If using RGB, a gets ignored
        // If using BW, use only r
        void SetPixel(const std::size_t& x, const std::size_t& y, const byte& r, const byte& g, const byte& b, const byte& a)
        {
            byte* px = GetPixel(x, y);

            switch (colorMode)
            {
            case COLOR_MODE::BW:
                px[0] = r;
                break;

            case COLOR_MODE::RGB:
                px[0] = r;
                px[1] = g;
                px[2] = b;
                break;

            case COLOR_MODE::RGBA:
                px[0] = r;
                px[1] = g;
                px[2] = b;
                px[3] = a;
                break;
            }

            return;
        }

        // Will write a bmp image
        bool Write(const std::string& filename)
        {
            if (!isInitialized)
                return false;

            bytestream data;

            data
                // BMP Header
                << ToBytes(byte2(0x4D42))               // signature
                << ToBytes(byte4(0x36 + sizeofPxlbfr))  // size of the bmp file (all bytes)
                << ToBytes(byte2(0))    // unused
                << ToBytes(byte2(0))    // unused
                << ToBytes(byte4(0x36)) // Offset where the pixel array begins (size of both headers)

                // DIB Header
                << ToBytes(byte4(0x28))   // Number of bytes in DIB header (without this field)
                << ToBytes(byte4(width))    // width
                << ToBytes(byte4(height))   // height
                << ToBytes(byte2(1))    // number of planes used
                << ToBytes(byte2(numChannelsFile * 8))   // bit-depth
                << ToBytes(byte4(0))    // no compression
                << ToBytes(byte4(width * height * numChannelsFile))   // Size of raw bitmap data (including padding)
                << ToBytes(byte4(0xB13)) // print resolution pixels/meter X
                << ToBytes(byte4(0xB13)) // print resolution pixels/meter Y
                << ToBytes(byte4(0))    // 0 colors in the color palette
                << ToBytes(byte4(0));   // 0 means all colors are important

            // Dumbass unusual pixel order of bmp made me do this...
            for (long long y = height - 1; y >= 0; y--)
                for (std::size_t x = 0; x < width; x++)
                {
                    std::size_t idx = CalculatePixelIndex(x, y);

                    switch (colorMode)
                    {
                    case COLOR_MODE::BW:
                        // pixelbfr ==> R-G-B ==> R-G-B ==> bmp format
                        data.write((pixelbfr + idx), 1); // B
                        data.write((pixelbfr + idx), 1); // G
                        data.write((pixelbfr + idx), 1); // R

                        break;

                    case COLOR_MODE::RGB:
                        // pixelbfr ==> R-G-B ==> R-G-B ==> bmp format
                        data.write((pixelbfr + idx + 2), 1); // B
                        data.write((pixelbfr + idx + 1), 1); // G
                        data.write((pixelbfr + idx + 0), 1); // R

                        break;

                    case COLOR_MODE::RGBA:
                        // pixelbfr ==> R-G-B-A ==> A-R-G-B ==> bmp format
                        data.write((pixelbfr + idx + 2), 1); // B
                        data.write((pixelbfr + idx + 1), 1); // G
                        data.write((pixelbfr + idx + 0), 1); // R
                        data.write((pixelbfr + idx + 3), 1); // A

                        break;
                    }

                }

            // write file
            byteofstream bs;
            bs.open(filename, byteofstream::binary);
            if (!bs.good())
                return false;

            bs << data;
            bs.close();

            return true;
        }

        // Will read a bmp image
        bool Read(std::string filename)
        {
            byteifstream bs;
            bs.open(filename, byteifstream::binary);
            if (!bs.good())
                return false;

            // Check BMP signature
            byte2 signature;
            FromBytes(bs, signature);
            if (signature != 0x4D42)
                return false;

            // Gather filesize
            byte4 fileLen;
            FromBytes(bs, fileLen);

            byte2 unused0;
            byte2 unused1;
            FromBytes(bs, unused0);
            FromBytes(bs, unused1);

            byte4 offsetPixelArray;
            FromBytes(bs, offsetPixelArray);

            byte4 dibHeadLen;
            FromBytes(bs, dibHeadLen);

            // Gather image dimensions
            byte4 imgWidth;
            byte4 imgHeight;
            FromBytes(bs, imgWidth);
            FromBytes(bs, imgHeight);

            byte2 numPlanes;
            FromBytes(bs, numPlanes);

            // Gather image bit-depth
            byte2 bitDepth;
            FromBytes(bs, bitDepth);
            switch (bitDepth)
            {
            case 24:
                colorMode = COLOR_MODE::RGB;
                break;
            case 32:
                colorMode = COLOR_MODE::RGBA;
            }

            byte4 compression;
            FromBytes(bs, compression);

            // Gather size of pixel buffer
            byte4 sizeofPixelBuffer;
            FromBytes(bs, sizeofPixelBuffer);

            byte4 printresX;
            byte4 printresY;
            FromBytes(bs, printresX);
            FromBytes(bs, printresY);

            byte4 colorsInPalette;
            byte4 importantColors;
            FromBytes(bs, colorsInPalette);
            FromBytes(bs, importantColors);

            // Initialize image
            ReInitialize(imgWidth, imgHeight, colorMode);

            // Dumbass unusual pixel order of bmp made me do this...
            for (long long y = imgHeight - 1; y >= 0; y--)
                for (std::size_t x = 0; x < imgWidth; x++)
                {
                    std::size_t idx = CalculatePixelIndex(x, y);

                    switch (colorMode)
                    {
                    case COLOR_MODE::RGB:
                        // bmp format ==> R-G-B ==> R-G-B ==> pixelbfr
                        bs.read((pixelbfr + idx + 2), 1); // Read B byte
                        bs.read((pixelbfr + idx + 1), 1); // Read G byte
                        bs.read((pixelbfr + idx + 0), 1); // Read R byte
                        break;

                    case COLOR_MODE::RGBA:

                        // bmp format ==> A-R-G-B ==> R-G-B-A ==> pixelbfr
                        bs.read((pixelbfr + idx + 2), 1); // Read B byte
                        bs.read((pixelbfr + idx + 1), 1); // Read G byte
                        bs.read((pixelbfr + idx + 0), 1); // Read R byte
                        bs.read((pixelbfr + idx + 3), 1); // Read A byte
                        break;

                    default:
                        break;
                    }
                }

            bs.close();
            return true;
        }

        ~BMP()
        {
            delete[] pixelbfr;
            pixelbfr = nullptr;

            return;
        }

    private:
        void ThrowException(std::string msg) const
        {
            throw msg;
            return;
        }

        std::size_t width;
        std::size_t height;
        std::size_t numChannelsFile; // Num channels of the file format
        std::size_t numChannelsPXBF; // Num channels of the pixel buffer
        COLOR_MODE colorMode;
        bool isInitialized;
        byte* pixelbfr;
        std::size_t sizeofPxlbfr; // how many bytes the pixelbuffer is long
    };
}
