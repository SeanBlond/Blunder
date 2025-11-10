#version 330 core
out vec4 FragColor;  
in vec3 normal;
in vec2 texCoord;

uniform sampler2D texture1;

void main()
{
    //FragColor = vec4(vec3(0.43179), 1);
    FragColor = texture(texture1, texCoord);
}