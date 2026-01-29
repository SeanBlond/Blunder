#include "font.h"

// Constructor
Font::Font(std::string fntFile, std::string fontImage)
{
    bitmapTexture = 0;

    // Setting up font character data
    ReadFNTFile(fntFile);

    // Adding the fontImage
    fontBitmap = new shdr::Texture2D(fontImage.c_str(), GL_LINEAR_MIPMAP_LINEAR, GL_TEXTURE_WRAP_S);

    // Temp Vertex Info to bind VAO and EBO to
    AddText("W", 0, 0, 1, glm::vec3(1));

    // Configuring VAO and VBO
    glGenVertexArrays(1, &textVAO);
    glGenBuffers(1, &textVBO);
    glGenBuffers(1, &textEBO);

    glBindVertexArray(textVAO);

    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(TextVertex), &vertices[0], GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, textEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_DYNAMIC_DRAW);

    // Position Attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TextVertex), (void*)0);

    // TexCoord Attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TextVertex), (void*)offsetof(TextVertex, TexCoord));

    // TextColor Attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(TextVertex), (void*)offsetof(TextVertex, TextColor));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    // Creating Shader
    const char* textVertexShader = R"(
        #version 330 core
        layout (location = 0) in vec2 vertex;
        layout (location = 1) in vec2 TexCoord;
        layout (location = 2) in vec3 TextColor;
        out vec2 TexCoords;
        out vec3 Color;
        
        uniform mat4 projection;

        void main()
        {
            gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
            TexCoords = TexCoord;
            Color = TextColor;
        }  
        )";

    const char* textFragmentShader = R"(
        #version 330 core
        in vec2 TexCoords;
        in vec3 Color;
        out vec4 fragColor;

        uniform sampler2D text;

        void main()
        {    
            vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
            fragColor = vec4(Color, 1.0) * sampled;
            //fragColor = vec4(TexCoords, 0.0, 1.0);
        }  
        )";

    textShader = new shdr::Shader(textVertexShader, textFragmentShader, 1);
}
Font::~Font()
{
    textShader->deallocateShader();
    delete textShader;
    delete fontBitmap;
    textShader = nullptr;
    fontBitmap = nullptr;
}

