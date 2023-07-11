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

// Valors per als components que necessitem dels focus de llum
uniform vec3 posFocus;  // en SCA

out vec3 fcolor;
out vec3 LSCO, normalSCO;
out vec4 vertexSCO;

out vec3 fmatamb;
out vec3 fmatdiff;
out vec3 fmatspec;
out float fmatshin;

void main()
{
    fmatamb = matamb;
    fmatdiff = matdiff;
    fmatspec = matspec;
    fmatshin = matshin;

    mat3 normalMatrix = inverse(transpose(mat3(view * TG)));
    normalSCO = vec3(normalMatrix * normal);

    vertexSCO = view * TG * vec4(vertex, 1.0);

    LSCO = (posFocus.xyz - vertexSCO.xyz);

    fcolor = matdiff;
    gl_Position = proj * view * TG * vec4 (vertex, 1.0);
}
