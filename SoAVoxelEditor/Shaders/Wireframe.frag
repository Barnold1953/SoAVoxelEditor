#version 130

// Interpolated values from the vertex shaders
in vec4 fragmentColor;

// Ouput data
out vec4 color;

// Values that stay constant for the whole mesh.

void main(){
    color = vec4(fragmentColor);
}