#include <iostream>
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
                ((double)x / 800.0) * 255.0,
                (1.0 - ((double)x / 800.0)) * 255.0,
                (1.0 - ((double)y / 800.0)) * 255.0,
                255);
        }

    // Save to file
    if (!bmp.Write("test.bmp"))
        std::cout << "This isn't working!" << std::endl;
    return;
}

int main()
{
    Example3();

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
