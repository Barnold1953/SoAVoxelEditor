#version 130

in vec3 vertexPosition;

// Output data ; will be interpolated for each fragment.
out vec4 fragmentColor;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;
uniform vec4 meshColor;

void main(){
	// Output position of the vertex, in clip space : MVP * (position)
	gl_Position =  MVP * vec4( vertexPosition ,1);
    
	fragmentColor = meshColor;
}