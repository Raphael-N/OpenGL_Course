// OpenGLCourseApp.cpp : Diese Datei enthält die Funktion "main". Hier beginnt und endet die Ausführung des Programms.
//

#include <cstdio>
#include <cstring>
#include <cstdlib>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

using namespace glm;

// Window dimensions
const GLint WIDTH = 800;
const GLint HEIGHT = 600;
const float TO_RADIANS = 3.14159265359f / 180.0f;

GLuint VAO;
GLuint VBO;
GLuint shader;
GLuint uniformModel;

bool direction = false;
float triOffset = 0.0f;
float triMaxOffset = 0.7f;
float triIncrement = 0.005f;

float currentAngle = 0.0f;

bool sizeDirection = false;
float currentSize = 0.4f;
float sizeIncrement = 0.01f;
float maxSize = 0.8f;





// Vertex Shader
const char* vertexShader =
	"#version 460 \n"
	""
	"layout (location = 0) in vec3 pos; \n"
	"\n"
	"uniform mat4 model;\n"
	"\n"
	"\n"
	"void main() { \n"
	"	gl_Position = model * vec4(pos, 1.0);\n"
	"}";


// Fragment Shader
const char* fragmentShader =
	"#version 460 \n"
	""
	"out vec4 color;\n"
	"void main() {\n"
	"	color = vec4(1.0);\n"
	"}";

void addShader(const GLuint theProgram, const char* shaderCode, const GLenum shaderType) {
	GLuint theShader = glCreateShader(shaderType);
	const GLchar* theCode[1];
	theCode[0] = shaderCode;
	
	GLint codeLength[1];
	codeLength[0] = strlen(shaderCode);

	glShaderSource(theShader, 1, theCode, codeLength);
	glCompileShader(theShader);

	GLint result = 0;
	GLchar errorLog[1024] = { 0 };

	glLinkProgram(theShader);
	glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);

	if (!result) {
		glGetShaderInfoLog(theShader, sizeof(errorLog), nullptr, errorLog);
		printf("Error compiling the %d shader: %s\n", shaderType, errorLog);
		return;
	}
	glAttachShader(theProgram, theShader);
}

void compileShaders() {
	shader = glCreateProgram();
	if (!shader) {
		printf("Error creating shader program");
		return;
	}
	addShader(shader, vertexShader, GL_VERTEX_SHADER);
	addShader(shader, fragmentShader, GL_FRAGMENT_SHADER);

	GLint result = 0;
	GLchar errorLog[1024] = {0};

	glLinkProgram(shader);
	glGetProgramiv(shader, GL_LINK_STATUS, &result);

	if (!result) {
		glGetProgramInfoLog(shader, sizeof(errorLog), nullptr, errorLog);
		printf("Error linking program: %s\n", errorLog);
		return;
	}

	glValidateProgram(shader);
	glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);

	if (!result) {
		glGetProgramInfoLog(shader, sizeof(errorLog), nullptr, errorLog);
		printf("Error validating program: %s\n", errorLog);
		return;
	}

	uniformModel = glGetUniformLocation(shader, "model");
}

void createTriangle() {
	GLfloat vertices[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		0.0f, 1.0f, 0.0f
	};

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}


int main() {
	if (!glfwInit()) {
		printf("GLFW initialization failed");
		glfwTerminate();
		return 1;
	}
	// Setup GLFW window properties

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	// Core Profile = no backwards compat
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "test window", NULL, NULL);

	if (!mainWindow) {
		printf("GLFW window creation failed");
		glfwTerminate();
		return 1;
	}

	// Get Buffer size information
	int bufferWidth;
	int bufferHeight;
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	// Draw to main window
	glfwMakeContextCurrent(mainWindow);

	// Allow modern extension features
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK) {
		printf("GLEW initialization failed");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	// Setup Viewport size
	glViewport(0, 0, bufferWidth, bufferHeight);

	createTriangle();
	compileShaders();

	// Loop until window closed

	while (!glfwWindowShouldClose(mainWindow)) {
		// Get + handle user input events
		glfwPollEvents();

		if(direction) {
			triOffset += triIncrement;
		} else {
			triOffset -= triIncrement;
		}

		if(abs(triOffset) >= triMaxOffset) {
			direction = !direction;
		}

		currentAngle += 0.1f;
		if(currentAngle > 360) {
			currentAngle = 0.0f;
		}

		if(sizeDirection) {
			currentSize += sizeIncrement;
		} else {
			currentSize -= sizeIncrement;
		}

		if(currentSize >= maxSize || currentSize <= maxSize/4) {
			sizeDirection = !sizeDirection;
		}

		

		// clear window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		glUseProgram(shader);

		mat4 model(1.0f);
		model = rotate(model, currentAngle * TO_RADIANS, vec3(0.0f, 0.0f, 1.0f));
		model = translate(model, vec3(triOffset, 0.0f, 0.0f));
		model = scale(model, vec3(currentSize, currentSize, 0.0f));
		
		

		glUniformMatrix4fv(uniformModel, 1, false, value_ptr(model));
		
		glBindVertexArray(VAO);

		glDrawArrays(GL_TRIANGLES, 0, 3);
		
		glBindVertexArray(0);
		glUseProgram(0);

		

		glfwSwapBuffers(mainWindow);
	}


	return 0;
}

// Programm ausführen: STRG+F5 oder Menüeintrag "Debuggen" > "Starten ohne Debuggen starten"
// Programm debuggen: F5 oder "Debuggen" > Menü "Debuggen starten"
