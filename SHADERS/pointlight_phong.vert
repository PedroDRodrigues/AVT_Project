#version 430

#define NUM_POINT_LIGHTS 6
#define NUM_SPOT_LIGHTS 2

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

uniform mat4 m_pvm;
uniform mat4 m_viewModel;
uniform mat3 m_normal;

in vec4 position;
in vec4 normal;

out Data {
    vec3 fragPosition;
    vec3 fragNormal;
} DataOut;

void main() {
    // Transform position and normal into view space
    vec4 viewPosition = m_viewModel * position;
    DataOut.fragPosition = viewPosition.xyz;

    DataOut.fragNormal = normalize(m_normal * normal.xyz);

    // Set the position in clip space
    gl_Position = m_pvm * position;
}
