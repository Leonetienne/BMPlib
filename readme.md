# BMPlib
Look at main.cpp for more examples

Disclaimer!!:
> This is in no way, shape or form complete or compatible with every possible bmp!
> It does exactly what I need it to, and that is to convert most bmp images to pixel buffers, convert between rgb/rgba/bw pixel buffers and write it all back to a bmp image.
> I am just publishing this in case someone wants to do said things and does not care about incompatibilities with some bmps.

Without saying, this compiles cleanly with `g++ main.cpp -Wall -Wextra -Wpedantic`.

## Basic usage:
*Assuming `using namespace BMPlib`.

##### Read image
```c++
BMP bmp;
bmp.Read("cute.bmp");
```

##### Write image
```c++
bmp.Write("cute.bmp");
```

##### Create and modify image
```c++
BMP bmp(800, 600);               // Default is RGB
bmp.SetPixel(0, 0, 255, 0, 255); // Make topleft pixel pink
```

##### Create images of different color spaces
```c++
BMP bw(800, 600, BMP::COLOR_MODE::BW); // Black/white image
bw.SetPixel(0, 0, 128);                // Make topleft pixel gray

BMP rgb(800, 600, BMP::COLOR_MODE::RGB); // RGB image. It's the default color space tho..
rgb.SetPixel(10, 20, 255, 0, 255);       // Make pixel at (x10, y20) pink

BMP rgba(800, 600, BMP::COLOR_MODE::RGBA); // RGBA image. RGB with transparency
rgba.SetPixel(50, 60, 0, 0, 0, 0);        // Make pixel completely transparent
```

##### Get pixel data
##### Modify individual pixel channels
```c++
BMP bmp(800, 600);                  // Default is RGB
byte* pixel = bmp.GetPixel(20, 25); // Pixel at (x20, y25)
pixel[0] = 33; // Set red channel
pixel[1] = 25; // Set green channel
pixel[2] = 19; // Set green channel
pixel[3] = 99; // Set alpha channel (if image type is rgba)
```

##### Convert between color spaces
```c++
BMP bmp(800, 600);                    // Default is RGB
bmp.ConvertTo(BMP::COLOR_MODE::RGBA); // Now it's RGBA
```

##### Convert color to B/W
```c++
BMP bmp(800, 600); // Default is RGB

// This converts color to bw the "color"-way. bw = r*0.3 + g*0.59 + b*0.11
bmp.ConvertTo(BMP::COLOR_MODE::BW);

// This converts color to bw the "value"-way. bw = (r+g+b) / 3
bmp.ConvertTo(BMP::COLOR_MODE::BW, true);
```

##### Load B/W image
```c++
bmp.Read("cute.bmp"); // BMP doesn't support true BW so it's RGB with redundant channels
bmp.ConvertTo(BMP::COLOR_MODE::BW, true); // Convert to BW color space to save memory. Also pass "true" for "non-color-data" (like, a PBR map).
```

##### Get raw pixel buffer
```c++
BMP bmp(800, 600);                    // Default is RGB
byte* pixelbfr = bmp.GetPixelBuffer();// byte is just unsigned char

// pixelbfr formats:
// each byte represents a channel of a pixel
// pixel channels always lie next to each other (RGBRGBRGB) = ([RGB][RGB][RGB])
// formula for index by pixel coordinates: numChannels * ((y * width) + x); (always points to the first byte)
// for   BW: VVVVVVVVVVVVVVV  -> 15 pixels
// for  RGB: RGBRGBRGBRGBRGB  -> 5 pixels
// for RGBA: RGBARGBARGBARGBA -> 4 pixels
```

##### Check BMPlib version
```
BMPlib #defines BMPLIB_VERSION <some double value>
```

## License
Don't Be a Jerk: The Open Source Software License.
Last Update: Jan, 7, 2021

This software is free and open source.

- *I* am the software author. *I* might be a *we*, but that's OK.
- *You* are the user of this software. *You* might also be a *we*, and that's also OK!

> This is free software.  I will never charge you to use, license, or obtain this software.  Doing so would make me a jerk.

> You may use this code (and by "code" I mean *anything* contained within in this project) for whatever you want.  Personal use, Educational use, Corporate use, Military use, and all other uses are OK!  Limiting how you can use something free would make me a jerk.

> I offer no warranty on anything, ever.  I've tried to ensure that there are no gaping security holes where using this software might automatically send your credit card information to aliens or erase your entire hard drive, but it might happen.  I'm sorry.  However, I warned you, so you can't sue me.  Suing people over free software would make you a jerk.

> If you find bugs, it would be nice if you let me know so I can fix them.  You don't have to, but not doing so would make you a jerk.

> Speaking of bugs, I am not obligated to fix anything nor am I obligated to add a feature for you.  Feeling entitled about free software would make you a jerk.

> If you add a new feature or fix a bug, it would be nice if you contributed it back to the project.  You don't have to, but not doing so would make you a jerk.   The repository/site you obtained this software from should contain a way for you to contact me.  Contributing to open source makes you awesome!

> If you use this software, you don't have to give me any credit, but it would be nice.

Don't be a jerk.
Enjoy your free software!
