#include "scenebasic_uniform.h"

#include <cstdio>
#include <cstdlib>
#include <string>
using std::string;

#include <iostream>
using std::cerr;
using std::endl;

#include <windows.h>

#include "helper/glutils.h"
#include "helper/utilities.h"

using glm::vec3;

SceneBasic_Uniform::SceneBasic_Uniform() {}

void SceneBasic_Uniform::initScene(GLFWwindow *inWindow) {
	window = inWindow;

	glEnable(GL_DEPTH_TEST); // enable depth buffer test

	glDepthFunc(GL_LESS);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glfwSetWindowUserPointer(window, this);

	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	compile();

	std::cout << std::endl;

	// initialise projection matrix
	gProjectionMatrix = glm::perspective(
		glm::radians(45.0f), static_cast<float>(width) / height, 0.1f, 100.0f);

	// initialise point light properties
	gLight.pos = glm::vec3(0.0f, 3.0f, 0.0f);
	gLight.dir = glm::vec3(0.3f, -0.7f, -0.5f);
	gLight.La = glm::vec3(0.3f);
	gLight.Ld = glm::vec3(1.0f);
	gLight.Ls = glm::vec3(1.0f);
	gLight.att = glm::vec3(1.0f, 0.0f, 0.0f);

	// initialise material properties
	gMaterial.Ka = glm::vec3(0.2f);
	gMaterial.Kd = glm::vec3(1.0f, 1.0f, 1.0f);
	gMaterial.Ks = glm::vec3(0.2f, 0.7f, 1.0f);
	gMaterial.shininess = 40.0f;

	// load texture and normal map
	texStone = std::make_shared<Texture>();
	texStone->generate("./media/images/Fieldstone.bmp");

	texStoneNormal = std::make_shared<Texture>();
	texStoneNormal->generate("./media/images/FieldstoneBumpDOT3.bmp");

	texWhite = std::make_shared<Texture>();
	texWhite->generate("./media/images/White.bmp");

	texWhiteNormal = std::make_shared<Texture>();
	texWhiteNormal->generate("./media/images/WhiteBumpDOT3.bmp");

	texCrate = std::make_shared<Texture>();
	texCrate->generate("./media/images/diffuse.bmp");

	// load cube environment map texture
	gCubeEnvMap.generate(
		"./media/images/cm_front.bmp", "./media/images/cm_back.bmp",
		"./media/images/cm_left.bmp", "./media/images/cm_right.bmp",
		"./media/images/cm_top.bmp", "./media/images/cm_bottom.bmp");

	// load model
	gTorusModel.loadModel("./media/models/torus.obj", false);
	gCubeModel.loadModel("./media/models/cube.obj", true);

	// initialise model matrices
	auto wallVA = CreateWallVA();
	walls = {
		{glm::translate(glm::vec3(-2.0f, 0.0f, -3.0f)), wallVA, texStone,
		 texStoneNormal}, // BackWall1
		{glm::translate(glm::vec3(0.0f, 0.0f, -3.0f)), wallVA, texStone,
		 texStoneNormal}, // BackWall2
		{glm::translate(glm::vec3(2.0f, 0.0f, -3.0f)), wallVA, texStone,
		 texStoneNormal}, // BackWall3
		{glm::translate(glm::vec3(-2.0f, 2.0f, -3.0f)), wallVA, texStone,
		 texStoneNormal}, // BackWall4
		{glm::translate(glm::vec3(0.0f, 2.0f, -3.0f)), wallVA, texStone,
		 texStoneNormal}, // BackWall5
		{glm::translate(glm::vec3(2.0f, 2.0f, -3.0f)), wallVA, texStone,
		 texStoneNormal}, // BackWall6

		{glm::translate(glm::vec3(-2.0f, 0.0f, 3.0f)) *
			 glm::rotate(glm::mat4(1.0f), glm::radians(180.0f),
						 glm::vec3(0.0f, 1.0f, 0.0f)),
		 wallVA, texStone, texStoneNormal}, // FrontWall1
		{glm::translate(glm::vec3(0.0f, 0.0f, 3.0f)) *
			 glm::rotate(glm::mat4(1.0f), glm::radians(180.0f),
						 glm::vec3(0.0f, 1.0f, 0.0f)),
		 wallVA, texStone, texStoneNormal}, // FrontWall2
		{glm::translate(glm::vec3(2.0f, 0.0f, 3.0f)) *
			 glm::rotate(glm::mat4(1.0f), glm::radians(180.0f),
						 glm::vec3(0.0f, 1.0f, 0.0f)),
		 wallVA, texStone, texStoneNormal}, // FrontWall3
		{glm::translate(glm::vec3(-2.0f, 2.0f, 3.0f)) *
			 glm::rotate(glm::mat4(1.0f), glm::radians(180.0f),
						 glm::vec3(0.0f, 1.0f, 0.0f)),
		 wallVA, texStone, texStoneNormal}, // FrontWall4
		{glm::translate(glm::vec3(0.0f, 2.0f, 3.0f)) *
			 glm::rotate(glm::mat4(1.0f), glm::radians(180.0f),
						 glm::vec3(0.0f, 1.0f, 0.0f)),
		 wallVA, texStone, texStoneNormal}, // FrontWall5
		{glm::translate(glm::vec3(2.0f, 2.0f, 3.0f)) *
			 glm::rotate(glm::mat4(1.0f), glm::radians(180.0f),
						 glm::vec3(0.0f, 1.0f, 0.0f)),
		 wallVA, texStone, texStoneNormal}, // FrontWall6

		{glm::translate(glm::vec3(-3.0f, 0.0f, -2.0f)) *
			 glm::rotate(glm::mat4(1.0f), glm::radians(90.0f),
						 glm::vec3(0.0f, 1.0f, 0.0f)),
		 wallVA, texStone, texStoneNormal}, // LeftWall1
		{glm::translate(glm::vec3(-3.0f, 0.0f, 0.0f)) *
			 glm::rotate(glm::mat4(1.0f), glm::radians(90.0f),
						 glm::vec3(0.0f, 1.0f, 0.0f)),
		 wallVA, texStone, texStoneNormal}, // LeftWall2
		{glm::translate(glm::vec3(-3.0f, 0.0f, 2.0f)) *
			 glm::rotate(glm::mat4(1.0f), glm::radians(90.0f),
						 glm::vec3(0.0f, 1.0f, 0.0f)),
		 wallVA, texStone, texStoneNormal}, // LeftWall3
		{glm::translate(glm::vec3(-3.0f, 2.0f, -2.0f)) *
			 glm::rotate(glm::mat4(1.0f), glm::radians(90.0f),
						 glm::vec3(0.0f, 1.0f, 0.0f)),
		 wallVA, texStone, texStoneNormal}, // LeftWall4
		{glm::translate(glm::vec3(-3.0f, 2.0f, 0.0f)) *
			 glm::rotate(glm::mat4(1.0f), glm::radians(90.0f),
						 glm::vec3(0.0f, 1.0f, 0.0f)),
		 wallVA, texStone, texStoneNormal}, // LeftWall5
		{glm::translate(glm::vec3(-3.0f, 2.0f, 2.0f)) *
			 glm::rotate(glm::mat4(1.0f), glm::radians(90.0f),
						 glm::vec3(0.0f, 1.0f, 0.0f)),
		 wallVA, texStone, texStoneNormal}, // LeftWall6

		{glm::translate(glm::vec3(3.0f, 0.0f, -2.0f)) *
			 glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f),
						 glm::vec3(0.0f, 1.0f, 0.0f)),
		 wallVA, texStone, texStoneNormal}, // RightWall1
		{glm::translate(glm::vec3(3.0f, 0.0f, 0.0f)) *
			 glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f),
						 glm::vec3(0.0f, 1.0f, 0.0f)),
		 wallVA, texStone, texStoneNormal}, // RightWall2
		{glm::translate(glm::vec3(3.0f, 0.0f, 2.0f)) *
			 glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f),
						 glm::vec3(0.0f, 1.0f, 0.0f)),
		 wallVA, texStone, texStoneNormal}, // RightWall3
		{glm::translate(glm::vec3(3.0f, 2.0f, -2.0f)) *
			 glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f),
						 glm::vec3(0.0f, 1.0f, 0.0f)),
		 wallVA, texStone, texStoneNormal}, // RightWall4
		{glm::translate(glm::vec3(3.0f, 2.0f, 0.0f)) *
			 glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f),
						 glm::vec3(0.0f, 1.0f, 0.0f)),
		 wallVA, texStone, texStoneNormal}, // RightWall5
		{glm::translate(glm::vec3(3.0f, 2.0f, 2.0f)) *
			 glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f),
						 glm::vec3(0.0f, 1.0f, 0.0f)),
		 wallVA, texStone, texStoneNormal}, // RightWall6

	};

	floor = { glm::translate(glm::vec3(0.0f, -0.5f, 0.0f)) *
				 glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f),
							 glm::vec3(1.0f, 0.0f, 0.0f)) *
				 glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 3.0f, 1.0f)),
			 wallVA, texWhite, texWhiteNormal };
}

