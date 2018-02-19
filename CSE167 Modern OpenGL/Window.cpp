#include "Window.h"

const char* window_title = "GLFW Starter Project";
Cube * cube;
OBJObject* bunny;
OBJObject* dragon;
OBJObject* bear;
OBJObject* curr;
OBJObject* pointLight;
OBJObject* spotLight;

GLint normShaderProgram;
GLint phongShaderProgram;
GLint lightShaderProgram;
GLint shaderProgram;
GLint scene = 1;
GLint directional = 1;
GLint ulightDir;
glm::vec3 dLightDir = glm::vec3(2.0f, -1.0f, 0.0f);
float x_inc = -0.25f;
float z_inc = 0.25f;


// On some systems you need to change this to the absolute path
#define VERTEX_SHADER_PATH "./shader.vert"
#define FRAGMENT_SHADER_PATH "./shader.frag"
#define NVERTEX_SHADER_PATH "./nshader.vert"
#define NFRAGMENT_SHADER_PATH "./nshader.frag"
#define LVERTEX_SHADER_PATH "./lightshader.vert"
#define LFRAGMENT_SHADER_PATH "./lightshader.frag"
#define TRANSLATE_CONSTANT 20.0f
#define ROTATE_CONSTANT 0.5f

// Default camera parameters
glm::vec3 cam_pos(0.0f, 0.0f, 20.0f);		// e  | Position of camera
glm::vec3 cam_look_at(0.0f, 0.0f, 0.0f);	// d  | This is where the camera looks at
glm::vec3 cam_up(0.0f, 1.0f, 0.0f);			// up | What orientation "up" is

int Window::width;
int Window::height;
float Window::xpos;
float Window::ypos;
bool Window::movement;
bool Window::xy_movement;
float Window::last_xpos;
float Window::last_ypos;
glm::vec3 Window::last_point;

glm::mat4 Window::P;
glm::mat4 Window::V;

void Window::initialize_objects()
{
	// Load the shader program. Make sure you have the correct filepath up top
    bunny = new OBJObject("./bunny.obj");
    dragon = new OBJObject("./dragon.obj");
    bear = new OBJObject("./bear.obj");
    bear->translate(glm::vec3(0.0f, 8.0f, 0.0f));
    
    pointLight = new OBJObject("./sphere.obj");
    pointLight->scale(glm::vec3(0.20f));
    pointLight->translate(glm::vec3(-5.0f, -5.0f, 0.0f));
    
    spotLight = new OBJObject("./cone.obj");
    spotLight->scale(glm::vec3(0.20f));
    spotLight->translate(glm::vec3(0.0f, 5.0f, 0.0f));
    
    bunny->setMaterial(glm::vec3(0.0215f, 0.1745f, 0.0215f),
                       glm::vec3(0.07568f, 0.61424f, 0.07568f),
                       glm::vec3(0.633f, 0.727811f, 0.633f),
                       0.6f * 128.0f);
    
    dragon->setMaterial(glm::vec3(0.05f, 0.0f, 0.0f),
                        glm::vec3(0.5f, 0.4f, 0.4f),
                        glm::vec3(0.7f, 0.04f, 0.04f),
                        0.0078125f);
    
    bear->setMaterial(glm::vec3(0.5f),
                      glm::vec3(0.5f),
                      glm::vec3(0.5f),
                      0.25f * 128.0f);

    curr = bunny;
    
	phongShaderProgram = LoadShaders(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH);
    normShaderProgram = LoadShaders(NVERTEX_SHADER_PATH, NFRAGMENT_SHADER_PATH);
    lightShaderProgram = LoadShaders(LVERTEX_SHADER_PATH, LFRAGMENT_SHADER_PATH);
    
    shaderProgram = normShaderProgram;
    
    glUseProgram(phongShaderProgram);
    ulightDir = glGetUniformLocation(phongShaderProgram, "dirLight.direction");
    glUniform3fv(glGetUniformLocation(phongShaderProgram, "viewPos"), 1, &cam_pos[0]);
    glUniform3f(glGetUniformLocation(phongShaderProgram, "dirLight.color"), 0.33f, 0.635f, 0.477f);
    glUniform3fv(glGetUniformLocation(phongShaderProgram, "pointLight.color"), 1, &(glm::vec3(0.5f)[0]));
    glUniform3fv(glGetUniformLocation(phongShaderProgram, "pointLight.position"), 1, &(glm::vec3(-5.0f, -5.0f, 0.0f)[0]));
    glUniform3fv(glGetUniformLocation(phongShaderProgram, "spotLight.color"), 1, &(glm::vec3(0.5f)[0]));
    glUniform3fv(glGetUniformLocation(phongShaderProgram, "spotLight.position"), 1, &(glm::vec3(0.0f, 5.0f, 0.0f)[0]));
    glUniform3fv(glGetUniformLocation(phongShaderProgram, "spotLight.direction"), 1, &(glm::vec3(0.0f, -5.0f, 0.0f)[0]));
    glUniform1f(glGetUniformLocation(phongShaderProgram, "spotLight.spot_cutoff"), glm::radians(37.5f));
    glUniform1f(glGetUniformLocation(phongShaderProgram, "spotLight.spot_exp"), 0.9f);
    
}

