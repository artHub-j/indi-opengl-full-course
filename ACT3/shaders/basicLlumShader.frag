#version 330 core

in vec3 fColor;
in vec3 matamb_interp;
in vec3 matdiff_interp;
in vec3 matspec_interp;
in float matshin_interp;

in vec4 vertexSCO;
in vec4 vertexSCA;
in vec3 normalSCO;
in vec3 LSCO;

in vec3 snitchEsqLSCO, snitchDretLSCO;

uniform int opacitat; // 'Boolea' per indicar quan aplicar la transparencia de les finestres.
uniform mat3 NM;
uniform vec3 llumAmbient;
uniform vec3 colorFocus, colorFocusSnitchEsq, colorFocusSnitchDret;

//--------------------------------------
out vec4 FragColor;
//--------------------------------------

//-------------------------------------------------------
//   Funció de soroll per fer efectes especials amb el shader
//	<https://www.shadertoy.com/view/4dS3Wd>
//	By Morgan McGuire @morgan3d, http://graphicscodex.com
//-------------------------------------------------------
float hash(float n) { return fract(sin(n) * 1e4); }
float hash(vec2 p) { return fract(1e4 * sin(17.0 * p.x + p.y * 0.1) * (0.1 + abs(sin(p.y * 13.0 + p.x)))); }

float noise(vec2 x) {
	vec2 i = floor(x);
	vec2 f = fract(x);

	// Four corners in 2D of a tile
	float a = hash(i);
	float b = hash(i + vec2(1.0, 0.0));
	float c = hash(i + vec2(0.0, 1.0));
	float d = hash(i + vec2(1.0, 1.0));

	// Same code, with the clamps in smoothstep and common subexpressions
	// optimized away.
	vec2 u = f * f * (3.0 - 2.0 * f);
        return mix(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y-0.5;
}

//--------------------------------------
//    Funcions del model de Phong
//--------------------------------------

vec3 Ambient() {
    return llumAmbient * matamb_interp;
}

vec3 Difus (vec3 NormSCO, vec3 L, vec3 colFocus)
{
    // Tant sols retorna el terme difús
    // S'assumeix que els vectors que es reben com a paràmetres estan normalitzats
    vec3 colRes = vec3(0);
    // Càlcul component difusa, si n'hi ha
    if (dot (L, NormSCO) > 0)
      colRes = colFocus * matdiff_interp * dot (L, NormSCO);
    return (colRes);
}

vec3 Especular (vec3 NormSCO, vec3 L, vec4 vertSCO, vec3 colFocus)
{
    // Tant sols retorna el terme especular!
    // Els vectors rebuts com a paràmetres (NormSCO i L) estan normalitzats
    vec3 colRes = vec3 (0);
    // Si la llum ve de darrera o el material és mate no fem res
    if ((dot(NormSCO,L) < 0) || (matshin_interp == 0))
      return colRes;  // no hi ha component especular

    // Calculem R i V
    vec3 R = reflect(-L, NormSCO); // equival a: 2.0*dot(NormSCO,L)*NormSCO - L;
    vec3 V = normalize(-vertSCO.xyz); // perquè la càmera està a (0,0,0) en SCO

    if (dot(R, V) < 0)
      return colRes;  // no hi ha component especular

    // Calculem i retornem la component especular
    float shine = pow(max(0.0, dot(R, V)), matshin_interp);
    return (matspec_interp * colFocus * shine);
}


//--------------------------------------
//    Mètode main
//--------------------------------------
void main()
{
    float W = 2.0;

    bool pujada = (mod(vertexSCA.x, W) > 0 && mod(vertexSCA.x, W) <= 0.2) || (mod(vertexSCA.z, W) > 0 && mod(vertexSCA.z, W) <= 0.2);
    bool pla = (mod(vertexSCA.x, W) > 0.2 && mod(vertexSCA.x, W) < 1.8) || (mod(vertexSCA.z, W) > 0.2 && mod(vertexSCA.z, W) < 1.8);
    bool baixada = (mod(vertexSCA.x, W) > 1.8 && mod(vertexSCA.x, W) < 2) || (mod(vertexSCA.z, W) > 1.8 && mod(vertexSCA.z, W) < 2 );

    vec3 normalAux;
    float r = noise(vec2(vertexSCA.x, vertexSCA.y));

    // Modifiquem el valor de les normals del terra per simular textura de rajoles.
    if (vertexSCA.y < 0.1)
    {
        if (pujada) { // Pujada de la rampa que separa les rajoles (de amplada 0.2) amb una normal vec3(-1, 1, 0).
            normalAux = vec3(-1, 1, 0) + (r  * vec3(1, 0, 0));
        }
        else if (baixada) { // Baixada de la rampa que separa les rajoles (de amplada 0.2) amb una normal vec3(1, 1, 0).
            normalAux = vec3(1, 1, 0) + (r * vec3(1, 0, 0));
        }
        else if (pla) { // Rajola de 1.6 x 1.6 amb una normal vertical (vec3(0, 1, 0)).
            normalAux = vec3(0, 1, 0) + (r * vec3(1, 0, 0));
        }
        normalAux = normalize(NM * normalAux);
    }
    else { // Per a la resta de l'escena apliquem les normals calculades als modelsTransform de MyGLWidget.
        normalAux = normalSCO;
    }

    // Sumatori del modelPhong amb la component Ambient, les components Difuses i Especulars de cada focus que hi ha a escena:
    //          1. Focus d'escena, 2. Focus de l'Snitch Esquerre i 3. Focus de l'Snitch Dret.
    vec3 modelPhong = Ambient() +
                      Difus(normalize(normalAux), normalize(LSCO), colorFocus) +
                      Especular(normalize(normalAux), normalize(LSCO), vertexSCO, colorFocus);

    modelPhong += Difus(normalize(normalAux), normalize(snitchEsqLSCO), colorFocusSnitchEsq) +
                  Especular(normalize(normalAux), normalize(snitchEsqLSCO), vertexSCO, colorFocusSnitchEsq);

    modelPhong += Difus(normalize(normalAux), normalize(snitchDretLSCO), colorFocusSnitchDret) +
                  Especular(normalize(normalAux), normalize(snitchDretLSCO), vertexSCO, colorFocusSnitchDret);

    FragColor = vec4(modelPhong, 1);

    if (opacitat == 1) { // Quan pintem la finestra i les ampolles li donem translucitat (a = 0.5).
        FragColor.a = 0.5;
    }
    else {
        FragColor.a = 1; // La resta de models de l'escena continuen sent opacs (a = 1).
    }
}
