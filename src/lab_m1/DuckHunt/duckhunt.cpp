#include "lab_m1/DuckHunt/duckhunt.h"

#include <vector>
#include <iostream>

#include "lab_m1/DuckHunt/transform2D.h"
#include "lab_m1/DuckHunt/object2D.h"

#define VERTICAL M_PI_2
#define DOWN M_PI
#define INITIAL_SPEED 150
#define INCREASE_SPEED 20
#define ESCAPE_SPEED 200
#define FALL_SPEED 300
#define LIVES_MARGIN 30
#define LIVES_DISTANCE 40
#define BULLETS_LEFT_MARGIN 20
#define BULLETS_UP_MARGIN 90
#define BULLETS_DISTANCE 40
#define CURRENTSCORE_START_RIGHT_MARGIN 225
#define CURRENTSCORE_UP_MARGIN 76
#define WAREFRAME_X_POSITION 230
#define WAREFRAME_Y_POSITION 80
#define DUCK_CENTER_RELATIVE_X 36
#define DUCK_CENTER_RELATIVE_Y 55 
#define DUCK_RIGHT_MARGIN 120
#define DUCK_UP_MARGIN 100
#define GRASS_HEIGHT 200
#define DUCK_OX_DIMM 100
#define DUCK_OY_DIMM 70
#define HEAD_RADIX 65
#define BEAK_RADIX 85
#define RIGHT_WING_RADIX 18
#define LEFT_WING_RADIX 22

using namespace std;
using namespace m1;

int numberOfLives = 3;
int numberOfBullets = 3;
int currentScore = 0;
int decreased = 0;
int pozitie = 0; 
int wingAngleSign = 1;
int level = 0;
int level_hit = 0;

float currentTime = 0;
float duckAngle = 0;
float wingAngle = 1.26;
float wingRotation = 0;
float duckNotDisplayedTime = 0;
float duckDisplayedTime = 0;

bool duckHit = false;
bool duckEscaped = false;

glm::vec2 duckBodyTranslate= glm::vec2(36,60);
glm::vec2 duckBeakTranslate= glm::vec2(0,0);
glm::vec2 duckWingTranslate= glm::vec2(0,5);
glm::vec2 duckPosition = glm::vec2(0, 0);
glm::vec2 directionSign = glm::vec2(1, 1);
glm::vec2 duckCenter;

DuckHunt::DuckHunt()
{

}

DuckHunt::~DuckHunt()
{

}

void DuckHunt::Init()
{
	glm::ivec2 resolution = window->GetResolution();
	auto camera = GetSceneCamera();
	camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
	camera->SetPosition(glm::vec3(0, 0, 50));
	camera->SetRotation(glm::vec3(0, 0, 0));
	camera->Update();
	GetCameraInput()->SetActive(false);

	glm::vec3 corner = glm::vec3(0, 0, 0);

	Mesh* grass = object2D::CreateRectangle("grass", corner, resolution.x, 200, glm::vec3(0.4, 0.8, 0), true);
	Mesh* life = object2D::CreateCircle("life", corner, 15, glm::vec3(1, 0, 0), true);
	Mesh* bullet = object2D::CreateRectangle("bullet", corner, 15, 30, glm::vec3(0, 0.4, 0), true);
	Mesh* wireframe = object2D::CreateRectangle("wireframe", corner, 210, 58, glm::vec3(0.11, 0.145, 0.298), true);
	Mesh* currentScoreBar = object2D::CreateRectangle("currentScoreBar", corner, 4, 50, glm::vec3(0.066, 0.4, 0.69), true);
	Mesh* duckBeak = object2D::CreateTriangle("duckBeak", corner, 20, 30, glm::vec3(1, 0.874, 0.227), true);
	Mesh* duckHead = object2D::CreateCircle("duckHead", corner, 30, glm::vec3(0, 0.6, 0.298), true);
	Mesh* duckBody = object2D::CreateTriangle("duckBody", corner, 72, 120, glm::vec3(0.4, 0.2, 0), true);
	Mesh* duckRightWing = object2D::CreateTriangle("duckRightWing", corner, 40, 50, glm::vec3(0.4, 0.2, 0), true);
	Mesh* duckLeftWing = object2D::CreateTriangle2("duckLeftWing", corner, 40, 50, glm::vec3(0.4, 0.2, 0), true);

	AddMeshToList(grass);
	AddMeshToList(life);
	AddMeshToList(bullet);
	AddMeshToList(wireframe);
	AddMeshToList(currentScoreBar);
	AddMeshToList(duckBeak);
	AddMeshToList(duckHead);
	AddMeshToList(duckBody);
	AddMeshToList(duckRightWing);
	AddMeshToList(duckLeftWing);

}

