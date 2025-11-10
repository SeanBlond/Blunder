#ifndef TEXT_INPUT
#pragma once

#include <sstream>
#include <iostream>
#include <iomanip>
#include <vector>

#include "../../ew/external/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "../../shader/shader.h"
#include "../../math/smath.h"
#include "../../Font/font.h"
#include "UIRenderer.h"
#include "../Color.h"

namespace ui
{
    class TextInput
    {
    public:
        TextInput(std::string text, int cursor = 0) : text(text), typing(false) { setCursor(cursor); setSelection(0); }

        // Getters
        int getCursor() { return cursor; }
        std::string getText() { return text; }
        bool getTyping() { return typing; }
        bool getStored() { return valueStored; }
        int getSelection() { return selectPos; }
        bool getSelecting() { return selecting; }

        // Setters
        void setCursor(int cursor) { this->cursor = smath::clamp(cursor, -(int)(text.length()), 0); }
        void setText(std::string text) { this->text = text; }
        void setTyping(bool typing) { this->typing = typing; }
        void setStored(bool stored) { this->valueStored = stored; }
        void setSelection(int position) { this->selectPos = smath::clamp(position, 0, (int)text.length()); }
        void setSelecting(bool selecting) { this->selecting = selecting; }

        // Functions
        void insert(char character);
        void insert(const char* string);
        void remove();
        void shiftCursor(int amount);
        void resetSelection();
        void updateSelection();
        void selectAll();
        void renderText(ui::UIRenderer* renderer, float x, float y, float scale, glm::vec3 color = glm::vec3(1), TextAlign alignment = LEFT);
        std::string returnSelected();
        std::string cutSelected();

    private:
        int cursor;
        bool typing;
        bool selecting;
        int selectPos;
        bool valueStored;
        std::string text;

        glm::vec2 getCursorPosition(std::string text, int cursor, float x, float y, float scale, TextAlign alignment, Font* font);
    };
}

#endif // !TEXT_NPUT
