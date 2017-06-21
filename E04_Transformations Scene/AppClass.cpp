#include "AppClass.h"
void Application::InitVariables(void)
{
	//Change this to your name and email
	m_sProgrammer = "Giovanni Aleman - ga9494@rit.edu";

	//Fill the vector with meshe
	for (GLuint i = 0; i < 46; i++)
	{
		MyMesh* mesh = new MyMesh();
		meshes.push_back(mesh);
		meshes[i]->GenerateCube(1.0f, C_BLACK);
	}
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	//ArcBall();

	//Is the first person camera active?
	CameraRotation();
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	matrix4 m4View = m_pCameraMngr->GetViewMatrix();
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix();
	
	matrix4 m4Scale = glm::scale(1.0f, 1.0f, 1.0f);
	static float value = 0.0f;
	value += 0.01f;

	GLfloat x = 0, y = 0; //Temp x and y for the models
	const GLfloat z = -1.0f; //Cubes are always at -1

	//Loop for each height layer of the sprite
	for (GLuint i = 0; i < 8; i++)
	{
		//Switch for each number
		switch (i)
		{
			case 0: //First layer
			{
				y = 1.0f;
				x = 0;

				//Position 2 cubes, switch sides, position 2 more
				for (GLuint j = 0; j < 4; j++)
				{
					x++;
					meshes[j]->Render(m4Projection, m4View, glm::translate(x + value, y, z) * m4Scale);

					if (x == 2.0f)
						x = -3.0f;
				}

				break;
			}

			case 1: //Layer 2
			{
				y = 2.0f;
				x = 3.0f;

				//Loop while increasing by to. if the number is greater than 5, make it negative 5
				for (GLuint j = 4; j < 8; j++)
				{
					meshes[j]->Render(m4Projection, m4View, glm::translate(x + value, y, z) * m4Scale);
					x += 2.0f;

					if (x > 5.0f)
						x = -5.0f;

				}

				break;
			}

			case 2: //Layer 3
			{
				y = 3.0f;
				x = -3.0f;

				//Main body loop. Positions 7 cubes in a row
				for (GLuint j = 8; j < 15; j++)
				{
					meshes[j]->Render(m4Projection, m4View, glm::translate(x + value, y, z) * m4Scale);
					x++;
				}

				//Position the 2 arms by checking for even vs odd
				for (GLuint j = 15; j < 17; j++)
				{
					if (!(j % 2))
						x = 5.0f;
					else
						x = -5.0f;
					meshes[j]->Render(m4Projection, m4View, glm::translate(x + value, y, z) * m4Scale);
				}

				break;
			}

			case 3: //Layer 4
			{
				y = 4.0f;
				x = -5.0f;

				//This layer is just a line, so position, then add 1 to x
				for (GLuint j = 17; j < 28; j++)
				{
					meshes[j]->Render(m4Projection, m4View, glm::translate(x + value, y, z) * m4Scale);
					x++;
				}

				break;
			}

			case 4: //Layer 5
			{
				y = 5.0f;
				x = -4.0f;

				//Loop and check if the x value is the absolute value of two. If it is, skip that number by adding one
				for (GLuint j = 28; j < 35; j++)
				{
					meshes[j]->Render(m4Projection, m4View, glm::translate(x + value, y, z) * m4Scale);
					x++;

					if (abs(x) == 2)
						x++;
				}

				break;
			}

			case 5: //Layer 6
			{
				y = 6.0f;
				x = -3.0f;

				//Another line, so position, then and add one
				for (GLuint j = 35; j < 42; j++)
				{
					meshes[j]->Render(m4Projection, m4View, glm::translate(x + value, y, z) * m4Scale);
					x++;
				}

				break;
			}

			case 6: //Layer 7
			{
				y = 7.0f;
				x = -2.0f;

				//Position one, then flip and position the other
				for (GLuint j = 42; j < 44; j++)
				{
					meshes[j]->Render(m4Projection, m4View, glm::translate(x + value, y, z) * m4Scale);
					x = -x;
				}

				break;
			}

			case 7: //Final layer
			{
				y = 8.0f;
				x = 3.0f;

				//Same as before: Position, then flip, then position
				for (GLuint j = 44; j < 46; j++)
				{
					meshes[j]->Render(m4Projection, m4View, glm::translate(x + value, y, z) * m4Scale);
					x = -x;
				}

				break;
			}

			//Not used
			default:
				y = 0.0f;
		}
	}


	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();
	
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
	for (auto i : meshes)
	{
		SafeDelete(i);
	}

	//release GUI
	ShutdownGUI();
}