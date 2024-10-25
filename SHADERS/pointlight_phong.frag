#version 430

#define NUM_POINT_LIGHTS 6
#define NUM_SPOT_LIGHTS 2

struct DirectionalLight {
    vec4 direction;
}; 

uniform	sampler2D texUnitDiff;
uniform	sampler2D texUnitDiff1;
uniform	sampler2D texUnitSpec;
uniform	sampler2D texUnitNormalMap;
uniform sampler2D normalSphereMap;
uniform sampler2D sphereMap;
uniform samplerCube cubeMap;

uniform bool normalMap;  //for normal mapping
uniform bool specularMap;
uniform uint diffMapCount;
uniform bool shadowMode;

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
};

uniform Materials mat;

uniform int texMode;

uniform sampler2D texmap;
uniform sampler2D texmap1;
uniform sampler2D texmap2;

uniform bool spotlight_mode;
uniform bool pointlight_mode;
uniform bool dirlight_mode;

in Data {
    vec3 fragPosition;
    vec3 fragNormal;
    vec2 tex_coord;
    vec3 skyboxTexCoord;
} DataIn;

out vec4 colorOut;

vec4 CalcDirLight(DirectionalLight dirLight, vec3 n, vec3 e) {
    vec4 spec = vec4(0.0);
    vec3 l = normalize(vec3(-dirLight.direction));
    float intensity = max(dot(n, l), 0.0);
    
    vec4 diff, auxSpec;

    if(texMode == 3) {
        if(mat.texCount == 0) {
		    diff = mat.diffuse;
		    auxSpec = mat.specular;
	    }else {
		    if(diffMapCount == 0){
			    diff = mat.diffuse;
            }
		    else if(diffMapCount == 1){
			    diff = mat.diffuse * texture(texUnitDiff, DataIn.tex_coord);
		    }
            else{
			    diff = mat.diffuse * texture(texUnitDiff, DataIn.tex_coord) * texture(texUnitDiff1, DataIn.tex_coord);
            }
		    if(specularMap) {
			    auxSpec = mat.specular * texture(texUnitSpec, DataIn.tex_coord);
		    }else{
			    auxSpec = mat.specular;
	        }
         }
         if (intensity > 0.0) {
            vec3 h = normalize(l + e);
            float intSpec = max(dot(h, n), 0.0);
            spec = auxSpec * pow(intSpec, mat.shininess);
        }
    }else{
        if (intensity > 0.0) {
            vec3 h = normalize(l + e);
            float intSpec = max(dot(h, n), 0.0);
            spec = mat.specular * pow(intSpec, mat.shininess);
        }
    }


    // return max(intensity * mat.diffuse + spec, mat.ambient);

    vec4 texel;
    vec4 texel1;

    if (texMode == 0) {
        return max(intensity * mat.diffuse + spec, mat.ambient);
    } else if (texMode == 1) {
        texel = texture(texmap, DataIn.tex_coord);
        texel1 = texture(texmap2, DataIn.tex_coord);
        return max(intensity * texel * texel1 + spec, 0.07 * texel);
    }else if (texMode == 3) {
        return vec4((max(intensity * diff, diff*0.15) + spec).rgb, 1.0);
    }
    else {
        texel = texture(texmap1, DataIn.tex_coord);
        return max(intensity * texel + spec, 0.07 * texel);
    }
}

vec4 CalcPointLight(vec3 l, vec3 n, vec3 e) {
    vec4 spec = vec4(0.0);
    float intensity = max(dot(n, l), 0.0);


    vec4 diff, auxSpec;

    if(texMode == 3) {
        if(mat.texCount == 0) {
		    diff = mat.diffuse;
		    auxSpec = mat.specular;
	    }else {
		    if(diffMapCount == 0){
			    diff = mat.diffuse;
            }
		    else if(diffMapCount == 1){
			    diff = mat.diffuse * texture(texUnitDiff, DataIn.tex_coord);
		    }
            else{
			    diff = mat.diffuse * texture(texUnitDiff, DataIn.tex_coord) * texture(texUnitDiff1, DataIn.tex_coord);
            }
		    if(specularMap) {
			    auxSpec = mat.specular * texture(texUnitSpec, DataIn.tex_coord);
		    }else{
			    auxSpec = mat.specular;
	        }
         }
         if (intensity > 0.0) {
            vec3 h = normalize(l + e);
            float intSpec = max(dot(h, n), 0.0);
            spec = auxSpec * pow(intSpec, mat.shininess);
        }
    }else{
        if (intensity > 0.0) {
            vec3 h = normalize(l + e);
            float intSpec = max(dot(h, n), 0.0);
            spec = mat.specular * pow(intSpec, mat.shininess);
        }
    }

    // return max(intensity * mat.diffuse + spec, mat.ambient);

    vec4 texel;
    vec4 texel1;

    if (texMode == 0) {
        return max(intensity * mat.diffuse + spec, mat.ambient);
    } else if (texMode == 1) {
        texel = texture(texmap, DataIn.tex_coord);
        texel1 = texture(texmap2, DataIn.tex_coord);
        return max(intensity * texel * texel1 + spec, 0.07 * texel);
    }else if (texMode == 3) {
        return vec4((max(intensity * diff, diff*0.15) + spec).rgb, 1.0);
    }else {
        texel = texture(texmap1, DataIn.tex_coord);
        return max(intensity * texel + spec, 0.07 * texel);
    }
}

