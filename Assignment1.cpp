#include "stdafx.h"

#include "..\glew\glew.h"	// include GL Extension Wrangler
#include "..\glfw\glfw3.h"	// include GLFW helper library
#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

using namespace std;

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 800;

glm::vec3 square_rotation = glm::vec3(0.8f, 0.0f, 0.0f);
glm::vec3 camera_translation = glm::vec3(0.0f, 25.0f, -5.0f);
glm::vec3 spinner_scale = glm::vec3(1.0f, 1.0f, 1.0f);
glm::vec3 revolver_scale = glm::vec3(0.6f, 0.6f, 0.6f);
int rendering_mode = 2;

double previous_y_position;
bool left_button_down = false;

const float SQUARE_ROTATION_STEP = 0.2f;
const float SIZE_STEP = 0.2f;

static void mouse_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (GLFW_PRESS == action)
			left_button_down = true;
		else if (GLFW_RELEASE == action)
			left_button_down = false;
	}
}
// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	//Shift + S to grow the spinner
	if (mode == 1 && key == GLFW_KEY_S && action == GLFW_PRESS)
	{
		spinner_scale.x += SIZE_STEP;
		spinner_scale.y += SIZE_STEP;
		spinner_scale.z += SIZE_STEP;
	}
	//S to shrink the spinner
	else if (key == GLFW_KEY_S && action == GLFW_PRESS)
	{
		spinner_scale.x -= SIZE_STEP;
		spinner_scale.y -= SIZE_STEP;
		spinner_scale.z -= SIZE_STEP;
	}

	//Shift + R to grow the spinner
	if (mode == 1 && key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		revolver_scale.x += SIZE_STEP;
		revolver_scale.y += SIZE_STEP;
		revolver_scale.z += SIZE_STEP;
	}
	//R to shrink the spinner
	else if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		revolver_scale.x -= SIZE_STEP;
		revolver_scale.y -= SIZE_STEP;
		revolver_scale.z -= SIZE_STEP;
	}

	//P to change to points rendering
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
		rendering_mode = 0;

	//W to change to lines rendering
	if (key == GLFW_KEY_W && action == GLFW_PRESS)
		rendering_mode = 1;

	//T to change to full square (or triangles) rendering
	if (key == GLFW_KEY_T && action == GLFW_PRESS)
		rendering_mode = 2;

	//Left key to make the square rotate in the Z direction
	if (key == GLFW_KEY_LEFT)
		square_rotation.z += SQUARE_ROTATION_STEP;

	//Right key to make the square rotate in the opposite Z direction
	if (key == GLFW_KEY_RIGHT)
		square_rotation.z -= SQUARE_ROTATION_STEP;

	//Up key to make the square rotate in the X direction
	if (key == GLFW_KEY_UP)
		square_rotation.x += SQUARE_ROTATION_STEP;

	//Down key to make the square rotate in the opposite X direction
	if (key == GLFW_KEY_DOWN)
		square_rotation.x -= SQUARE_ROTATION_STEP;
}

