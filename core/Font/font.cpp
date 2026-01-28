#include "font.h"

// Constructor
Font::Font(std::string fntFile, std::string fontImage)
{
    projection = glm::mat4(1.0f);
    bitmapTexture = 0;

    ReadFNTFile(fntFile);

    // Vertice Info
    float vertices[]{
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 1.0f,
        1.0f, 0.0f
    };

    // Configuring VAO and VBO
    glGenVertexArrays(1, &textVAO);
    glGenBuffers(1, &textVBO);

    glBindVertexArray(textVAO);

    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    // Creating Shader
    const char* textVertexShader = R"(
        #version 330 core
        layout (location = 0) in vec2 vertex;
        out vec2 TexCoords;
        
        uniform mat4 transform;
        uniform mat4 projection;

        void main()
        {
            gl_Position = projection * transform * vec4(vertex.xy, 0.0, 1.0);
            TexCoords = vertex;
            TexCoords.y = 1 - TexCoords.y;
        }  
        )";

    const char* textFragmentShader = R"(
        #version 330 core
        in vec2 TexCoords;
        out vec4 color;

        uniform sampler2D text;
        uniform vec3 textColor;

        void main()
        {    
            vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
            color = vec4(textColor, 1.0) * sampled;
            //color = vec4(TexCoords, 0.0, 1.0);
        }  
        )";

    textShader = new shdr::Shader(textVertexShader, textFragmentShader, 1);
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
    fontSize = stof(tempString.substr(targetString.size(), tempString.size() - targetString.size()));
    std::cout << "Read for size: " << fontSize << std::endl;
    
    // Finding char range
    targetString = "charset=";
    while (tempString.substr(0, targetString.size()) != targetString || file.eof())
    {
        file >> tempString;
    }
    std::string tempValueRead = tempString.substr(targetString.size() + 1, tempString.size() - targetString.size() - 2);
    std::regex charRange(R"(^(\d{1,3})\-(\d{1,3}))");
    std::smatch matches;
    std::regex_search(tempValueRead, matches, charRange);
    firstChar = stoi(matches[1]);
    lastChar = stoi(matches[2]);

    // Finding line height
    targetString = "lineHeight=";
    while (tempString.substr(0, targetString.size()) != targetString || file.eof())
    {
        file >> tempString;
    }
    lineHeight = stof(tempString.substr(targetString.size(), tempString.size() - targetString.size()));
    std::cout << "Read for height: " << lineHeight << std::endl;

    // Finding bitmap size
    targetString = "scaleW=";
    while (tempString.substr(0, targetString.size()) != targetString || file.eof())
    {
        file >> tempString;
    }
    bitmapSize.x = stof(tempString.substr(targetString.size(), tempString.size() - targetString.size()));
    targetString = "scaleH=";
    while (tempString.substr(0, targetString.size()) != targetString || file.eof())
    {
        file >> tempString;
    }
    bitmapSize.y = stof(tempString.substr(targetString.size(), tempString.size() - targetString.size()));
    std::cout << "Read for Bitmap Size: (" << bitmapSize.x << ", " << bitmapSize.y << ")" << std::endl;


}

void Font::RenderText(std::string text, float x, float y, float scale, glm::vec3 color, TextAlign alignment)
{

    // Calculating Alignment
    float alignmentOffset = 0;
    if (alignment == RIGHT)
    {
        float textWidth = 0;
        std::string::const_iterator c;
        for (c = text.begin(); c != text.end(); c++)
        {
            textWidth += (c == (text.end() - 1) ? 0 : Characters[*c].Bearing.x) + Characters[*c].Advance >> 6;
        }
        alignmentOffset = textWidth * scale;
    }
    else if (alignment == CENTER)
    {
        float textWidth = 0;
        std::string::const_iterator c;
        for (c = text.begin(); c != text.end(); c++)
        {
            textWidth += (c == (text.end() - 1) ? 0 : Characters[*c].Bearing.x) + Characters[*c].Advance >> 6;
        }
        alignmentOffset = textWidth * scale * 0.5;
    }

    // activate corresponding render state	
    textShader->useShader();
    textShader->setVec3("textColor", color);
    textShader->setMat4("projection", projection);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(textVAO);

    // Offsetting X
    x -= (alignmentOffset);

    // Storing initial X
    float initialX = x;

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];
        
        // Checking for New Line
        if (*c == '\n')
        {
            y -= ch.Size.y * 1.3 * scale;
            x = initialX;
            continue;
        }
        else if (*c == ' ')
        {
            x += (ch.Advance >> 6) * scale;
            continue;
        }

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - ((ch.Size.y - ch.Bearing.y) * scale);

        // Creating Transformation Matrix for each character
        glm::mat4 transform;
        transform = smath::translate(glm::vec3(xpos, ypos, 1.0f)) * smath::scale(glm::vec3(ch.Size.x * scale, ch.Size.y * scale, 0.0f));
        textShader->setMat4("transform", transform);

        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, bitmapTexture);

        // update content of textVBO memory
        glBindBuffer(GL_ARRAY_BUFFER, textVBO);

        // render quad
        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, 1);

        // now advance cursors for next glyph
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (1/64th times 2^6 = 64)
    }
    //std::cout << text << " final x pos: " << x << std::endl;
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}