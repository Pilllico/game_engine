#version 150

uniform sampler2D texture_map;

uniform mat4 mvp_matrix;

in vec4 a_position;
in vec2 a_texcoord;

out vec2 v_texcoord;
out vec3 v_position;

//! [0]
void main()
{
    // Calculate vertex position in screen space

    vec4 a_p_copy = a_position;
    a_p_copy.z = texture2D(texture_map, a_texcoord).x;
    gl_Position = mvp_matrix * a_p_copy;

    // Pass texture coordinate to fragment shader
    // Value will be automatically interpolated to fragments inside polygon faces
    v_texcoord = a_texcoord;
    v_position = a_p_copy.xyz;
}
//! [0]
