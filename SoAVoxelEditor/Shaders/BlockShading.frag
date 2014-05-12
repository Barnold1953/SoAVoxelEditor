#version 130

// Interpolated values from the vertex shaders
in vec4 fragmentColor;
in vec3 normal_worldspace;
in vec2 tPos;

// Ouput data
out vec4 color;

// Values that stay constant for the whole mesh.

uniform vec3 lightPosition_worldspace;
uniform sampler2D texture;

void main(){
    
    vec3 l = lightPosition_worldspace;
    vec3 n = normal_worldspace;
    
    //diffuse
    float cosTheta = clamp( dot( n, l ), 0,1 );
    
    //specular
    //vec3 H = normalize(l + EyeDirection_worldspace);
    //float NdotH = dot(n, H);
	//NdotH = clamp(NdotH, 0.0, 1.0);
	
	//vec3 diffuseColor = fragmentColor.rgb * cosTheta;
    vec3 diffuseColor = texture2D(texture, tPos).rgb * cosTheta;
       
    color = vec4(diffuseColor, fragmentColor.a);
}