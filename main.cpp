#include <iostream>
#include <iomanip>
#include <random>
#include "BMPlib.h"

using namespace BMPlib;

void Example1()
{
    // Another example
    // Read an image, convert it to grayscale and save it again
    try
    {
        BMP bmpr;
        if (!bmpr.Read("test.bmp"))
            std::cout << "Too bad!" << std::endl;

        bmpr.ConvertTo(BMP::COLOR_MODE::BW);
        bmpr.Write("other_image.bmp");
    }
    catch (std::string& s)
    {
        std::cout << s << std::endl;
    }
    return;
}

void Example2()
{
    // Another example
    // Read an image, convert it to RGBA and make it translucent
    try
    {
        BMP bmpr;
        if (!bmpr.Read("test.bmp"))
            std::cout << "Yikes!" << std::endl;

        bmpr.ConvertTo(BMP::COLOR_MODE::RGBA);

        for (std::size_t x = 0; x < bmpr.GetWidth(); x++)
            for (std::size_t y = 0; y < bmpr.GetHeight(); y++)
            {
                byte* pixel = bmpr.GetPixel(x, y); // Since we only want to change one value, it's easier to fetch the pixel itself
                pixel[3] = 134; // Set channel A of RGBA (at index 3) to value 134
            }

        bmpr.Write("other_image.bmp");
    }
    catch (std::string& s)
    {
        std::cout << s << std::endl;
    }
    return;
}

void Example3()
{
    // Another example
    // Create image object of size 800x600px
    BMP bmp(800, 600);
    
    // Generate nice color gradient image
    for (std::size_t x = 0; x < 800; x++)
        for (std::size_t y = 0; y < 600; y++)
        {
            bmp.SetPixel(x, y,
                (byte)(       ((double)x / 800.0) * 255.0),
                (byte)((1.0 - ((double)x / 800.0)) * 255.0),
                (byte)((1.0 - ((double)y / 800.0)) * 255.0),
                255);
        }

    // Save to file
    if (!bmp.Write("test.bmp"))
        std::cout << "This isn't working!" << std::endl;
    return;
}

void Example4()
{
    // Another example
    // Will mix up the RGB channels randomly per pixel
    // Will also work with RGBA
    BMP bmp;
    bmp.Read("onion.bmp");

    // It would seriously mess up BW images, so let's account for that
    if (bmp.GetColorMode() != BMP::COLOR_MODE::BW)
        bmp.ConvertTo(BMP::COLOR_MODE::RGB);

    std::random_device rd;
    std::mt19937 rng(rd());

    for (std::size_t x = 0; x < bmp.GetWidth(); x++)
        for (std::size_t y = 0; y < bmp.GetHeight(); y++)
        {
            // swap channel randomly for cool effect
            byte* px = bmp.GetPixel(x, y);
            int rnd = rng() % 6;

            switch (rnd)
            {
            case 0:
                bmp.SetPixel(x, y, px[0], px[1], px[2]);
                break;
            case 1:
                bmp.SetPixel(x, y, px[0], px[2], px[1]);
                break;
            case 2:
                bmp.SetPixel(x, y, px[1], px[0], px[2]);
                break;
            case 3:
                bmp.SetPixel(x, y, px[1], px[2], px[0]);
                break;
            case 4:
                bmp.SetPixel(x, y, px[2], px[1], px[0]);
                break;
            case 5:
                bmp.SetPixel(x, y, px[2], px[0], px[1]);
                break;
            }
        }

    bmp.Write("onion_out.bmp");
    return;
}

void ExampleDiamonds()
{
    // Another example
    // Will generate a black/white image of diamonds

    // 62 is the pixel size of a diamond
    BMP bmp(62 * 10 , 62 * 6, BMP::COLOR_MODE::BW);

    for (std::size_t x = 0; x < bmp.GetWidth(); x++)
        for (std::size_t y = 0; y < bmp.GetHeight(); y++)
            bmp.SetPixel(x, y, tan(x / 20.0) * tan(y / 20.0) * 255);


    bmp.Write("diamonds.bmp");

    return;
}

void ExampleDiamondsWithSparkles()
{
    // Another examples
    // Will generate an image of diamonds with sparkles

    std::random_device rd;
    std::mt19937 rng(rd());

    // 62 is the pixel size of a diamond
    BMP bmp(62 * 10, 62 * 6);

    for (std::size_t x = 0; x < bmp.GetWidth(); x++)
        for (std::size_t y = 0; y < bmp.GetHeight(); y++)
        {
            byte* px = bmp.GetPixel(x, y);

            // Generate main diamond pattern
            byte bwpx = tan(x / 20.0) * tan(y / 20.0) * 255;
            memset(px, bwpx, 3); // Set rgb to bwpx value

            // Generate sparkles
            int tsX = x % 63; // tilespace x
            int tsY = y % 63; // tilespace y
            if (abs(tsX - 31) * 0.7 * abs(tsY - 31) * 0.7 < 30)
                if (!(rng() % 8))
                {
                    px[0] = rng() % 255;
                    px[1] = rng() % 255;
                    px[2] = rng() % 255;
                }
        }

    bmp.Write("diamonds.bmp");

    return;
}

int main()
{
    ExampleDiamonds();

    return 0;
}

void Debugging()
{
    // Bytereader for debugging
    // ignore this if you just want to use the lib

    bytestream ss;
    ss << ToBytes(byte4(696969));
    byte4 myint;
    FromBytes(ss, myint);
    std::cout << (int)myint << std::endl;

    std::basic_string<byte> str = ss.str();
    for (std::size_t i = 0; i < str.size(); i++)
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)str[i] << ' ';
    std::cout << std::endl;

return;
}