void Font::ReadFNTFile(std::string filePath)
{
    std::ifstream file(filePath);

    // Checking if file was opened correctly
    if (!file)
    {
        std::cout << "FAILED TO OPEN .fnt FILE AT PATH " << filePath << std::endl;
        return;
    }

    // Finding Text Size
    std::string tempString = "";
    std::string targetString = "size=";
    while (tempString.substr(0, targetString.size()) != targetString || file.eof())
    {
        file >> tempString;
    }
    if (file.eof())
    {
        std::cout << "Failed to find target in file" << std::endl;
        return;
    }
    fontSize = stoi(tempString.substr(targetString.size(), tempString.size() - targetString.size()));
    //std::cout << "Read for size: " << fontSize << std::endl;
    
    // Finding char range
    targetString = "charset=";
    while (tempString.substr(0, targetString.size()) != targetString || file.eof())
    {
        file >> tempString;
    }
    if (file.eof())
    {
        std::cout << "Failed to find target in file" << std::endl;
        return;
    }
    std::string tempValueRead = tempString.substr(targetString.size() + 1, tempString.size() - targetString.size() - 2);
    std::regex charRange(R"(^(\d{1,3})\-(\d{1,3}))");
    std::smatch matches;
    std::regex_search(tempValueRead, matches, charRange);
    firstChar = stoi(matches[1]);
    lastChar = stoi(matches[2]);

    // Finding line height
    targetString = "lineHeight=";
    while (tempString.substr(0, targetString.size()) != targetString && !file.eof())
    {
        file >> tempString;
    }
    if (file.eof())
    {
        std::cout << "Failed to find target in file" << std::endl;
        return;
    }
    lineHeight = stoi(tempString.substr(targetString.size(), tempString.size() - targetString.size()));
    //std::cout << "Read for height: " << lineHeight << std::endl;

    // Finding bitmap size
    targetString = "scaleW=";
    while (tempString.substr(0, targetString.size()) != targetString && !file.eof())
    {
        file >> tempString;
    }
    if (file.eof())
    {
        std::cout << "Failed to find target in file" << std::endl;
        return;
    }
    bitmapSize.x = stoi(tempString.substr(targetString.size(), tempString.size() - targetString.size()));
    targetString = "scaleH=";
    while (tempString.substr(0, targetString.size()) != targetString && !file.eof())
    {
        file >> tempString;
    }
    if (file.eof())
    {
        std::cout << "Failed to find target in file" << std::endl;
        return;
    }
    bitmapSize.y = stoi(tempString.substr(targetString.size(), tempString.size() - targetString.size()));
    //std::cout << "Read for Bitmap Size: (" << bitmapSize.x << ", " << bitmapSize.y << ")" << std::endl;

    // Getting number of characters to read
    int characterCount = 0;
    targetString = "count=";
    while (tempString.substr(0, targetString.size()) != targetString && !file.eof())
    {
        file >> tempString;
    }
    if (file.eof())
    {
        std::cout << "Failed to find target in file" << std::endl;
        return;
    }
    characterCount = stoi(tempString.substr(targetString.size(), tempString.size() - targetString.size()));
    //std::cout << "Read for character count: " << characterCount << std::endl;

    // Getting rid of empty line
    std::getline(file, tempString);

    // Gettind data for each character
    for (int i = 0; i < characterCount; i++)
    {
        // Getting Line Data
        std::string readLine;
        getline(file, readLine);
        std::istringstream lineStream(readLine);

        // Reading line data to character
        Character tempChar;

        // Finding X pos
        std::string tempCharData = "";
        std::string targetString = "x=";
        while (tempCharData.substr(0, targetString.size()) != targetString && !file.eof())
        {
            lineStream >> tempCharData;
        }
        if (file.eof())
        {
            std::cout << "Failed to find target in file" << std::endl;
            return;
        }
        float tempFloat = stof(tempCharData.substr(targetString.size(), tempCharData.size() - targetString.size()));
        tempChar.Positions.x = tempFloat / (float)bitmapSize.x;

        // Finding Y pos
        targetString = "y=";
        while (tempCharData.substr(0, targetString.size()) != targetString && !file.eof())
        {
            lineStream >> tempCharData;
        }
        if (file.eof())
        {
            std::cout << "Failed to find target in file" << std::endl;
            return;
        }
        tempFloat = stof(tempCharData.substr(targetString.size(), tempCharData.size() - targetString.size()));
        tempChar.Positions.y = tempFloat / (float)bitmapSize.y;
    
        // Finding Char Size
        // Finding X size
        targetString = "width=";
        while (tempCharData.substr(0, targetString.size()) != targetString && !file.eof())
        {
            lineStream >> tempCharData;
        }
        if (file.eof())
        {
            std::cout << "Failed to find target in file" << std::endl;
            return;
        }
        tempChar.Size.x = stoi(tempCharData.substr(targetString.size(), tempCharData.size() - targetString.size()));

        // Finding Y size
        targetString = "height=";
        while (tempCharData.substr(0, targetString.size()) != targetString && !file.eof())
        {
            lineStream >> tempCharData;
        }
        if (file.eof())
        {
            std::cout << "Failed to find target in file" << std::endl;
            return;
        }
        tempChar.Size.y = stoi(tempCharData.substr(targetString.size(), tempCharData.size() - targetString.size()));
        //std::cout << "Read Char Size: (" << tempChar.Size.x << ", " << tempChar.Size.y << ")" << std::endl;
        
        // Completing position values
        tempChar.Positions.z = tempChar.Positions.x + (float)tempChar.Size.x / (float)bitmapSize.x;
        tempChar.Positions.w = tempChar.Positions.y + (float)tempChar.Size.y / (float)bitmapSize.y;
        //std::cout << "Read Char Pos: (" << tempChar.Positions.x << ", " << tempChar.Positions.y << ") and (" << 
        //   tempChar.Positions.z << ", " << tempChar.Positions.w << ")" << std::endl;

        // Finding Bearings 
        // Finding X bearing
        targetString = "xoffset=";
        while (tempCharData.substr(0, targetString.size()) != targetString && !file.eof())
        {
            lineStream >> tempCharData;
        }
        if (file.eof())
        {
            std::cout << "Failed to find target in file" << std::endl;
            return;
        }
        tempChar.Bearing.x = stoi(tempCharData.substr(targetString.size(), tempCharData.size() - targetString.size()));

        // Finding Y bearing
        targetString = "yoffset=";
        while (tempCharData.substr(0, targetString.size()) != targetString && !file.eof())
        {
            lineStream >> tempCharData;
        }
        if (file.eof())
        {
            std::cout << "Failed to find target in file" << std::endl;
            return;
        }
        tempChar.Bearing.y = stoi(tempCharData.substr(targetString.size(), tempCharData.size() - targetString.size()));
        //std::cout << "Read Char Bearing: (" << tempChar.Bearing.x << ", " << tempChar.Bearing.y << ")" << std::endl;

        // Finding Advance
        targetString = "xadvance=";
        while (tempCharData.substr(0, targetString.size()) != targetString && !file.eof())
        {
            lineStream >> tempCharData;
        }
        if (file.eof())
        {
            std::cout << "Failed to find target in file" << std::endl;
            return;
        }
        tempChar.Advance = stoi(tempCharData.substr(targetString.size(), tempCharData.size() - targetString.size()));
        //std::cout << "Read Char Advance: " << tempChar.Advance << std::endl;
        
        // Adding Character
        Characters.push_back(tempChar);
    }

    // It was successful!
    std::cout << "Successfully loaded .fnt file into characters" << std::endl;
}

