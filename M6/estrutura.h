#pragma once
#include <string>

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

#include "Shader.h"
#include "Luz.h"
#include "Camera.h"

class Estrutura {
private:
	int Largura;
	int Altura;

	Luz LuzCena;
	Camera CameraCena;

public:
	Shader ShaderCena = Shader("", "");

public:
	Estrutura() {
		LuzCena = Luz();
		CameraCena = Camera();
	}

	Estrutura(GLFWwindow* window, std::string vertexShader, std::string fragmentShader) {
		glfwGetFramebufferSize(window, &Largura, &Altura);
		glViewport(0, 0, Largura, Altura);

		ShaderCena = Shader(vertexShader.c_str(), fragmentShader.c_str());

		glUseProgram(ShaderCena.ID);
		glEnable(GL_DEPTH_TEST);

		LuzCena = Luz();
		CameraCena = Camera();
	}

public:
	void CarregarEstrutura() {
		CameraCena.CarregarCamera(ShaderCena, (float)Largura, (float)Altura);
		LuzCena.SetarLuz(ShaderCena);
	}

	Shader* ObterRefShaderCena() {
		return &ShaderCena;
	}

	void NovaLuz(glm::vec3 novaLuz) {
		LuzCena = Luz(novaLuz);
	}

	Camera* ObterRefCamera() {
		return &CameraCena;
	}
};