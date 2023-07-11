#version 330 core

in vec3 vertex;
in vec3 normal;

in vec3 matamb;
in vec3 matdiff;
in vec3 matspec;
in float matshin;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 TG;

uniform vec3 posFocus;

out vec4 vertexSCO;
out vec3 normalSCO;
out vec3 fmatamb;
out vec3 fmatdiff;
out vec3 fmatspec;
out float fmatshin;

out vec3 LSCO;

void main()
{	
    fmatamb = matamb;
    fmatdiff = matdiff;
    fmatspec = matspec;
    fmatshin = matshin;
    mat3 normalMatrix = inverse(transpose(mat3 (view * TG)));
    normalSCO = vec3(normalMatrix * normal);

    // Aquí heu de completar el càlcul que falta
    // ...

    vec4 vertexSCO = view * TG * vec4(vertex, 1.0);
    //vec3 posFocusSCO = view * vec4(posFocus, 1.0);

    LSCO = normalize(posFocus.xyz - vertexSCO.xyz);

    gl_Position = proj * view * TG * vec4 (vertex, 1.0);
}
