#ifndef UI_POSITIONING
#pragma once

#include <glm/glm.hpp>

namespace ui
{
    struct WindowPosition
    {
        /*
            THE STRUCTURE
             *---------*
             |         |
             |         | < (height)
             |         |
             *---------*
             ^    ^
             | (width)
             |
            (offset.x, offset.y)
        */

        // Constructors
        WindowPosition(float width, float height, float xoffset, float yoffset, float bufferSize, float unitScale)
            : dimensions(glm::vec2(width, height)), offset(glm::vec2(xoffset, yoffset)), bufferSize(bufferSize), unitScale(unitScale) {}
            WindowPosition(float width, float height, glm::vec2 offset, float bufferSize, float unitScale)
            : dimensions(glm::vec2(width, height)), offset(offset), bufferSize(bufferSize), unitScale(unitScale) {}
            WindowPosition(glm::vec2 dimensions, glm::vec2 offset, float bufferSize, float unitScale)
            : dimensions(dimensions), offset(offset), bufferSize(bufferSize), unitScale(unitScale) {}

        // Getters
        float getBuffer() const { return bufferSize * unitScale; }
        float getWidth() const { return dimensions.x; }
        float getHeight() const { return dimensions.y; }
        float getXOffset() const { return offset.x; }
        float getYOffset() const { return offset.y; }
        float getAspectRatio() const { return (dimensions.x / dimensions.y); }
        glm::vec4 getCorners() const { return glm::vec4(offset.x, offset.y, dimensions.x + offset.x, dimensions.y + offset.y); }
        glm::vec4 getBufferedCorners() const { return glm::vec4(getBuffer() + offset.x, getBuffer() + offset.y, dimensions.x + offset.x - getBuffer(), dimensions.y + offset.y - getBuffer()); }

        // Setters
        void setDimensions(glm::vec2 dimensions) { this->dimensions = dimensions; }
        void setDimensions(float width, float height) { this->dimensions = glm::vec2(width, height); }
        void setOffset(glm::vec2 offset) { this->offset = offset; }
        void setOffset(float xOffset, float yOffset) { this->offset = glm::vec2(xOffset, yOffset); }
        void setBufferSize(float bufferSize) { this->bufferSize = bufferSize; }
        void setPosition(float width, float height, float xOffset, float yOffset, float bufferSize) { setDimensions(width, height); setOffset(xOffset, yOffset); setBufferSize(bufferSize); }
        void setPosition(glm::vec2 dimensions, glm::vec2 offset, float bufferSize) { setDimensions(dimensions); setOffset(offset); setBufferSize(bufferSize); }
        void setPosition(float width, float height, float xOffset, float yOffset) { setDimensions(width, height); setOffset(xOffset, yOffset); }
        void setPosition(glm::vec2 dimensions, glm::vec2 offset) { setDimensions(dimensions); setOffset(offset); }

        // Member Variables
        glm::vec2 dimensions;
        glm::vec2 offset;
        float bufferSize;
        float unitScale;
    };

    struct ElementPosition
    {
        /*
                 THE STRUCTURE
            (lx,ty)----|----(rx,ty)      (x,w)----|----(z,w)
               |       |       |           |      |      |
               |       |       |     OR    |      |      |
               |       |       |           |      |      |
            (lx,by)----|----(rx,by)      (x,y)----|----(z,y)
                       ^
                     split

            Coordinates are expected to be relative to parentWindow origin
        */

        // Constructors
        ElementPosition(glm::vec4 corners, float split, WindowPosition* parentWindow)
            : left_x(corners.x), right_x(corners.z), bottom_y(corners.y), top_y(corners.w), split(split), parentWindow(parentWindow) {}
        ElementPosition(glm::vec2 center, glm::vec2 size, float split, WindowPosition* parentWindow)
            : left_x(center.x - size.x * 0.5f), right_x(center.x + size.x * 0.5f), bottom_y(center.y - size.y * 0.5f), top_y(center.y + size.y * 0.5f), split(split), parentWindow(parentWindow) {}
        ElementPosition(float left_x, float right_x, float bottom_y, float top_y, float split, WindowPosition* parentWindow)
            : left_x(left_x), right_x(right_x), bottom_y(bottom_y), top_y(top_y), split(split), parentWindow(parentWindow) {}
        ElementPosition()
            : left_x(0), right_x(0), bottom_y(0), top_y(0), split(0), parentWindow(nullptr) {}

