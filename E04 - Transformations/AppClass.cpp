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


	// Initialize the cube model we will be reusing
	m_cube = new MyMesh();
	m_cube->GenerateCube(1.0f, C_BLACK);


	// Initialize a stream and string to read in the alien
	std::stringstream myStream(alienModelStr);
	std::string str;

	int row = 0;
	int rowCount = -1;
	// Read in the model string, for every '1', create a block
	// position at it's appropriate world position
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
				m_positions.push_back(vector3((float)col, (float)(rowCount - row), 0.0f));
				//std::cout << "\tCreating cube at (" << m_Cubes.back().position.x
				//	<< ", " << m_Cubes.back().position.y  << ")" << std::endl;
			}
		}
		row++;
	}

	std::cout << "\nCubes generated: " << m_positions.size() << std::endl;
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

	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();

	// position tracks the position of the entire alien as a whole
	static vector3 position(0, 0, 0);
	// tracks whether we are moving the alien right or left
	static bool moveLeft = true;

	// how far we should move the alien each frame
	static const vector3 delta(0.15, 0.0, 0.0);

	// add/remove the delta depending on direction we are moving
	if (moveLeft)
	{
		position -= delta;
	}
	else
	{
		position += delta;
	}

	// change direction we are moving after we hit the boundaries
	if (position.x < -10.0f || position.x > 0.0f)
	{
		moveLeft = !moveLeft;
	}

	// objectMat tracks the model matrix of the entire alien
	matrix4 objectMat = glm::translate(IDENTITY_M4, position);
	matrix4 modelMatrix;

	for (int i = 0; i < m_positions.size(); i++) {
		// render each block at its appropriate position, reusing the mesh object
		modelMatrix = glm::translate(objectMat, m_positions[i]);
		m_cube->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), modelMatrix);
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
	SafeDelete(m_cube);

	//release GUI
	ShutdownGUI();
}