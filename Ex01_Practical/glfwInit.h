#ifndef GLFWINIT_H
#define GLFWINIT_H

#include <iostream>
#include <vector>
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "Simplex/Simplex.h"
/*GLFW app class*/
class GLFWApp
{
	public:
		GLint init(); //Initializes GLFW
		GLint run(); //Runs main loop
		GLint end(); //Cleanup

	private:
		//Ideally these wouldn't be static, but I couldn't find
		//A way to implement theme without being static
		static GLfloat camSpeed;
		static Simplex::CameraManager* camMang;
		static Simplex::SystemSingleton* system;
		static bool toggleRot; //Whether the torus should rotate
		GLFWwindow* window; //Window pointer to create a window
		GLfloat torusRotY = 90.0f; //Current torus rotation angle
		std::vector<glm::vec3> stops; //std::vectors of stops
	
		void readFile(const GLchar* path); //File read method
		GLint width, height; //Width + height vars

		//Static callbacks
		static void errorCallback(GLint error, const GLchar* errorStr);
		static void keyCallback(GLFWwindow* window, GLint key, GLint scancode, GLint action, GLint mods);
};
#endif