void SceneBasic_Uniform::compile() {
	try {
		// compile and link a vertex and fragment shader pair
		gNormalMapShader.compileShader("shader/normalTanTex.vert");
		gNormalMapShader.compileShader("shader/normalMap.frag");
		gNormalMapShader.link();

		gCubemapShader.compileShader("shader/normalTanTex.vert");
		gCubemapShader.compileShader("shader/cubemap.frag");
		gCubemapShader.link();

		hairShader.compileShader("shader/normalTanTex.vert");
		hairShader.compileShader("shader/hair.geom");
		hairShader.compileShader("shader/hair.frag");
		hairShader.link();

		shadowShader.compileShader("shader/shadow.vert");
		shadowShader.compileShader("shader/shadow.frag");
		shadowShader.link();

		shadowDebugShader.compileShader("shader/2dSpirit.vert");
		shadowDebugShader.compileShader("shader/2dSpirit.frag");
		shadowDebugShader.link();
	}
	catch (GLSLProgramException &e) {
		cerr << e.what() << endl;
		exit(EXIT_FAILURE);
	}
}

void SceneBasic_Uniform::updateLigthPosition() {
	static float angle = 0.0f;
	static float x = 0.0f, y = 0.0f;
	float speed = glm::radians(30.0f);

	angle += speed * frameTime;
	if (angle > glm::two_pi<float>()) {
		angle -= glm::two_pi<float>();
	}

	x += 0.5f * frameTime;
	y += 0.2f * frameTime;

	float radius = 3.0f;
	float offsetY = y;

	gLight.pos.x = radius * glm::cos(angle);
	gLight.pos.y = 3.0f;
	gLight.pos.z = radius * glm::sin(angle);
}

