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

struct SpotLight
{
    vec3 color;
    vec3 position;
    vec3 direction;
    float spot_cutoff;
    float spot_exp;
};

// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.
in vec3 vertex_normal;
in vec3 frag_position;

// You can output many things. The first vec4 type output determines the color of the fragment
//out vec4 color;
out vec4 frag_color;

uniform vec3 viewPos;
uniform Material material;
uniform DirLight dirLight;
uniform PointLight pointLight;
uniform SpotLight spotLight;
uniform int directional;
uniform int scene;

vec3 calcDirLight(DirLight light, vec3 normal, vec3 view_dir);
vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 view_dir);
vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 frag_pos, vec3 view_dir);

void main()
{
    vec3 norm = normalize(vertex_normal);
    vec3 view_dir = normalize(viewPos - frag_position);
    
    vec3 result;
    
    if(scene == 1) {
        if(directional == 0) {
            result = material.ambient;
        }
        else {
            result = calcDirLight(dirLight, norm, view_dir);
        }
    }
    
    if(scene == 2) {
        result = calcPointLight(pointLight, norm, frag_position, view_dir);
    }
    
    if(scene == 3) {
        result = calcSpotLight(spotLight, norm, frag_position, view_dir);
    }
    
    if(directional == 1 && scene != 1) {
        result += calcDirLight(dirLight, norm, view_dir);
    }
    
    frag_color = vec4(result, 1.0f);
}

// Calculates color when using a directional light
vec3 calcDirLight(DirLight light, vec3 normal, vec3 view_dir)
{
    vec3 light_dir = normalize(-light.direction);
    // Diffuse shading
    float diff = max(dot(normal, light_dir), 0.0f);
    // Specular shading
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0f), material.shininess);
    // Combine results
    vec3 ambient = light.color * material.ambient; //vec3( 0.3f, 0.24f, 0.14f) * material.ambient; //
    vec3 diffuse = light.color * diff * material.diffuse; //vec3(0.7f, 0.42f, 0.26f) * material.diffuse * diff; //
    vec3 specular = light.color * spec * material.specular; //vec3(0.5f, 0.5f, 0.5f) * material.specular * spec; //
    return (ambient + diffuse + specular);
}

// Calculates color when using a point light
vec3 calcPointLight(PointLight light, vec3 normal, vec3 frag_position, vec3 view_dir) {
    vec3 light_dir = normalize(light.position - frag_position);
    // Diffuse shading
    float diff = max(dot(normal, light_dir), 0.0f);
    // Specular shading
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0f), material.shininess);
    // Attenuation
    float dist = length(light.position - frag_position);
    float attenuation = 1.0f / (0.09f * dist);
    // Combine results
    vec3 ambient = light.color * material.ambient * attenuation;
    vec3 diffuse = light.color * diff * material.diffuse * attenuation;
    vec3 specular = light.color * spec * material.specular * attenuation;
    return(ambient + diffuse + specular);
}

// Calcuates color when using a spotlight
vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 frag_pos, vec3 view_dir)
{
    vec3 light_dir = normalize(light.position - frag_position);
    float theta = dot(light_dir, normalize(-light.direction));
    if(theta > cos(light.spot_cutoff)) {
        // Diffuse shading
        float diff = max(dot(normal, light_dir), 0.0f);
        // Specular shading
        vec3 reflect_dir = reflect(-light_dir, normal);
        float spec = pow(max(dot(view_dir, reflect_dir), 0.0f), material.shininess);
        // Attenuation
        float dist = length(light.position - frag_position);
        float attenuation = 1.0f / (0.09f * pow(dist, light.spot_exp));
        // Combine results
        vec3 ambient = light.color * material.ambient * attenuation;
        vec3 diffuse = light.color * diff * material.diffuse * attenuation;
        vec3 specular = light.color * spec * material.specular * attenuation;
        return(ambient + diffuse + specular);
    }
    return vec3(0.0f);
}
