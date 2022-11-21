#include "stb_image.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "learnopengl/shader.h"
#include "learnopengl/camera.h"
#include "learnopengl/model.h"

#include <iostream>
#include <math.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);
unsigned int loadCubemap(vector<std::string> faces);

// settings
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 900;

// camera
Camera camera(glm::vec3(-20.0f, 0.0f, 5.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

float pts[10][2] = { {-2,-0.0735878},{3,-0.539789},{8,-0.154111},{13,0.39687},{18,0.137219},{23,0.36858},{28,-0.40811},{33,-0.434616},{38,-0.122662},{43,-0.305223} };
int len = 10;
int fact(int n)//阶乘函数
{
	if (n == 0)
	{
		return 1;
	}
	else
	{
		int res = 1;
		for (int i = n; i > 0; i--)
		{
			res *= i;
		}
		return res;
	}
}

float bezier(float t)
{
	int n = len - 1;
	float Bt = 0;
	float Xt = 0;
	float Yt = 0;
	for (int i = 0; i <= n; i++)
	{
		Bt = fact(n) / (fact(n - i) * fact(i)) * pow((1 - t), (n - i)) * pow(t, i); //计算伯恩斯坦基底
		Xt += Bt * (pts[i][0]);
		Yt += Bt * (pts[i][1]);
	}
	return Yt;
}
int main()
{
	glfwInit();	// glfw: initialize and configure
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "The Tortoise and the Hare", NULL, NULL);	// glfw window creation
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);	// tell GLFW to capture our mouse

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))	// glad: load all OpenGL function pointers
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glEnable(GL_DEPTH_TEST);	// configure global opengl state

	// build and compile shaders
	Shader grassShader("resources/shader/grassGround.vs", "resources/shader/grassGround.fs");
	Shader hareShader("resources/shader/hare.vs", "resources/shader/hare.fs");
	Shader shader("resources/shader/6.1.cubemaps.vs", "resources/shader/6.1.cubemaps.fs");
	Shader skyboxShader("resources/shader/skyboxShader.vs", "resources/shader/skyboxShader.fs");
	Shader tortoiseShader("resources/shader/tortoise.vs", "resources/shader/tortoise.fs");
	Shader gateShader("resources/shader/gate.vs", "resources/shader/gate.fs");
	Shader trophyShader("resources/shader/trophy.vs", "resources/shader/trophy.fs");
	// set up vertex data (and buffer(s)) and configure vertex attributes
	float cubeVertices[] = {
		// positions          // texture Coords
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};
	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	Model gateModel("resources/gate/uploads_files_3661170_gate.obj");
	Model tortoiseModel("resources/turtle/uploads_files_2184392_Turtle_OBJ.obj");
	Model hareModel("resources/rabbit/uploads_files_991253_Rabbit.obj");
	Model grassGroundModel("resources/grass/10450_Rectangular_Grass_Patch_v1_iterations-2.obj");
	Model trophyModel("resources/trophy/uploads_files_863594_Trophy.obj");
	// cube VAO
	unsigned int cubeVAO, cubeVBO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	// skybox VAO
	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	// load cube textures
	unsigned int cubeTexture = loadTexture("resources/textures/texture.jpeg");

	vector<std::string> faces
	{
		"resources/textures/skybox/right.jpg",
		"resources/textures/skybox/left.jpg",
		"resources/textures/skybox/top.jpg",
		"resources/textures/skybox/bottom.jpg",
		"resources/textures/skybox/front.jpg",
		"resources/textures/skybox/back.jpg"
	};
	unsigned int cubemapTexture = loadCubemap(faces);

	// shader configuration
	shader.use();
	shader.setInt("texture1", 0);
	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);

	glm::vec3 lightPosition = glm::vec3(0.0f, 20.0f, 20.0f);
	glm::vec3 lightforgate = glm::vec3(0.0f, 2.0f, 50.0f);
	float time = 0.0f;

	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		float currentFrame = glfwGetTime();//返回时间
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		time = time + deltaTime;//tFrame就是时间，一直增长

		processInput(window);

		// render
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.use();
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(5.0f, -0.5f, 0.5f));
		model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		shader.setMat4("model", model);
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);
		// cubes
		glBindVertexArray(cubeVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, cubeTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		trophyShader.use();
		glm::mat4 trophyMatrix = glm::mat4(1.0f);
		glm::mat4 trophyview = camera.GetViewMatrix();
		glm::mat4 trophyprojection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		trophyMatrix = glm::translate(trophyMatrix, glm::vec3(-3.0, -2.0, 40.0));
		trophyMatrix = glm::scale(trophyMatrix, glm::vec3(4.0f, 4.0f, 4.0f));
		trophyShader.setMat4("view", trophyview);
		trophyShader.setMat4("projection", trophyprojection);
		trophyShader.setVec3("lightPosition", lightPosition);
		trophyShader.setVec3("viewPos", camera.Position);
		trophyShader.setMat4("model", trophyMatrix);
		trophyModel.Draw(trophyShader);

		hareShader.use();
		glm::mat4 hareMatrix = glm::mat4(1.0f);
		glm::mat4 ourview = camera.GetViewMatrix();
		glm::mat4 ourprojection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

		if (time < 8)
			hareMatrix = glm::translate(hareMatrix, glm::vec3(1.0, -2.0, -2.0 + 2.5 * time));
		else if (time > 8 && time < 35)
			hareMatrix = glm::translate(hareMatrix, glm::vec3(1.0, -2.0, 18.0));
		else if (time > 35 && time < 43)
			hareMatrix = glm::translate(hareMatrix, glm::vec3(1.0, -2.0, 18.0 + 2.5 * (time - 35)));
		else
			hareMatrix = glm::translate(hareMatrix, glm::vec3(1.0, -2.0, 38.0));
		hareMatrix = glm::scale(hareMatrix, glm::vec3(0.03f, 0.03f, 0.03f));
		hareShader.setMat4("view", ourview);
		hareShader.setMat4("projection", ourprojection);
		hareShader.setVec3("lightPosition", lightPosition);
		hareShader.setVec3("viewPos", camera.Position);
		hareShader.setMat4("model", hareMatrix);
		hareModel.Draw(hareShader);

		tortoiseShader.use();
		glm::mat4 tortoiseMatrix = glm::mat4(1.0f);
		glm::mat4 tortoiseView = camera.GetViewMatrix();
		glm::mat4 tortoiseProjection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

		if (time < 40)
			tortoiseMatrix = glm::translate(tortoiseMatrix, glm::vec3(-3.0, -2.0 + bezier(time / 40), -2.0 + time));
		//模型沿z轴正方向运动，y轴方向为地形起伏
		else tortoiseMatrix = glm::translate(tortoiseMatrix, glm::vec3(-3.0, -2.0, 38.0));
		tortoiseMatrix = glm::scale(tortoiseMatrix, glm::vec3(0.03f, 0.03f, 0.03f));

		tortoiseShader.setMat4("view", tortoiseView);
		tortoiseShader.setMat4("projection", tortoiseProjection);
		tortoiseShader.setVec3("lightPosition", lightPosition);
		tortoiseShader.setVec3("viewPos", camera.Position);
		tortoiseShader.setMat4("model", tortoiseMatrix);
		tortoiseModel.Draw(tortoiseShader);

		gateShader.use();
		glm::mat4 gateMatrix = glm::mat4(1.0f);
		glm::mat4 gateView = camera.GetViewMatrix();
		glm::mat4 gateProjection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

		gateMatrix = glm::translate(gateMatrix, glm::vec3(0.0f, -2.0f, 35.0f)); // translate it down so it's at the center of the scene
		gateMatrix = glm::scale(gateMatrix, glm::vec3(2.5f, 1.0f, 1.0f));
		gateMatrix = glm::rotate(gateMatrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		gateShader.setMat4("view", gateView);
		gateShader.setMat4("projection", gateProjection);
		gateShader.setVec3("lightPosition", lightforgate);
		gateShader.setVec3("viewPos", camera.Position);
		gateShader.setMat4("model", gateMatrix);

		gateModel.Draw(gateShader);

		grassShader.use();
		glm::mat4 grassview = camera.GetViewMatrix();
		glm::mat4 grassprojection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 grassmodel = glm::mat4(1.0f);
		grassmodel = glm::translate(grassmodel, glm::vec3(0.0f, -10.0f, 0.0f));
		grassmodel = glm::rotate(grassmodel, glm::radians(90.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
		grassShader.setMat4("model", grassmodel);
		grassShader.setMat4("view", grassview);
		grassShader.setMat4("projection", grassprojection);
		grassGroundModel.Draw(grassShader);

		// draw skybox as last
		glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
		skyboxShader.use();
		view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
		skyboxShader.setMat4("view", view);
		skyboxShader.setMat4("projection", projection);
		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); // set depth function back to default

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteBuffers(1, &cubeVBO);
	glDeleteBuffers(1, &skyboxVBO);

	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

// utility function for loading a 2D texture from file
unsigned int loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

// loads a cubemap texture from 6 individual texture faces
// order: +X (right) -X (left) +Y (top) -Y (bottom) +Z (front)  -Z (back)
unsigned int loadCubemap(vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}