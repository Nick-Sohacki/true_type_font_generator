// Prevent the compiler from adding any padding to the following structs.
#pragma pack(push, 1)

struct character_info
{
    int BoundingBoxMinX;
    int BoundingBoxMinY;
    int BoundingBoxMaxX;
    int BoundingBoxMaxY;

    int TextureMinX;
    int TextureMinY;
    int TextureMaxX;
    int TextureMaxY;

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

int main()
{
    // Declare the font info data.
    static font_info FontInfo;

    // Load the font info data.
    FILE *FontInfoFile = fopen("calibri14.info", "rb");
    fread(&FontInfo, sizeof(font_info), 1, FontInfoFile);

    // Load the font texture data.
    int ChannelCount;
    int TextureWidth;
    int TextureHeight;
    uint8_t *TextureData = stbi_load("calibri14.tga", &TextureWidth, &TextureHeight, &ChannelCount, 0);

    // Define the text we are going to render.
    const char *TextToRender = "text to render";

    // Define the position to render the text.
    float PositionX = 0.0f;
    float PositionY = 0.0f;

    // Iterate over each character in the string.
    for (int I = 0; TextToRender[I] != '\0'; I++)
    {
        // Get the render data for the current character.
        character_info *CharInfo = &FontInfo.Characters[TextToRender[I]];

        // Define the vertex coordinates of the character.
        float CharRectMinX = PositionX + CharInfo->BoundingBoxMinX;
        float CharRectMinY = PositionY + CharInfo->BoundingBoxMinY;
        float CharRectMaxX = PositionX + CharInfo->BoundingBoxMaxX;
        float CharRectMaxY = PositionY + CharInfo->BoundingBoxMaxY;

        // Define the UV coordinates of the character.
        float CharUVCoordsMinX = (float)CharInfo->TextureMinX/(float)TextureWidth;
        float CharUVCoordsMinY = (float)CharInfo->TextureMinY/(float)TextureHeight;
        float CharUVCoordsMaxX = (float)CharInfo->TextureMaxX/(float)TextureWidth;
        float CharUVCoordsMaxY = (float)CharInfo->TextureMaxY/(float)TextureHeight;

        // Put vertex and UV coords in vertex buffer.
        // ...

        // Advance to the next character.
        PositionX += CharInfo->XAdvance;
    }

    // To render another line below previous one, advance the Y position.
    PositionY += FontInfo.YAdvance;
}
