#include "AppClass.h"

const char* alienModelStr = R"AlienModel(8
00100000100
00010001000
00111111100
01101110110
11111111111
10111111101
10100000101
00011011000
)AlienModel";

void Application::InitVariables(void)
{
	////Change this to your name and email
	//m_sProgrammer = "Alberto Bobadilla - labigm@rit.edu";

	////Alberto needed this at this position for software recording.
	//m_pWindow->setPosition(sf::Vector2i(710, 0));

	std::stringstream myStream(alienModelStr);
	std::string str;

	int row = 0;
	int rowCount = -1;
	while (std::getline(myStream, str))
	{
		//std::cout << "Read line: " << str << std::endl;
		if (row == 0)
		{
			rowCount = std::stoi(str, nullptr, 10);
			row++;
			continue;
		}
		for (int col = 0; col < str.length(); col++)
		{
			if (str[col] == '1')
			{
				m_Cubes.push_back(MyModel());
				m_Cubes.back().position = vector3(col, rowCount - row, 0);
				m_Cubes.back().mesh.GenerateCube(1.0f, C_BLACK);

				//std::cout << "\tCreating cube at (" << m_Cubes.back().position.x
				//	<< ", " << m_Cubes.back().position.y  << ")" << std::endl;
			}
		}
		row++;
	}

	std::cout << "Cubes generated: " << m_Cubes.size() << std::endl;
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

	static vector3 position(0, 0, 0);
	static bool moveLeft = true;

	static const vector3 delta(0.15, 0.0, 0.0);

	if (moveLeft)
	{
		position -= delta;
	}
	else
	{
		position += delta;
	}

	if (position.x < -10.0f || position.x > 0.0f)
	{
		moveLeft = !moveLeft;
	}

	matrix4 objectMat = glm::translate(IDENTITY_M4, position);

	for (int i = 0; i < m_Cubes.size(); i++) {
		m_Cubes[i].mesh.Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), glm::translate(objectMat, m_Cubes[i].position));
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
	//release GUI
	ShutdownGUI();
}