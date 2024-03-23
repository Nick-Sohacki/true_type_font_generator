## Overview
This tool allows you to render text in a low-level graphics application (i.e. OpenGL) in a fast and easy way. Given a TTF file as input, it generates two files:
1. An image file containing all ASCII characters, that looks like the following: ![alt text](https://github.com/Nick-Sohacki/true_type_font_generator/blob/master/res/comic36.png)
2. A binary file containing the render data for each character; the structure for this data is the following:
```
// Define the data for rendering a single character.
struct character_info
{
    // The bounding box of the character; simply add the current position to this to get the rectangle to draw.
    int BoundingBoxMinX;
    int BoundingBoxMinY;
    int BoundingBoxMaxX;
    int BoundingBoxMaxY;

    // The pixel coordinates of the character in the image file.
    int TextureMinX;
    int TextureMinY;
    int TextureMaxX;
    int TextureMaxY;

    // The amount to advance to the next character.
    float XAdvance;
};

// Define the data for rendering the font.
struct font_info
{
    // The size of the font.
    int Size;

    // The amount to advance to the next row/line of text.
    float YAdvance;

    // Array of character data. Note that while we only use 96 characters, we still define it to be the entire 128 characters for simple access.
    character_info Characters[128];
};
```

Refer to [example.cpp](https://github.com/Nick-Sohacki/true_type_font_generator/blob/master/src/example.cpp) to see how to use these files in your application.

## Usage
```
ttf_generator [TTF name] [font size]
```
Where "TTF name" is the name of a TTF file in C:/Windows/Fonts/ and "font size" is the desired pixel size of the font.

## Usage example
To generate render data for the Times New Roman font with a size of 24:
```
ttf_generator times 24
```
"times" is the name of the Times New Roman TTF file in C:/Windows/Fonts/.

## Libraries Used

This project uses STB's TTF and image write libraries.
