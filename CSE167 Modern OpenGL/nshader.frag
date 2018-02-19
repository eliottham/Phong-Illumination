#version 330 core
struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct DirLight
{
    vec3 color;
    vec3 direction;
};

struct PointLight
{
    vec3 color;
    vec3 position;
};

struct Spotlight
{
    vec3 color;
    vec3 position;
    vec3 direction;
    float spot_cutoff;
    float spot_exp;
};

// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.
in vec3 vertex_color;

// You can output many things. The first vec4 type output determines the color of the fragment
//out vec4 color;
out vec4 frag_color;

uniform vec3 viewPos;
uniform int render;
uniform Material material;

void main()
{
    frag_color = vec4(vertex_color, 1.0f);
}
