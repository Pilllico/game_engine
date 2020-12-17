#version 450 core // Minimal GL version support expected from the GPU

layout(location=0) in vec3 vPosition; // The 1st input attribute is the position (CPU side: glVertexAttrib 0)
layout(location=1) in vec3 vNormal;
layout(location=2) in vec2 vTexCoord;

struct LightSource {
    vec3 position;
    vec3 color;
    vec3 direction;
    float intensity;
    int isActive;
    mat4 depthMVP;
};

const int number_of_light = 4;
uniform LightSource lightSources[number_of_light];

uniform mat4 projectionMat, modelViewMat, normalMat;

out vec3 fPosition;
out vec3 fPositionWorldSpace;
out vec3 fNormal;
out vec2 fTexCoord;
out vec4 fPosLightSpace[number_of_light];

void main() {
    fPositionWorldSpace = vPosition;
    vec4 p = modelViewMat * vec4 (vPosition, 1.0);
    gl_Position =  projectionMat * p; // mandatory to fire rasterization properly

    for(int i = 0; i < number_of_light; i++){
        fPosLightSpace[i] = lightSources[i].depthMVP * vec4 (vPosition, 1.0);
    }

    vec4 n = normalMat * vec4 (vNormal, 1.0);

    fPosition = p.xyz;
    fNormal = normalize (n.xyz);
    fTexCoord = vTexCoord;
}
