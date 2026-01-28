#include "mesh.h"

// Constructors
Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, PrimitiveType type)
{
    this->vertices = vertices;
    this->indices = indices;
    this->type = type;

    createMesh();
}
Mesh::Mesh(Mesh* mesh)
{
    this->vertices = mesh->vertices;
    this->indices = mesh->indices;
    this->type = mesh->type;

    createMesh();
}

// Functions
void Mesh::createMesh()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_DYNAMIC_DRAW);

    // Vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    // Vertex Normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

    // Vertex Texture Coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoord));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
void Mesh::DrawMesh(bool lines, bool points)
{
    // Drawing the mesh
    glBindVertexArray(VAO);

    // Backfill Culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // Rendering Lines or Faces
    if (lines)
        glPolygonMode(GL_FRONT, GL_LINE);
    else
        glPolygonMode(GL_FRONT, GL_FILL);

    // Rendering Points
    if (points)
    {
        glPointSize(10.0f);
        glDrawArrays(GL_POINTS, 0, vertices.size());
    }
    // Rendering the Object
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}
void Mesh::updateMesh(const Mesh& mesh)
{
    this->vertices = mesh.vertices;
    this->indices = mesh.indices;

    glBindVertexArray(VAO);

    std::cout << "Updating mesh with " << vertices.size() << " vertices" << std::endl;

    // Updating VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_DYNAMIC_DRAW);

    // Updating EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_DYNAMIC_DRAW);
}
void Mesh::outputMesh()
{
    std::cout << "Positions | Normals | UV Coords" << std::endl;
    for (int i = 0; i < vertices.size(); i++)
    {
        std::cout << "Vertice #" << i << ":   ";
        std::cout << (vertices[i].Position.x) << ", " << (vertices[i].Position.y) << ", " << (vertices[i].Position.z) << " | ";
        std::cout << (vertices[i].Normal.x) << ", " << (vertices[i].Normal.y) << ", " << (vertices[i].Normal.z) << " | ";
        std::cout << (vertices[i].TexCoord.x) << ", " << (vertices[i].TexCoord.y) << std::endl;
    }
}

// Operators
Mesh Mesh::operator=(const Mesh& aMesh)
{
    updateMesh(aMesh);

    return *this;
}

// Extra Functions
int countCharInString(std::string text, char key)
{
    int count = 0;
    for (int i = 0; i < text.size(); i++)
    {
        if (text[i] == key)
            count++;
    }
    return count;
}