void Font::UpdateMesh()
{
    glBindVertexArray(textVAO);

    // Updating VBO
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(TextVertex), &vertices[0], GL_DYNAMIC_DRAW);

    // Updating EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, textEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
void Font::AddText(std::string text, float x, float y, float scale, glm::vec3 color, TextAlign alignment)
{
    // Calculating Alignment
    float alignmentOffset = 0;
    if (alignment == RIGHT)
    {
        float textWidth = 0;
        for (int i = 0; i < text.size(); i++)
        {
            textWidth += (i == (text.size() - 1) ? 0 : getCharacter(text[i]).Bearing.x) + getCharacter(text[i]).Advance >> 6;
        }
        alignmentOffset = textWidth * scale;
    }
    else if (alignment == CENTER)
    {
        float textWidth = 0;
        for (int i = 0; i < text.size(); i++)
        {
            textWidth += (i == (text.size() - 1) ? 0 : getCharacter(text[i]).Bearing.x) + getCharacter(text[i]).Advance >> 6;
        }
        alignmentOffset = textWidth * scale * 0.5;
    }

    // Offsetting X
    x -= (alignmentOffset);

    // Storing initial X
    float initialX = x;

    // iterate through all characters
    for (int i = 0; i < text.size(); i++)
    {
        Character ch = getCharacter(text[i]);
        
        // Checking for New Line
        if (text[i] == '\n')
        {
            y -= lineHeight *1.3 * scale;
            x = initialX;
            continue;
        }
        else if (text[i] == ' ')
        {
            x += (ch.Advance >> 6) * scale;
            continue;
        }

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - ((ch.Size.y - ch.Bearing.y) * scale);
        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;

        // Adding Vertex Information
        TextVertex tempVertices[4] = {
                         // Positions                    // TexCoord (Flipped)                       // TextColor
            TextVertex({ glm::vec2(xpos,     ypos),      glm::vec2(ch.Positions.x, ch.Positions.w),  color }),  // Bottom Left
            TextVertex({ glm::vec2(xpos + w, ypos),      glm::vec2(ch.Positions.z, ch.Positions.w),  color }),  // Bottom Right
            TextVertex({ glm::vec2(xpos + w, ypos + h),  glm::vec2(ch.Positions.z, ch.Positions.y),  color }),  // Top Right
            TextVertex({ glm::vec2(xpos,     ypos + h),  glm::vec2(ch.Positions.x, ch.Positions.y),  color }),   // Top Left
        };
        vertices.insert(vertices.end(), std::begin(tempVertices), std::end(tempVertices));


        // Adding Index Information
        unsigned int offset = indices.size();
        unsigned int tempIndices[6] = {
            offset,     offset + 1,  offset + 2,
            offset + 2, offset + 3,  offset
        };
        indices.insert(indices.end(), std::begin(tempIndices), std::end(tempIndices));

        // now advance cursors for next glyph
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (1/64th times 2^6 = 64)
    }
}
void Font::RenderText(glm::mat4 projection)
{
    // Updating Mesh Data
    UpdateMesh();

    // Setting up shader and texture
    textShader->setMat4("projection", projection);
    textShader->useShader();
    fontBitmap->Bind(0);

    // Rendeing the meshes
    glBindVertexArray(textVAO);
    int count = vertices.size() * 2 / 3;
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Clearing Vertices & Indices after rendering
    vertices.clear();
    indices.clear();
}