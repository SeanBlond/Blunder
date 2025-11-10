#include "font.h"

// Constructor
Font::Font(std::string fontName, int fontSize = 64)
{
    projection = glm::mat4(1.0f);
    texture = 0;

    // FreeType
    FT_Library ft;
    // All functions return a value different than 0 whenever an error occurred
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
    }

    // find path to font
    if (fontName.empty())
    {
        std::cout << "ERROR::FREETYPE: Failed to load font_name" << std::endl;
    }

    // load font as face
    FT_Face face;

    if (FT_New_Face(ft, fontName.c_str(), 0, &face)) {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
    }
    else {
        // set size to load glyphs as
        FT_Set_Pixel_Sizes(face, 0, 64);

        // disable byte-alignment restriction
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // load first 128 characters of ASCII set
        for (unsigned char c = 0; c < 128; c++)
        {
            // Load character glyph 
            if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            {
                std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
                continue;
            }

            // generate texture
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
            );

            // set texture options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            // now store character for later use
            Character character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                static_cast<unsigned int>(face->glyph->advance.x)
            };
            Characters.insert(std::pair<char, Character>(c, character));
        }
        glBindTexture(GL_TEXTURE_2D, 0);
    }


    // destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

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
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);

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