void SceneBasic_Uniform::setLightMaterialUniform(GLSLProgram &prog) {
	// set light properties
	prog.setUniform("uLight.pos", gLight.pos);
	prog.setUniform("uLight.dir", gLight.dir);

	prog.setUniform("uLight.La", gLight.La);
	prog.setUniform("uLight.Ld", gLight.Ld);
	prog.setUniform("uLight.Ls", gLight.Ls);
	prog.setUniform("uLight.att", gLight.att);

	// set material properties
	prog.setUniform("uMaterial.Ka", gMaterial.Ka);
	prog.setUniform("uMaterial.Kd", gMaterial.Kd);
	prog.setUniform("uMaterial.Ks", gMaterial.Ks);
	prog.setUniform("uMaterial.shininess", gMaterial.shininess);
}

void SceneBasic_Uniform::updateFPS() {
	float currentFrame = glfwGetTime();
	frameTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
}

void SceneBasic_Uniform::update(float t) {
	// update your angle here
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		mainCamera.ProcessKeyboard(Camera_Movement::FORWARD, frameTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		mainCamera.ProcessKeyboard(Camera_Movement::BACKWARD, frameTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		mainCamera.ProcessKeyboard(Camera_Movement::LEFT, frameTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		mainCamera.ProcessKeyboard(Camera_Movement::RIGHT, frameTime);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		mainCamera.ProcessKeyboard(Camera_Movement::DOWN, frameTime);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		mainCamera.ProcessKeyboard(Camera_Movement::UP, frameTime);

	rotateAngle += rotateSpeed * frameTime;

	if (animateLight) {
		updateLigthPosition();
	}

	updateFPS();
}

void SceneBasic_Uniform::drawWall(const Wall &wall, const glm::mat4 &viewMatrix,
	const glm::mat4 &projectionMatrix) {
	// calculate matrices
	glm::mat3 normalMatrix =
		glm::mat3(glm::transpose(glm::inverse(wall.modelMatrix)));

	// set uniform variables
	gNormalMapShader.setUniform("uProjectionMatrix", projectionMatrix);
	gNormalMapShader.setUniform("uViewMatrix", viewMatrix);
	gNormalMapShader.setUniform("uModelMatrix", wall.modelMatrix);
	gNormalMapShader.setUniform("uNormalMatrix", normalMatrix);

	// set texture and normal map
	gNormalMapShader.setUniform("uTextureSampler", 0);
	gNormalMapShader.setUniform("uNormalSampler", 1);

	glActiveTexture(GL_TEXTURE0);
	wall.texture->bind();

	glActiveTexture(GL_TEXTURE1);
	wall.texNormal->bind();

	glBindVertexArray(wall.va->gVAO);      // make VAO active
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // render the vertices
}

void SceneBasic_Uniform::drawModel(GLSLProgram &shader, SimpleModel &model,
	const glm::mat4 &modelMatrix,
	const glm::mat4 &viewMatrix,
	const glm::mat4 &projectionMatrix,
	Texture &texture, Texture &normalMap) {
	// calculate matrices
	glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(modelMatrix)));

	// set uniform variables
	shader.setUniform("uProjectionMatrix", projectionMatrix);
	shader.setUniform("uViewMatrix", viewMatrix);
	shader.setUniform("uModelMatrix", modelMatrix);
	shader.setUniform("uNormalMatrix", normalMatrix);

	glActiveTexture(GL_TEXTURE0);
	texture.bind();

	glActiveTexture(GL_TEXTURE1);
	normalMap.bind();

	model.drawModel();
}

void SceneBasic_Uniform::render_scene(const glm::mat4 &viewMatrix,
	const glm::mat4 &projectionMatrix) {

	cubeModelMatrix = glm::translate(glm::vec3(1.0f, 0.0f, 1.0f)) *
		glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));

	torusModelMatrix = glm::translate(glm::vec3(-1.0f, 1.0f, -1.0f)) *
		glm::scale(glm::mat4(1.0f), glm::vec3(0.8f));
	auto rotation =
		glm::rotate(glm::radians(rotateAngle), glm::vec3(1.0f, 0.0f, 0.0f));
	torusModelMatrix *= rotation;

	// render shadow map at depth frame buffer
	{
		depthFrameBuffer.StartFill();
		float orthoXHalfSize = 5.0f;
		glm::mat4 lightProjection =
			glm::ortho(-orthoXHalfSize, orthoXHalfSize, -orthoXHalfSize, orthoXHalfSize, 0.1f, 10.0f);
		glm::mat4 lightView = glm::lookAt(gLight.pos, mainCamera.Front, glm::vec3(0.0, 1.0, 0.0));
		glm::mat4 lightSpaceMatrix = lightProjection * lightView;
		//std::cout << gLight.pos.x << "," << gLight.pos.y << "," << gLight.pos.z << std::endl;

		shadowShader.use();
		shadowShader.setUniform("lightSpaceMatrix", lightSpaceMatrix);

		shadowShader.setUniform("model", cubeModelMatrix);
		gCubeModel.drawModel();

		shadowShader.setUniform("model", torusModelMatrix);
		gTorusModel.drawModel();

		depthFrameBuffer.EndFill();
		glViewport(0, 0, width, height);
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		{
			gNormalMapShader.use();
			gNormalMapShader.setUniform("lightSpaceMatrix", lightSpaceMatrix);
			gNormalMapShader.setUniform("uShadowMap", 2);
			glActiveTexture(GL_TEXTURE2);
			depthFrameBuffer.GetTexture().bind();
		}
	}

	// =============== wall ===============
	{
		// use the shaders associated with the shader program
		gNormalMapShader.use();
		setLightMaterialUniform(gNormalMapShader);

		// set viewing position
		gNormalMapShader.setUniform("uViewpoint", glm::vec3(0.0f, 0.0f, 4.0f));

		gNormalMapShader.setUniform("useNormalSampler", true);
		for (auto &wall : walls) {
			drawWall(wall, viewMatrix, projectionMatrix);
		}
	}

	// =============== cube ===============
	{
		gNormalMapShader.use();
		gNormalMapShader.setUniform("useNormalSampler", false);
		drawModel(gNormalMapShader, gCubeModel, cubeModelMatrix, viewMatrix,
			projectionMatrix, *texCrate, *texCrate);

	}

	// =============== torus ===============
	{
		// use the shaders associated with the shader program
		gCubemapShader.use();
		setLightMaterialUniform(gCubemapShader);

		// set viewing position
		gCubemapShader.setUniform("uViewpoint", glm::vec3(0.0f, 0.0f, 4.0f));

		// set cube environment map

		gCubemapShader.setUniform("cubemapBlendFactor", cubemapBlendFactor);

		// render model
		gCubemapShader.setUniform("uEnvironmentMap", 0);
		drawModel(gCubemapShader, gTorusModel, torusModelMatrix, viewMatrix,
			projectionMatrix, gCubeEnvMap, gCubeEnvMap);

		hairShader.use();
		drawModel(hairShader, gTorusModel, torusModelMatrix, viewMatrix,
			projectionMatrix, gCubeEnvMap, gCubeEnvMap);
	}

	// =============== floor ===============
	gNormalMapShader.use();
	gNormalMapShader.setUniform("useNormalSampler", true);
	drawWall(floor, viewMatrix, projectionMatrix);

	// render a debug shadow map at screen corner
	if (displayShadowMap)
	{
		shadowDebugShader.use();
		glm::mat4 model(1.0f);
		shadowDebugShader.setUniform("model", model);
		shadowDebugShader.setUniform("view", glm::mat4(1.0f));
		glm::mat4 projection = glm::ortho(0, width, 0, height);
		shadowDebugShader.setUniform("projection", glm::mat4(1.0f));

		shadowDebugShader.setUniform("tex", 0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthFrameBuffer.GetTexture().GetId());

		glBindVertexArray(spirit.gVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	// flush the graphics pipeline
	glFlush();
}

// key press or release callback function
void SceneBasic_Uniform::key_callback(GLFWwindow *window, int key, int scancode,
	int action, int mods) {
	auto app =
		reinterpret_cast<SceneBasic_Uniform *>(glfwGetWindowUserPointer(window));

	// close the window when the ESCAPE key is pressed
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		// set flag to close the window
		glfwSetWindowShouldClose(window, GL_TRUE);
		return;
	}

	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
		app->rotateSpeed -= 10.0f;

		if (app->rotateSpeed <= 0.0f) {
			app->rotateSpeed = 0.0f;
		}
	}

	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
		app->rotateSpeed += 10.0f;

		if (app->rotateSpeed >= 180.0f) {
			app->rotateSpeed = 180.0f;
		}
	}

	if (key == GLFW_KEY_MINUS && action == GLFW_PRESS) {
		app->cubemapBlendFactor -= 0.1f;

		if (app->cubemapBlendFactor < 0.0f) {
			app->cubemapBlendFactor = 0.0f;
		}
	}

	if (key == GLFW_KEY_EQUAL && action == GLFW_PRESS) {
		app->cubemapBlendFactor += 0.1f;

		if (app->cubemapBlendFactor > 1.0f) {
			app->cubemapBlendFactor = 1.0f;
		}
	}

	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
		MessageBox(nullptr, L"Space pressed.", L"Message", MB_OK);
		app->animateLight = !app->animateLight;
	}

	if (key == GLFW_KEY_M && action == GLFW_PRESS) {
		app->displayShadowMap = !app->displayShadowMap;
	}
}

