#include "MyMesh.h"
void MyMesh::Init(void)
{
	m_bBinded = false;
	m_uVertexCount = 0;

	m_VAO = 0;
	m_VBO = 0;

	m_pShaderMngr = ShaderManager::GetInstance();
}
void MyMesh::Release(void)
{
	m_pShaderMngr = nullptr;

	if (m_VBO > 0)
		glDeleteBuffers(1, &m_VBO);

	if (m_VAO > 0)
		glDeleteVertexArrays(1, &m_VAO);

	m_lVertex.clear();
	m_lVertexPos.clear();
	m_lVertexCol.clear();
}
MyMesh::MyMesh()
{
	Init();
}
MyMesh::~MyMesh() { Release(); }
MyMesh::MyMesh(MyMesh& other)
{
	m_bBinded = other.m_bBinded;

	m_pShaderMngr = other.m_pShaderMngr;

	m_uVertexCount = other.m_uVertexCount;

	m_VAO = other.m_VAO;
	m_VBO = other.m_VBO;
}
MyMesh& MyMesh::operator=(MyMesh& other)
{
	if (this != &other)
	{
		Release();
		Init();
		MyMesh temp(other);
		Swap(temp);
	}
	return *this;
}
void MyMesh::Swap(MyMesh& other)
{
	std::swap(m_bBinded, other.m_bBinded);
	std::swap(m_uVertexCount, other.m_uVertexCount);

	std::swap(m_VAO, other.m_VAO);
	std::swap(m_VBO, other.m_VBO);

	std::swap(m_lVertex, other.m_lVertex);
	std::swap(m_lVertexPos, other.m_lVertexPos);
	std::swap(m_lVertexCol, other.m_lVertexCol);

	std::swap(m_pShaderMngr, other.m_pShaderMngr);
}
void MyMesh::CompleteMesh(vector3 a_v3Color)
{
	uint uColorCount = m_lVertexCol.size();
	for (uint i = uColorCount; i < m_uVertexCount; ++i)
	{
		m_lVertexCol.push_back(a_v3Color);
	}
}
void MyMesh::AddVertexPosition(vector3 a_v3Input)
{
	m_lVertexPos.push_back(a_v3Input);
	m_uVertexCount = m_lVertexPos.size();
}
void MyMesh::AddVertexColor(vector3 a_v3Input)
{
	m_lVertexCol.push_back(a_v3Input);
}
void MyMesh::CompileOpenGL3X(void)
{
	if (m_bBinded)
		return;

	if (m_uVertexCount == 0)
		return;

	CompleteMesh();

	for (uint i = 0; i < m_uVertexCount; i++)
	{
		//Position
		m_lVertex.push_back(m_lVertexPos[i]);
		//Color
		m_lVertex.push_back(m_lVertexCol[i]);
	}
	glGenVertexArrays(1, &m_VAO);//Generate vertex array object
	glGenBuffers(1, &m_VBO);//Generate Vertex Buffered Object

	glBindVertexArray(m_VAO);//Bind the VAO
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);//Bind the VBO
	glBufferData(GL_ARRAY_BUFFER, m_uVertexCount * 2 * sizeof(vector3), &m_lVertex[0], GL_STATIC_DRAW);//Generate space for the VBO

	// Position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)0);

	// Color attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)(1 * sizeof(vector3)));

	m_bBinded = true;

	glBindVertexArray(0); // Unbind VAO
}
void MyMesh::Render(matrix4 a_mProjection, matrix4 a_mView, matrix4 a_mModel)
{
	// Use the buffer and shader
	GLuint nShader = m_pShaderMngr->GetShaderID("Basic");
	glUseProgram(nShader); 

	//Bind the VAO of this object
	glBindVertexArray(m_VAO);

	// Get the GPU variables by their name and hook them to CPU variables
	GLuint MVP = glGetUniformLocation(nShader, "MVP");
	GLuint wire = glGetUniformLocation(nShader, "wire");

	//Final Projection of the Camera
	matrix4 m4MVP = a_mProjection * a_mView * a_mModel;
	glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(m4MVP));
	
	//Solid
	glUniform3f(wire, -1.0f, -1.0f, -1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);  

	//Wire
	glUniform3f(wire, 1.0f, 0.0f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_POLYGON_OFFSET_LINE);
	glPolygonOffset(-1.f, -1.f);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);
	glDisable(GL_POLYGON_OFFSET_LINE);

	glBindVertexArray(0);// Unbind VAO so it does not get in the way of other objects
}
void MyMesh::AddTri(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft)
{
	//C
	//| \
	//A--B
	//This will make the triangle A->B->C 
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);
}
void MyMesh::AddQuad(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft, vector3 a_vTopRight)
{
	//C--D
	//|  |
	//A--B
	//This will make the triangle A->B->C and then the triangle C->B->D
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);

	AddVertexPosition(a_vTopLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopRight);
}
void MyMesh::GenerateCube(float a_fSize, vector3 a_v3Color)
{
	if (a_fSize < 0.01f)
		a_fSize = 0.01f;

	Release();
	Init();

	float fValue = a_fSize * 0.5f;
	//3--2
	//|  |
	//0--1

	vector3 point0(-fValue,-fValue, fValue); //0
	vector3 point1( fValue,-fValue, fValue); //1
	vector3 point2( fValue, fValue, fValue); //2
	vector3 point3(-fValue, fValue, fValue); //3

	vector3 point4(-fValue,-fValue,-fValue); //4
	vector3 point5( fValue,-fValue,-fValue); //5
	vector3 point6( fValue, fValue,-fValue); //6
	vector3 point7(-fValue, fValue,-fValue); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCuboid(vector3 a_v3Dimensions, vector3 a_v3Color)
{
	Release();
	Init();

	vector3 v3Value = a_v3Dimensions * 0.5f;
	//3--2
	//|  |
	//0--1
	vector3 point0(-v3Value.x, -v3Value.y, v3Value.z); //0
	vector3 point1(v3Value.x, -v3Value.y, v3Value.z); //1
	vector3 point2(v3Value.x, v3Value.y, v3Value.z); //2
	vector3 point3(-v3Value.x, v3Value.y, v3Value.z); //3

	vector3 point4(-v3Value.x, -v3Value.y, -v3Value.z); //4
	vector3 point5(v3Value.x, -v3Value.y, -v3Value.z); //5
	vector3 point6(v3Value.x, v3Value.y, -v3Value.z); //6
	vector3 point7(-v3Value.x, v3Value.y, -v3Value.z); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCone(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	int heightDivisions = 1;

	Release();
	Init();

	// Replace this with your code
	std::vector<vector3> vertexDataCyl(a_nSubdivisions * 2);
	const float TWO_PI = 2 * PI;
	// build the bottom circle
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		float angle = (float)i / a_nSubdivisions * TWO_PI;
		vertexDataCyl[i] = vector3(a_fRadius * sin(angle), -a_fHeight/2.0f, a_fRadius * cos(angle));
	}

	// basically does the same thing as the cyclinder code, except
	// the radius gradually decreases as the quad strip gets closer to
	// the tip of the cone
	//
	// finally the topmost row of vertices is connected to the tip
	// of the cone to form a strip of singular triangles
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		vector3 botLeft(vertexDataCyl[i].x, vertexDataCyl[i].y, vertexDataCyl[i].z);
		vector3 botRight(vertexDataCyl[(i + 1) % a_nSubdivisions].x, vertexDataCyl[(i + 1) % a_nSubdivisions].y, vertexDataCyl[(i + 1) % a_nSubdivisions].z);

		// draw bottom
		AddTri(botRight, botLeft, vector3(0.0f, -a_fHeight/2.0f, 0.0f));

		//draw top
		AddTri(botLeft, botRight, vector3(0.0, a_fHeight/2.0f, 0.0));
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCylinder(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Replace this with your code
	GenerateCylinderGeometry(a_fRadius, a_fHeight, a_nSubdivisions);
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
std::vector<vector3> MyMesh::GenerateCylinderGeometry(float a_fRadius, float a_fHeight, int a_nSubdivisions, bool drawCaps, bool ccw)
{
	std::vector<vector3> vertexDataCyl(a_nSubdivisions * 2);
	const float TWO_PI = 2 * PI;
	// build the bottom circle
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		float angle = (float)i / a_nSubdivisions * TWO_PI;
		vertexDataCyl[i] = vector3(a_fRadius * sin(angle), -a_fHeight / 2.0f, a_fRadius * cos(angle));
	}

	vector3 verticalOffset(0.0f, a_fHeight, 0.0f);

	// generate the positions around the edge of the cyclinder
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		vector3 botLeft = vertexDataCyl[i];
		vector3 botRight = vertexDataCyl[(i + 1) % a_nSubdivisions];

		if (ccw) 
		{
			if (drawCaps) 
			{
				// draw bottom
				AddTri(botRight, botLeft, vector3(0.0f, -a_fHeight / 2.0f, 0.0f));
				// draw top
				AddTri(botLeft + verticalOffset, botRight + verticalOffset, vector3(0.0f, -a_fHeight / 2.0f, 0.0f) + verticalOffset);
			}
			//draw middle
			AddTri(botLeft, botRight, botRight + verticalOffset);
			AddTri(botLeft + verticalOffset, botLeft, botRight + verticalOffset);
		}
		else 
		{
			if (drawCaps)
			{
				// draw bottom
				AddTri(botLeft, botRight, vector3(0.0f, -a_fHeight / 2.0f, 0.0f));
				// draw top
				AddTri(botRight + verticalOffset, botLeft + verticalOffset, vector3(0.0f, -a_fHeight / 2.0f, 0.0f) + verticalOffset);
			}
			//draw middle
			AddTri(botRight, botLeft, botRight + verticalOffset);
			AddTri(botLeft, botLeft + verticalOffset, botRight + verticalOffset);
		}
	}

	return vertexDataCyl;
}
void MyMesh::GenerateTube(float a_fOuterRadius, float a_fInnerRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Replace this with your code
	// a tube is just two cylinders connected slight differently at the top + bottom
	std::vector<vector3> outerCircle = GenerateCylinderGeometry(a_fOuterRadius, a_fHeight, a_nSubdivisions, false, true);
	std::vector<vector3> innerCircle = GenerateCylinderGeometry(a_fInnerRadius, a_fHeight, a_nSubdivisions, false, false);

	vector3 verticalOffset(0.0f, a_fHeight, 0.0f);

	for (int i = 0; i < a_nSubdivisions; i++)
	{
		vector3 botLeft(outerCircle[i].x, outerCircle[i].y, outerCircle[i].z);
		vector3 botRight(outerCircle[(i + 1) % a_nSubdivisions].x, outerCircle[(i + 1) % a_nSubdivisions].y, outerCircle[(i + 1) % a_nSubdivisions].z);
		vector3 topLeft(innerCircle[i].x, innerCircle[i].y, innerCircle[i].z);
		vector3 topRight(innerCircle[(i + 1) % a_nSubdivisions].x, innerCircle[(i + 1) % a_nSubdivisions].y, innerCircle[(i + 1) % a_nSubdivisions].z);

		// draw bottom
		AddTri(botRight, botLeft, topLeft);
		AddTri(topLeft, topRight, botRight);

		//draw top
		AddTri(botLeft + verticalOffset, botRight + verticalOffset, topLeft + verticalOffset);
		AddTri(topRight + verticalOffset, topLeft + verticalOffset, botRight + verticalOffset);
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTorus(float a_fOuterRadius, float a_fInnerRadius, int a_nSubdivisionsHeight, int a_nSubdivisionsAxis, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_nSubdivisionsHeight < 3)
		a_nSubdivisionsHeight = 3;
	if (a_nSubdivisionsHeight > 360)
		a_nSubdivisionsHeight = 360;

	if (a_nSubdivisionsAxis < 3)
		a_nSubdivisionsAxis = 3;
	if (a_nSubdivisionsAxis > 360)
		a_nSubdivisionsAxis = 360;

	Release();
	Init();

	// Replace this with your code
	// followed math available at http://paulbourke.net/geometry/torus/
	// a torus is a series of connected cylinders, whereas the ring is the base of a tube
	// think of it as one vertical circle at distance from a horizontal circle with radius r
	float r1 = (a_fOuterRadius - a_fInnerRadius) / 2.0f;
	float r0 = a_fInnerRadius + r1;

	std::vector<vector3> vertexData = std::vector<vector3>();
	const float TWO_PI = 2 * PI;
	for (int i = 0; i < a_nSubdivisionsAxis; i++)
	{
		float thetaCur = (float)i / a_nSubdivisionsAxis * TWO_PI;
		float thetaNext = (float)(i+1) / a_nSubdivisionsAxis * TWO_PI;

		for (int j = 0; j < a_nSubdivisionsHeight; j++) {
			float phiCur = (float)j / a_nSubdivisionsHeight * TWO_PI;
			float phiNext = (float)(j+1) / a_nSubdivisionsHeight * TWO_PI;

			vector3 curLeft(cos(thetaCur) * (r0 + r1 * cos(phiCur)), sin(thetaCur) * (r0 + r1 * cos(phiCur)), r1 * sin(phiCur));
			vector3 curRight(cos(thetaCur) * (r0 + r1 * cos(phiNext)), sin(thetaCur) * (r0 + r1 * cos(phiNext)), r1 * sin(phiNext));
			
			vector3 nextLeft(cos(thetaNext) * (r0 + r1 * cos(phiCur)), sin(thetaNext) * (r0 + r1 * cos(phiCur)), r1 * sin(phiCur));
			vector3 nextRight(cos(thetaNext) * (r0 + r1 * cos(phiNext)), sin(thetaNext) * (r0 + r1 * cos(phiNext)), r1 * sin(phiNext));

			AddTri(curRight, curLeft, nextRight);
			AddTri(curLeft, nextLeft, nextRight);
		}
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateSphere(float a_fRadius, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	//Sets minimum and maximum of subdivisions
	if (a_nSubdivisions < 3)
	{
		GenerateCube(a_fRadius * 2.0f, a_v3Color);
		return;
	}

	Release();
	Init();

	// Replace this with your code

	int	slices = a_nSubdivisions;
	int	stacks = a_nSubdivisions;
	float radius = a_fRadius;

	float delta, phi;
	const float TWO_PI = 2 * PI;
	int numVertices = slices * stacks;
	std::vector<std::vector<vector3>> vertexData(stacks - 1, std::vector<vector3>(slices));

	// compute all the vertices along the surface of the sphere
	for (int j = 0; j < stacks - 1; j++)
	{
		phi = ((float)(j + 1) / stacks) * PI;
		for (int i = 0; i < slices; i++)
		{
			delta = ((float)i / slices) * TWO_PI;
			float x = radius * cos(delta) * sin(phi);
			float y = radius * sin(delta) * sin(phi);
			float z = radius * cos(phi);
			vertexData[j][i] = vector3( x, y, z );
		}
	}

	// contruct the quad strips for each stack of the sphere
	// except for the first and last stacks
	for (int j = 0; j < stacks - 2; j++)
	{
		for (int i = 0; i < slices; i++)
		{
			vector3 botLeft = vertexData[j][i%slices];
			vector3 topRight = vertexData[j + 1][(i + 1) % slices];
			vector3 topLeft = vertexData[j + 1][i%slices];
			vector3 botRight = vertexData[j][(i + 1) % slices];

			AddTri(botLeft, topLeft, topRight);
			AddTri(botLeft, topRight, botRight);
		}
	}

	// connect the top row of quads to the tippy toppiest of the sphere,
	// then connect the bottom row of quads to the bottiest bottom of the sphere,
	// by creating a strip of triangles
	for (int i = 0; i < slices; i++)
	{
		vector3 botLeft = vertexData[0][i%slices];
		vector3 botRight = vertexData[0][(i + 1) % slices];
		vector3 topRight = vertexData[stacks - 2][(i + 1) % slices];
		vector3 topLeft = vertexData[stacks - 2][i%slices];

		AddTri(vector3( 0.0, 0.0, -radius ), topRight, topLeft);
		AddTri(vector3(0.0, 0.0, radius ), botLeft, botRight);
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}