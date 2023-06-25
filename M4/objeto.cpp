#include <iostream>
#include <string>
#include <assert.h>
#include <sstream>
#include <fstream>

#include "tipoObjeto.cpp"

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

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
		float RotacaoX;
		float RotacaoY;
		float RotacaoZ;
		glm::vec3 Escala;
		glm::vec3 Posicao;

	public:
		Objeto() {}
		Objeto(TipoObjeto tipo) {
			Tipo = tipo;

			RotacaoX = RotacaoY = RotacaoZ = 0;
			Posicao = glm::vec3(0.0f, 0.0f, 0.0f);
			Escala = glm::vec3(0.2f, 0.2f, 0.2f);
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

		glm::mat4 ObterModel() {
			glm::mat4 model = glm::mat4(1);

			model = glm::translate(model, Posicao);
			model = glm::scale(model, Escala);

			model = glm::rotate(model, RotacaoY, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::rotate(model, RotacaoX, glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::rotate(model, RotacaoZ, glm::vec3(0.0f, 0.0f, 1.0f));

			return model;
		}

		void Renderizar(Shader &shader) {
			GLint modelLoc = glGetUniformLocation(shader.ID, "model");
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(ObterModel()));

			shader.setFloat("ka", Tipo.ObterKa());
			shader.setFloat("kd", Tipo.ObterKd());
			shader.setFloat("ks", Tipo.ObterKs());
			shader.setFloat("q", Tipo.ObterQ());

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, ObterIdTextura());

			glBindVertexArray(ObterVAO());
			glDrawArrays(GL_TRIANGLES, 0, ObterQuantidadeVertices());
		}

		void RotacionarX(float intensidade) {
			RotacaoX += glm::radians(intensidade);
		}

		void RotacionarY(float intensidade) {
			RotacaoY += glm::radians(intensidade);
		}

		void RotacionarZ(float intensidade) {
			RotacaoZ += glm::radians(intensidade);
		}

		void MoverX(float intensidade) {
			Mover(intensidade, 0, 0);
		}

		void MoverY(float intensidade) {
			Mover(0, intensidade, 0);
		}

		void MoverZ(float intensidade) {
			Mover(0, 0, intensidade);
		}

		void AlterarTamanho(float intensidade)
		{
			Escala += glm::vec3(intensidade, intensidade, intensidade);
		}
	private:
		void Mover(float x, float y, float z) {
			Posicao += glm::vec3(x, y, z);
		}
};