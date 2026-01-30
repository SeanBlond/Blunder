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
void TextInput::addText(ui::UIRenderer* renderer, glm::vec3 position, float scale, glm::vec3 color, TextAlign alignment)
{
    // Rendering Cursor & Selection
    if (typing)
    {
        // Cursor
        glm::vec2 cursorPos = getCursorPosition(text, cursor, position.x, position.y, scale, alignment, renderer->getTextRenderer());
        //renderer->renderQuad(glm::vec3(cursorPos, 0), glm::vec2(5, scale), glm::vec3(0.8f));
        renderer->addQuad(glm::vec3(cursorPos, 0.25f), glm::vec2(3, 60 * scale), glm::vec3(0.8f));

        // Selection
        if (selecting)
        {
            glm::vec2 selectEndPos = getCursorPosition(text, (selectPos - (int)text.length()), position.x, position.y, scale, alignment, renderer->getTextRenderer());
            glm::vec2 selectPos = (cursorPos + selectEndPos) * 0.5f;
            float selectSize = abs(selectEndPos.x - cursorPos.x);
            renderer->addQuad(glm::vec3(selectPos, 0.225f), glm::vec2(selectSize, 60 * scale), colors::selectionBlue.rgb());
        }
    }

    // Rendering Text
    renderer->addText(text, position, scale, color, alignment);
}
glm::vec2 TextInput::getCursorPosition(std::string text, int cursor, float x, float y, float scale, TextAlign alignment, Font* font)
{
    // Calculating Alignment
    float alignmentOffset = 0;
    float xpos = 0.0f;
    float ypos = 0.0f;

    if (alignment == RIGHT)
    {
        float textWidth = 0;
        for (int i = 0; i < text.size(); i++)
        {
            textWidth += (i == (text.size() - 1) ? 0 : font->getCharacter(text[i]).Bearing.x) + font->getCharacter(text[i]).Advance;
        }
        alignmentOffset = textWidth * scale;
    }
    else if (alignment == CENTER)
    {
        float textWidth = 0;
        for (int i = 0; i < text.size(); i++)
        {
            textWidth += (i == (text.size() - 1) ? 0 : font->getCharacter(text[i]).Bearing.x) + font->getCharacter(text[i]).Advance;
        }
        alignmentOffset = textWidth * scale * 0.5f;
    }

    // Offsetting X
    x -= (alignmentOffset);

    // Storing initial X
    float initialX = x;

    // Checking if the cursor is at the beginning
    if (text.size() + cursor == 0)
    {
        // Returning space right before the first char
        Character ch = font->getCharacter(text[0]);

        // Checking for New Line
        if (text[0] == '\n')
        {
            y -= font->getLineHeight() * 1.3 * scale;
            x = initialX;
        }
        else if (text[0] == ' ')
        {
            x += ch.Advance * scale;
        }

        // Calculating pos relative to the letter
        xpos = x + (ch.Bearing.x * scale);
        ypos = y;
    }

    // Iterate through all characters
    for (int i = 0; i < text.size() + cursor; i++)
    {
        Character ch = font->getCharacter(text[i]);

        // Checking for New Line
        if (text[i] == '\n')
        {
            y -= font->getLineHeight() * 1.3 * scale;
            x = initialX;
        }
        else if (text[i] == ' ')
        {
            x += ch.Advance * scale;
        }

        // Advancing to next character
        x += ch.Advance * scale;

        // Calculating pos relative to the letter
        xpos = x + (ch.Bearing.x * scale);
        ypos = y;

    }

    return glm::vec2(xpos, ypos);
}