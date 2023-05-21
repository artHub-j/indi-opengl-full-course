#version 330 core

in vec3 vertex;

uniform mat4 TG;

in vec3 color; // Enviem els buffers de color com a in al vertex shader
out vec3 fcolor; // Els enviem com out cap al fragment shader per pintar cada vertex/cara segons els colors especificats.

void main()  {
    fcolor = color;
    gl_Position = TG * vec4 (vertex, 1.0);   
}