// The MAIN function, from here we start the application and run the game loop
int main()
{
	std::cout << "Starting GLFW context, OpenGL 3.3" << std::endl;
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Assignment 1", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_callback);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	// Define the viewport dimensions
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	glViewport(0, 0, width, height);

	// Build and compile our shader program
	// Vertex shader

	// Read the Vertex Shader code from the file
	string vertex_shader_path = "vertex.shader";
	string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_shader_path, ios::in);

	if (VertexShaderStream.is_open()) {
		string Line = "";
		while (getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory ?\n", vertex_shader_path.c_str());
		getchar();
		exit(-1);
	}

	// Read the Fragment Shader code from the file
	string fragment_shader_path = "fragment.shader";
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_shader_path, std::ios::in);

	if (FragmentShaderStream.is_open()) {
		std::string Line = "";
		while (getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory?\n", fragment_shader_path.c_str());
		getchar();
		exit(-1);
	}

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(vertexShader, 1, &VertexSourcePointer, NULL);
	glCompileShader(vertexShader);
	// Check for compile time errors
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// Fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(fragmentShader, 1, &FragmentSourcePointer, NULL);
	glCompileShader(fragmentShader);
	// Check for compile time errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// Link shaders
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// Check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader); //free up memory
	glDeleteShader(fragmentShader);

	glUseProgram(shaderProgram);

	//Spinner cube
	GLfloat cube[] = {
		-1.0f,   1.0f,   1.0f, // top left
		-1.0f,  -1.0f,   1.0f, // bottom left
		1.0f,  -1.0f,   1.0f, // bottom right
		-1.0f,   1.0f,   1.0f, // top left
		1.0f,   1.0f,   1.0f, // top right
		1.0f,  -1.0f,   1.0f, // bottom right
		-1.0f,   1.0f,  -1.0f, // top left
		-1.0f,  -1.0f,  -1.0f, // bottom left
		1.0f,  -1.0f,  -1.0f, // bottom right
		-1.0f,   1.0f,  -1.0f, // top left
		1.0f,   1.0f,  -1.0f, // top right
		1.0f,  -1.0f,  -1.0f, // bottom right
		-1.0f,   1.0f,   1.0f, // top left
		-1.0f,  -1.0f,   1.0f, // bottom left
		-1.0f,  -1.0f,  -1.0f, // bottom right
		-1.0f,   1.0f,   1.0f, // top left
		-1.0f,   1.0f,  -1.0f, // top right
		-1.0f,  -1.0f,  -1.0f, // bottom right
		1.0f,   1.0f,   1.0f, // top left
		1.0f,  -1.0f,   1.0f, // bottom left
		1.0f,  -1.0f,  -1.0f, // bottom right
		1.0f,   1.0f,   1.0f, // top left
		1.0f,   1.0f,  -1.0f, // top right
		1.0f,  -1.0f,  -1.0f, // bottom right
		-1.0f,   1.0f,   1.0f, // top left
		1.0f,   1.0f,   1.0f, // bottom left
		1.0f,   1.0f,  -1.0f, // bottom right
		-1.0f,   1.0f,   1.0f, // top left
		-1.0f,   1.0f,  -1.0f, // top right
		1.0f,   1.0f,  -1.0f, // bottom right
		-1.0f,  -1.0f,   1.0f, // top left
		1.0f,  -1.0f,   1.0f,  // bottom left
		1.0f,  -1.0f,  -1.0f, // bottom right
		-1.0f,  -1.0f,   1.0f, // top left
		-1.0f,  -1.0f,  -1.0f, // top right
		1.0f,  -1.0f,  -1.0f, // bottom right
	};

	GLfloat spinner_color[] = { 
		1.0f, 0.0f, 0.0f, 1.0f
	};
	GLfloat revolver_color[] = {
		0.0f, 0.0f, 1.0f, 1.0f
	};
	GLint colorLoc = glGetUniformLocation(shaderProgram, "color");

	GLuint VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);
	glPointSize(10);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO

	//MVP
	GLuint transformLoc = glGetUniformLocation(shaderProgram, "model_matrix");
	GLuint viewMatrixLoc = glGetUniformLocation(shaderProgram, "view_matrix");
	GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection_matrix");

	float rotation = 0.0;
	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		glProgramUniform4fv(shaderProgram, colorLoc, 1, spinner_color);
		// Check if any events have been activated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();

		// Render
		// Clear the colorbuffer
		glClearColor(0.9f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//Used to rotate both meshes on the X, Y and Z direction
		glm::mat4 mesh_rotation_matrix;
		mesh_rotation_matrix = glm::rotate(mesh_rotation_matrix, square_rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		mesh_rotation_matrix = glm::rotate(mesh_rotation_matrix, square_rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		mesh_rotation_matrix = glm::rotate(mesh_rotation_matrix, rotation, glm::vec3(0.0f, 1.0f, 0.0f));

		//Used to grow and shrink the spinner
		glm::mat4 spinner_scale_matrix;
		spinner_scale_matrix = glm::scale(mesh_rotation_matrix, spinner_scale);

		//Used to make the camera always look at the spinner
		glm::mat4 camera_look_at = glm::lookAt(camera_translation, glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));

		//Projection matrix so that everything is properly sized
		glm::mat4 projection_matrix;
		projection_matrix = glm::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.0f, 100.0f);

		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(spinner_scale_matrix));
		glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, glm::value_ptr(camera_look_at));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection_matrix));

		glBindVertexArray(VAO);

		//Renders the spinner cube either completely, just the edges or just the points.
		if (rendering_mode == 0)
		{
			glDrawArrays(GL_POINTS, 0, 36);
		}
		else if (rendering_mode == 1)
		{
			glDrawArrays(GL_LINES, 0, 36);
		}
		else if (rendering_mode == 2)
		{
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		glBindVertexArray(0);

		//change the revolvers cube color to blue
		glProgramUniform4fv(shaderProgram, colorLoc, 1, revolver_color);
		glm::mat4 revolver_matrix;
		//translate and scale the revolver cube so that it is smaller and spinning around the spinner cube
		revolver_matrix = glm::translate(mesh_rotation_matrix, glm::vec3(10.0f, 0.0f, 0.0f));
		revolver_matrix = glm::scale(revolver_matrix, revolver_scale);
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(revolver_matrix));
		
		glBindVertexArray(VAO);
		//Renders the revolver cube either completely, just the edges or just the points.
		if (rendering_mode == 0)
		{
			glDrawArrays(GL_POINTS, 0, 36);
		}
		else if (rendering_mode == 1)
		{
			glDrawArrays(GL_LINES, 0, 36);
		}
		else if (rendering_mode == 2)
		{
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		glBindVertexArray(0);

		// Swap the screen buffers
		glfwSwapBuffers(window);
		rotation += 0.01;

		//This handles the zooming in and out of the camera.
		if (left_button_down)
		{
			double x;
			double y;
			glfwGetCursorPos(window, &x, &y);

			if (previous_y_position - y >= 0 && camera_translation.y >= 5)
			{
				camera_translation.y -= 0.5f;
				previous_y_position = y;
			}
			else if(previous_y_position - y < 0)
			{
				camera_translation.y += 0.5f;
				previous_y_position = y;
			}
		}
	}

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}
