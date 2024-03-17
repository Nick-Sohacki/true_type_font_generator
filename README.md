## Overview
This tool allows you to render text in a low-level graphics application (i.e. OpenGL) in a fast and easy way. It uses STB's image and TTF libraries.

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

## Output Files
This tool will output two files: an image file containing all ASCII characters (i.e. "times24.tga") and a data file containing render data (i.e. "times24.info"). The render data file simply contains the vertex coordinates and UV coordinates for each character such that it can easily passed to a low-level graphics API.

## Using the Output Files
Refer to [example.cpp](https://github.com/Nick-Sohacki/true_type_font_generator/blob/master/src/example.cpp) in the "src" directory to see how to use the output files in your own program to render text.
