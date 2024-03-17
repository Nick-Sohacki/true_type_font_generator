#include <stdint.h>
#include <stdio.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

// Prevent the compiler from adding any padding to the following structs.
#pragma pack(push, 1)

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

#pragma pack(pop)

// The first ASCII char we want numerically is ' ' (32) and the last is '~' (126).
const int ASCII_START = 32;
const int ASCII_END = 126;

int main(int NumArgs, char **Args)
{
    int ReturnCode = 0;

    // Ensure correct number of arguments.
    if (NumArgs == 3)
    {
        // Extract the arguments.
        const char *FontName = Args[1];
        const int FontSize = atoi(Args[2]);

        // Get the font path.
        char FontPath[256];
        sprintf_s(FontPath, sizeof(FontPath), "C:/Windows/Fonts/%s.ttf", FontName);

        // Open the font file.
        FILE *FontFile;
        if (fopen_s(&FontFile, FontPath, "rb") == 0)
        {
            // Read the font file.
            fseek(FontFile, 0, SEEK_END);
            long FileSize = ftell(FontFile);
            fseek(FontFile, 0, SEEK_SET);
            uint8_t *FontBuffer = (uint8_t *)malloc(FileSize);
            fread(FontBuffer, FileSize, 1, FontFile);
            fclose(FontFile);

            // Initialize stb's font info.
            static stbtt_fontinfo StbFontInfo;
            if (stbtt_InitFont(&StbFontInfo, FontBuffer, 0) != 0)
            {
                // Define our font info.
                static font_info FontFileInfo;
                FontFileInfo.Size = FontSize;

                // Get the vertical metrics of the font.
                int Ascent;
                int Descent;
                int LineGap;
                stbtt_GetFontVMetrics(&StbFontInfo, &Ascent, &Descent, &LineGap);

                // Calculate the scale factor from the ascent.
                float ScaleFactor = (float)FontSize/(float)Ascent;

                // Set the distance to advance to the next line (equation is from stbtt_GetFontVMetrics's documentation).
                FontFileInfo.YAdvance = ScaleFactor*(Ascent - Descent + LineGap);

                // Get the bounding box that fits all characters.
                int TotalBoundingBoxMinX;
                int TotalBoundingBoxMinY;
                int TotalBoundingBoxMaxX;
                int TotalBoundingBoxMaxY;
                stbtt_GetFontBoundingBox(&StbFontInfo, &TotalBoundingBoxMinX, &TotalBoundingBoxMinY, &TotalBoundingBoxMaxX, &TotalBoundingBoxMaxY);

                // Calculate the height of the bitmap from the total bounding box.
                int BitmapHeight = (int)ceilf(ScaleFactor*(TotalBoundingBoxMaxY - TotalBoundingBoxMinY + 1));

                // Fill out the data for each character, including texture coordinates.
                int CharacterTexturePosX = 1;
                for (char I = ASCII_START; I <= ASCII_END; I++)
                {
                    // Get the horizontal metrics for this character.
                    int AdvanceWidth;
                    int LeftSideBearing;
                    stbtt_GetCodepointHMetrics(&StbFontInfo, I, &AdvanceWidth, &LeftSideBearing);

                    // Set the horizontal distance to advance to the next character.
                    FontFileInfo.Characters[I].XAdvance = ScaleFactor*AdvanceWidth;

                    // Get the bounding box for this character.
                    stbtt_GetCodepointBitmapBox(&StbFontInfo, I, ScaleFactor, ScaleFactor,
                                                &FontFileInfo.Characters[I].BoundingBoxMinX, &FontFileInfo.Characters[I].BoundingBoxMinY,
                                                &FontFileInfo.Characters[I].BoundingBoxMaxX, &FontFileInfo.Characters[I].BoundingBoxMaxY);

                    // Convert positive Y to be up instead of down.
                    int Tmp = FontFileInfo.Characters[I].BoundingBoxMinY;
                    FontFileInfo.Characters[I].BoundingBoxMinY = -1*FontFileInfo.Characters[I].BoundingBoxMaxY;
                    FontFileInfo.Characters[I].BoundingBoxMaxY = -1*Tmp;

                    // Calculate width and height of character.
                    int CodePointWidth = FontFileInfo.Characters[I].BoundingBoxMaxX - FontFileInfo.Characters[I].BoundingBoxMinX + 1;
                    int CodePointHeight = FontFileInfo.Characters[I].BoundingBoxMaxY - FontFileInfo.Characters[I].BoundingBoxMinY + 1;

                    // Calculate the texture coordinates of character.
                    FontFileInfo.Characters[I].TextureMinX = CharacterTexturePosX;
                    FontFileInfo.Characters[I].TextureMinY = BitmapHeight - CodePointHeight;
                    FontFileInfo.Characters[I].TextureMaxX = CharacterTexturePosX + CodePointWidth;
                    FontFileInfo.Characters[I].TextureMaxY = BitmapHeight;

                    CharacterTexturePosX += CodePointWidth + 1; // Ensure one byte of padding.
                }

                // Get the width of the bitmap, ensuring 4 byte alignment as OpenGL crashes otherwise.
                int BitmapWidth = ((FontFileInfo.Characters[ASCII_END].TextureMaxX + 2) + 3) & (~3);

                // Calculate the bitmap size.
                int BitmapSize = BitmapWidth*BitmapHeight;

                // Allocate the bitmap.
                uint8_t *Bitmap = (uint8_t *)calloc(BitmapSize, 1);

                // Fill bitmap with glyphs while also getting the texture coordinates for each character.
                for (char I = ASCII_START; I <= ASCII_END; I++)
                {
                    // Calculate width and height of character.
                    int CodePointWidth = FontFileInfo.Characters[I].BoundingBoxMaxX - FontFileInfo.Characters[I].BoundingBoxMinX + 1;
                    int CodePointHeight = FontFileInfo.Characters[I].BoundingBoxMaxY - FontFileInfo.Characters[I].BoundingBoxMinY + 1;

                    // Draw the glyph to the bitmap.
                    stbtt_MakeCodepointBitmap(&StbFontInfo, &Bitmap[FontFileInfo.Characters[I].TextureMinX], CodePointWidth, CodePointHeight, BitmapWidth, ScaleFactor, ScaleFactor, I);
                }

                // Convert the bitmap to RGBA.
                uint32_t *BitmapRgba = (uint32_t *)malloc(BitmapWidth*BitmapHeight*4);
                for (int I = 0; I < BitmapSize; I++)
                {
                    BitmapRgba[I] = (Bitmap[I] << 24) | 0x00ffffff;
                }

                // Create the texture name.
                char FontTextureFileName[256];
                sprintf_s(FontTextureFileName, sizeof(FontTextureFileName), "%s%d.tga", FontName, FontSize);

                // Write the bitmap to disk. We disable run-length encoding so loading it will be faster (at the cost of disk space).
                stbi_write_tga_with_rle = 0;
                stbi_write_tga(FontTextureFileName, BitmapWidth, BitmapHeight, 4, BitmapRgba);

                // Create the font info filename.
                char FontInfoFileName[256];
                sprintf_s(FontInfoFileName, sizeof(FontInfoFileName), "%s%d.info", FontName, FontSize);

                // Open and write info file.
                FILE *FontInfoFile;
                if (fopen_s(&FontInfoFile, FontInfoFileName, "wb") == 0)
                {
                    fwrite(&FontFileInfo, sizeof(FontFileInfo), 1, FontInfoFile);
                    fclose(FontInfoFile);
                }
                else
                {
                    printf("failed to open the font info file\n");
                    ReturnCode = 1;
                }
            }
            else
            {
                printf("stbtt_InitFont() failed\n");
                ReturnCode = 1;
            }
        }
        else
        {
            printf("failed to open TTF file; make sure the filename is correct and that it is in C:/Windows/Fonts\n");
            ReturnCode = 1;
        }
    }
    else
    {
        printf("\nusage: ttf_generator [TTF name] [font size]\n");
    }

    return ReturnCode;
}
