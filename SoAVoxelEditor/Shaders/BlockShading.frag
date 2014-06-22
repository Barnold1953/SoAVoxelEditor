#version 130

// Interpolated values from the vertex shaders
in vec4 fragmentColor;
in vec3 normal_worldspace;
in vec2 tPos;
in float tType;


// Ouput data
out vec4 color;

// Values that stay constant for the whole mesh.

uniform vec3 lightPosition_worldspace;
uniform sampler2D texture;
uniform sampler2D selTexture;

void main(){
    
    vec3 l = lightPosition_worldspace;
    vec3 n = normal_worldspace;
    
    float cosTheta = clamp( dot( n, l ), 0,1 );
    
    float hold = tType * .000000001;
    if (tType == 0.0){
        vec3 tColor = texture2D(texture, tPos).rgb * cosTheta;
        color = vec4(tColor, fragmentColor.a);
    }
    else if(tType == 1.0){
        vec3 tSelColor = texture2D(selTexture, tPos).rgb * cosTheta;
        color = vec4(tSelColor, fragmentColor.a);
    }
    else{
        vec3 diffuseColor = fragmentColor.rgb * cosTheta;
        color = vec4(diffuseColor, fragmentColor.a);
    }
    //diffuse
    
    
    //specular
    //vec3 H = normalize(l + EyeDirection_worldspace);
    //float NdotH = dot(n, H);
	//NdotH = clamp(NdotH, 0.0, 1.0);
	
	//vec3 diffuseColor = texture2D(texture, tPos).rgb * cosTheta;
    
}