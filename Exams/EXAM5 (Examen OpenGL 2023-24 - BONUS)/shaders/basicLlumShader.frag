#version 330 core

//out vec4 FragColor;
//in vec3 fcolor;

in vec4 vertexSCO;

in vec3 normalSCO;
in vec3 fmatamb;
in vec3 fmatdiff;
in vec3 fmatspec;
in float fmatshin;

in vec3 LSCO;

out vec4 FragColor;

// Valors per als components que necessitem dels focus de llum
vec3 llumAmbient = vec3(0.2, 0.2, 0.2);
vec3 colFocus = vec3(1, 1, 1);

vec3 Ambient() {
    return llumAmbient * fmatamb;
}

vec3 Difus (vec3 NormSCO, vec3 L, vec3 colorFocus)
{
    // Fixeu-vos que SOLS es retorna el terme difús
    // S'assumeix que els vectors que es reben com a paràmetres estan normalitzats
    vec3 colRes = vec3(0);
    // Càlcul component difusa, si n'hi ha
    if (dot (L, NormSCO) > 0)
      colRes = colorFocus * fmatdiff * dot (L, NormSCO);
    return (colRes);
}

vec3 Especular (vec3 NormSCO, vec3 L, vec4 vertSCO, vec3 colorFocus)
{
    // Fixeu-vos que SOLS es retorna el terme especular!
    // Assumim que els vectors estan normalitzats
    vec3 colRes = vec3 (0);
    // Si la llum ve de darrera o el material és mate no fem res
    if ((dot(NormSCO,L) < 0) || (fmatshin == 0))
      return colRes;  // no hi ha component especular

    // Calculem R i V
    vec3 R = reflect(-L, NormSCO); // equival a: 2.0*dot(NormSCO,L)*NormSCO - L;
    vec3 V = normalize(-vertSCO.xyz); // perquè la càmera està a (0,0,0) en SCO

    if (dot(R, V) < 0)
      return colRes;  // no hi ha component especular

    float shine = pow(max(0.0, dot(R, V)), fmatshin);
    return (fmatspec * colorFocus * shine);
}


void main()
{	
    vec3 phongModel = Ambient() + Difus(normalize(normalSCO), normalize(LSCO), colFocus) +
                          Especular(normalize(normalSCO), normalize(LSCO), vertexSCO, colFocus);

    FragColor = vec4(phongModel, 1.0f);
}