// Treat this as a destructor function. Delete dynamically allocated memory here.
void Window::clean_up()
{
	delete(cube);
    delete(bunny);
    delete(dragon);
    delete(bear);
    delete(pointLight);
    delete(spotLight);
	glDeleteProgram(phongShaderProgram);
    glDeleteProgram(normShaderProgram);
}

GLFWwindow* Window::create_window(int width, int height)
{
	// Initialize GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return NULL;
	}

	// 4x antialiasing
	glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__ // Because Apple hates comforming to standards
	// Ensure that minimum OpenGL version is 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Enable forward compatibility and allow a modern OpenGL context
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Create the GLFW window
	GLFWwindow* window = glfwCreateWindow(width, height, window_title, NULL, NULL);

	// Check if the window could not be created
	if (!window)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		fprintf(stderr, "Either GLFW is not installed or your graphics card does not support modern OpenGL.\n");
		glfwTerminate();
		return NULL;
	}

	// Make the context of the window
	glfwMakeContextCurrent(window);

	// Set swap interval to 1
	glfwSwapInterval(1);

	// Get the width and height of the framebuffer to properly resize the window
	glfwGetFramebufferSize(window, &width, &height);
	// Call the resize callback to make sure things get drawn immediately
	Window::resize_callback(window, width, height);

	return window;
}

void Window::resize_callback(GLFWwindow* window, int width, int height)
{
#ifdef __APPLE__
	glfwGetFramebufferSize(window, &width, &height); // In case your Mac has a retina display
#endif
	Window::width = width;
	Window::height = height;
	// Set the viewport size. This is the only matrix that OpenGL maintains for us in modern OpenGL!
	glViewport(0, 0, width, height);

	if (height > 0)
	{
		P = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);
		V = glm::lookAt(cam_pos, cam_look_at, cam_up);
	}
}

void Window::idle_callback()
{
    dLightDir.x += x_inc;
    dLightDir.z += z_inc;
    
    if(dLightDir.x == -2.0f || dLightDir.x == 2.0f) {
        x_inc *= -1.0f;
    }
    
    if(dLightDir.z == -2.0f || dLightDir.z == 2.0f) {
        z_inc *= -1.0f;
    }
}

void Window::display_callback(GLFWwindow* window)
{

	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use the shader of programID
    glUseProgram(shaderProgram);
    
    glUniform1i(glGetUniformLocation(phongShaderProgram, "directional"), directional);
    glUniform1i(glGetUniformLocation(phongShaderProgram, "scene"), scene);
    glUniform3fv(ulightDir, 1, &dLightDir[0]);
    
    curr->draw(shaderProgram);
    
    if(shaderProgram == phongShaderProgram) {
        if(scene == 2) {
            glUseProgram(lightShaderProgram);
            glUniform3fv(glGetUniformLocation(lightShaderProgram, "color"), 1, &(glm::vec3(0.5f)[0]));
            pointLight->draw(lightShaderProgram);
        }
        if(scene == 3) {
            glUseProgram(lightShaderProgram);
            glUniform3fv(glGetUniformLocation(lightShaderProgram, "color"), 1, &(glm::vec3(0.5f)[0]));
            spotLight->draw(lightShaderProgram);
        }
    }

	// Gets events, including input such as keyboard and mouse or window resizing
	glfwPollEvents();
	// Swap buffers
	glfwSwapBuffers(window);
}

