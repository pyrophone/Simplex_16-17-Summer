#ifndef GLFWINIT_H
#define GLFWINIT_H

#include <iostream>
#include <vector>
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "Simplex/Simplex.h"
#include "AdjacencyNode.h"

/*GLFW app class*/
class GLFWApp
{
	public:
		GLint init(); //Initializes GLFW
		GLint run(); //Runs main loop
		GLint end(); //Cleanup

	private:
		static GLfloat camSpeed;
		static Simplex::CameraManager* camMng;
		static Simplex::SystemSingleton* system;
		static std::vector<AdjacencyNode> adjacenyList; //Adjacency List
		static bool doPath, getPath; //Bools to run the path and to get the path
		GLFWwindow* window = nullptr; //Window pointer to create a window
		
		GLint width, height; //Width + height vars
		static GLuint currentNode; //The current node number

		//Static callbacks
		static void errorCallback(GLint error, const GLchar* errorStr);
		static void keyCallback(GLFWwindow* window, GLint key, GLint scancode, GLint action, GLint mods);
};
#endif