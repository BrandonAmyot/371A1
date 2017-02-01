//Lab 2
//modified from http://learnopengl.com/

#include "stdafx.h"

#include "..\glew\glew.h"	// include GL Extension Wrangler
#include "..\glfw\glfw3.h"	// include GLFW helper library
#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <glm.hpp>			// include GL Math
#include "gtc\matrix_transform.hpp"
#include "gtc\type_ptr.hpp"
#include <type_vec3.hpp>

using namespace std;

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;

glm::vec3 triangle_scale;
glm::vec3 camera_translation = glm::vec3(0.0f, 0.0f, -1.0f);

const float TRIANGLE_MOVEMENT_STEP = 0.1f;
const float CAMERA_PAN_STEP = 0.2f;

// Declare the matrices
glm::mat4 model_matrix;
glm::mat4 view_matrix;
glm::mat4 proj_matrix;


// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	std::cout << key << std::endl;
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key == GLFW_KEY_A && action == GLFW_PRESS)
		camera_translation.x += CAMERA_PAN_STEP;
	if (key == GLFW_KEY_D && action == GLFW_PRESS)
		camera_translation.x -= CAMERA_PAN_STEP;
	if (key == GLFW_KEY_W && action == GLFW_PRESS)
		camera_translation.y -= CAMERA_PAN_STEP;
	if (key == GLFW_KEY_S && action == GLFW_PRESS)
		camera_translation.y += CAMERA_PAN_STEP;

	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
		triangle_scale.x += TRIANGLE_MOVEMENT_STEP;
	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
		triangle_scale.x -= TRIANGLE_MOVEMENT_STEP;
	if (key == GLFW_KEY_UP && action == GLFW_PRESS)
		triangle_scale.y += TRIANGLE_MOVEMENT_STEP;
	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
		triangle_scale.y -= TRIANGLE_MOVEMENT_STEP;
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
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Triangle", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);
	
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

	// Get input
	int profileCurveNum;
	int trajCurveNum;
	vector<glm::vec3*>* profileCurve = new vector<glm::vec3*>;
	vector<glm::vec3*>* trajCurve = new vector<glm::vec3*>;
	float x, y, z;
	ifstream inputFile;
	inputFile.open("input_a1.txt");
	
	if (inputFile.is_open()) {
		string line;
		getline(inputFile, line);
		int objectType = stoi(line);
		
		if (objectType == 0) { // transitional sweep
			// profile curve
			getline(inputFile, line);
			profileCurveNum = stoi(line);
			for (int i = 0; i < profileCurveNum; i++) {
				getline(inputFile, line);
				stringstream lineStream(line);

				lineStream >> x >> y >> z;
				cout << x << ", " << y << ", " << z << endl;

				profileCurve->push_back(new glm::vec3(x, y, z));
			}
			// trajectory curve
			getline(inputFile, line);
			trajCurveNum = stoi(line);
			for (int i = 0; i < trajCurveNum; i++) {
				getline(inputFile, line);
				stringstream lineStream(line);

				lineStream >> x >> y >> z;
				cout << x << ", " << y << ", " << z << endl;

				trajCurve->push_back(new glm::vec3(x, y, z));
			}
		}
		else { // rotational sweep

		}
	}

	int size = profileCurve->size()*trajCurve->size() * 6;
	int index = 0, indicesIndex = 0;
	GLfloat* vertices = new GLfloat[size];
	
	int indicesSize = ((profileCurve->size() - 1) * (trajCurve->size() - 1) * 2 * 3);
	GLuint* indices = new GLuint[indicesSize];

	for (int p = 0; p < profileCurve->size(); p++) {
		
		glm::vec3* pVec = profileCurve->at(p);

		for (int t = 0; t < trajCurve->size(); t++) {
			
			float height = (float(p) / float(profileCurve->size()));

			glm::vec3* tVec = trajCurve->at(t);

			vertices[index] = pVec->x + tVec->x;
			vertices[index + 1] = pVec->y + tVec->y;
			vertices[index + 2] = pVec->z + tVec->z;
			vertices[index + 3] = height;
			vertices[index + 4] = height;
			vertices[index + 5] = 0;
			index += 6;

			if (t > 0 && p > 0) {
				indices[indicesIndex] = index / 6 - 1;
				indices[indicesIndex + 1] = index / 6 - 2;
				indices[indicesIndex + 2] = index / 6 - trajCurve->size() - 2;

				indices[indicesIndex + 3] = index / 6 - 1;
				indices[indicesIndex + 4] = index / 6 - trajCurve->size() - 1;
				indices[indicesIndex + 5] = index / 6 - trajCurve->size() - 2;
				indicesIndex += 6;
			}
		}
	}
	//TESTING
	int c = 0;
	for (int i = 0; i < size; i++)
	{
		printf("%f ", vertices[i]);
		c++;
		if (c > 2)
		{
			c = 0;
			std::cout << std::endl;
		}
	}
	c = 0;
	std::cout << "Indices " << indicesSize << std::endl;
	for (int i = 0; i < indicesSize; i++)
	{
		printf("%d ", indices[i]);
		c++;
		if (c > 2)
		{
			c = 0;
			std::cout << std::endl;
		}
	}

	

	GLuint VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices)*size, vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices)*indicesSize, indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3* sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind
	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO

	triangle_scale = glm::vec3(1.0f); //shorthand, initializes all 4 compenents to 1.0f

	GLint mm = glGetUniformLocation(shaderProgram, "model");
	GLint vm = glGetUniformLocation(shaderProgram, "view");
	GLint pm = glGetUniformLocation(shaderProgram, "proj");

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();

		// Render
		// Clear the colorbuffer
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glm::mat4 model_matrix;
		model_matrix = glm::scale(model_matrix, triangle_scale);

		glm::mat4 view_matrix;
		view_matrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 1.0f),	 // camera positioned here
									glm::vec3(0.0f, 0.0f, 0.0f), // looks at origin
									glm::vec3(0.0f, 1.0f, 0.0f));// up vector

		//view_matrix = glm::translate(view_matrix, camera_translation);

		glm::mat4 proj_matrix;
		proj_matrix = glm::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.0f, 100.0f);


		glUniformMatrix4fv(mm, 1, GL_FALSE, glm::value_ptr(model_matrix));
		glUniformMatrix4fv(vm, 1, GL_FALSE, glm::value_ptr(view_matrix));
		glUniformMatrix4fv(pm, 1, GL_FALSE, glm::value_ptr(proj_matrix));

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indicesSize, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}
