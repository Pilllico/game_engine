#version 150

uniform sampler2D texture_grass;
uniform sampler2D texture_rock;
uniform sampler2D texture_snow;

in vec2 v_texcoord;
in vec3 v_position;
//! [0]
void main()
{
    // Set fragment color from texture
    if (v_position.z > 0.4f){
        gl_FragColor = texture2D(texture_snow, v_texcoord);
    }
    else if (v_position.z > 0.3f){
        float alt = (v_position.z - 0.3f) * 10;
        gl_FragColor = alt * texture2D(texture_snow, v_texcoord) + (1 - alt) * texture2D(texture_rock, v_texcoord);
    }
    else if (v_position.z > 0.2f){
        gl_FragColor = texture2D(texture_rock, v_texcoord);
    }
    else if (v_position.z > 0.1f){
        float alt = (v_position.z - 0.1f) * 10;
        gl_FragColor = alt * texture2D(texture_rock, v_texcoord) + (1 - alt) * texture2D(texture_grass, v_texcoord);
    }
    else{
        gl_FragColor = texture2D(texture_grass, v_texcoord);
    }
}
//! [0]

