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
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);

	glBindVertexArray(0);// Unbind VAO so it does not get in the way of other objects
}
void MyMesh::AddTri(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft)
{
	//C
	//| \
	//A--B
	//This will make the triang A->B->C 
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);
}
void MyMesh::AddQuad(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft, vector3 a_vTopRight)
{
	//C--D
	//|  |
	//A--B
	//This will make the triang A->B->C and then the triang C->B->D
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

	Release();
	Init();

	// Replace this with your code
	vector3 height(0, a_fHeight, 0); //Vector for the height
	
	//Temp Vectors to hold the points
	vector3 p1;
	vector3 p2;

	//Determines the basic angle that should be used for the cos and sin
	float rInc = 360.0f / a_nSubdivisions;

	//Loop for the amount of subdivisions
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		//For everything but the last number
		if (i < a_nSubdivisions - 1)
		{
			//Determine p1 and p2, add the sides
			p1 = vector3(a_fRadius * cos(glm::radians(rInc * i)), -a_fHeight, a_fRadius * sin(glm::radians(rInc * i)));
			p2 = vector3(a_fRadius * cos(glm::radians(rInc * (i + 1))), -a_fHeight, a_fRadius * sin(glm::radians(rInc * (i + 1))));
			AddTri(p1, height, p2); //Side Face
			AddTri(-height, p1, p2); //Base Face
		}

		//Last num is a special case
		else
		{
			//p2 is zero here, otherwise the mesh is off
			p1 = vector3(a_fRadius * cos(glm::radians(rInc * i)), -a_fHeight, a_fRadius * sin(glm::radians(rInc * i)));
			p2 = vector3(a_fRadius * cos(0), -a_fHeight, a_fRadius * sin(0));
			AddTri(p1, height, p2); //Side Face
			AddTri(-height, p1, p2); //Base Face
		}
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
	vector3 height(0, a_fHeight, 0);  //Vector for the height

	//Temp Vectors to hold the points
	vector3 p1;
	vector3 p2;
	vector3 p3;
	vector3 p4;

	//Determines the basic angle that should be used for the cos and sin
	float rInc = 360.0f / a_nSubdivisions;

	//Loop for the amount of subdivisions
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		//For everything but the last number
		if (i < a_nSubdivisions - 1)
		{
			//Determine p1, p2, p3, and p4, and add the sides
			p1 = vector3(a_fRadius * cos(glm::radians(rInc * i)), a_fHeight, a_fRadius * sin(glm::radians(rInc * i)));
			p2 = vector3(a_fRadius * cos(glm::radians(rInc * (i + 1))), a_fHeight, a_fRadius * sin(glm::radians(rInc * (i + 1))));
			p3 = vector3(a_fRadius * cos(glm::radians(rInc * i)), -a_fHeight, a_fRadius * sin(glm::radians(rInc * i)));
			p4 = vector3(a_fRadius * cos(glm::radians(rInc * (i + 1))), -a_fHeight, a_fRadius * sin(glm::radians(rInc * (i + 1))));
			AddTri(p1, height, p2); //Top Face
			AddQuad(p1, p2, p3, p4); //Side Face
			AddTri(-height, p3, p4); //Bottom Face
		}

		//Last num is a special case
		else
		{
			//p2 and p4 are zero here, for the same reason as before
			p1 = vector3(a_fRadius * cos(glm::radians(rInc * i)), a_fHeight, a_fRadius * sin(glm::radians(rInc * i)));
			p2 = vector3(a_fRadius * cos(0), a_fHeight, a_fRadius * sin(0));
			p3 = vector3(a_fRadius * cos(glm::radians(rInc * i)), -a_fHeight, a_fRadius * sin(glm::radians(rInc * i)));
			p4 = vector3(a_fRadius * cos(0), -a_fHeight, a_fRadius * sin(0));
			AddTri(p1, height, p2); //Top Face
			AddQuad(p1, p2, p3, p4); //Side Face
			AddTri(-height, p3, p4); //Bottom Face
		}
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
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
	vector3 height(0, a_fHeight, 0);

	//Outer Point Temp Vecs
	vector3 oP1;
	vector3 oP2;
	vector3 oP3;
	vector3 oP4;

	//Inner Point Temp Vecs
	vector3 iP1;
	vector3 iP2;
	vector3 iP3;
	vector3 iP4;

	float rInc = 360.0f / a_nSubdivisions;

	//Loop for the amount of subdivisions
	for (int i = 0; i < a_nSubdivisions; i++)
	{
		//For everything but the last number
		if (i < a_nSubdivisions - 1)
		{
			//Determine each of the points, and add the sides
			oP1 = vector3(a_fOuterRadius * cos(glm::radians(rInc * i)), a_fHeight, a_fOuterRadius * sin(glm::radians(rInc * i)));
			oP2 = vector3(a_fOuterRadius * cos(glm::radians(rInc * (i + 1))), a_fHeight, a_fOuterRadius * sin(glm::radians(rInc * (i + 1))));
			oP3 = vector3(a_fOuterRadius * cos(glm::radians(rInc * i)), -a_fHeight, a_fOuterRadius * sin(glm::radians(rInc * i)));
			oP4 = vector3(a_fOuterRadius * cos(glm::radians(rInc * (i + 1))), -a_fHeight, a_fOuterRadius * sin(glm::radians(rInc * (i + 1))));

			iP1 = vector3(a_fInnerRadius * cos(glm::radians(rInc * i)), a_fHeight, a_fInnerRadius* sin(glm::radians(rInc * i)));
			iP2 = vector3(a_fInnerRadius * cos(glm::radians(rInc * (i + 1))), a_fHeight, a_fInnerRadius * sin(glm::radians(rInc * (i + 1))));
			iP3 = vector3(a_fInnerRadius * cos(glm::radians(rInc * i)), -a_fHeight, a_fInnerRadius * sin(glm::radians(rInc * i)));
			iP4 = vector3(a_fInnerRadius * cos(glm::radians(rInc * (i + 1))), -a_fHeight, a_fInnerRadius * sin(glm::radians(rInc * (i + 1))));

			AddQuad(iP1, iP2, oP1, oP2); //Top Face
			AddQuad(oP1, oP2, oP3, oP4); //Outside Face
			AddQuad(oP3, oP4, iP3, iP4); //Bottom Face
			AddQuad(iP3, iP4, iP1, iP2); //Inside Face
		}

		//Last num is a special case
		else
		{
			//iP2, iP4, oP2, and oP4 are zero here, for the same reasons as before
			oP1 = vector3(a_fOuterRadius * cos(glm::radians(rInc * i)), a_fHeight, a_fOuterRadius * sin(glm::radians(rInc * i)));
			oP2 = vector3(a_fOuterRadius * cos(0), a_fHeight, a_fOuterRadius * sin(0));
			oP3 = vector3(a_fOuterRadius * cos(glm::radians(rInc * i)), -a_fHeight, a_fOuterRadius * sin(glm::radians(rInc * i)));
			oP4 = vector3(a_fOuterRadius * cos(0), -a_fHeight, a_fOuterRadius * sin(0));

			iP1 = vector3(a_fInnerRadius * cos(glm::radians(rInc * i)), a_fHeight, a_fInnerRadius* sin(glm::radians(rInc * i)));
			iP2 = vector3(a_fInnerRadius * cos(0), a_fHeight, a_fInnerRadius * sin(0));
			iP3 = vector3(a_fInnerRadius * cos(glm::radians(rInc * i)), -a_fHeight, a_fInnerRadius * sin(glm::radians(rInc * i)));
			iP4 = vector3(a_fInnerRadius * cos(0), -a_fHeight, a_fInnerRadius * sin(0));

			AddQuad(iP1, iP2, oP1, oP2); //Top Face
			AddQuad(oP1, oP2, oP3, oP4); //Outside Face
			AddQuad(oP3, oP4, iP3, iP4); //Bottom Face
			AddQuad(iP3, iP4, iP1, iP2); //Inside Face
		}
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTorus(float a_fOuterRadius, float a_fInnerRadius, int a_nSubdivisionsA, int a_nSubdivisionsB, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_nSubdivisionsA < 3)
		a_nSubdivisionsA = 3;
	if (a_nSubdivisionsA > 360)
		a_nSubdivisionsA = 360;

	if (a_nSubdivisionsB < 3)
		a_nSubdivisionsB = 3;
	if (a_nSubdivisionsB > 360)
		a_nSubdivisionsB = 360;

	Release();
	Init();

	// Replace this with your code
	vector3 p; //Temporary vector
	std::vector<vector3> points; //std::vector of the points

	const GLfloat twoPi = 2.0f * glm::pi<GLfloat>(); //Just two pi...

	//Loop for the amount of subdivisions A
	for (GLint i = 0; i < a_nSubdivisionsA; i++)
	{
		//Loop for the amount of subivision B
		for (GLint j = 0; j <= a_nSubdivisionsB; j++)
		{
			for (GLint k = 0; k <= 1; k++)
			{
				//Get a and b for the calculation
				GLfloat a = (i + k) % a_nSubdivisionsA + 0.5f;
				GLfloat b = (GLfloat)(j % a_nSubdivisionsB);

				//Determine the point and push it onto the vector
				p = vector3((a_fOuterRadius + a_fInnerRadius * cos(a * twoPi / a_nSubdivisionsA)) * cos(b * twoPi / a_nSubdivisionsB),
							 a_fInnerRadius * sin(a * twoPi / a_nSubdivisionsA),
							 (a_fOuterRadius + a_fInnerRadius * cos(a * twoPi / a_nSubdivisionsA)) * sin(b * twoPi / a_nSubdivisionsB));

				points.push_back(p);
			}
		}
	}

	//Loop for the amount of points in point vector minus two, and increase by twos
	for (GLuint i = 0; i < points.size() - 2; i += 2)
	{
		AddQuad(points[i], points[i + 1], points[i + 2], points[i + 3]);
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
	if (a_nSubdivisions < 1)
	{
		GenerateCube(a_fRadius * 2.0f, a_v3Color);
		return;
	}
	if (a_nSubdivisions > 6)
		a_nSubdivisions = 6;

	Release();
	Init();

	// Replace this with your code
	//Hold values used in initalizing icosahedron
	GLfloat t;
	GLfloat y;

	//Special case for the radius being one
	if (a_fRadius == 1)
	{
		t = sin(a_fRadius);
		y = cos(a_fRadius);
	}

	//Everything else should work with this
	else
	{
		t = cos(a_fRadius);
		y = sin(a_fRadius);
	}

	//Inital points
	std::vector<vector3> points = { glm::normalize(vector3(-y,  t,  0.0)),
									glm::normalize(vector3( y,  t,  0.0)),
									glm::normalize(vector3(-y, -t,  0.0)),
									glm::normalize(vector3( y, -t,  0.0)),
																		 
									glm::normalize(vector3(0.0, -y,  t)),
									glm::normalize(vector3(0.0,  y,  t)),
									glm::normalize(vector3(0.0, -y, -t)),
									glm::normalize(vector3(0.0,  y, -t)),
																		 
									glm::normalize(vector3( t,  0.0, -y)),
									glm::normalize(vector3( t,  0.0,  y)),
									glm::normalize(vector3(-t,  0.0, -y)),
									glm::normalize(vector3(-t,  0.0,  y)) };

	//initial indicies
	std::vector<GLint> indicies = {  0, 11,  5,  0,  5,  1,  0,  1,  7,  0,  7, 10,  0, 10, 11,
									 1,  5,  9,  5, 11,  4, 11, 10,  2, 10,  7,  6,  7,  1,  8,
									 3,  9,  4,  3,  4,  2,  3,  2,  6,  3,  6,  8,  3,  8,  9,
									 4,  9,  5,  2,  4, 11,  6,  2, 10,  8,  6,  7,  9,  8,  1 };

	//Temp Vector for midpoints
	vector3 t12; 
	vector3 t23;
	vector3 t31;
	
	//Loop for the amount of subdivisions
	for (GLint i = 0; i < a_nSubdivisions; i++)
	{
		std::vector<GLint> newIndicies; //Holds the new indicies

		//Loop for the amount of indicies but increase by 3 since 3 indicies are used for 1 tri
		for (GLuint j = 0; j <  indicies.size(); j += 3)
		{
			//Get the midpoints between each of the three vectors that make up
			t12 = (points[indicies[j]] + points[indicies[j + 1]]) / 2.0f;
			t23 = (points[indicies[j + 1]] + points[indicies[j + 2]]) / 2.0f;
			t31 = (points[indicies[j + 2]] + points[indicies[j]]) / 2.0f;

			//Normalize the midpoints
			t12 = glm::normalize(t12);
			t23 = glm::normalize(t23);
			t31 = glm::normalize(t31);

			//Add the midpoints to the list of points
			points.push_back(t12); //Size - 3
			points.push_back(t23); //Size - 2
			points.push_back(t31); //Size - 1

			//Push back the new indicies for the triangles
			newIndicies.push_back(indicies[j]);
			newIndicies.push_back(points.size() - 3);
			newIndicies.push_back(points.size() - 1);
			
			newIndicies.push_back(indicies[j + 1]);
			newIndicies.push_back(points.size() - 2);
			newIndicies.push_back(points.size() - 3);
			
			newIndicies.push_back(indicies[j + 2]);
			newIndicies.push_back(points.size() - 1);
			newIndicies.push_back(points.size() - 2);

			newIndicies.push_back(points.size() - 3);
			newIndicies.push_back(points.size() - 2);
			newIndicies.push_back(points.size() - 1);
		}
		indicies = newIndicies; //Set the indicies to the new indicies
	}

	//Add the triangles
	for (GLuint i = 0; i < indicies.size(); i += 3)
	{
		AddTri(points[indicies[i]], points[indicies[i + 1]], points[indicies[i + 2]]);
	}

	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}