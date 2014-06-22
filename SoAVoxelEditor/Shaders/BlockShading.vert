#version 130

in vec3 vertexPosition;
in vec4 vertexColor;
in vec3 vertexNormal;
in vec2 textCoordinate;
in float textureType;


// Output data ; will be interpolated for each fragment.
out vec4 fragmentColor;
out vec3 normal_worldspace;
out vec2 tPos;
out float tType;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;

void main(){
    tPos = textCoordinate;
	// Output position of the vertex, in clip space : MVP * (position)
	gl_Position =  MVP * vec4( vertexPosition,1);
	
    normal_worldspace = normalize(vertexNormal);
    
	fragmentColor = vertexColor;
    tType = textureType;
}