        // Multi-Getters
        glm::vec4 getCorners() const { return glm::vec4(left_x, bottom_y, right_x, top_y); }
        glm::vec4 getLeftCorners(glm::vec2 offset = glm::vec2(0)) const { return glm::vec4(left_x + offset.x, bottom_y + offset.y, split + offset.x, top_y + offset.y); }
        glm::vec4 getLeftCorners(float width) const { return glm::vec4(split - width, bottom_y, split, top_y); }
        glm::vec4 getRightCorners(glm::vec2 offset = glm::vec2(0)) const { return glm::vec4(split + offset.x, bottom_y + offset.y, right_x + offset.x, top_y + offset.y); }
        glm::vec4 getRightCorners(float width) const { return glm::vec4(split, bottom_y, split + width, top_y); }
        glm::vec4 getEndCorners(float width) const { return glm::vec4(right_x - width, bottom_y, right_x, top_y); }
        float getXOffset() const { return (parentWindow ? parentWindow->getXOffset() : 0); }
        float getYOffset() const { return (parentWindow ? parentWindow->getYOffset() : 0); }
        glm::vec2 getOffset() const { return glm::vec2(getXOffset(), getYOffset()); }
        glm::vec4 getOffsetCorners() const { return glm::vec4(getXOffset(), getYOffset(), getXOffset(), getYOffset()); }
        float getMiddleAfterSplit() const { return split + (right_x - split) * 0.5f; }
        float getWidthAfterSplit() const { return right_x - split; }
        float getMiddleBeforeSplit() const { return split + (split - left_x) * 0.5f; }
        float getWidthBeforeSplit() const { return split - left_x; }
        float getWidth() const { return abs(right_x - left_x); }
        float getHeight() const { return abs(top_y - bottom_y); }
        float getXCenter() const { return left_x + getWidth() * 0.5f; }
        float getYCenter() const { return bottom_y + getHeight() * 0.5f; }
        float getBuffer() const { return (parentWindow ? parentWindow->getBuffer() : 0); }
        float getFixedUnit() const { return (parentWindow ? parentWindow->getBuffer() : 0); }

        // Multi-Setters
        void setCorners(glm::vec4 corners) {
            left_x = corners.x;
            right_x = corners.z;
            bottom_y = corners.y;
            top_y = corners.z;
        }
        void setCorners(float left_x, float right_x, float bottom_y, float top_y) {
            this->left_x = left_x;
            this->right_x = right_x;
            this->bottom_y = bottom_y;
            this->top_y = top_y;
        }
        void setPositions(glm::vec4 corners, float split)
        {
            setCorners(corners);
            this->split = split;
        }
        void setPositions(float left_x, float right_x, float bottom_y, float top_y, float split)
        {
            setCorners(left_x, right_x, bottom_y, top_y);
            this->split = split;
        }

        float left_x, right_x, bottom_y, top_y, split;
        WindowPosition* parentWindow;
    };

    class Interactable
    {
    public:
        // Constructor
        Interactable(glm::vec2 position) : position(position) {}
        Interactable() : position(glm::vec2(0)) {}

        // Getters
        glm::vec2 getPosition() const { return position; }

        // Setters
        void setPosition(glm::vec2 position) { this->position = position; }

        // Functions
        virtual bool checkCollision(glm::vec2 position) = 0;

    protected:
        glm::vec2 position;
    };

    class QuadInteractable : public Interactable
    {
    public:
        // Constructors
        QuadInteractable(glm::vec2 position, glm::vec2 size) { setDimensions(position, size); }
        QuadInteractable(glm::vec4 corners) { setDimensions(corners); }

        // Getters
        glm::vec2 getPosition() const { return Interactable::getPosition(); }
        glm::vec2 getSize() const { return size; }
        glm::vec4 getCorners() const { return glm::vec4(
            position.x - (size.x * 0.5f), 
            position.y - (size.y * 0.5f), 
            position.x + (size.x * 0.5f),
            position.y + (size.y * 0.5f)
            ); }

        // Setters
        void setDimensions(glm::vec2 position, glm::vec2 size) { Interactable::position = position; this->size = size; }
        void setDimensions(glm::vec4 corners) { setDimensions(
            glm::vec2((corners.z - corners.x) * 0.5f + corners.x, (corners.w - corners.y) * 0.5f + corners.y),
            glm::vec2(corners.z - corners.x, corners.w - corners.y)); }

        // Functions
        bool checkCollision(glm::vec2 position) override { return smath::checkUICollision(position, getCorners()); }

    private:
        glm::vec2 size;
    };

    class EllipseInteractable : public Interactable
    {
    public:
        // Constructor
        EllipseInteractable(glm::vec2 position, glm::vec2 radii) : Interactable(position), radii(radii) {}
        EllipseInteractable(glm::vec2 position, float radius) : Interactable(position), radii(glm::vec2(radius)) {}
        EllipseInteractable(glm::vec2 position, float radiusX, float radiusY) : Interactable(position), radii(glm::vec2(radiusX, radiusY)) {}

        // Getters
        glm::vec2 getPosition() const { return Interactable::getPosition(); }
        glm::vec2 getRadii() const { return radii; }

        // Setters
        void setXRadius(float radius) { this->radii.x = radius; }
        void setYRadius(float radius) { this->radii.y = radius; }
        void setRadius(float radius) { this->radii = glm::vec2(radius); }
        void setRadii(glm::vec2 radii) { this->radii = radii; }

        // Functions
        bool checkCollision(glm::vec2 position) override { return smath::checkUICollisionEllipse(position, Interactable::position, radii); }

    private:
        glm::vec2 radii;
    };
}
#endif // !UI_POSITIONING
