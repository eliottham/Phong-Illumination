#ifndef OBJOBJECT_H
#define OBJOBJECT_H

#define GLFW_INCLUDE_GLEXT
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
// Use of degrees is deprecated. Use radians instead.
#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

struct Vertex
{
    glm::vec3 position;
    glm::vec3 color;
    glm::vec3 normal;
};

struct Material
{
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
};

class OBJObject
{
private:
    std::vector<GLuint> indices;
    std::vector<glm::vec3> vpositions;
    std::vector<glm::vec3> vnormals;
    glm::mat4 toWorld;
    glm::mat4 origin_mx;
    glm::vec3 size, position, last_position;
    glm::mat4 rmx, tmx, smx;
    std::vector<Vertex> vertices;
    Material material;
    
public:
    OBJObject(const char* filepath);
    ~OBJObject();
    void parse(const char* filepath);
    void draw(GLuint shaderProgram);
    void rotate(float deg, glm::vec3 vec);
    void translate(glm::vec3 vec);
    void scale(glm::vec3 vec);
    float angle, last_angle;
    void setCenter();
    void setScale();
    GLuint VBO, VAO, EBO;
    GLuint uProjection, uModelview, uModel;
    void setMaterial(glm::vec3, glm::vec3, glm::vec3, float);
};

#endif

