#include <iostream>
#include <string>
#include <assert.h>
#include <sstream>
#include <fstream>

#include "tipoObjeto.cpp"

// GLAD
#include <glad/glad.h>

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "stb_image.h"
#include <vector>

class Objeto
{
private:
	TipoObjeto Tipo;
	glm::mat4 Model;
	float Velocidade;

public:
	Objeto() {}
	Objeto(TipoObjeto tipo) {
		Velocidade = 5.0f;
		Tipo = tipo;
		Model = glm::scale(glm::mat4(1), glm::vec3(0.2f, 0.2f, 0.2f));
	}

public:
	GLuint ObterVAO() {
		return Tipo.ObterVAO();
	}

	GLuint* ObterEnderecoVAO() {
		return Tipo.ObterEndereçoVAO();
	}

	GLuint ObterIdTextura() {
		return Tipo.ObterIdTextura();
	}

	int ObterQuantidadeVertices() {
		return Tipo.ObterQuantidadeVertices();
	}

	void Renderizar(Shader& shader) {
		shader.setMat4("model", glm::value_ptr(Model));
		shader.setFloat("ka", Tipo.ObterKa());
		shader.setFloat("kd", Tipo.ObterKd());
		shader.setFloat("ks", Tipo.ObterKs());
		shader.setFloat("q", Tipo.ObterQ());

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, ObterIdTextura());

		glBindVertexArray(ObterVAO());
		glDrawArrays(GL_TRIANGLES, 0, ObterQuantidadeVertices());
	}

	void RotacionarFrente(float delta) {
		Model = glm::rotate(Model, glm::radians(VelocidadeRotacao() * delta), glm::vec3(1.0f, 0.0f, 0.0f));
	}

	void RotacionarTras(float delta) {
		Model = glm::rotate(Model, glm::radians(-VelocidadeRotacao() * delta), glm::vec3(1.0f, 0.0f, 0.0f));
	}

	void RotacionarDireita(float delta) {
		Model = glm::rotate(Model, glm::radians(VelocidadeRotacao() * delta), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	void RotacionarEsquerda(float delta) {
		Model = glm::rotate(Model, glm::radians(-VelocidadeRotacao() * delta), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	void MoverDireta(float delta) {
		Mover(Velocidade * delta, 0, 0);
	}

	void MoverEsquerda(float delta) {
		Mover(-Velocidade * delta, 0, 0);
	}

	void MoverCima(float delta) {
		Mover(0, Velocidade * delta, 0);
	}

	void MoverBaixo(float delta) {
		Mover(0, -Velocidade * delta, 0);
	}

	void MoverFrente(float delta) {
		Mover(0, 0, -Velocidade * delta);
	}

	void MoverTras(float delta) {
		Mover(0, 0, Velocidade * delta);
	}

	void AumentarTamanho(float delta)
	{
		Model = glm::scale(Model, glm::vec3(VelocidadeEscala() * delta, VelocidadeEscala() * delta, VelocidadeEscala() * delta));
	}

	void DiminuirTamanho(float delta)
	{
		Model = glm::scale(Model, glm::vec3(-VelocidadeEscala() * delta, -VelocidadeEscala() * delta, -VelocidadeEscala() * delta));
	}

private:
	void Mover(float x, float y, float z) {
		Model = glm::translate(Model, glm::vec3(x, y, z));
	}

	float VelocidadeRotacao()
	{
		return Velocidade * 2.0f;
	}

	float VelocidadeEscala()
	{
		return Velocidade / 5.0f;
	}
};