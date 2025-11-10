#include "TextInput.h"
using namespace ui;

void TextInput::insert(char character)
{
    if (selecting)
        remove();

    if (text.length() <= 0)
        cursor == 0;

    if (cursor != 0)
        text.insert(text.end() + cursor, character);
    else
        text.push_back(character);
}
void TextInput::insert(const char* string)
{
    if (selecting)
        remove();

    if (text.length() <= 0)
        cursor == 0;

    if (cursor != 0)
        text.insert(text.length() + cursor, string);
    else
        text += string;
}
void TextInput::remove()
{
    if (text.length() <= 0)
        return;

    if (!selecting)
    {
        if (cursor == 0)
            text.pop_back();
        else if (cursor > -(int)text.length())
            text.erase(text.end() + cursor - 1);
    }
    else
    {
        if (cursor == 0)
            text.erase(text.begin() + selectPos, text.end());
        else
        {
            if (text.length() + cursor < selectPos) 
            {
                text.erase(text.end() + cursor, text.begin() + selectPos);
                cursor += selectPos - ((int)text.length() + cursor);
            }
            else 
                text.erase(text.begin() + selectPos, text.end() + cursor);
        }
        resetSelection();
    }

    // Clamping Cursor after deletion
    setCursor(cursor);
}
void TextInput::shiftCursor(int amount)
{
    setCursor(cursor + amount);
}
void TextInput::resetSelection()
{
    selecting = false;
    selectPos = 0;
}
void TextInput::updateSelection()
{
    if (!selecting)
    {
        selecting = true;
        setSelection((int)text.length() + cursor);
    }
}
void TextInput::selectAll()
{
    setCursor(0);
    selecting = true;
    setSelection(-(int)text.length());
    updateSelection();
}
std::string TextInput::returnSelected()
{
    std::string substring;
    if (selecting)
    {
        if (text.length() + cursor < selectPos)
            substring = text.substr(text.length() + cursor, (selectPos - text.length() + cursor));
        else
            substring = text.substr(selectPos, (text.length() + cursor - selectPos));

    }
    return substring;
}
std::string TextInput::cutSelected()
{
    std::string substring = returnSelected();
    
    if (selecting)
        remove();

    return substring;
}
void TextInput::renderText(ui::UIRenderer* renderer, float x, float y, float scale, glm::vec3 color, TextAlign alignment)
{
    // Rendering Cursor & Selection
    if (typing)
    {
        // Cursor
        glm::vec2 cursorPos = getCursorPosition(text, cursor, x, y, scale, alignment, renderer->getTextRenderer());
        //renderer->renderQuad(glm::vec3(cursorPos, 0), glm::vec2(5, scale), glm::vec3(0.8f));
        renderer->renderQuad(glm::vec3(cursorPos, 0.25f), glm::vec2(3, 60 * scale), glm::vec3(0.8f));

        // Selection
        if (selecting)
        {
            glm::vec2 selectEndPos = getCursorPosition(text, (selectPos - (int)text.length()), x, y, scale, alignment, renderer->getTextRenderer());
            glm::vec2 selectPos = (cursorPos + selectEndPos) * 0.5f;
            float selectSize = abs(selectEndPos.x - cursorPos.x);
            renderer->renderQuad(glm::vec3(selectPos, 0.24f), glm::vec2(selectSize, 60 * scale), colors::selectionBlue.rgb());
        }
    }

    // Rendering Text
    renderer->renderText(text, x, y, scale, color, alignment);
}
glm::vec2 TextInput::getCursorPosition(std::string text, int cursor, float x, float y, float scale, TextAlign alignment, Font* font)
{
    // Storing initial X
    float initialX = x;

    // Calculating Alignment
    float alignmentOffset = 0;
    if (alignment == RIGHT)
    {
        float textWidth = 0;
        std::string::const_iterator c;
        for (c = text.begin(); c != text.end(); c++)
        {
            textWidth += (c == (text.end() - 1) ? 0 : font->getCharacter(c).Bearing.x) + font->getCharacter(c).Advance >> 6;
        }
        alignmentOffset = textWidth * scale;
    }
    else if (alignment == CENTER)
    {
        float textWidth = 0;
        std::string::const_iterator c;
        for (c = text.begin(); c != text.end(); c++)
        {
            textWidth += (c == (text.end() - 1) ? 0 : font->getCharacter(c).Bearing.x) + font->getCharacter(c).Advance >> 6;
        }
        alignmentOffset = textWidth * scale * 0.5;
    }

    // iterate through all characters
    std::string::const_iterator c;
    float xpos, ypos;
    for (c = text.begin(); c != text.end() + cursor; c++)
    {
        Character ch = font->getCharacter(c);

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

        xpos = x + ch.Bearing.x * scale - alignmentOffset;
        ypos = y - ((ch.Size.y - ch.Bearing.y) * scale);

        // now advance cursors for next glyph
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (1/64th times 2^6 = 64)
    }
    if (text.length() > 0)
    {
        Character ch = font->getCharacter(text.end() - 1);
        xpos = x + ch.Bearing.x * scale - alignmentOffset;
    }
    else
    {
        xpos = x - alignmentOffset;
    }
    ypos = y + (46 * scale * 0.5f);

    return glm::vec2(xpos, ypos);
}