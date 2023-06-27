#pragma once
//GLM

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

class Luz
{
private:
	glm::vec3 Posicao;
	glm::vec3 Cor;
public:
	Luz() {
		Posicao = glm::vec3(0, 5.0, 2.0);
		Cor = glm::vec3(1, 1, 1);
	}

	Luz(glm::vec3 pos) {
		Posicao = pos;

		Cor = glm::vec3(1, 1, 1);
	}

public:
	void SetarLuz(Shader& shader)
	{
		shader.setVec3("lightPos", Posicao.x, Posicao.y, Posicao.z);
		shader.setVec3("lightColor", Cor.x, Cor.y, Cor.z);
	}
};