void DuckHunt::FrameStart()
{
	glClearColor(0.4, 0.7, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	glViewport(0, 0, resolution.x, resolution.y);
}

void DuckHunt::Update(float deltaTimeSeconds)
{
	glm::ivec2 resolution = window->GetResolution();

	modelMatrix = glm::mat3(1);
	modelMatrix *= transform2D::Translate(0, 0);
	RenderMesh2D(meshes["grass"], shaders["VertexColor"], modelMatrix);

	int i;

	// Scene elements
	for (int i = 0; i < numberOfLives; i++)
	{
		modelMatrix = glm::mat3(1);
		modelMatrix *= transform2D::Translate(LIVES_MARGIN + LIVES_DISTANCE * i, resolution.y - LIVES_MARGIN);
		RenderMesh2D(meshes["life"], shaders["VertexColor"], modelMatrix);
	}

	for (int i = 0; i < numberOfBullets; i++)
	{
		modelMatrix = glm::mat3(1);
		modelMatrix *= transform2D::Translate(BULLETS_LEFT_MARGIN + BULLETS_DISTANCE * i, resolution.y - BULLETS_UP_MARGIN);
		RenderMesh2D(meshes["bullet"], shaders["VertexColor"], modelMatrix);
	}

	for (i = 1; i <= currentScore; i++)
	{
		modelMatrix = glm::mat3(1);
		modelMatrix *= transform2D::Translate(resolution.x - CURRENTSCORE_START_RIGHT_MARGIN + (i - 1) * 4, resolution.y - CURRENTSCORE_UP_MARGIN);
		RenderMesh2D(meshes["currentScoreBar"], shaders["VertexColor"], modelMatrix);
	}

	modelMatrix = glm::mat3(1);
	modelMatrix *= transform2D::Translate(resolution.x - WAREFRAME_X_POSITION, resolution.y - WAREFRAME_Y_POSITION);
	RenderMesh2D(meshes["wireframe"], shaders["VertexColor"], modelMatrix);

	if (!duckHit && !duckEscaped && numberOfLives)
	{
		duckPosition.x += directionSign.x * deltaTimeSeconds * (INITIAL_SPEED + level * INCREASE_SPEED);
		duckPosition.y += directionSign.y * deltaTimeSeconds * (INITIAL_SPEED + level * INCREASE_SPEED);
		duckCenter = duckPosition + glm::vec2(DUCK_CENTER_RELATIVE_X, DUCK_CENTER_RELATIVE_Y);

		if (directionSign.x == 1)
			if (directionSign.y == 1)
				duckAngle = -1;
			else
				duckAngle = -2;
		if (directionSign.x == -1)
			if (directionSign.y == 1)
				duckAngle = 1;
			else
				duckAngle = 2;

		if (duckPosition.x + DUCK_RIGHT_MARGIN >= resolution.x)
			directionSign.x = -1;
		if (duckPosition.y + DUCK_UP_MARGIN >= resolution.y)
			directionSign.y = -1;

		if (duckPosition.x <= 0)
			directionSign.x = 1;
		if (duckPosition.y <= GRASS_HEIGHT)
			directionSign.y = 1;

		wingRotation += wingAngleSign * 2 * deltaTimeSeconds;
		if (wingRotation >= 1.2)
			wingAngleSign = -1;
		if (wingRotation <= 0)
			wingAngleSign = 1;

		if(duckCenter.y > GRASS_HEIGHT)
			duckDisplayedTime += deltaTimeSeconds;

		if (duckDisplayedTime >= 5 || numberOfBullets == 0)
		{
			duckEscaped = true;
			numberOfLives--;
		}
	}

	if(duckHit)
	{
		wingRotation = 0;
		duckAngle = DOWN;
		if(duckPosition.y >= 0)
			duckPosition.y += -1 * deltaTimeSeconds * (FALL_SPEED + level * INCREASE_SPEED);
		else
		{
			duckNotDisplayedTime += deltaTimeSeconds;
			if (duckNotDisplayedTime >= 1)
			{
				duckHit = false;
				numberOfBullets = 3;
				duckNotDisplayedTime = 0;
				duckDisplayedTime = 0;
			}
		}
		duckCenter = duckPosition + glm::vec2(DUCK_CENTER_RELATIVE_X, DUCK_CENTER_RELATIVE_Y);
	}

	if(duckEscaped)
	{
		duckAngle = 0;
		if (duckPosition.y < resolution.y)
		{
			duckPosition.y += deltaTimeSeconds * (ESCAPE_SPEED + level * INCREASE_SPEED);
			duckCenter = duckPosition + glm::vec2(DUCK_CENTER_RELATIVE_X, DUCK_CENTER_RELATIVE_Y);
			wingRotation += wingAngleSign * 2 * deltaTimeSeconds;
			if (wingRotation >= 1.2)
				wingAngleSign = -1;
			if (wingRotation <= 0)
				wingAngleSign = 1;
		}
		else
		{
			duckNotDisplayedTime += deltaTimeSeconds;
			if (duckNotDisplayedTime >= 1)
			{
				duckEscaped = false;
				if(numberOfLives)
					numberOfBullets = 3;
				duckNotDisplayedTime = 0;
				duckDisplayedTime = 0;
				duckPosition.y = 0;
			}
		}
	}

	// Duck elements
	modelMatrix = glm::mat3(1);
	modelMatrix *= transform2D::Translate(duckCenter.x + HEAD_RADIX * cos(VERTICAL + duckAngle), duckCenter.y + HEAD_RADIX * sin(VERTICAL + duckAngle));
	RenderMesh2D(meshes["duckHead"], shaders["VertexColor"], modelMatrix);

	modelMatrix = glm::mat3(1);
	modelMatrix *= transform2D::Translate(duckCenter.x + BEAK_RADIX * cos(VERTICAL + duckAngle), duckCenter.y + BEAK_RADIX * sin(VERTICAL + duckAngle));
	modelMatrix *= transform2D::Translate(0, 0) * transform2D::Rotate(duckAngle) * transform2D::Translate(0, 0);
	RenderMesh2D(meshes["duckBeak"], shaders["VertexColor"], modelMatrix);

	modelMatrix = glm::mat3(1);
	modelMatrix *= transform2D::Translate(duckPosition.x, duckPosition.y);
	modelMatrix *= transform2D::Translate(duckBodyTranslate.x, duckBodyTranslate.y) * transform2D::Rotate(duckAngle) * transform2D::Translate(-duckBodyTranslate.x, -duckBodyTranslate.y);
	RenderMesh2D(meshes["duckBody"], shaders["VertexColor"], modelMatrix);
	
	modelMatrix = glm::mat3(1);
	modelMatrix *= transform2D::Translate(duckCenter.x + RIGHT_WING_RADIX * cos(duckAngle), duckCenter.y + RIGHT_WING_RADIX * sin(duckAngle));
	modelMatrix *= transform2D::Translate(duckWingTranslate.x, duckWingTranslate.y) * transform2D::Rotate(duckAngle - wingAngle - wingRotation) * transform2D::Translate(-duckWingTranslate.x, -duckWingTranslate.y);
	RenderMesh2D(meshes["duckRightWing"], shaders["VertexColor"], modelMatrix);

	modelMatrix = glm::mat3(1);
	modelMatrix *= transform2D::Translate(duckCenter.x + LEFT_WING_RADIX * cos(duckAngle + M_PI), duckCenter.y + LEFT_WING_RADIX * sin(duckAngle + M_PI));
	modelMatrix *= transform2D::Translate(duckWingTranslate.x, duckWingTranslate.y) * transform2D::Rotate(duckAngle + wingAngle + wingRotation) * transform2D::Translate(-duckWingTranslate.x, -duckWingTranslate.y);
	RenderMesh2D(meshes["duckLeftWing"], shaders["VertexColor"], modelMatrix);
}

void DuckHunt::FrameEnd()
{
	DrawCoordinateSystem();
}

void DuckHunt::OnInputUpdate(float deltaTime, int mods)
{
	
}

void DuckHunt::OnKeyPress(int key, int mods)
{
	
}

void DuckHunt::OnKeyRelease(int key, int mods)
{

}

void DuckHunt::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	
}

void DuckHunt::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	glm::ivec2 resolution = window->GetResolution();
	mouseY = (int)resolution.y - mouseY;
	
	if (window->MouseHold(GLFW_MOUSE_BUTTON_1))
		if (numberOfBullets)
			if (mouseX > duckCenter.x - DUCK_OX_DIMM && mouseX < duckCenter.x + DUCK_OX_DIMM && mouseY > duckCenter.y - DUCK_OY_DIMM && mouseY < duckCenter.y + DUCK_OY_DIMM && !duckHit && !duckEscaped)
			{
				numberOfBullets--;
				currentScore++;
				duckHit = true;
				level_hit++;
				if (level_hit == 5)
				{
					level++;
					level_hit = 0;
				}
			}
			else
				numberOfBullets--;
}

void DuckHunt::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{

}

void DuckHunt::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{

}

void DuckHunt::OnWindowResize(int width, int height)
{

}