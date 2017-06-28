#include "AppClass.h"
void Application::InitVariables(void)
{
	//Change this to your name and email
	m_sProgrammer = "Giovanni Aleman - ga9494@rit.edu";

	//Alberto needed this at this position for software recording.
	m_pWindow->setPosition(sf::Vector2i(710, 0));
	
	//Set the position and target of the camera
	//(I'm at [0,0,10], looking at [0,0,0] and up is the positive Y axis)
	m_pCameraMngr->SetPositionTargetAndView(AXIS_Z * 20.0f, ZERO_V3, AXIS_Y);

	//if the light position is zero move it
	if (m_pLightMngr->GetPosition(1) == ZERO_V3)
		m_pLightMngr->SetPosition(vector3(0.0f, 0.0f, 3.0f));

	//if the background is cornflowerblue change it to black (its easier to see)
	if (vector3(m_v4ClearColor) == C_BLUE_CORNFLOWER)
	{
		m_v4ClearColor = vector4(ZERO_V3, 1.0f);
	}
	
	//if there are no segments create 7
	if(m_uOrbits < 1)
		m_uOrbits = 7;

	float fSize = 1.0f; //initial size of orbits

	//creating a color using the spectrum 
	uint uColor = 650; //650 is Red
	//prevent division by 0
	float decrements = 250.0f / (m_uOrbits > 1? static_cast<float>(m_uOrbits - 1) : 1.0f); //decrement until you get to 400 (which is violet)
	/*
		This part will create the orbits, it start at 3 because that is the minimum subdivisions a torus can have
	*/
	uint uSides = 3; //start with the minimal 3 sides
	for (uint i = uSides; i < m_uOrbits + uSides; i++)
	{
		vector3 v3Color = WaveLengthToRGB(uColor); //calculate color based on wavelength
		m_shapeList.push_back(m_pMeshMngr->GenerateTorus(fSize, fSize - 0.1f, 3, i, v3Color)); //generate a custom torus and add it to the meshmanager
		fSize += 0.5f; //increment the size for the next orbit
		uColor -= static_cast<uint>(decrements); //decrease the wavelength
		oAngleAmount.push_back(360.0f / (GLfloat)i);
	}

	//Positions the spheres at the proper starting positiona. w for these two vars is the amount of divisions of the torus
	vector2 startVec(1, 0); // Temporary starting vector increased by 0.5f.
	std::vector<vector2> oOffsetVecs; //Vectors of vec4's used to get the modified angle coords
	for (GLuint i = 0; i < m_uOrbits; i++)
	{ 
		startVec.y = (GLfloat)i + 3.0f;
		oOffsetVecs.push_back(startVec);
		startVec.x += 0.5f;
	}

	//This calculates the proper position for each of the stops based on the previously calc'd angles.
	for (GLuint i = 0; i < m_uOrbits; i++)
	{
		std::vector<vector3> tmp;
		for (GLuint k = 0; k < oOffsetVecs[i].y; k++)
		{	//Set the x and y to the cos and sin of the angle times k, plus the offset
			tmp.push_back(vector3(cos(glm::radians(oAngleAmount[i] * k)) * oOffsetVecs[i].x,
								  sin(glm::radians(oAngleAmount[i] * k)) * oOffsetVecs[i].x, 0) );
		}
		counter.push_back(0); //Set the default count position to 0
		stops.push_back(tmp);
	}
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	matrix4 m4View = m_pCameraMngr->GetViewMatrix(); //view Matrix
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix(); //Projection Matrix
	matrix4 m4Offset = IDENTITY_M4; //offset of the orbits, starts as the global coordinate system
	/*
		The following offset will orient the orbits as in the demo, start without it to make your life easier.
	*/
	m4Offset = glm::rotate(IDENTITY_M4, 90.0f, AXIS_Z);

	//Time stuff
	static float fTimer = 0;	//store the new timer
	static uint uClock = m_pSystem->GenClock(); //generate a new clock for that timer
	fTimer += m_pSystem->GetDeltaTime(uClock); //get the delta time for that timer

	GLfloat per = MapValue(fTimer, 0.0f, 1.0f, 0.0f, 1.0f);

	// draw a shapes
	for (uint i = 0; i < m_uOrbits; ++i)
	{
		matrix4 m4Model;
		m_pMeshMngr->AddMeshToRenderList(m_shapeList[i], glm::rotate(m4Offset, 90.0f, AXIS_X));
		//Get the correct position from the stops vector and lerp them
		//In cases of counter[i] being the last num in the std::vec, lerp the last and first nums
		if(counter[i] < stops[i].size() - 1)
			m4Model = glm::translate(m4Offset, glm::lerp(stops[i][counter[i]], stops[i][counter[i] + 1], per));
		else
			m4Model = glm::translate(m4Offset, glm::lerp(stops[i][stops[i].size() - 1], stops[i][0], per));

		//Check if the timer should be reset and mod the counter[i]
		if (per >= 1.0f)
		{
			counter[i]++;
			counter[i] %= stops[i].size();
			fTimer = 0;
		}
		//draw spheres
		m_pMeshMngr->AddSphereToRenderList(m4Model * glm::scale(vector3(0.1)), C_WHITE);
	}

	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();
	
	//draw gui
	DrawGUI();
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	//release GUI
	ShutdownGUI();
}