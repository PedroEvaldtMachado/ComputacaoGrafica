#pragma once

#include <iostream>
#include <string>
#include <assert.h>
#include <sstream>
#include <fstream>
#include <vector>
#include <random>

// GLAD
#include <glad/glad.h>

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "stb_image.h"
#include "tipoObjeto.h"

class Objeto
{
private:
	TipoObjeto Tipo;
	glm::mat4 Model;
	glm::vec3 Escala;
	float Velocidade;

	bool MoverAutomaticamente;
	int PontosPorCiclo;
	int Ponto;
	float Movimento;
	float FreioMovimento;
	vector<glm::vec3> Pontos;

public:
	Objeto() {}
	Objeto(TipoObjeto tipo) {
		PontosPorCiclo = 4;
		Velocidade = 5.0f;
		FreioMovimento = Velocidade * 2.f;
		Movimento = 0.0f;

		MoverAutomaticamente = false;
		Tipo = tipo;
		Escala = glm::vec3(0.1f, 0.1f, 0.1f);
		Model = glm::scale(glm::mat4(1), Escala);
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
		Mover(0, 0, Velocidade * delta);
	}

	void MoverTras(float delta) {
		Mover(0, 0, -Velocidade * delta);
	}

	void AumentarTamanho(float delta)
	{
		Model = glm::scale(Model, glm::vec3(1) + (VelocidadeEscala() * delta));
	}

	void DiminuirTamanho(float delta)
	{
		Model = glm::scale(Model, glm::vec3(1) - (VelocidadeEscala() * delta));
	}

	void AutomatizarMovimento() 
	{
		MoverAutomaticamente = true;
		Movimento = Velocidade;
		GerarCaminho(1);
		Ponto = 0;
	}

	void Controlar() 
	{
		MoverAutomaticamente = false;
		Ponto = 0;
	}

	bool EstaAutomatico() 
	{
		return MoverAutomaticamente;
	}

	void AplicarMovimentoAutomatico(float delta) 
	{
		if (MoverAutomaticamente)
		{
			Movimento += delta;

			if (Movimento > FreioMovimento)
			{
				while (Movimento > FreioMovimento)
				{
					Movimento -= FreioMovimento;
				}

				// Redefine após passar 2 vezes pela sequencia
				Ponto = (Ponto + 4) % ((int)Pontos.size() * 2);
			}

			int voltar = 0;

			if (Ponto >= Pontos.size()) 
			{
				voltar = 1;
			}

			float qtdMovimento = abs(voltar - (Movimento / FreioMovimento));
			glm::vec3 destino = ObterPontoBezier(Ponto % 4, qtdMovimento);

			Model[3].x = 0.0f;
			Model[3].y = 0.0f;
			Model[3].z = 0.0f;

			Model = glm::translate(Model, destino);
		}
	}

	void Mover(float x, float y, float z) {
		Model = glm::translate(Model, glm::vec3(x, y, z));
	}

	void SobrescreverMatriz(glm::mat4 model) {
		Model = model;
	}

private:
	void GerarCaminho(int qtdCiclos)
	{
		std::random_device rd;
		std::default_random_engine eng(rd());
		std::uniform_int_distribution<int> distr(1, 1000);

		Pontos = vector<glm::vec3>();

		glm::vec3 posInicial = glm::vec3(Model[3].x, Model[3].y, Model[3].z);
		Pontos.insert(Pontos.end(), posInicial);

		for (int i = 1; i < qtdCiclos * PontosPorCiclo; i++)
		{
			Pontos.insert(Pontos.end(), glm::vec3(Rnd(distr, eng), Rnd(distr, eng), Rnd(distr, eng)));
		}
	}

	float Rnd(std::uniform_int_distribution<int>& distr, std::default_random_engine& eng) {
		return (distr(eng) - 500) / 100.f;
	}

	glm::vec3 ObterPontoBezier(int ponto, float t) {
		glm::vec3 valor;

		valor = (float)pow(1 - t, 3) * Pontos[ponto % (int)Pontos.size()]
			+ 3 * t * (float)pow(1 - t, 2) * Pontos[(ponto + 1) % (int)Pontos.size()]
			+ 3 * (float)pow(t, 2) * (1 - t) * Pontos[(ponto + 2) % (int)Pontos.size()]
			+ (float)pow(t, 3) * Pontos[(ponto + 3) % (int)Pontos.size()];

		return valor;
	}

	float VelocidadeRotacao()
	{
		return Velocidade * 5.0f;
	}

	float VelocidadeEscala()
	{
		return Velocidade / 5.0f;
	}
};
