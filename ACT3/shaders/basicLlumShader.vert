#version 330 core

//-------------------------------
in vec3 vertex;
in vec3 normal;
in vec3 matamb;
in vec3 matdiff;
in vec3 matspec;
in float matshin;


out vec3 matamb_interp;
out vec3 matdiff_interp;
out vec3 matspec_interp;
out float matshin_interp;
//-------------------------------
uniform mat4 proj;
uniform mat4 view;
uniform mat4 TG;
uniform mat3 NM;

uniform vec3 posFocus;
uniform vec3 snitchFocusEsq, snitchFocusDret;
//-------------------------------
out vec3 fColor;

out vec4 vertexSCO;
out vec4 vertexSCA;

out vec3 normalSCO;

out vec3 LSCO;
out vec3 snitchEsqLSCO, snitchDretLSCO;

void main()
{	
    gl_Position = proj * view * TG * vec4 (vertex, 1.0);

    fColor = matdiff;
    matamb_interp  = matamb;
    matdiff_interp = matdiff;
    matspec_interp = matspec;
    matshin_interp = matshin;

    // Calcul dels vertex en SCA i SCO de l'escena.
    vertexSCA = TG * vec4(vertex, 1.0);
    vertexSCO = view * TG * vec4(vertex, 1.0);

    // Calcul de la normal de l'escena.
    normalSCO = NM * normal;
    normalSCO = normalize(normalSCO);

    // Calcul de les L del focus d'escena i del focus dels 2 Snitchs en SCO.
    LSCO = normalize(posFocus.xyz - vertexSCO.xyz);
    snitchEsqLSCO = normalize(snitchFocusEsq.xyz - vertexSCO.xyz);
    snitchDretLSCO = normalize(snitchFocusDret.xyz - vertexSCO.xyz);
}
