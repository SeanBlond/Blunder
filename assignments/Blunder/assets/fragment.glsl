#version 330 core
out vec4 FragColor;  
in vec3 normal;
in vec2 texCoord;

uniform float time;

// Variables
float PI = 3.141592654;
float gridAmount = 60.0;
vec3 red = vec3(1.0, 0.0, 0.0);
vec3 blue = vec3(0.0, 0.0, 1.0);
vec3 darkBlue = vec3(0.0, 0.0, 0.2);

void main()
{
    // Creating a centered UV map
    vec2 uv = texCoord  - vec2(0.5);
    
    // Creating a grid of UVs
    vec2 grid = fract(uv * gridAmount) - vec2(0.5);

    // Creating circles on the UV map
    float circle = 1.0 - length(grid);

    // Creating a Spiral by converting to polar coordinates   
    vec2 floorUV = floor(uv * gridAmount) / gridAmount;
    
    float r = length(floorUV);
	float a = atan(floorUV.y, floorUV.x);
    float rings = 40.0;
	float spiral = sin(rings * (r - (4.0 / rings) * (a + 2.5 * time)));
	spiral = (spiral + 1.) / 2.;
    float mixSpiral = pow(spiral, (1.0/8.0));
    
    // Combining the Spiral with the Circles
    float mixValue = mix(0.0, mixSpiral, circle);
    mixValue = pow(mixValue, 0.5);
    mixValue = 1.0 - step(mixValue, 0.8);
    
    // Changing the Colors of the Spiral
    vec3 color = mix(blue, red, spiral);
    color = mix(color, vec3(1.0), pow(spiral, 8.0) * 0.7);
    color = mix(darkBlue, color, mixValue);

    // Outputting the final color
    FragColor = vec4(color, 1.0);
}
