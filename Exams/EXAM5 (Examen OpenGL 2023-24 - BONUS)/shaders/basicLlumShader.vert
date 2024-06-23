#version 330 core
/* DECLARACIONS ORIGINALS
in vec3 vertex;
in vec3 normal;

in vec3 matamb;
in vec3 matdiff;
in vec3 matspec;
in float matshin;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 TG;

out vec3 fcolor;
vec3 colorFocus=vec3(1,1,1);
vec3 llumAmbient=vec3(0.1,0.1,0.1);
vec3 posFocus=vec3 (1,1,1);


vec3 Ambient() {
    return llumAmbient * matamb;
}

vec3 Difus (vec3 NormSCO, vec3 L, vec3 colorFocus)
{
    // Fixeu-vos que SOLS es retorna el terme difús
    // S'assumeix que els vectors que es reben com a paràmetres estan normalitzats
    vec3 colRes = vec3(0);
    // Càlcul component difusa, si n'hi ha
    if (dot (L, NormSCO) > 0)
      colRes = colorFocus * matdiff * dot (L, NormSCO);
    return (colRes);
}

vec3 Especular (vec3 NormSCO, vec3 L, vec4 vertSCO, vec3 colorFocus)
{
    // Fixeu-vos que SOLS es retorna el terme especular!
    // Assumim que els vectors estan normalitzats
    vec3 colRes = vec3 (0);
    // Si la llum ve de darrera o el material és mate no fem res
    if ((dot(NormSCO,L) < 0) || (matshin == 0))
      return colRes;  // no hi ha component especular

    // Calculem R i V
    vec3 R = reflect(-L, NormSCO); // equival a: 2.0*dot(NormSCO,L)*NormSCO - L;
    vec3 V = normalize(-vertSCO.xyz); // perquè la càmera està a (0,0,0) en SCO

    if (dot(R, V) < 0)
      return colRes;  // no hi ha component especular

    float shine = pow(max(0.0, dot(R, V)), matshin);
    return (matspec * colorFocus * shine);
}
*/

in vec3 vertex;
in vec3 normal;

in vec3 matamb;
in vec3 matdiff;
in vec3 matspec;
in float matshin;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 TG;

out vec4 vertexSCO;
out vec3 normalSCO;
out vec3 fmatamb;
out vec3 fmatdiff;
out vec3 fmatspec;
out float fmatshin;

out vec3 LSCO;

uniform vec3 posFoc;  // en SCA

void main()
{	
    /* CODI ORIGINAL
    vec4 vertexSCO=view * TG * vec4(vertex,1.0);
    mat3 NormalMatrix=inverse(transpose(mat3(view * TG)));
    vec3 normalSCO = normalize(NormalMatrix * normal);
    vec3 LSCO = normalize(posFocus-vertexSCO.xyz);
    fcolor=Ambient()+Difus(normalSCO, LSCO, colorFocus)+Especular(normalSCO, LSCO, vertexSCO, colorFocus);

    gl_Position = proj * vertexSCO;
    */

    fmatamb = matamb;
    fmatdiff = matdiff;
    fmatspec = matspec;
    fmatshin = matshin;
    mat3 normalMatrix = inverse(transpose(mat3 (view * TG)));
    normalSCO = normalize(normalMatrix * normal);

    // Aquí heu de completar el càlcul que falta
    // ...

    vertexSCO = view * TG * vec4(vertex, 1.0);
    LSCO = normalize(posFoc.xyz - vertexSCO.xyz);

    gl_Position = proj * view * TG * vec4 (vertex, 1.0);
}
