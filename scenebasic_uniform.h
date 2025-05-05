#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

// glad must be the first one
#include <glad/glad.h>

#include "DepthFrameBuffer.h"
#include "Wall.h"

//
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>

#include "helper/Camera.h"
#include "helper/SimpleModel.h"
#include "helper/Texture.h"
#include "helper/glslprogram.h"
#include "helper/scene.h"
#include "Spirit.h"

struct VertexArraya {};

class SceneBasic_Uniform : public Scene {
private:
	GLFWwindow *window;

	// frame stats
	float gFrameRate = 60.0f;

	float rotateAngle = 0.0f;

	float frameTime = 0.0f;

	float lastFrame = 0.0f;

	// controls
	glm::vec2 lastMousePosition;

	bool rightMouseButtonDown = false;

	float rotateSpeed = 90.0f;

	float cubemapBlendFactor = 1.0f;

	bool displayShadowMap = false;

	bool animateLight = true;

	// camera
	Camera topRightCamera{ glm::vec3(0.0f, 10.0f, 0.0f),
						  glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -90.0f };
	Camera bottomLeftCamera{ glm::vec3(-0.5f, 1.5f, 2.5f),
							glm::vec3(0.0f, 1.0f, 0.0f) };
	Camera mainCamera{ glm::vec3(0.0f, 5.0f, 4.0f), glm::vec3(0.0f, 1.0f, 0.0f),
					  -90.0f, -45.0f };

	glm::mat4 gProjectionMatrix; // projection matrix

	// shader
	GLSLProgram gNormalMapShader;     // shader program object
	GLSLProgram gCubemapShader;
	GLSLProgram hairShader;
	GLSLProgram shadowShader;
	GLSLProgram shadowDebugShader;

	// model
	std::vector<Wall> walls;
	Wall floor;
	SimpleModel gTorusModel; // scene object model
	SimpleModel gCubeModel;  // scene object model
	Spirit spirit;

	glm::mat4 cubeModelMatrix;
	glm::mat4 torusModelMatrix;

	// light
	Light gLight;       // light properties
	Material gMaterial; // material properties

	// textures
	std::shared_ptr<Texture> texStone;
	std::shared_ptr<Texture> texStoneNormal;
	std::shared_ptr<Texture> texWhite;
	std::shared_ptr<Texture> texWhiteNormal;
	std::shared_ptr<Texture> texCrate;
	Texture gCubeEnvMap; // cube environment map

	// framebuffer
	opengl::DepthFrameBuffer depthFrameBuffer;

public:
private:
	void compile();

	void updateLigthPosition();

	void setLightMaterialUniform(GLSLProgram &prog);

	void updateFPS();

	void drawWall(const Wall &wall, const glm::mat4 &viewMatrix,
		const glm::mat4 &projectionMatrix);
	void drawModel(GLSLProgram &shader, SimpleModel &model,
		const glm::mat4 &modelMatrix, const glm::mat4 &viewMatrix,
		const glm::mat4 &projectionMatrix, Texture &texture,
		Texture &normalMap);

	void render_scene(const glm::mat4 &viewMatrix,
		const glm::mat4 &projectionMatrix);

	static void key_callback(GLFWwindow *window, int key, int scancode,
		int action, int mods);
	static void cursor_position_callback(GLFWwindow *window, double xpos,
		double ypos);
	static void mouse_button_callback(GLFWwindow *window, int button, int action,
		int mods);

public:
	SceneBasic_Uniform();

	void initScene(GLFWwindow *inWindow);
	void update(float t);
	void render();
	void resize(int, int);
};

#endif // SCENEBASIC_UNIFORM_H
