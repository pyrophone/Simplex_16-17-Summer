#include "glfwInit.h"

//Static var initializations
GLfloat GLFWApp::camSpeed = 0.5f;
Simplex::CameraManager* GLFWApp::camMng = nullptr;
Simplex::SystemSingleton* GLFWApp::system = nullptr;
std::vector<AdjacencyNode> GLFWApp::adjacenyList;
GLuint GLFWApp::currentNode = 0;
bool GLFWApp::doPath = false;
bool GLFWApp::getPath = false;

//Initialize the window
GLint GLFWApp::init()
{
	width = 800;
	height = 600;

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
	window = glfwCreateWindow(width, height, "Exam 2 - Practical", nullptr, nullptr); //The window pointer

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

	//Set up nodes
	adjacenyList.push_back(AdjacencyNode(glm::vec3(  30.0f, 0.0f,  0.0f)));
	adjacenyList.push_back(AdjacencyNode(glm::vec3(  20.0f, 0.0f,  20.0f)));
	adjacenyList.push_back(AdjacencyNode(glm::vec3(  15.0f, 0.0f, -10.0f)));
	adjacenyList.push_back(AdjacencyNode(glm::vec3(  10.0f, 0.0f,  4.0f)));
	adjacenyList.push_back(AdjacencyNode(glm::vec3(   0.0f, 0.0f,  0.0f)));
	adjacenyList.push_back(AdjacencyNode(glm::vec3(  -5.0f, 0.0f,  18.0f)));
	adjacenyList.push_back(AdjacencyNode(glm::vec3(-17.0f, 0.0f, 5.0f)));
	adjacenyList.push_back(AdjacencyNode(glm::vec3(-13.0f, 0.0f, -15.0f)));
	adjacenyList.push_back(AdjacencyNode(glm::vec3(-25.0f, 0.0f,  2.5f)));
	adjacenyList.push_back(AdjacencyNode(glm::vec3(-33.5f, 0.0f,   15.0f)));

	//Set up node connections
	adjacenyList[0].addAdjacentNode(adjacenyList[1]);
	adjacenyList[0].addAdjacentNode(adjacenyList[2]);

	adjacenyList[1].addAdjacentNode(adjacenyList[0]);
	adjacenyList[1].addAdjacentNode(adjacenyList[3]);
	adjacenyList[1].addAdjacentNode(adjacenyList[5]);

	adjacenyList[2].addAdjacentNode(adjacenyList[0]);
	adjacenyList[2].addAdjacentNode(adjacenyList[3]);
	adjacenyList[2].addAdjacentNode(adjacenyList[4]);
	adjacenyList[2].addAdjacentNode(adjacenyList[7]);

	adjacenyList[3].addAdjacentNode(adjacenyList[1]);
	adjacenyList[3].addAdjacentNode(adjacenyList[2]);
	adjacenyList[3].addAdjacentNode(adjacenyList[4]);
	adjacenyList[3].addAdjacentNode(adjacenyList[5]);

	adjacenyList[4].addAdjacentNode(adjacenyList[2]);
	adjacenyList[4].addAdjacentNode(adjacenyList[3]);
	adjacenyList[4].addAdjacentNode(adjacenyList[5]);
	adjacenyList[4].addAdjacentNode(adjacenyList[6]);
	adjacenyList[4].addAdjacentNode(adjacenyList[7]);

	adjacenyList[5].addAdjacentNode(adjacenyList[1]);
	adjacenyList[5].addAdjacentNode(adjacenyList[3]);
	adjacenyList[5].addAdjacentNode(adjacenyList[4]);
	adjacenyList[5].addAdjacentNode(adjacenyList[6]);
	adjacenyList[5].addAdjacentNode(adjacenyList[9]);

	adjacenyList[6].addAdjacentNode(adjacenyList[4]);
	adjacenyList[6].addAdjacentNode(adjacenyList[5]);
	adjacenyList[6].addAdjacentNode(adjacenyList[7]);
	adjacenyList[6].addAdjacentNode(adjacenyList[9]);

	adjacenyList[7].addAdjacentNode(adjacenyList[2]);
	adjacenyList[7].addAdjacentNode(adjacenyList[4]);
	adjacenyList[7].addAdjacentNode(adjacenyList[6]);
	adjacenyList[7].addAdjacentNode(adjacenyList[8]);

	adjacenyList[8].addAdjacentNode(adjacenyList[7]);
	adjacenyList[8].addAdjacentNode(adjacenyList[9]);

	adjacenyList[9].addAdjacentNode(adjacenyList[5]);
	adjacenyList[9].addAdjacentNode(adjacenyList[6]);
	adjacenyList[9].addAdjacentNode(adjacenyList[8]);

	return 0;
}

