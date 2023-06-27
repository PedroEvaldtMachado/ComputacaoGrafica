#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

class Camera {
private:
	glm::vec3 Posicao;
	glm::vec3 Frente;
	glm::vec3 Up;
	float Yaw, Pitch, Fov, Velocidade;
	bool FirstMouse = true;
	float Sensibilidade, LastX, LastY;
public:
	Camera() {
		Posicao = glm::vec3(0.0, 0.0, 3.0);
		Frente = glm::vec3(0.0, 0.0, -1.0);
		Up = glm::vec3(0.0, 1.0, 0.0);

		Yaw = -90.0f;
		Pitch = 0.0f;
		Fov = 45;
		Velocidade = 5.0f;

		Sensibilidade = 0.001f;
		LastX = LastY = 0.0f;
	}

	void AlterarFov(float insensidade) {
		Fov += insensidade;

		if (Fov < 1)
		{
			Fov = 1.0f;
		}
		if (Fov > 45)
		{
			Fov = 45.0f;
		}
	}

	void CarregarCamera(Shader& shader, float width, float height)
	{
		glm::mat4 projection = glm::perspective(glm::radians(Fov), width / height, 0.1f, 100.0f);
		shader.setMat4("projection", glm::value_ptr(projection));

		glm::mat4 view = glm::lookAt(Posicao, Posicao + Frente, Up);
		shader.setMat4("view", glm::value_ptr(view));

		shader.setVec3("cameraPos", Posicao.x, Posicao.y, Posicao.z);
	}

	void MoverFrente(float deltaTime) {
		Posicao = Posicao + (Velocidade * deltaTime * Frente);
	}

	void MoverTraz(float deltaTime) {
		Posicao = Posicao - (Velocidade * deltaTime * Frente);
	}

	void GirarDireita(float deltaTime) {
		Posicao = Posicao + glm::normalize(glm::cross(Frente, Up)) * Velocidade * deltaTime;
	}

	void GirarEsquerda(float deltaTime) {
		Posicao = Posicao - glm::normalize(glm::cross(Frente, Up)) * Velocidade * deltaTime;
	}

	void RotacionarFrente(float deltaTime) {
		Pitch += VelocidadeRotacao() * deltaTime;

		AplicarRotacao();
	}

	void RotacionarTras(float deltaTime) {
		Pitch -= VelocidadeRotacao() * deltaTime;

		AplicarRotacao();
	}

	void RotacionarDireita(float deltaTime) {
		Yaw += VelocidadeRotacao() * deltaTime;

		AplicarRotacao();
	}

	void RotacionarEsquerda(float deltaTime) {
		Yaw -= VelocidadeRotacao() * deltaTime;

		AplicarRotacao();
	}

	float VelocidadeRotacao() {
		return Velocidade * 2.0f;
	}

	void AplicarRotacao() {
		if (Pitch > 89.0f)
		{
			Pitch = 89.0f;
		}
		if (Pitch < -89.0f)
		{
			Pitch = -89.0f;
		}

		glm::vec3 front;

		front.x = cos(glm::radians(Pitch)) * cos(glm::radians(Yaw));
		front.y = sin(glm::radians(Pitch));
		front.z = cos(glm::radians(Pitch)) * sin(glm::radians(Yaw));

		Frente = glm::normalize(front);

		glm::vec3 right = glm::normalize(glm::cross(Frente, glm::vec3(0.0, 1.0, 0.0)));
		Up = glm::normalize(glm::cross(right, Frente));
	}

	void MoverComMouse(GLFWwindow* window) {
		double dxpos, dypos;

		glfwGetCursorPos(window, &dxpos, &dypos);

		float xpos = (float)dxpos, ypos = (float)dypos;

		if (FirstMouse)
		{
			LastX = (float)xpos;
			LastY = (float)ypos;
			FirstMouse = false;
		}

		float xoffset = xpos - LastX;
		float yoffset = LastY - ypos;
		LastX = xpos;
		LastY = ypos;

		xoffset *= Sensibilidade;
		yoffset *= Sensibilidade;

		Yaw += xoffset;
		Pitch += yoffset;

		AplicarRotacao();
	}

	void AlterarFovComMouse(double scrollOffset) {
		AlterarFov(-(float)scrollOffset);
	}
};