#pragma warning(disable : 4996)
/* Hello Triangle - c�digo adaptado de https://learnopengl.com/#!Getting-started/Hello-Triangle
 *
 * Adaptado por Rossana Baptista Queiroz
 * para a disciplina de Processamento Gr�fico - Jogos Digitais - Unisinos
 * Vers�o inicial: 7/4/2017
 * �ltima atualiza��o em 02/03/2022
 *
 */

#include <iostream>
#include <string>
#include <assert.h>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <chrono>

#include "objeto.cpp"

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

// Carregar os tipos dos objetos contidos no diret�rio dos modelos
void carregarTiposObjetos(string diretorio);

// Prot�tipo da fun��o de callback de teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// Valida se pressionou ou soltou o bot�o
bool pressRelease(int action);

// Executa a interacao dos comandos com objeto
void executarAcaoObjeto(Objeto &objeto);

// Print das instru��es de funcionamento
void mostrarInstrucoes();

// Mostra os tipos de objetos carregados
int selecionarTipoNovoObjeto();

// Dimens�es da janela (pode ser alterado em tempo de execu��o)
const GLuint WIDTH = 1000, HEIGHT = 1000;

// Diret�rios dos modelos
std::string diretorioModelos = "../M4/objetos/";

bool rotateUp = false, rotateDown = false, rotateLeft = false, rotateRight = false, rotateAngleLeft = false, rotateAngleRight = false;
bool moveUp = false, moveDown = false, moveLeft = false, moveRight = false, moveFront = false, moveBack = false;
bool bigger = false, smaller = false;

bool createObject = true;
int selectedObject = 0;
vector<TipoObjeto> tiposObjetos;
vector<Objeto> objetos;

// Fun��o MAIN
int main()
{
	// Inicializa��o da GLFW
	glfwInit();

	//Muita aten��o aqui: alguns ambientes n�o aceitam essas configura��es
	//Voc� deve adaptar para a vers�o do OpenGL suportada por sua placa
	//Sugest�o: comente essas linhas de c�digo para desobrir a vers�o e
	//depois atualize (por exemplo: 4.5 com 4 e 5)
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//Essencial para computadores da Apple
//#ifdef __APPLE__
//	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//#endif

	// Cria��o da janela GLFW
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "M4 -- Pedro Evaldt Machado.", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Fazendo o registro da fun��o de callback para a janela GLFW
	glfwSetKeyCallback(window, key_callback);

	// GLAD: carrega todos os ponteiros d fun��es da OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
	}

	// Obtendo as informa��es de vers�o
	const GLubyte* renderer = glGetString(GL_RENDERER); /* get renderer string */
	const GLubyte* version = glGetString(GL_VERSION); /* version as a string */
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;

	// Definindo as dimens�es da viewport com as mesmas dimens�es da janela da aplica��o
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	// Compilando e buildando o programa de shader
	Shader shader = Shader("../M4/shaders/hello.vs", "../M4/shaders/hello.fs");

	glUseProgram(shader.ID);

	GLint modelLoc = glGetUniformLocation(shader.ID, "model");
	glUniform1i(glGetUniformLocation(shader.ID, "tex_buffer"), 0);
	glEnable(GL_DEPTH_TEST);

	carregarTiposObjetos(diretorioModelos);

	mostrarInstrucoes();

	// Loop da aplica��o - "game loop"
	while (!glfwWindowShouldClose(window))
	{
		// Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as fun��es de callback correspondentes
		glfwPollEvents();

		// Limpa o buffer de cor
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //cor de fundo
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (createObject)
		{
			// l�gica para proximo objeto em tela ser diferente.
			int novoObjetoTipo = selecionarTipoNovoObjeto();

			Objeto novoObjeto = Objeto(tiposObjetos[novoObjetoTipo]);
			objetos.insert(objetos.end(), novoObjeto);

			createObject = false;

			selectedObject = objetos.size() - 1;
		}

		for (int i = 0; i < objetos.size(); i++)
		{
			if (i == selectedObject)
			{
				executarAcaoObjeto(objetos[i]);
			}

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(objetos[i].ObtetModel()));

			struct tm* data_hora_atual;
			time_t segundos;
			time(&segundos);
			data_hora_atual = localtime(&segundos);

			uint64_t ms = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();

			glActiveTexture(GL_TEXTURE0);

			if (i != selectedObject
				|| i == selectedObject && ms % 1500 < 1000)
			{
				glBindTexture(GL_TEXTURE_2D, objetos[i].ObterIdTextura());
			}
			else 
			{
				glBindTexture(GL_TEXTURE_2D, 0);
			}

			// Chamada de desenho - drawcall
			// Poligono Preenchido - GL_TRIANGLES (12 TRIANGLES * 3 PONTOS)
			glBindVertexArray(objetos[i].ObterVAO());
			glDrawArrays(GL_TRIANGLES, 0, objetos[i].ObterQuantidadeVertices());
		}

		moveUp = moveDown = moveLeft = moveRight = false;

		// Troca os buffers da tela
		glfwSwapBuffers(window);
	}

	for (int i = 0; i < objetos.size(); i++)
	{
		// Pede pra OpenGL desalocar os buffers
		glDeleteVertexArrays(1, objetos[i].ObterEnderecoVAO());
	}
	
	// Finaliza a execu��o da GLFW, limpando os recursos alocados por ela
	glfwTerminate();

	return 0;
}

