#include "glfwInit.h"

//Static var initializations
GLfloat GLFWApp::camSpeed = 0.5f;
Simplex::CameraManager* GLFWApp::camMang = nullptr;
Simplex::SystemSingleton* GLFWApp::system = nullptr;
bool GLFWApp::toggleRot = false;

//Initialize the window
GLint GLFWApp::init()
{
	readFile("Ex1_Practical.ini"); //Read the ini
	std::cout << "Initializing GLFW . . ." << std::endl;
	//Check if glew fails to initialize
	if (!glfwInit())
	{
		std::cout << "Failed to initiate GLFW!" << std::endl;
		return -1;
	}

	//Set the error callback
	glfwSetErrorCallback(errorCallback);

	//GLFW window hints
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	window = glfwCreateWindow(width, height, "Exam 1 - Practical", nullptr, nullptr); //The window pointer

	//Check if the window fails to init
	if (window == nullptr)
	{
		std::cout << "Failed to create window!" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	//Set key callback
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPos(window, this->width / 2, this->height / 2);

	std::cout << "Initializing GLEW . . ." << std::endl;
	glewExperimental = GL_TRUE;

	//Check if glew initializes
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW!" << std::endl;
		return -1;
	}

	glfwGetFramebufferSize(window, &this->width, &this->height);

	//OpenGL functions
	glClearColor(0.4f, 0.6f, 0.9f, 1.0f);
	glViewport(0, 0, width, height);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	return 0;
}

GLint GLFWApp::run()
{
	//Setup Simplex systems
	system = Simplex::SystemSingleton::GetInstance();
	camMang = Simplex::CameraManager::GetInstance();
	camMang->SetPositionTargetAndUp(glm::vec3(0.0, 0.0, 7.5f),
									glm::vec3(0.0, 0.0, -1.0f),
									glm::vec3(0.0, 1.0, 0.0));

	Simplex::MeshManager* meshMang = Simplex::MeshManager::GetInstance();

	glm::vec3 ballScale(0.1f, 0.1f, 0.1f); //Default ball scale
	glm::mat4 bMat; //Ball Matrix
	glm::mat4 tMat; //Torus Matrix

	GLfloat dTime = 0; //Delta time
	unsigned int uClock = system->GenClock(); //Clock

	GLfloat percent; //Lerp Percent
	glm::vec3 curPos = glm::vec3(); //Torus current pos
	GLuint route = 0; //Route for Lerp

	//Main Loop
	while (!glfwWindowShouldClose(window))
	{
		dTime += system->GetDeltaTime(uClock); //Get dTime

		//Get Input and clear the screen
		glfwPollEvents();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Render the skybox
		meshMang->AddSkyboxToRenderList();

		//If toggleRot is on, let the torus rotate
		if (toggleRot)
		{
			torusRotY += 0.01f;
			
			if (torusRotY == 360)
				torusRotY = 0;
		}

		//Render each of the points as an icosphere
		for (GLuint i = 0; i < stops.size(); i++)
		{
			bMat = glm::mat4(1.0f); //Reset the ball mat to identity mat
			bMat = glm::translate(bMat, stops[i]); //Translate the bMat by the position at stops[i]
			bMat = glm::scale(bMat, ballScale); //Scale the bMat by the scale
			meshMang->AddIcoSphereToRenderList(bMat, glm::vec3(1.0f, 0.0f, 0.0f)); //Render the spheres with a red color
		}

		//Get the lerp percent
		percent = Simplex::MapValue(dTime, 0.0f, 1.0f, 0.0f, 1.0f);
		
		//If there aren't any stops, position the torus at 0
		if (stops.size() == 0)
			curPos = glm::vec3();

		else
		{
			//Stops that aren't at the end of the vector
			if (route < stops.size() - 1)
			{
				curPos = glm::lerp(stops[route], stops[route + 1], percent);
			}

			//The last stop in the vector
			else
			{
				curPos = glm::lerp(stops[stops.size() - 1], stops[0], percent);
			}

			//Check if the percent has reached 100
			if (percent >= 1.0f)
			{
				route++;
				route %= stops.size();
				dTime = 0;
			}
		}

		tMat = glm::mat4(1.0f);  //Reset the torus mat to identity mat
		tMat = glm::translate(tMat, curPos); //Translate the torus by curPos
		tMat *= glm::yawPitchRoll(0.0f, torusRotY, 0.0f); //Rotate the torus

		meshMang->AddTorusToRenderList(tMat, glm::vec3(0.0f, 1.0f, 0.0f)); //Render the torus with green color
		meshMang->Render(); //Render the scene
		meshMang->ClearRenderList(); //Clear the render list

		glfwSwapBuffers(window); //Swap buffers
	}
	return 0;
}

GLint GLFWApp::end()
{
	Simplex::ReleaseAllSingletons(); //Release the singletons
	glfwTerminate(); //Terminate GLFW
	return 0;
}

void GLFWApp::readFile(const GLchar* path)
{
	Simplex::FileReader reader;

	//Check if the file can be red
	if (reader.ReadFile(path) == Simplex::OUT_ERR_FILE_MISSING)
	{
		return;
	}
	
	reader.Rewind();

	//Loop through each line of the file
	while (reader.ReadNextLine() == Simplex::OUT_RUNNING)
	{
		//Get the word, check for compatible word
		Simplex::String configWord = reader.GetFirstWord();
	
		//Gets the resolution
		if (configWord == "Resolution:")
		{
			sscanf_s(reader.m_sLine.c_str(), "Resolution: [%d,%d]", &width, &height);;
		}

		//Gets a stop and puts it into the list
		if (configWord == "Stop:")
		{
			glm::vec3 vals;
			sscanf_s(reader.m_sLine.c_str(), "Stop: [%f,%f,%f]", &vals.x, &vals.y, &vals.z);
			stops.push_back(vals);
		}
	}
}

void GLFWApp::errorCallback(GLint error, const GLchar* errorStr)
{
	std::cout << "GLFW Error: " << error << errorStr << std::endl;
}

void GLFWApp::keyCallback(GLFWwindow* window, GLint key, GLint scancode, GLint action, GLint mods)
{
	//Cam movement
	if (key == GLFW_KEY_W && action == GLFW_REPEAT)
		camMang->MoveForward(camSpeed);
	if (key == GLFW_KEY_S && action == GLFW_REPEAT)
		camMang->MoveForward(-camSpeed);
	if (key == GLFW_KEY_D && action == GLFW_REPEAT)
		camMang->MoveSideways(camSpeed);
	if (key == GLFW_KEY_A && action == GLFW_REPEAT)
		camMang->MoveSideways(-camSpeed);
	if (key == GLFW_KEY_E && action == GLFW_REPEAT)
		camMang->MoveVertical(camSpeed);
	if (key == GLFW_KEY_Q && action == GLFW_REPEAT)
		camMang->MoveVertical(-camSpeed);

	//Toggle torus rotation
	if (key == GLFW_KEY_R && action == GLFW_PRESS)
		toggleRot = !toggleRot;
}