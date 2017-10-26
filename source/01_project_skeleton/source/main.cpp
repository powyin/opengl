

#include "platform.hpp"

// third-party libraries
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

// standard C++ libraries
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <cmath>

#include "ShaderId.h"

// tdogl classes
#include "Program.h"

// constants
const glm::vec2 SCREEN_SIZE(600, 600);

// globals
GLFWwindow* gWindow = NULL;
tdogl::Program* gProgram = NULL;
GLuint gVAO = 0;
GLuint gVBO = 0;
GLuint gVEO = 0;


// loads the vertex shader and fragment shader, and links them to make the global gProgram
static void LoadShaders() {
    std::vector<tdogl::Shader> shaders;
    shaders.push_back(tdogl::Shader::shaderFromFile(ResourcePath("vertex-shader.txt"), GL_VERTEX_SHADER));
    shaders.push_back(tdogl::Shader::shaderFromFile(ResourcePath("fragment-shader.txt"), GL_FRAGMENT_SHADER));
    gProgram = new tdogl::Program(shaders);
}


// loads a triangle into the VAO global
static void LoadTriangle() {


	// make and bind the VBO
	glGenBuffers(1, &gVBO);
	glBindBuffer(GL_ARRAY_BUFFER, gVBO);
	// Put the three triangle verticies into the VBO
	GLfloat vertexData[] = {
		//  X     Y     Z   R     G     B     A
		0.0f, 0.0f, 0.0f,  1.0f, 1.0f, 1.0f, 1.0f,
		0.0f, 0.5f, 0.0f,  1.f, 0.1f, 0.0f,  1.0f,
		0.3f, 0.3f, 0.0f,  0.0f, 0.0f, 0.6f, 1.0f,
		0.5f, 0.0f, 0.0f,  0.0f, 0.8f, 0.0f, 1.0f,
		0.3f,-0.3f, 0.0f,  0.9f, 0.0f, 0.0f, 1.0f,
		0.0f,-0.5f, 0.0f,  0.3f, 0.3f, 0.3f, 1.0f,
		-0.3f,-0.3f, 0.0f,  0.7f, 0.7f, 0.7f, 1.0f,
		-0.5f, 0.0f, 0.0f,  1.0f, 0.0f, 1.0f, 1.0f,
		-0.3f, 0.3f, 0.0f,  0.0f, 1.0f, 0.0f, 1.0f,
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);




	// make and bind the VAO
	glGenVertexArrays(1, &gVAO);
	glBindVertexArray(gVAO);

	// make and bin the VEO
	glGenBuffers(1, &gVEO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVEO);
	GLubyte indexBuf[] =
	{
		0, 1, 2, 0, 2, 3, // Quad 0
		0, 3, 4, 0, 4, 5, // Quad 1
		0, 5, 6, 0, 6, 7, // Quad 2
	};
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexBuf), indexBuf, GL_STATIC_DRAW);

    // connect the xyz to the "vert" attribute of the vertex shader
	glEnableVertexAttribArray(gProgram->attrib("vect"));
    glVertexAttribPointer(gProgram->attrib("vect"), 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), NULL);

	// connect the xyz to the "color" attribute of the vertex shader
	glEnableVertexAttribArray(gProgram->attrib("color"));
	glVertexAttribPointer(gProgram->attrib("color"), 4, GL_FLOAT, GL_TRUE, 7 * sizeof(GLfloat), (const void*)(3*sizeof(GLfloat)));


	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


enum {color, depth, numRenderBuffer};
GLuint frameBuffer, renderbuffer[numRenderBuffer];
GLuint textBuffer = 0;
ShaderId* mk = 0;
Pow* mPow = 0;

static void createFrameBuffer() {
	glGenRenderbuffers(numRenderBuffer, renderbuffer);

	glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer[color]);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, 256, 256);

	glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer[depth]);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, 256, 256);

	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBuffer);

	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, renderbuffer[color]);
	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderbuffer[depth]);

	glEnable(GL_DEPTH_TEST);
}

static void beginFBO() {
	if (textBuffer == 0) {
		glGenTextures(1, &textBuffer);
		glBindTexture(GL_TEXTURE_2D, textBuffer);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexImage2D(	GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textBuffer, 0);

	if (mPow == 0) {
		mPow = new Pow();
		mPow->initialize();
	}



}

static void endFBO() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}



// draws a single frame
static void Render() {
    // clear everything
    glClearColor(0.3f, 0.3f, 0.3f, 1);  // black
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BITS);

	beginFBO();

	// clear everything
	glClearColor(0.3f, 0.3f, 0.3f, 1);  // black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BITS);

	// bind the program (the shaders)
	glUseProgram(gProgram->object());

	// bind the VAO (the triangle)
	glBindVertexArray(gVAO);


	glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_BYTE, 0);


	glDrawArrays(GL_TRIANGLE_FAN, 0, 9);



	glBindVertexArray(0);
	glUseProgram(0);
	endFBO();

	glUniform1i(mPow->_texture, 0);
	//! »æÖÆµØÃæ
	glBindTexture(GL_TEXTURE_2D, textBuffer);




	glVertexAttribPointer(mPow->_position, 3, GL_FLOAT, false, sizeof(Vertex), &grounds[0].x);
	glVertexAttribPointer(mPow->_uv, 2, GL_FLOAT, false, sizeof(Vertex), &grounds[0].u);

	glDrawArrays(GL_TRIANGLES, 0, sizeof(grounds) / sizeof(grounds[0]));


	glBindTexture(GL_TEXTURE_2D, 0);


    // swap the display buffers (displays what was just drawn)
    glfwSwapBuffers(gWindow);
}

void OnError(int errorCode, const char* msg) {
    throw std::runtime_error(msg);
}

// the program starts here
void AppMain() {
    // initialise GLFW
    glfwSetErrorCallback(OnError);
    if(!glfwInit())
        throw std::runtime_error("glfwInit failed");
    
    // open a window with GLFW
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    gWindow = glfwCreateWindow((int)SCREEN_SIZE.x, (int)SCREEN_SIZE.y, "OpenGL", NULL, NULL);
    if(!gWindow)
        throw std::runtime_error("glfwCreateWindow failed. Can your hardware handle OpenGL 3.2?");

    // GLFW settings
    glfwMakeContextCurrent(gWindow);
    
    // initialise GLEW
    glewExperimental = GL_TRUE; //stops glew crashing on OSX :-/
    if(glewInit() != GLEW_OK)
        throw std::runtime_error("glewInit failed");

    // print out some info about the graphics drivers
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;

    // make sure OpenGL version 3.2 API is available
    if(!GLEW_VERSION_3_2)
        throw std::runtime_error("OpenGL 3.2 API is not available.");


    // load vertex and fragment shaders into opengl
    LoadShaders();

    // create buffer and fill it with the points of the triangle
    LoadTriangle();

	// create frameBuffer
	createFrameBuffer();

    // run while the window is open
    while(!glfwWindowShouldClose(gWindow)){
        // process pending events
        glfwPollEvents();
        
        // draw one frame
        Render();
    }

    // clean up and exit
    glfwTerminate();
}



int main(int argc, char *argv[]) {
    try {
        AppMain();
    } catch (const std::exception& e){
        std::cerr << "ERROR: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }



    return EXIT_SUCCESS;
}
