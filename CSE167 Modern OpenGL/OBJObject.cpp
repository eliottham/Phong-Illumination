#include "OBJObject.h"
#include "Window.h"
OBJObject::OBJObject(const char *filepath)
{
    toWorld = glm::mat4(1.0f);
    parse(filepath);
    
    angle = 0.0f;
    last_angle = 0.0f;
    rmx = glm::mat4(1.0f);
    tmx = glm::mat4(1.0f);
    smx = glm::mat4(1.0f);
    position = glm::vec3(0.0f);
    last_position = glm::vec3(0.0f);
    size = glm::vec3(1.0f);
    
    setCenter();
    setScale();
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
    
    // Vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
    // Vertex colors
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, color));
    // Vertex normals
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

OBJObject::~OBJObject() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void OBJObject::parse(const char *filepath)
{
    //TODO parse the OBJ file
    // Populate the face indices, vertices, and normals vectors with the OBJ Object data
    FILE* fp;
    float x, y, z;
    float r, g, b;
    unsigned int v1, v2, v3, n1, n2, n3;
    int c1, c2;
    
    fp = fopen(filepath, "r");
    
    while((c1 = fgetc(fp)) != EOF) {
        if(c1 == 'v') {
            c2 = fgetc(fp);
            if(c2 == ' ') {
                fscanf(fp, "%f %f %f %f %f %f", &x, &y, &z, &r, &g, &b);
                vpositions.push_back(glm::vec3(x, y, z));
            }
            else if(c2 == 'n') {
                fscanf(fp, "%f %f %f", &x, &y, &z);
                vnormals.push_back(glm::vec3(x, y, z));
            }
        }
        else if(c1 == 'f') {
            c2 = fgetc(fp);
            if(c2 == ' ') {
                fscanf(fp, "%u//%u %u//%u %u//%u", &v1, &n1, &v2, &n2, &v3, &n3);
                indices.push_back(v1 -1);
                indices.push_back(v2 - 1);
                indices.push_back(v3 - 1);
            }
        }
        
        if(feof(fp)) break;
    }
    fclose(fp);
    
    for(int i = 0; i < vpositions.size(); i++) {
        Vertex v;
        v.position = vpositions[i];
        v.color = glm::normalize(vnormals[i]) * 0.5f + 0.5f;
        v.normal = vnormals[i];
        
        vertices.push_back(v);
    }
}

void OBJObject::draw(GLuint shaderProgram)
{
    this->toWorld = tmx * rmx * smx * glm::mat4(1.0f);
    glm::mat4 modelview = Window::V * toWorld;
    
    uProjection = glGetUniformLocation(shaderProgram, "projection");
    uModelview = glGetUniformLocation(shaderProgram, "modelview");
    uModel = glGetUniformLocation(shaderProgram, "model");
    
    glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
    glUniformMatrix4fv(uModelview, 1, GL_FALSE, &modelview[0][0]);
    glUniformMatrix4fv(uModel, 1, GL_FALSE, &toWorld[0][0]);
    
    glUniform3fv(glGetUniformLocation(shaderProgram, "material.ambient"), 1, &(material.ambient[0]));
    glUniform3fv(glGetUniformLocation(shaderProgram, "material.diffuse"), 1, &(material.diffuse[0]));
    glUniform3fv(glGetUniformLocation(shaderProgram, "material.specular"), 1, &(material.specular[0]));
    
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size() * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    
}

void OBJObject::rotate(float deg, glm::vec3 vec)
{
    this->angle+= deg;
    if (this->angle > 360.0f || this->angle < -360.0f) this->angle = 0.0f;
    this->rmx = glm::rotate(glm::mat4(1.0f), this->angle / 180.0f * glm::pi<float>(), vec) * this->rmx;
}

void OBJObject::translate(glm::vec3 vec)
{
    this->position = this->last_position + vec;
    this->tmx = glm::translate(glm::mat4(1.0f), this->position);
    this->last_position = this->position;
}

void OBJObject::scale(glm::vec3 vec)
{
    this->size *= vec;
    this->smx = glm::scale(glm::mat4(1.0f), this->size);
}

void OBJObject::setCenter()
{
    float max_x = 0, max_y = 0, max_z = 0, min_x = 0, min_y = 0, min_z = 0;
    // Find the midpoint for each axis
    for(auto it = vpositions.begin(); it != vpositions.end(); ++it) {
         if(it->x > max_x) {
         max_x = it->x;
         }
         else if(it->x < min_x) {
         min_x = it->x;
         }
         
         if(it->y > max_y) {
         max_y = it->y;
         }
         else if(it->y < min_y) {
         min_y = it->y;
         }
         
         if(it->z > max_z) {
         max_z = it->z;
         }
         else if(it->z < min_z) {
         min_z = it->z;
         }
    }
    
    // Center the object
    glm::vec3 origin = glm::vec3((max_x + min_x)/2.0f, (max_y + min_y)/2.0f, (max_z + min_z)/2.0f) * -1.0f;
    printf("%f %f %f\n", origin.x, origin.y, origin.z);
    this->origin_mx = glm::translate(glm::mat4(1.0f), origin);
    translate(origin);
}

void OBJObject::setScale()
{
    float scale_factor = 0;
    
    for(auto it = vpositions.begin(); it != vpositions.end(); ++it) {
        glm::vec3 v = abs(this->origin_mx * glm::vec4(*it, 1.0f));
        if(v.x > scale_factor) scale_factor = v.x;
        if(v.y > scale_factor) scale_factor = v.y;
        if(v.z > scale_factor) scale_factor = v.z;
    }

    scale_factor = 2.0f/scale_factor;
    scale(glm::vec3(scale_factor));
}

void OBJObject::setMaterial(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess)
{
    this->material.ambient = ambient;
    this->material.diffuse = diffuse;
    this->material.specular = specular;
    this->material.shininess = shininess;
}
