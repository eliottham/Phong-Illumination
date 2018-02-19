#version 330 core
// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.
in vec3 vertex_color;

// You can output many things. The first vec4 type output determines the color of the fragment
//out vec4 color;
out vec4 frag_color;

uniform vec3 color;

void main()
{
    frag_color = vec4(color, 1.0f);
}
