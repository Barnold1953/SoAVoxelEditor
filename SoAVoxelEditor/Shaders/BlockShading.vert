#version 130

in vec3 vertexPosition;
in vec4 vertexColor;
in vec3 vertexNormal;
in vec2 textCoordinate;
in vec3 offset;
in float textureType;


// Output data ; will be interpolated for each fragment.
out vec4 fragmentColor;
out vec3 normal_worldspace;
out vec2 tPos;
out float tType;

// Values that stay constant for the whole mesh.
uniform mat4 VP;
uniform mat4 M;
mat4 tempM;
mat4 MVP;

void main(){
    tempM = M;
    tempM[0][0] += offset.x;
    tempM[0][1] += offset.y;
    tempM[0][2] += offset.z;
    MVP = VP * tempM;
    tPos = textCoordinate;
	// Output position of the vertex, in clip space : MVP * (position)
	gl_Position =  MVP * vec4( vertexPosition ,1);
	
    normal_worldspace = normalize(vertexNormal);
    
	fragmentColor = vertexColor;
    tType = textureType;
}