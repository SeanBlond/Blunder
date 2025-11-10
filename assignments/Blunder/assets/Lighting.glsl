#version 330 core
out vec4 FragColor;  
in vec3 normal;
in vec2 texCoord;

void main()
{
    vec3 light = vec3(0, 1, 1);
    float shading = (dot(normal, normalize(light)) + 1) / 2;
    vec3 color = vec3(0.9, 0.9, 0.9) * shading;

    FragColor = vec4(color, 1);
}