#version 330 core

out vec4 FragColor;
in vec3 fcolor;

uniform int hooliganMode; // Uniform per indicar l'estat al que es pintara la samarreta.

void main() {
    FragColor = vec4(fcolor, 1); // Pintem el Nino amb els color especificats segons els buffers del MyGLWidget.cpp
    float sum, gruix;
    sum = gl_FragCoord.x + gl_FragCoord.y;
    gruix = 60.f; // Indiquem el gruix de les diagonals.
    if (hooliganMode == 2 && FragColor == vec4(0.0, 0.0, 0.5, 1.0)) { // Les diagonals nomes es pinten a l'estat 2 (bracos aixecats).
        if (mod(sum, gruix) >= gruix/2) {
            FragColor = vec4(1, 1, 1, 1); // Pintem diagonals blanques.
        }
        else {
            FragColor = vec4(1, 0, 0, 1); // Pintem diagonals vermelles.
        }
    }
}
