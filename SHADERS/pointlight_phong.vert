#version 430

#define NUM_POINT_LIGHTS 6
#define NUM_SPOT_LIGHTS 2

uniform int texMode;

uniform mat4 m_Model;  

struct PointLight {
    vec4 position;
};

struct SpotLight {
    vec4 position;
    vec3 direction;
    float cutoff;
};

struct DirectionalLight {
    vec4 direction;
};

in vec4 texCoord;

uniform mat4 m_pvm;
uniform mat4 m_viewModel;
uniform mat3 m_normal;

in vec4 position;
in vec4 normal;

out Data {
    vec3 fragPosition;
    vec3 fragNormal;
    vec2 tex_coord;
    vec3 skyboxTexCoord;
} DataOut;

void main() {
    // Transform position and normal into view space

    vec4 viewPosition = m_viewModel * position;
    DataOut.fragPosition = viewPosition.xyz;
    DataOut.fragNormal = normalize(m_normal * normal.xyz);
    DataOut.tex_coord = texCoord.st;

    DataOut.skyboxTexCoord = vec3(m_Model * position);	//Transformação de modelação do cubo unitário 
	DataOut.skyboxTexCoord.x = - DataOut.skyboxTexCoord.x;

    // Set the position in clip space
    gl_Position = m_pvm * position;
}