// mouse movement callback function
void SceneBasic_Uniform::cursor_position_callback(GLFWwindow *window,
	double xpos, double ypos) {
	auto app =
		reinterpret_cast<SceneBasic_Uniform *>(glfwGetWindowUserPointer(window));

	auto deltaX = static_cast<float>(xpos - app->lastMousePosition.x);
	auto deltaY = static_cast<float>(app->lastMousePosition.y - ypos);

	if (app->rightMouseButtonDown) {
		app->mainCamera.ProcessMouseMovement(deltaX, deltaY);
	}

	app->lastMousePosition.x = static_cast<float>(xpos);
	app->lastMousePosition.y = static_cast<float>(ypos);
}

// mouse button callback function
void SceneBasic_Uniform::mouse_button_callback(GLFWwindow *window, int button,
	int action, int mods) {
	auto app =
		reinterpret_cast<SceneBasic_Uniform *>(glfwGetWindowUserPointer(window));

	if (button == GLFW_MOUSE_BUTTON_2 && action == GLFW_PRESS) {
		app->rightMouseButtonDown = true;
	}

	if (button == GLFW_MOUSE_BUTTON_2 && action == GLFW_RELEASE) {
		app->rightMouseButtonDown = false;
	}
}

void SceneBasic_Uniform::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glViewport(0, 0, width, height);

	render_scene(mainCamera.GetViewMatrix(),
		gProjectionMatrix); // render the scene

	glBindVertexArray(0);
}

void SceneBasic_Uniform::resize(int w, int h) {
	width = w;
	height = h;
	glViewport(0, 0, w, h);
}
