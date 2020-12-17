#version 450 core // Minimal GL version support expected from the GPU

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

in vec3 fPosition; // Shader input, linearly interpolated by default from the previous stage (here the vertex shader)
in vec3 fPositionWorldSpace;
in vec3 fNormal;
in vec2 fTexCoord;
in vec4 fPosLightSpace[number_of_light];

struct Material {
    vec3 albedo;
    float shininess;
};

uniform Material material;

out vec4 colorResponse; // Shader output: the color response attached to this fragment

float pi = 3.1415927;

uniform sampler2D shadowMap[number_of_light];

float ShadowCalculation(vec4 fPosLight, sampler2D shadowM, LightSource light){
    vec2 texelSize = 1.0 / textureSize(shadowM, 0);
    vec3 projCoords = fPosLight.xyz / fPosLight.w;
    projCoords = projCoords * 0.5 + 0.5;

    float closestDepth = texture2D(shadowM, projCoords.xy).r;
    float currentDepth = projCoords.z;

    float shadow = 0;
    float bias = max(0.05 * (1.0 - dot(fNormal, light.direction)), 0.005);

    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(shadowM, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;

    if (projCoords.z > 1.0) {shadow = 0.0f;}
    return shadow;
}

void main() {
    vec3 n = normalize(fNormal);

    // Linear barycentric interpolation does not preserve unit vectors
    vec3 wo = normalize (-fPosition); // unit vector pointing to the camera
    vec3 radiance = vec3(0,0,0);

    if( dot( n , wo ) >= 0.0 ) {
        for(int i = 0; i < number_of_light; i++ ){
            if( lightSources[i].isActive == 1 ) { // WE ONLY CONSIDER LIGHTS THAT ARE SWITCHED ON
                vec3 wi = normalize ( vec3((modelViewMat * vec4(lightSources[i].position,1)).xyz) - fPosition ); // unit vector pointing to the light source (change if you use several light sources!!!)
                if( dot( wi , n ) >= 0.0 ) { // WE ONLY CONSIDER LIGHTS THAT ARE ON THE RIGHT HEMISPHERE (side of the tangent plane)
                    vec3 wh = normalize( wi + wo ); // half vector (if wi changes, wo should change as well)
                    vec3 Li = lightSources[i].color * lightSources[i].intensity;

                    float shadow = ShadowCalculation(fPosLightSpace[i], shadowMap[i], lightSources[i]);
                    radiance += Li * min(max(1.0 - shadow, 0.1f), 2.0 / number_of_light) * material.albedo * (max(dot(n,wi),0.0) + pow(max(dot(n,wh),0.0),material.shininess));
                }
            }
        }
    }

    colorResponse = vec4 (radiance, 1.0); // Building an RGBA value from an RGB one.
}

