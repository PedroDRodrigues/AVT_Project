#version 430

#define NUM_POINT_LIGHTS 6
#define NUM_SPOT_LIGHTS 2

struct DirectionalLight {
    vec4 direction;
}; 

struct PointLight {
    vec4 position;
};

struct SpotLight {
    vec4 position;
    vec3 direction;
    float cutoff;
};

uniform PointLight pointLightsPos[NUM_POINT_LIGHTS];
uniform SpotLight spotLightsPos[NUM_SPOT_LIGHTS];
uniform DirectionalLight dirLight;

uniform vec4 fogColor;
uniform float fogStart;
uniform float fogDensity;

struct Materials {
    vec4 diffuse;
    vec4 ambient;
    vec4 specular;
    vec4 emissive;
    float shininess;
    int texCount;
    float alpha;
};

uniform Materials mat;

in Data {
    vec3 fragPosition;
    vec3 fragNormal;
} DataIn;

out vec4 colorOut;

vec4 CalcDirLight(DirectionalLight dirLight, vec3 n, vec3 e) {
    vec4 spec = vec4(0.0);
    vec3 l = normalize(vec3(-dirLight.direction));
    float intensity = max(dot(n, l), 0.0);

    if (intensity > 0.0) {
        vec3 h = normalize(l + e);
        float intSpec = max(dot(h, n), 0.0);
        spec = mat.specular * pow(intSpec, mat.shininess);
    }

    return max(intensity * mat.diffuse + spec, mat.ambient);
}

vec4 CalcPointLight(vec3 l, vec3 n, vec3 e) {
    vec4 spec = vec4(0.0);
    float intensity = max(dot(n, l), 0.0);

    if (intensity > 0.0) {
        vec3 h = normalize(l + e);
        float intSpec = max(dot(h, n), 0.0);
        spec = mat.specular * pow(intSpec, mat.shininess);
    }

    return max(intensity * mat.diffuse + spec, mat.ambient);
}

vec4 calcSpotLight(vec3 l, vec3 s, float cutoff, vec3 n, vec3 e) {
    vec4 spec = vec4(0.0);

    if (dot(s, l) > cutoff) {
        float intensity = max(dot(n, l), 0.0);

        if (intensity > 0.0) {
            vec3 h = normalize(l + e);
            float intSpec = max(dot(h, n), 0.0);
            spec = mat.specular * pow(intSpec, mat.shininess);
        }

        return max(intensity * mat.diffuse + spec, mat.ambient);
    }

    return mat.ambient;
}

void main() {
    vec3 n = normalize(DataIn.fragNormal);
    vec3 e = normalize(-DataIn.fragPosition);

    // Initialize color with the directional light contribution
    vec4 color = CalcDirLight(dirLight, n, e);

    // Add contributions from all point lights
    for (int i = 0; i < NUM_POINT_LIGHTS; i++) {
        vec3 pl = vec3(pointLightsPos[i].position - vec4(DataIn.fragPosition, 1.0));
        vec3 l = normalize(pl);
        color += CalcPointLight(l, n, e) / NUM_POINT_LIGHTS;
    }

    // Add contributions from all spot lights
    for (int i = 0; i < NUM_SPOT_LIGHTS; i++) {
        vec3 l = normalize(vec3(spotLightsPos[i].position - vec4(DataIn.fragPosition, 1.0)));
        vec3 s = normalize(-spotLightsPos[i].direction);
        float cutoff = spotLightsPos[i].cutoff;
        color += calcSpotLight(l, s, cutoff, n, e);
    }

    // Apply fog effect
    float distance = length(DataIn.fragPosition);
    float fogFactor = exp(-fogDensity * (distance - fogStart));
    fogFactor = clamp(fogFactor, 0.0, 1.0);
    vec4 foggedColor = mix(fogColor, color, fogFactor);

    colorOut = vec4(foggedColor.rgb, mat.alpha);
}