vec4 calcSpotLight(vec3 l, vec3 s, float cutoff, vec3 n, vec3 e) {
    vec4 spec = vec4(0.0);
    vec4 diff, auxSpec;

    if (dot(s, l) > cutoff) {
        float intensity = max(dot(n, l), 0.0);


        if(texMode == 3) {
            if(mat.texCount == 0) {
		        diff = mat.diffuse;
		        auxSpec = mat.specular;
	        }else {
		        if(diffMapCount == 0){
			        diff = mat.diffuse;
                }
		        else if(diffMapCount == 1){
			        diff = mat.diffuse * texture(texUnitDiff, DataIn.tex_coord);
		        }
                else{
			        diff = mat.diffuse * texture(texUnitDiff, DataIn.tex_coord) * texture(texUnitDiff1, DataIn.tex_coord);
                }
		        if(specularMap) {
			        auxSpec = mat.specular * texture(texUnitSpec, DataIn.tex_coord);
		        }else{
			        auxSpec = mat.specular;
	            }
             }
             if (intensity > 0.0) {
                vec3 h = normalize(l + e);
                float intSpec = max(dot(h, n), 0.0);
                spec = auxSpec * pow(intSpec, mat.shininess);
            }
        }else{
            if (intensity > 0.0) {
                vec3 h = normalize(l + e);
                float intSpec = max(dot(h, n), 0.0);
                spec = mat.specular * pow(intSpec, mat.shininess);
            }
        }

        vec4 texel;
        vec4 texel1;

        if (texMode == 0) {
            return max(intensity * mat.diffuse + spec, mat.ambient);
        } else if (texMode == 1) {
            texel = texture(texmap, DataIn.tex_coord);
            texel1 = texture(texmap2, DataIn.tex_coord);
            return max(intensity * texel *texel1 + spec, 0.07 * texel);
        }else if (texMode == 3) {
            return vec4((max(intensity * diff, diff*0.15) + spec).rgb, 1.0);
        }else {
            texel = texture(texmap1, DataIn.tex_coord);
            return max(intensity * texel + spec, 0.07 * texel);
        }

        //return max(intensity * mat.diffuse + spec, mat.ambient);
    }

    return mat.ambient;


}

void main() {
    vec3 e = normalize(-DataIn.fragPosition);
    vec4 color;
    vec3 n;
    if(!shadowMode){
        if(texMode < 4){
                if(normalMap){
		            n = normalize(2.0 * texture(texUnitNormalMap, DataIn.tex_coord).rgb - 1.0);  //normal in tangent space
                }else{
                    n = normalize(DataIn.fragNormal);
                }


                if (dirlight_mode) {
                    // Initialize color with the directional light contribution
                    color = CalcDirLight(dirLight, n, e);
                }
    
                if (pointlight_mode) {
                    // Add contributions from all point lights
                    for (int i = 0; i < NUM_POINT_LIGHTS; i++) {
                        vec3 pl = vec3(pointLightsPos[i].position - vec4(DataIn.fragPosition, 1.0));
                        vec3 l = normalize(pl);
                        color += CalcPointLight(l, n, e) / NUM_POINT_LIGHTS;
                    }
                }

                if (spotlight_mode) {
                    // Add contributions from all spot lights
                    for (int i = 0; i < NUM_SPOT_LIGHTS; i++) {
                        vec3 l = normalize(vec3(spotLightsPos[i].position - vec4(DataIn.fragPosition, 1.0)));
                        vec3 s = normalize(-spotLightsPos[i].direction);
                        float cutoff = spotLightsPos[i].cutoff;
                        color += calcSpotLight(l, s, cutoff, n, e);
                    }
                }

                color = vec4(color.rgb, mat.diffuse.a);

                // Apply fog effect
                float distance = length(DataIn.fragPosition);
                float fogFactor = exp(-fogDensity * (distance - fogStart));
                fogFactor = clamp(fogFactor, 0.0, 1.0);
                colorOut = mix(fogColor, color, fogFactor);

                // colorOut = vec4(foggedColor.rgb, mat.alpha);
        }else if (texMode==4){
            vec4 texel;
            texel = texture(texmap, DataIn.tex_coord);  //texel from element flare texture
		        if((texel.a == 0.0)  || (mat.diffuse.a == 0.0) ) discard;
		        else
			        colorOut = mat.diffuse * texel;
        }else if(texMode==5){
            vec4 texel;

            vec3 spec = mat.specular.rgb * pow(1, mat.shininess);
            texel = texture(texmap, DataIn.tex_coord);  
		
		    if(texel.a == 0.0) discard;
		    else
			    colorOut = vec4(max(1*texel.rgb + spec, 0.1*texel.rgb), texel.a);
	    }else if(texMode==6){
            vec4 texel;

            vec3 spec = mat.specular.rgb * pow(1, mat.shininess);
            texel = texture(texmap, DataIn.tex_coord);  
		
		    if(texel.a == 0.0) discard;
		    else
			    colorOut = vec4(max(1*texel.rgb + spec, 0.1*texel.rgb), texel.a);
	    } else if (texMode == 7) {   //SkyBox
		    colorOut = texture(cubeMap, DataIn.skyboxTexCoord);
        }
    }else{
        colorOut = vec4(0.5, 0.5, 0.5, 1.0);
    }
    
 }
   