// Mesh Building Functions
extern Mesh* mesh::loadFromFile(std::string filePath)
{
    // Setting Up Initial Data
    std::ifstream inputFile(filePath);
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // Checking if Input File was accessed correctly
    if (inputFile.fail())
    {
        std::cout << "Mesh File Failed to Load" << std::endl;
        return nullptr;
    }

    std::string tempLine;
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texCoords;

    // Running through each line assigning that value to the respective vector
    while (!inputFile.eof())
    {
        std::getline(inputFile, tempLine);
        std::istringstream line(tempLine);

        // Reading Vertex Data
        if (tempLine[0] == 'v')
        {
            // Reading Texture Coordinate
            if (tempLine[1] == 't')
            {
                glm::vec2 tempVec;
                std::string tempString;
                std::getline(line, tempString, ' '); // Getting rid of initial text

                // Getting Vector Values
                std::getline(line, tempString, ' ');
                tempVec.x = std::stof(tempString);
                std::getline(line, tempString, ' ');
                tempVec.y = std::stof(tempString);

                texCoords.push_back(tempVec);
            }

            // Reading Normal Data
            else if (tempLine[1] == 'n')
            {
                glm::vec3 tempVec;
                std::string tempString;
                std::getline(line, tempString, ' '); // Getting rid of initial text

                // Getting Vector Values
                std::getline(line, tempString, ' ');
                tempVec.x = std::stof(tempString);
                std::getline(line, tempString, ' ');
                tempVec.y = std::stof(tempString);
                std::getline(line, tempString, ' ');
                tempVec.z = std::stof(tempString);

                normals.push_back(tempVec);
            }

            // Reading position
            else
            {
                glm::vec3 tempVec;
                std::string tempString;
                std::getline(line, tempString, ' '); // Getting rid of initial text

                // Getting Vector Values
                std::getline(line, tempString, ' ');
                tempVec.x = std::stof(tempString);
                std::getline(line, tempString, ' ');
                tempVec.y = std::stof(tempString);
                std::getline(line, tempString, ' ');
                tempVec.z = std::stof(tempString);

                positions.push_back(tempVec);
            }
        }

        // Reading Indice Data
        else if (tempLine[0] == 'f')
        {
            std::string tempString;
            std::getline(line, tempString, ' '); // Getting rid of initial text


            for (int i = 0; i < 3; i++)
            {
                std::getline(line, tempString, ' ');
                std::istringstream faceLine(tempString);
                std::string tempValue;

                Vertex tempVertex;

                std::getline(faceLine, tempValue, '/');
                if (tempValue == tempString)
                {
                    tempVertex.Position = positions[std::stoi(tempValue) - 1];
                }
                else
                {
                    tempVertex.Position = positions[std::stoi(tempValue) - 1];

                    std::getline(faceLine, tempValue, '/');
                    tempVertex.TexCoord = texCoords[std::stoi(tempValue) - 1];

                    if (countCharInString(tempString, '/') >= 2)
                    {
                        std::getline(faceLine, tempValue, '/');
                        tempVertex.Normal = normals[std::stoi(tempValue) - 1];
                    }

                    vertices.push_back(tempVertex);
                    indices.push_back(vertices.size() - 1);
                }
            }
        }

        // Irrelevant Data
        else
        {
            continue;
        }

    }

    // Returning Mesh Data
    return new Mesh(vertices, indices);
}
extern Mesh* mesh::createSphere(float radius, int segments)
{
    // Mesh Data
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    float thetaStep = smath::TAU / (float)segments;
    float phiStep = smath::PI / (float)segments;

    // Generating Vertices
    for (int row = 0; row <= segments; row++)
    {
        float phi = row * phiStep;

        for (int col = 0; col <= segments; col++)
        {
            float theta = col * thetaStep;

            // Storing Vertex Data
            Vertex tempVertex;

            // Normals
            tempVertex.Normal = glm::vec3(
                cos(theta) * sin(phi),
                cos(phi),
                sin(theta) * sin(phi)
            );

            // Positions
            tempVertex.Position = tempVertex.Normal * radius;

            // UVs
            tempVertex.TexCoord = glm::vec2(
                ((float)col / (float)segments),
                ((float)(segments - row) / (float)segments)
            );

            // Saving Vertex Data
            vertices.push_back(tempVertex);
        }
    }

    // Generating Indices
    for (int row = 0; row < segments; row++)
    {
        for (int col = 0; col < segments; col++)
        {
            // Generating Indices
            int bl = row * (segments + 1) + col;  // tl--tr
            int br = bl + 1;                      //  |\ |
            int tl = bl + (segments + 1);         //  | \|
            int tr = tl + 1;                      // bl--br

            // Indices of Triangle 1
            indices.push_back(bl);  //     tr
            indices.push_back(br);  //    / |
            indices.push_back(tr);  //  bl-br

            // Indices of Triangle 2
            indices.push_back(bl);  //  tl-tr
            indices.push_back(tr);  //  | /
            indices.push_back(tl);  //  bl
        }
    }

    // Returning Mesh
    return new Mesh(vertices, indices, MESH_SPHERE);
}
extern Mesh* mesh::createCylinder(float radius, float height, int segments)
{
    // Mesh Data
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    float thetaStep = smath::TAU / (float)segments;

    // Generating Top Vertices
    for (int caps = -1; caps <= 1; caps += 2)
    {
        // Top Center Pivot
        Vertex centerVertex;
        centerVertex.Position = glm::vec3(0, (height / 2) * caps, 0);
        centerVertex.Normal = glm::vec3(0, caps, 0);
        centerVertex.TexCoord = glm::vec2(0.5f, 0.5f);
        vertices.push_back(centerVertex);

        for (int i = 0; i <= segments; i++)
        {
            float theta = i * thetaStep;

            Vertex tempVertex;

            // Vertex Position
            tempVertex.Position = glm::vec3(
                cos(theta) * radius,
                caps * (height / 2),
                (caps == 1 ? sin(smath::TAU - theta) * radius : sin(theta) * radius)
            );

            // Vertex Normal
            tempVertex.Normal = glm::vec3(0, caps, 0);

            // Vertex UV
            tempVertex.TexCoord = glm::vec2(
                (cos(theta) * 0.5f) + 0.5f,
                (sin(theta) * 0.5f) + 0.5f
            );

            vertices.push_back(tempVertex);
        }
    }

    int sideIndicesStart = vertices.size();

    // Generating Side Vertices
    for (int i = 0; i <= segments; i++)
    {
        float theta = i * thetaStep;

        Vertex tempVertex;

        // Vertex Position
        tempVertex.Position = glm::vec3(
            cos(theta) * radius,
            -(height / 2),
            sin(theta) * radius
        );

        // Vertex Normal
        tempVertex.Normal = glm::vec3(
            cos(theta),
            0,
            sin(theta)
        );

        // Vertex UV
        tempVertex.TexCoord = glm::vec2(
            ((float)i / segments),
            0
        );

        // Pushing Back Bottom vertex
        vertices.push_back(tempVertex);

        // Pushing Back Top vertex
        tempVertex.Position.y = (height / 2);
        tempVertex.TexCoord.y = 1;
        vertices.push_back(tempVertex);
    }

    // Generating Indices of the Bottom Circles
    for (int i = 1; i <= segments; i++)
    {
        indices.push_back(0);
        indices.push_back(i);
        indices.push_back(i + 1);
    }
    // Generating Indices of the Top Circles
    for (int i = segments + 3; i < (2 * segments + 3); i++)
    {
        indices.push_back(segments + 2);
        indices.push_back(i);
        indices.push_back(i + 1);
    }
    // Generating Indices of the Vertical Vertices
    for (int i = 0; i < segments; i++)
    {
        int bl = sideIndicesStart + i * 2;
        int br = bl + 2;
        int tl = bl + 1;
        int tr = br + 1;

        // First Triangle
        indices.push_back(bl);
        indices.push_back(tr);
        indices.push_back(br);

        // Second Triangle
        indices.push_back(bl);
        indices.push_back(tl);
        indices.push_back(tr);
    }

    // Returning Mesh
    return new Mesh(vertices, indices, MESH_CYLINDER);
}
extern Mesh* mesh::createPlane(float width, float height, int segments)
{
    // Mesh Data
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    float colStep = ((float)width / (float)segments);
    float rowStep = ((float)height / (float)segments);

    // Generating Vertices
    for (int row = 0; row <= segments; row++)
    {
        for (int col = 0; col <= segments; col++)
        {
            // Storing Vertex Data
            Vertex tempVertex;

            // Positions
            tempVertex.Position = glm::vec3(
                col * colStep - (width * 0.5f),
                0.0f,
                -row * rowStep + (height * 0.5f)
            );

            // Normals
            tempVertex.Normal = glm::vec3(0.0f, 1.0f, 0.0f);

            // UVs
            tempVertex.TexCoord = glm::vec2(
                ((float)col / (float)segments),
                ((float)row / (float)segments)
            );

            // Saving Vertex Data
            vertices.push_back(tempVertex);
        }
    }

    // Generating Indices
    for (int row = 0; row < segments; row++)
    {
        for (int col = 0; col < segments; col++)
        {
            // Generating Indices
            int bl = row * (segments + 1) + col;  // tl--tr
            int br = bl + 1;                      //  |\ |
            int tl = bl + (segments + 1);         //  | \|
            int tr = tl + 1;                      // bl--br

            // Indices of Triangle 1
            indices.push_back(bl);  //     tr
            indices.push_back(br);  //    / |
            indices.push_back(tr);  //  bl-br

            // Indices of Triangle 2
            indices.push_back(bl);  //  tl-tr
            indices.push_back(tr);  //  | /
            indices.push_back(tl);  //  bl
        }
    }

    // Returning Mesh
    return new Mesh(vertices, indices, MESH_PLANE);
}
extern Mesh* mesh::createTorus(float majorRadius, float minorRadius, int majorSegments, int minorSegments)
{
    // Mesh Data
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    float phiStep = smath::TAU / minorSegments;
    float thetaStep = smath::TAU / majorSegments;

    // Creating Vertices
    for (int i = 0; i <= minorSegments; i++)
    {
        float verticalRadius = sin(i * phiStep) * minorRadius;
        float largeRadius = majorRadius + (minorRadius * cos(i * phiStep));

        for (int j = 0; j <= majorSegments; j++)
        {
            Vertex tempVertex;

            // Vertex Position
            tempVertex.Position = glm::vec3(
                largeRadius * cos(j * thetaStep),
                verticalRadius,
                largeRadius * sin(j * thetaStep)
            );

            // Vertex Normal NEEDS TO BE DONE
            glm::vec3 circlePos = glm::vec3(majorRadius * cos(j * thetaStep), 0, majorRadius * sin(j * thetaStep));
            tempVertex.Normal = glm::vec3(glm::normalize(tempVertex.Position - circlePos));

            // Vertex UVs
            tempVertex.TexCoord = glm::vec2(
                (float)j / (float)majorSegments,
                (float)i / (float)minorSegments
            );

            vertices.push_back(tempVertex);
        }
    }

    // Creating indices
    int faceAmount = majorSegments * minorSegments;
    for (int i = 0; i < minorSegments; i++)
    {
        for (int j = 0; j < majorSegments; j++)
        {
            // Generating Indices
            int bl = j + (i * (majorSegments + 1));
            int br = bl + 1;
            int tl = bl + (majorSegments + 1);
            int tr = tl + 1;

            // Indices of Triangle 1
            indices.push_back(bl);
            indices.push_back(tl);
            indices.push_back(tr);

            // Indices of Triangle 2
            indices.push_back(tr);
            indices.push_back(br);
            indices.push_back(bl);
        }
    }

    // Returning Mesh Data
    return new Mesh(vertices, indices, MESH_TORUS);
}
extern Mesh* mesh::createCube(float length, float width, float height, int segments)
{
    // Mesh Data
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    float colStep = ((float)width / (float)segments);
    float rowStep = ((float)height / (float)segments);

    // Generating Vertices
    // XY Planes
    for (int row = 0; row <= segments; row++)
    {
        for (int col = 0; col <= segments; col++)
        {
            // Storing Vertex Data
            Vertex tempVertex;

            // Positions
            tempVertex.Position = glm::vec3(
                col * colStep - (width * 0.5f),
                row * rowStep - (height * 0.5f),
                length * 0.5f
            );

            // Normals
            tempVertex.Normal = glm::vec3(0.0f, 0.0f, -1.0f);

            // UVs
            tempVertex.TexCoord = glm::vec2(
                ((float)col / (float)segments),
                ((float)row / (float)segments)
            );

            // Saving Vertex Data
            vertices.push_back(tempVertex);

            // Creating vertice at opposite side
            tempVertex.Position = glm::vec3(
                col * colStep - (width * 0.5f),
                -row * rowStep + (height * 0.5f),
                -length * 0.5f
            );
            tempVertex.Normal.z *= -1;
            vertices.push_back(tempVertex);
        }
    }

    // XZ Planes
    for (int row = 0; row <= segments; row++)
    {
        for (int col = 0; col <= segments; col++)
        {
            // Storing Vertex Data
            Vertex tempVertex;

            // Positions
            tempVertex.Position = glm::vec3(
                col * colStep - (width * 0.5f),
                height * 0.5f,
                -row * rowStep + (length * 0.5f)
            );

            // Normals
            tempVertex.Normal = glm::vec3(0.0f, 1.0f, 0.0f);

            // UVs
            tempVertex.TexCoord = glm::vec2(
                ((float)col / (float)segments),
                ((float)row / (float)segments)
            );

            // Saving Vertex Data
            vertices.push_back(tempVertex);

            // Creating vertice at opposite side
            tempVertex.Position = glm::vec3(
                col * colStep - (width * 0.5f),
                -height * 0.5f,
                row * rowStep - (length * 0.5f)
            );
            tempVertex.Normal.y *= -1;
            vertices.push_back(tempVertex);
        }
    }

    // YZ Planes
    for (int row = 0; row <= segments; row++)
    {
        for (int col = 0; col <= segments; col++)
        {
            // Storing Vertex Data
            Vertex tempVertex;

            // Positions
            tempVertex.Position = glm::vec3(
                width * 0.5f,
                col * colStep - (height * 0.5f),
                row * rowStep - (length * 0.5f)
            );

            // Normals
            tempVertex.Normal = glm::vec3(1.0f, 0.0f, 0.0f);

            // UVs
            tempVertex.TexCoord = glm::vec2(
                ((float)col / (float)segments),
                ((float)row / (float)segments)
            );

            // Saving Vertex Data
            vertices.push_back(tempVertex);

            // Creating vertice at opposite side
            tempVertex.Position = glm::vec3(
                -width * 0.5f,
                col * colStep - (height * 0.5f),
                -row * rowStep + (length * 0.5f)
            );
            tempVertex.Normal.x *= -1;
            vertices.push_back(tempVertex);
        }
    }

    // Generating Indices
    for (int sides = 0; sides < 3; sides++)
    {
        int sideOffset = sides * pow(segments + 1, 2) * 2;
        for (int i = 0; i < (segments * segments); i++)
        {
            for (int j = 0; j < 2; j++)
            {
                // Generating Indices
                int bl = i * 2 + j + sideOffset + ((i / segments) * 2);   // tl--tr
                int br = bl + 2;                   //  |\ | 
                int tl = bl + (segments + 1) * 2;  //  | \| 
                int tr = tl + 2;                   // bl--br

                // Indices of Triangle 1
                indices.push_back(bl);  //     tr
                indices.push_back(br);  //    / |
                indices.push_back(tr);  //  bl-br

                // Indices of Triangle 2
                indices.push_back(bl);  //  tl-tr
                indices.push_back(tr);  //  | /
                indices.push_back(tl);  //  bl
            }
        }
    }
    // Returning Mesh Data
    return new Mesh(vertices, indices, MESH_CUBE);
}
