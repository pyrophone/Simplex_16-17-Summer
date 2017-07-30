#include "AppClass.h"
using namespace Simplex;
void Application::InitVariables(void)
{
	m_sProgrammer = "Giovanni Aleman - ga9494@rit.edu";
	//Set the position and target of the camera
	m_pCameraMngr->SetPositionTargetAndUp(
		vector3(0.0f, 0.0f, 100.0f), //Position
		vector3(0.0f, 0.0f, 99.0f),	//Target
		AXIS_Y);					//Up

	m_pLightMngr->SetPosition(vector3(0.0f, 3.0f, 13.0f), 1); //set the position of first light (0 is reserved for ambient light)

#ifdef DEBUG
	uint uInstances = 900;
#else
	uint uInstances = 1849;
#endif
	int nSquare = static_cast<int>(std::sqrt(uInstances));
	m_uObjects = nSquare * nSquare;
	uint uIndex = -1;
	for (int i = 0; i < nSquare; i++)
	{
		for (int j = 0; j < nSquare; j++)
		{
			uIndex++;
			m_pEntityMngr->AddEntity("Minecraft\\Cube.obj");
			vector3 v3Position = vector3(glm::sphericalRand(34.0f));
			matrix4 m4Position = glm::translate(v3Position);
			m_pEntityMngr->SetModelMatrix(m4Position);
		}
	}
	m_uOctantLevels = 1;
	m_pEntityMngr->Update();
	m_pRoot = new MyOctant(1, glm::vec3(70, 70, 70));
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the ArcBall active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();
	
	//Update Entity Manager
	m_pEntityMngr->Update();

	static std::vector<Node*> tmp; //tmp vector of nodes

	//If the prev octant level and the current are different, update the leaf nodes;
	if (m_uOctantLevelsPrev != m_uOctantLevels)
		tmp = m_pRoot->getNodeLeafs();

	//Uses all the node in tmp
	if (m_uOctantID > m_pRoot->getOctantCount())
	{
		for (GLuint i = 0; i < tmp.size(); i++)
		{
			m_pMeshMngr->AddWireCubeToRenderList(tmp[i]->getTransMat(), C_YELLOW); //Render each node

			//If the octant levels are the same, skip this
			if (m_uOctantLevelsPrev != m_uOctantLevels)
			{
				//Check if each entity is in the current node, and if it is, add that dimension
				for (GLuint j = 0; j < m_pEntityMngr->GetEntityCount(); j++)
				{
					static glm::vec3 octMax;
					static glm::vec3 octMin;
					static glm::vec3 entMax;
					static glm::vec3 entMin;

					//temp vec3's to clean up if statements
					octMax =  tmp[i]->getSize() + tmp[i]->getCenter() * 2.0f;
					octMin = -tmp[i]->getSize() + tmp[i]->getCenter() * 2.0f;
					entMax = m_pEntityMngr->GetRigidBody(j)->GetMaxGlobal();
					entMin = m_pEntityMngr->GetRigidBody(j)->GetMinGlobal();

					//Check x, y, and z to see if the entity is in the node
					if (entMax.x < octMin.x)
						continue;
					if (entMin.x > octMax.x)
						continue;

					if (entMax.y < octMin.y)
						continue;
					if (entMin.y > octMax.y)
						continue;

					if (entMax.z < octMin.z)
						continue;
					if (entMin.z > octMax.z)
						continue;

					//Adds the dimension to the entity
					m_pEntityMngr->AddDimension(j, i);
				}
			}
		}
		m_uOctantLevelsPrev = m_uOctantLevels; //Update the octant levels
	}

	//Uses the a specific node in tmp
	else
	{
		//Render the specific node
		m_pMeshMngr->AddWireCubeToRenderList(tmp[m_uOctantID]->getTransMat(), C_YELLOW);
		
		//Check if each entity is in the current node, and if it is, add that dimension
		for (GLuint i = 0; i < m_pEntityMngr->GetEntityCount(); i++)
		{
			//temp vec3's to clean up if statements
			glm::vec3 octMax = tmp[m_uOctantID]->getSize() + tmp[m_uOctantID]->getCenter() * 2.0f;
			glm::vec3 octMin = -tmp[m_uOctantID]->getSize() + tmp[m_uOctantID]->getCenter() * 2.0f;
			glm::vec3 entMax = m_pEntityMngr->GetRigidBody(i)->GetMaxGlobal();
			glm::vec3 entMin = m_pEntityMngr->GetRigidBody(i)->GetMinGlobal();

			//Check x, y, and z to see if the entity is in the node
			if (entMax.x < octMin.x)
				continue;
			if (entMin.x > octMax.x)
				continue;

			if (entMax.y < octMin.y)
				continue;
			if (entMin.y > octMax.y)
				continue;

			if (entMax.z < octMin.z)
				continue;
			if (entMin.z > octMax.z)
				continue;

			//Adds the dimension to the entity
			m_pEntityMngr->AddDimension(i, m_uOctantID);
		}
	}

	//Add objects to render list
	m_pEntityMngr->AddEntityToRenderList(-1, true);
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();
	
	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();
	
	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();
	
	//draw gui,
	DrawGUI();
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	//release GUI
	ShutdownGUI();
}