void Window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Check for a key press
	if (action == GLFW_PRESS)
	{
		// Check if escape was pressed
		if (key == GLFW_KEY_ESCAPE)
		{
			// Close the window. This causes the program to also terminate.
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
        // Toggle between the three objects
        if(key == GLFW_KEY_F1) {
            curr = bunny;
        }
        if(key == GLFW_KEY_F2) {
            curr = dragon;
        }
        if(key == GLFW_KEY_F3) {
            curr = bear;
        }
        
        // Scale up with 'S', scale down with 's'
        if(key == GLFW_KEY_S) {
            if(mods == GLFW_MOD_SHIFT) {
                curr->scale(glm::vec3(1.2f));
            }
            else {
                curr->scale(glm::vec3(0.8f));
            }
        }
        
        // Toggle object materials
        if(key == GLFW_KEY_N) {
            if(shaderProgram == normShaderProgram) {
                shaderProgram = phongShaderProgram;
            }
            else {
                shaderProgram = normShaderProgram;
            }
        }
        
        // Toggle between scenese
        if(key == GLFW_KEY_1) {
            scene = 1;
        }
        if(key == GLFW_KEY_2) {
            scene = 2;
        }
        if(key == GLFW_KEY_3) {
            scene = 3;
        }
        if(key == GLFW_KEY_4) {
            directional = (directional + 1) % 2;
        }
    }
}

// Function to track the coordinates of the mouse in terms of a sphere
glm::vec3 trackBallMapping(float x, float y)
{
    // Vec v is the synthesized 3D position of the mouse location on the trackball
    glm::vec3 v;
    // d is the depth of the mouse location
    float d;
    
    // Caluclate mouse x and y position in trackball coordinates
    v.x = (2.0f * x - (float)Window::width) / (float)Window::width;
    v.y = ((float)Window::height - 2.0f * y) / (float)Window::height;
    
    // Initially mouse z position is set to zero
    v.z = 0.0f;
    
    // This is the distance from trackball origin to mouse location w/o considering depth
    d = glm::length(v);
    // This limits d to values 1.0 or less
    d = (d < 1.0f) ? d : 1.0f;
    
    // Calculate the z coordinate of the mouse position on the trackball
    v.z = sqrtf(1.001f - d * d);
    
    // Return the mouse location on the surface of the trackball
    return glm::normalize(v);
}

// Get x and y position of mouse in Window coordinates
void Window::cursor_pos_callback(GLFWwindow *window, double x, double y)
{
    Window::xpos = x;
    Window::ypos = y;
    
    glm::vec3 direction;
    float pixel_diff, rot_angle, zoom_factor;
    glm::vec3 cur_point;
    
    if(Window::movement) {
        cur_point = trackBallMapping(Window::xpos, Window::ypos);
        direction = cur_point - Window::last_point;
        float velocity = glm::length(direction);
        
        if(velocity > 0.0001f) {
            glm::vec3 rot_axis = glm::cross(Window::last_point, cur_point);
            rot_angle = velocity * ROTATE_CONSTANT;
            curr->rotate(rot_angle, rot_axis);

        }
        Window::last_point = cur_point;
    }
    
    if(Window::xy_movement) {
        direction.x = Window::xpos - Window::last_xpos;
        direction.y = Window::last_ypos - Window::ypos;
        direction.z = 0.0f;
        
        curr->translate(direction/TRANSLATE_CONSTANT);
    }
    Window::last_xpos = Window::xpos;
    Window::last_ypos = Window::ypos;
}

// Get mouse presses for rotating objects
void Window::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    // Rotate the object with left click
    if(button == GLFW_MOUSE_BUTTON_LEFT) {
        if(action == GLFW_PRESS) {
            Window::movement = true;
            Window::last_point = trackBallMapping(Window::xpos, Window::ypos);
        }
        else {
            Window::movement = false;
        }
    }
    
    // Translate the object with right click
    if(button == GLFW_MOUSE_BUTTON_RIGHT) {
        if(action == GLFW_PRESS) {
            Window::xy_movement = true;
            Window::last_xpos = Window::xpos;
            Window::last_ypos = Window::ypos;
        }
        else {
            Window::xy_movement = false;
        }
    }
}

void Window::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    curr->translate(glm::vec3(0.0f, 0.0f, -yoffset));
}
