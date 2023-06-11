#include <iostream>
#include <string>
#include <assert.h>
#include <sstream>
#include <fstream>

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
		int QuantidadeVertices;
		float RotacaoX;
		float RotacaoY;
		float RotacaoZ;
		glm::vec3 Escala;
		glm::vec3 Posicao;
	public:
		GLuint VAO;
		GLuint IdTextura;

	public:
		Objeto() {
			QuantidadeVertices = RotacaoX = RotacaoY = RotacaoZ = 0;
			Posicao = glm::vec3(0.0f, 0.0f, 0.0f);
			Escala = glm::vec3(1.0f, 1.0f, 1.0f);
		}

	public:
		void IniciarObjeto(string diretorio, string modelo) {
			string diretorioImagem = CarregarObjeto(diretorio, modelo);
			CarregarTextura(diretorioImagem);
		}

		int ObterQuantidadeVertices() {
			return QuantidadeVertices;
		}

		glm::mat4 ObtetModel() {
			glm::mat4 model = glm::mat4(1);

			model = glm::translate(model, Posicao);
			model = glm::scale(model, Escala);

			model = glm::rotate(model, RotacaoX, glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::rotate(model, RotacaoY, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::rotate(model, RotacaoZ, glm::vec3(0.0f, 0.0f, 1.0f));

			return model;
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
		string CarregarObjeto(string diretorio, string modelo) {
			std::string diretorioModelo = diretorio + modelo;
			std::string diretorioMtl, diretorioImagem;

			std::vector<std::vector<int>> models;
			std::vector<std::vector<GLfloat>> vectors;
			std::vector<std::vector<GLfloat>> normalVectors;
			std::vector<std::vector<GLfloat>> textureVectors;

			std::ifstream infileModelo(diretorioModelo);
			std::string line;

			while (std::getline(infileModelo, line))
			{
				if (line[0] == 'v')
				{
					if (line[1] == 't')
					{
						std::vector<string> tokens;
						std::istringstream tokenizer{ line };
						std::string token;

						// separa as string por espaço e coloca no vetor destino
						while (tokenizer >> token)
						{
							tokens.push_back(token);
						}

						std::vector<GLfloat> textureLine;

						textureLine.insert(textureLine.end(), std::stof(tokens[1]));
						textureLine.insert(textureLine.end(), std::stof(tokens[2]));

						textureVectors.insert(textureVectors.end(), textureLine);
					}
					if (line[1] == 'n')
					{
						std::vector<string> tokens;
						std::istringstream tokenizer{ line };
						std::string token;

						// separa as string por espaço e coloca no vetor destino
						while (tokenizer >> token)
						{
							tokens.push_back(token);
						}

						std::vector<GLfloat> normalLine;

						normalLine.insert(normalLine.end(), std::stof(tokens[1]));
						normalLine.insert(normalLine.end(), std::stof(tokens[2]));
						normalLine.insert(normalLine.end(), std::stof(tokens[3]));

						normalVectors.insert(normalVectors.end(), normalLine);
					}
					else if (line[1] == ' ')
					{
						std::vector<string> tokens;
						std::istringstream tokenizer{ line };
						std::string token;

						// separa as string por espaço e coloca no vetor destino
						while (tokenizer >> token)
						{
							tokens.push_back(token);
						}

						std::vector<GLfloat> vectorLine;

						vectorLine.insert(vectorLine.end(), std::stof(tokens[1]));
						vectorLine.insert(vectorLine.end(), std::stof(tokens[2]));
						vectorLine.insert(vectorLine.end(), std::stof(tokens[3]));

						vectors.insert(vectors.end(), vectorLine);
					}
				}
				else if (line[0] == 'f' && line[1] == ' ')
				{
					std::vector<string> tokens;
					std::istringstream tokenizer { line };
					std::string token;

					// separa as string por espaço e coloca no vetor destino
					while (tokenizer >> token)
					{
						tokens.push_back(token);
					}

					for (int iToken = 1; iToken < tokens.size(); iToken++)
					{
						std::string delimiter = "/";
						std::vector<int> modelsLine;

						std::string tokenBarra = tokens[iToken].substr(0, tokens[iToken].find(delimiter));
						modelsLine.insert(modelsLine.end(), (std::stoi(tokenBarra) - 1));

						std::string aux = tokens[iToken].substr(tokens[iToken].find(delimiter) + 1, tokens[iToken].length());
						tokenBarra = aux.substr(0, aux.find(delimiter));
						modelsLine.insert(modelsLine.end(), (std::stoi(tokenBarra) - 1));

						tokenBarra = aux.substr(aux.find(delimiter) + 1, aux.length());
						modelsLine.insert(modelsLine.end(), (std::stoi(tokenBarra) - 1));

						models.insert(models.end(), modelsLine);
					}
				}
				else if ((int)line.find("mtllib") >= 0)
				{
					std::vector<string> tokens;
					std::istringstream tokenizer { line };
					std::string token;

					// separa as string por espaço e coloca no vetor destino
					while (tokenizer >> token)
					{
						tokens.push_back(token);
					}

					diretorioMtl = diretorio + tokens[1];
				}
			}

			std::ifstream infileMtl(diretorioMtl);

			while (std::getline(infileMtl, line))
			{
				if ((int)line.find("map_Kd") >= 0)
				{
					std::vector<string> tokens;
					std::istringstream tokenizer { line };
					std::string token;

					// separa as string por espaço e coloca no vetor destino
					while (tokenizer >> token)
					{
						tokens.push_back(token);
					}

					diretorioImagem = diretorio + tokens[1];
				}
			}

			QuantidadeVertices = models.size();
			std::vector<GLfloat> vertices;

			int indexVectors = 0;
			int indexTexture = 1;
			int indexNormals = 2;

			for (int i = 0; i < models.size(); i++)
			{
				vertices.insert(vertices.end(), vectors[models[i][indexVectors]][0]);
				vertices.insert(vertices.end(), vectors[models[i][indexVectors]][1]);
				vertices.insert(vertices.end(), vectors[models[i][indexVectors]][2]);
				vertices.insert(vertices.end(), 0.5f);
				vertices.insert(vertices.end(), 0.5f);
				vertices.insert(vertices.end(), 0.5f);
				vertices.insert(vertices.end(), textureVectors[models[i][indexTexture]][0]);
				vertices.insert(vertices.end(), textureVectors[models[i][indexTexture]][1]);
			}

			GLuint VBO;

			//Geração do identificador do VBO
			glGenBuffers(1, &VBO);

			//Faz a conexão (vincula) do buffer como um buffer de array
			glBindBuffer(GL_ARRAY_BUFFER, VBO);

			//Envia os dados do array de floats para o buffer da OpenGl
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);

			//Geração do identificador do VAO (Vertex Array Object)
			glGenVertexArrays(1, &VAO);

			// Vincula (bind) o VAO primeiro, e em seguida  conecta e seta o(s) buffer(s) de vértices
			// e os ponteiros para os atributos 
			glBindVertexArray(VAO);

			//Para cada atributo do vertice, criamos um "AttribPointer" (ponteiro para o atributo), indicando: 
			// Localização no shader * (a localização dos atributos devem ser correspondentes no layout especificado no vertex shader)
			// Numero de valores que o atributo tem (por ex, 3 coordenadas xyz) 
			// Tipo do dado
			// Se está normalizado (entre zero e um)
			// Tamanho em bytes 
			// Deslocamento a partir do byte zero 

			//Atributo posição (x, y, z)
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(0);

			//Atributo cor (r, g, b)
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
			glEnableVertexAttribArray(1);

			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
			glEnableVertexAttribArray(2);

			// Observe que isso é permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de vértice 
			// atualmente vinculado - para que depois possamos desvincular com segurança
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			// Desvincula o VAO (é uma boa prática desvincular qualquer buffer ou array para evitar bugs medonhos)
			glBindVertexArray(0);

			return diretorioImagem;
		}

		void CarregarTextura(string diretorioImagem) {
			// Gera o identificador da textura na memória 
			glGenTextures(1, &IdTextura);
			glBindTexture(GL_TEXTURE_2D, IdTextura);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			int width, height, nrChannels;
			unsigned char* data = stbi_load(diretorioImagem.c_str(), &width, &height, &nrChannels, 0);

			if (data)
			{
				if (nrChannels == 3) //jpg, bmp
				{
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				}
				else //png
				{
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
				}

				glGenerateMipmap(GL_TEXTURE_2D); // geração do mipmap
			}
			else
			{
				std::cout << "Failed to load texture" << std::endl;
			}

			stbi_image_free(data);
		}

		void Mover(float x, float y, float z) {
			Posicao += glm::vec3(x, y, z);
		}
};