GLint GLFWApp::run()
{
	//Setup Simplex systems
	Simplex::MeshManager* meshMang = Simplex::MeshManager::GetInstance();
	system = Simplex::SystemSingleton::GetInstance();
	camMng = Simplex::CameraManager::GetInstance();
	camMng->SetPositionTargetAndUp(glm::vec3(0.0f, 50.0f, 0.0f),
								   glm::vec3(0.0f,  0.0f, 0.0f),
								   glm::vec3(0.0f,  0.0f, 1.0f));


	GLfloat dTime = 0; //Delta time
	unsigned int uClock = system->GenClock(); //Clock

	GLuint route = 0; //Route for lerp
	GLfloat counter = 0; //Counter for the lerp

	glm::vec3 cubePos = adjacenyList[0].getPos(); //Square current pos
	GLuint cubeIndex = 0; //Index of the adjacency list the cube is at
	glm::mat4 cMat; //The cube matrix

	std::vector<AdjacencyNode*> adjacentNodes; //Pointers to the adjacent nodes
	std::vector<AdjacencyNode*> path; //Pointers to nodes along the path

	//Main Loop
	while (!glfwWindowShouldClose(window))
	{
		cMat = glm::mat4();
		dTime += system->GetDeltaTime(uClock); //Get dTime

		//Get Input and clear the screen
		glfwPollEvents();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Render the skybox
		meshMang->AddSkyboxToRenderList();

		//Render each of the nodes as an icosphere
		for (GLuint i = 0; i < adjacenyList.size(); i++)
		{
			if(i == currentNode) //Current highlighted node is green
				meshMang->AddIcoSphereToRenderList(adjacenyList[i].getMat(), glm::vec3(0.0f, 1.0f, 0.0f));
			else //Every other node is red
				meshMang->AddIcoSphereToRenderList(adjacenyList[i].getMat(), glm::vec3(1.0f, 0.0f, 0.0f));

			//Get the adjacent nodes and render the lines between them and the current node as blue
			adjacentNodes = adjacenyList[i].getAdjacentNodes();
			for (GLuint j = 0; j < adjacentNodes.size(); j++)
			{
				meshMang->AddLineToRenderList(glm::mat4(1.0f), adjacenyList[i].getPos(), adjacentNodes[j]->getPos(), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			}
		}

		//Render the cube
		cMat = glm::translate(cMat, cubePos);
		cMat = glm::scale(cMat, glm::vec3(2.0f, 2.0f, 2.0f));
		meshMang->AddCubeToRenderList(cMat, glm::vec3(1.0f, 1.0f, 1.0f));
		
		//Run the cube along the a* path
		if(doPath)
		{
			counter += 0.01f; //Add to the counter

			//Gets the path calculated by a*
			if (getPath)
			{
				for (GLuint i = 0; i < adjacenyList.size(); i++)
					adjacenyList[i].reset(); //Reset each of the nodes
				path = AdjacencyNode::getShortestPath(adjacenyList[cubeIndex], adjacenyList[currentNode]);
				getPath = false;
			}

			//Stops that aren't at the end of the vector
			if (route < path.size() - 1)
			{
				//Lerp from the current to the next one
				cubePos = glm::lerp(path[route]->getPos(), path[route + 1]->getPos(), counter);
			}

			//The last stop in the vector
			else
			{
				//Set the cube index, make sure the position is right, set doPath to false, reset route and count
				cubeIndex = currentNode;
				cubePos = adjacenyList[cubeIndex].getPos();
				doPath = false;
				route = 0;
				counter = 0.0f;
			}

			//Check if the percent has reached 100
			if (counter >= 1.0f)
			{
				route++;
				route %= path.size();
				counter = 0;
			}
		}

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

void GLFWApp::errorCallback(GLint error, const GLchar* errorStr)
{
	std::cout << "GLFW Error: " << error << errorStr << std::endl;
}

void GLFWApp::keyCallback(GLFWwindow* window, GLint key, GLint scancode, GLint action, GLint mods)
{
	//Cam movement
	if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT))
		camMng->MoveForward(camSpeed);
	if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT))
		camMng->MoveForward(-camSpeed);
	if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT))
		camMng->MoveSideways(camSpeed);
	if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT))
		camMng->MoveSideways(-camSpeed);
	if (key == GLFW_KEY_E && (action == GLFW_PRESS || action == GLFW_REPEAT))
		camMng->MoveVertical(camSpeed);
	if (key == GLFW_KEY_Q && (action == GLFW_PRESS || action == GLFW_REPEAT))
		camMng->MoveVertical(-camSpeed);
	if (key == GLFW_KEY_F && action == GLFW_PRESS)
		doPath = !doPath;

	//Switches between different nodes
	switch (key)
	{
		case(GLFW_KEY_1):
			currentNode = 0;
			getPath = true;
			break;
		case(GLFW_KEY_2):
			currentNode = 1;
			getPath = true;
			break;
		case(GLFW_KEY_3):
			currentNode = 2;
			getPath = true;
			break;
		case(GLFW_KEY_4):
			currentNode = 3;
			getPath = true;
			break;
		case(GLFW_KEY_5):
			currentNode = 4;
			getPath = true;
			break;
		case(GLFW_KEY_6):
			currentNode = 5;
			getPath = true;
			break;
		case(GLFW_KEY_7):
			currentNode = 6;
			getPath = true;
			break;
		case(GLFW_KEY_8):
			currentNode = 7;
			getPath = true;
			break;
		case(GLFW_KEY_9):
			currentNode = 8;
			getPath = true;
			break;
		case(GLFW_KEY_0) :
			currentNode = 9;
			getPath = true;
			break;
		default:
			break;
	}
}