void carregarTiposObjetos(string diretorio)
{
	vector<string> files;

	DIR* dirp = opendir(diretorio.c_str());
	struct dirent* dp;

	while ((dp = readdir(dirp)) != NULL) {
		std::string file_name(dp->d_name);
		if (file_name.size() >= 4 && file_name.substr(file_name.size() - 4) == ".obj") {
			files.push_back(file_name);
		}
	}
	
	closedir(dirp);

	for (int i = 0; i < files.size(); i++)
	{
		string modelo = "";

		TipoObjeto novoTipoObjeto = TipoObjeto(diretorio, files[i]);
		tiposObjetos.insert(tiposObjetos.end(), novoTipoObjeto);
	}
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_KP_8)
	{
		rotateUp = pressRelease(action);
	}

	if (key == GLFW_KEY_KP_2)
	{
		rotateDown = pressRelease(action);
	}

	if (key == GLFW_KEY_KP_4)
	{
		rotateLeft = pressRelease(action);
	}

	if (key == GLFW_KEY_KP_6)
	{
		rotateRight = pressRelease(action);
	}

	if (key == GLFW_KEY_KP_7)
	{
		rotateAngleLeft = pressRelease(action);
	}

	if (key == GLFW_KEY_KP_9)
	{
		rotateAngleRight = pressRelease(action);
	}

	if (key == GLFW_KEY_W)
	{
		moveUp = pressRelease(action);
	}

	if (key == GLFW_KEY_S)
	{
		moveDown = pressRelease(action);
	}

	if (key == GLFW_KEY_A)
	{
		moveLeft = pressRelease(action);
	}

	if (key == GLFW_KEY_D)
	{
		moveRight = pressRelease(action);
	}

	if (key == GLFW_KEY_F)
	{
		moveFront = pressRelease(action);
	}

	if (key == GLFW_KEY_B)
	{
		moveBack = pressRelease(action);
	}

	if (key == GLFW_KEY_KP_ADD)
	{
		bigger = pressRelease(action);
	}

	if (key == GLFW_KEY_KP_SUBTRACT)
	{
		smaller = pressRelease(action);
	}

	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
	{
		createObject = true;
	}
	else if (key == GLFW_KEY_BACKSPACE && action == GLFW_PRESS)
	{
		objetos.resize(objetos.size() - 1);
		selectedObject = objetos.size() - 1;
	}

	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
	{
		if (selectedObject > 0)
		{
			selectedObject--;
		}
		else
		{
			selectedObject = objetos.size() - 1;
		}
	}

	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
	{
		if (selectedObject < (objetos.size() - 1))
		{
			selectedObject++;
		}
		else
		{
			selectedObject = 0;
		}
	}
}

bool pressRelease(int action)
{
	if (action == GLFW_PRESS)
	{
		return true;
	}
	else if (action == GLFW_RELEASE)
	{
		return false;
	}
}

void executarAcaoObjeto(Objeto& objeto)
{
	if (rotateUp && !rotateDown)
	{
		objeto.RotacionarX(1);
	}
	else if (rotateDown && !rotateUp)
	{
		objeto.RotacionarX(-1);
	}

	if (rotateLeft && !rotateRight)
	{
		objeto.RotacionarY(1);
	}
	else if (rotateRight && !rotateLeft)
	{
		objeto.RotacionarY(-1);
	}

	if (rotateAngleRight && !rotateAngleLeft)
	{
		objeto.RotacionarZ(1);
	}
	else if (rotateAngleLeft && !rotateAngleRight)
	{
		objeto.RotacionarZ(-1);
	}

	if (moveUp && !moveDown)
	{
		objeto.MoverY(0.01f);
	}
	if (moveDown && !moveUp)
	{
		objeto.MoverY(-0.01f);
	}

	if (moveRight && !moveLeft)
	{
		objeto.MoverX(0.01f);
	}
	if (moveLeft && !moveRight)
	{
		objeto.MoverX(-0.01f);
	}

	if (moveFront && !moveBack)
	{
		objeto.MoverZ(-0.01f);
	}
	if (moveBack && !moveFront)
	{
		objeto.MoverZ(0.01f);
	}

	if (bigger && !smaller)
	{
		objeto.AlterarTamanho(0.002f);
	}
	else if (smaller && !bigger)
	{
		objeto.AlterarTamanho(-0.002f);
	}
}

void mostrarInstrucoes() {
	cout << "\nO objeto selecionado ira aparecer piscando" << endl;
	cout << "'<' '>'\t\t\t\t- Mudar a selecao clicar nas setinhas de esquerda e direita do teclado" << endl;
	cout << "WASD\t\t\t\t- Mover objeto selecionado" << endl;
	cout << "R e F\t\t\t\t- Mover no eixo Z (frente <> traz) objeto selecionado" << endl;
	cout << "4,5,6,7,8,9 do teclado numerico\t- Rotacionar o objeto selecionado" << endl;
	cout << "'+' e '-' do teclado numerico\t- Aumentar e dimuir objeto selecionado" << endl;
	cout << "'ENTER'\t\t\t\t- Adicionar objeto na cena, escolher no console o novo tipo do objeto" << endl;
	cout << "'BACKSPACE'\t\t\t- Remover objeto selecionado da cena" << endl;
}

int selecionarTipoNovoObjeto() 
{
	cout << "\nSeleciona um dos tipos de objetos: " << endl;

	for (int i = 0; i < tiposObjetos.size(); i++)
	{
		cout << " " + to_string(i + 1) + string(" - ") + tiposObjetos[i].ObterNome() << endl;
	}

	int selecionado;

	do
	{
		string input;
		cout << "Opcao: ";
		getline(cin, input);

		try {
			int i = std::stoi(input);

			selecionado = i;
		}
		catch (std::invalid_argument const& e) {
			selecionado = -1;
		}
	} while (selecionado <= 0 || selecionado > tiposObjetos.size());

	return --selecionado;
}