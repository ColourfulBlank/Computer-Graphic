#version 330

struct LightSource {
    vec3 position;
    vec3 rgbIntensity;
};

in VsOutFsIn {
	vec3 position_ES; // Eye-space position
	vec3 normal_ES;   // Eye-space normal
	LightSource light;
} fs_in;


out vec4 fragColour;
uniform vec4 colour;
uniform int joint;


struct Material {
    vec3 kd;
    vec3 ks;
    float shininess;
};
uniform Material material;

// Ambient light intensity for each RGB component.
uniform vec3 ambientIntensity;


vec3 phongModel(vec3 fragPosition, vec3 fragNormal) {
	LightSource light = fs_in.light;

    // Direction from fragment to light source.
    vec3 l = normalize(light.position - fragPosition);

    // Direction from fragment to viewer (origin - fragPosition).
    vec3 v = normalize(-fragPosition.xyz);

    float n_dot_l = max(dot(fragNormal, l), 0.0);

    n_dot_l = max(round(n_dot_l * 2) / 2 , 0.2);

	vec3 diffuse;
	diffuse = material.kd * n_dot_l;

    vec3 specular = vec3(0.0);

    if (n_dot_l > 0.0) {
		// Halfway vector.
		vec3 h = normalize(v + l);
        float n_dot_h = max(dot(fragNormal, h), 0.0);
        specular = material.ks * pow(n_dot_h, material.shininess);
        float specMask = ( pow(dot(h, fragNormal), material.shininess) > 0.3 ) ? 1 : 0;
        specular = specular * specMask;
    }

    float edgeDetection = (dot(v, fragNormal) > 0.3) ? 1 : 0;
    return ambientIntensity + light.rgbIntensity * (diffuse + specular) * edgeDetection;
}

void main() {
    if (joint == 1) {
        fragColour = colour;
    } else {
	   fragColour = vec4(phongModel(fs_in.position_ES, fs_in.normal_ES), 1.0);
    }
}
