#pragma once

#include <iostream>
#include <string>
#include <assert.h>
#include <sstream>
#include <fstream>
#include <vector>

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "stb_image.h"

class TipoObjeto
{
private:
	string Nome;
	int QuantidadeVertices;
	GLuint VAO;
	GLuint IdTextura;

	float Ka;
	float Kd;
	float Ks;
	float Q;
public:
	TipoObjeto() {}
	TipoObjeto(string diretorio, string modelo) {
		QuantidadeVertices = 0;
		Ka = Kd = Ks = 1.0f;
		Q = 1.0f;

		string diretorioImagem = CarregarObjeto(diretorio, modelo);
		CarregarTextura(diretorioImagem);
	}

public:
	string ObterNome() {
		return Nome;
	}

	int ObterQuantidadeVertices() {
		return QuantidadeVertices;
	}

	GLuint ObterVAO() {
		return VAO;
	}

	GLuint* ObterEndereçoVAO() {
		return &VAO;
	}

	GLuint ObterIdTextura() {
		return IdTextura;
	}

	float ObterKa() {
		return Ka;
	}

	float ObterKd() {
		return Kd;
	}

	float ObterKs() {
		return Ks;
	}

	float ObterQ() {
		return Q;
	}
private:
	string CarregarObjeto(string diretorio, string modelo) {
		Nome = modelo;

		std::string diretorioModelo = diretorio + modelo;
		std::string diretorioMtl, diretorioImagem;
		std::vector<GLfloat> vertices;

		ifstream inputFile;
		inputFile.open(diretorioModelo.c_str());

		if (!diretorioModelo.empty())
		{
			if (inputFile.is_open())
			{
				std::vector<std::vector<GLfloat>> vectors;
				std::vector<std::vector<GLfloat>> normalVectors;
				std::vector<std::vector<GLfloat>> textureVectors;

				int indexVectors = 0;
				int indexTexture = 1;
				int indexNormals = 2;

				std::string line;

				while (!inputFile.eof() && std::getline(inputFile, line))
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

							vertices.insert(vertices.end(), vectors[modelsLine[indexVectors]][0]);
							vertices.insert(vertices.end(), vectors[modelsLine[indexVectors]][1]);
							vertices.insert(vertices.end(), vectors[modelsLine[indexVectors]][2]);
							vertices.insert(vertices.end(), 0.5f);
							vertices.insert(vertices.end(), 0.5f);
							vertices.insert(vertices.end(), 0.5f);
							vertices.insert(vertices.end(), textureVectors[modelsLine[indexTexture]][0]);
							vertices.insert(vertices.end(), textureVectors[modelsLine[indexTexture]][1]);
							vertices.insert(vertices.end(), normalVectors[modelsLine[indexNormals]][0]);
							vertices.insert(vertices.end(), normalVectors[modelsLine[indexNormals]][1]);
							vertices.insert(vertices.end(), normalVectors[modelsLine[indexNormals]][2]);

							QuantidadeVertices++;
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

				inputFile.close();
			}
		}

		if (!diretorioMtl.empty())
		{
			inputFile.open(diretorioMtl.c_str());
			std::string line;

			while (!inputFile.eof() && std::getline(inputFile, line))
			{
				std::vector<string> tokens;
				std::istringstream tokenizer { line };
				std::string token;

				// separa as string por espaço e coloca no vetor destino
				while (tokenizer >> token)
				{
					tokens.push_back(token);
				}

				if ((int)line.find("map_Kd") >= 0)
				{
					diretorioImagem = diretorio + tokens[1];
				}
				else if ((int)line.find("Ka") >= 0)
				{
					if (tokens.size() > 2)
					{
						Ka = std::stof(tokens[1]);
					}
				}
				else if ((int)line.find("Kd") >= 0)
				{
					if (tokens.size() > 2)
					{
						Kd = std::stof(tokens[1]);
					}
				}
				else if ((int)line.find("Ks") >= 0)
				{
					if (tokens.size() > 2)
					{
						Ks = std::stof(tokens[1]);
					}
				}
				else if ((int)line.find("q") >= 0)
				{
					if (tokens.size() > 2)
					{
						Q = std::stof(tokens[1]);
					}
				}
			}
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
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		//Atributo cor (r, g, b)
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (void*)(8 * sizeof(GLfloat)));
		glEnableVertexAttribArray(3);

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
};