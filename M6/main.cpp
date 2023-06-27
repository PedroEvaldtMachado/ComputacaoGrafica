#pragma warning(disable : 4996)

#include <iostream>
#include <string>
#include <assert.h>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <chrono>
#include <vector>

#include "tipoObjeto.h"
#include "objeto.h"
#include "estrutura.h"

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Carregar os tipos dos objetos contidos no diretório dos modelos
void carregarTiposObjetos(string diretorio);

// Protótipo da função de callback do scroll
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// Protótipo da função de callback de teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// Valida se pressionou ou soltou o botão
bool pressRelease(int action, bool estado);

// Executa a interacao dos comenados com camera
void executarAcaoCamera(GLFWwindow* window, Camera& camera, float deltaTime);

// Executa a interacao dos comandos com objeto
void executarAcaoObjeto(Objeto &objeto, float deltaTime);

// Print das instruções de funcionamento
void mostrarInstrucoes();

// Mostra os tipos de objetos carregados
int selecionarTipoNovoObjeto();

// Dimensões da janela (pode ser alterado em tempo de execução)
const GLuint WIDTH = 1000, HEIGHT = 1000;

// Diretórios dos modelos
std::string diretorioModelos = "../M6/objetos/";

bool rotateUp = false, rotateDown = false, rotateLeft = false, rotateRight = false;
bool moveUp = false, moveDown = false, moveLeft = false, moveRight = false;
bool bigger = false, smaller = false;
bool alterarEstadoMover = false;

float scrollOffset;

bool createObject = true;
int selectedObject = 0;
vector<TipoObjeto> tiposObjetos;
vector<Objeto> objetos;

bool cameraSelecionada = false;

// Função MAIN
int main()
{
	// Inicialização da GLFW
	glfwInit();

	//Muita atenção aqui: alguns ambientes não aceitam essas configurações
	//Você deve adaptar para a versão do OpenGL suportada por sua placa
	//Sugestão: comente essas linhas de código para desobrir a versão e
	//depois atualize (por exemplo: 4.5 com 4 e 5)
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//Essencial para computadores da Apple
//#ifdef __APPLE__
//	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//#endif

	// Criação da janela GLFW
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "M6 -- Pedro Evaldt Machado.", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Fazendo o registro da função de callback para a janela GLFW
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, key_callback);

	// GLAD: carrega todos os ponteiros d funções da OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
	}

	// Obtendo as informações de versão
	const GLubyte* renderer = glGetString(GL_RENDERER); /* get renderer string */
	const GLubyte* version = glGetString(GL_VERSION); /* version as a string */
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;

	Estrutura estrutura = Estrutura(window, "../M6/shaders/vertex_shader.vs", "../M6/shaders/fragment_shader.fs");
	estrutura.NovaLuz(glm::vec3(-2.0, 10.0, 2.0));

	carregarTiposObjetos(diretorioModelos);
	mostrarInstrucoes();

	bool carregarTextura = true;
	float lastFrame = 0.0f;

	// Loop da aplicação - "game loop"
	while (!glfwWindowShouldClose(window))
	{
		// Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as funções de callback correspondentes
		glfwPollEvents();

		// Limpa o buffer de cor
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //cor de fundo
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (createObject)
		{
			// lógica para proximo objeto em tela ser diferente.
			int novoObjetoTipo = selecionarTipoNovoObjeto();

			Objeto novoObjeto = Objeto(tiposObjetos[novoObjetoTipo]);
			objetos.insert(objetos.end(), novoObjeto);

			createObject = false;
			selectedObject = (int)objetos.size() - 1;
		}

		float currentFrame = (float)glfwGetTime();
		float deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		if (cameraSelecionada)
		{
			executarAcaoCamera(window, *(estrutura.ObterRefCamera()), deltaTime);
		}

		for (int i = 0; i < objetos.size(); i++)
		{
			if (!cameraSelecionada && i == selectedObject)
			{
				executarAcaoObjeto(objetos[i], deltaTime);
			}

			objetos[i].AplicarMovimentoAutomatico(deltaTime);

			estrutura.CarregarEstrutura();
			objetos[i].Renderizar(*(estrutura.ObterRefShaderCena()));
		}

		moveUp = moveDown = moveLeft = moveRight = alterarEstadoMover = false;

		// Troca os buffers da tela
		glfwSwapBuffers(window);
	}

	for (int i = 0; i < objetos.size(); i++)
	{
		// Pede pra OpenGL desalocar os buffers
		glDeleteVertexArrays(1, objetos[i].ObterEnderecoVAO());
	}
	
	// Finaliza a execução da GLFW, limpando os recursos alocados por ela
	glfwTerminate();

	return 0;
}

void carregarTiposObjetos(string diretorio)
{
	std::cout << "Carregando texturas do diretorio..." << endl;

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

	int totalArquivos = (int)files.size();

	for (int i = 0; i < files.size(); i++)
	{
		string modelo = "";

		TipoObjeto novoTipoObjeto = TipoObjeto(diretorio, files[i]);
		tiposObjetos.insert(tiposObjetos.end(), novoTipoObjeto);

		std::cout << to_string(i + 1) + '/' + to_string(totalArquivos) << endl;
	}

	std::cout << "Finalizado!" << endl;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	scrollOffset += (float)yoffset;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_KP_8)
	{
		rotateUp = pressRelease(action, rotateUp);
	}

	if (key == GLFW_KEY_KP_5)
	{
		rotateDown = pressRelease(action, rotateDown);
	}

	if (key == GLFW_KEY_KP_4)
	{
		rotateLeft = pressRelease(action, rotateLeft);
	}

	if (key == GLFW_KEY_KP_6)
	{
		rotateRight = pressRelease(action, rotateRight);
	}

	if (key == GLFW_KEY_W)
	{
		moveUp = pressRelease(action, moveUp);
	}

	if (key == GLFW_KEY_S)
	{
		moveDown = pressRelease(action, moveDown);
	}

	if (key == GLFW_KEY_A)
	{
		moveLeft = pressRelease(action, moveLeft);
	}

	if (key == GLFW_KEY_D)
	{
		moveRight = pressRelease(action, moveRight);
	}

	if (key == GLFW_KEY_M && action == GLFW_PRESS)
	{
		alterarEstadoMover = true;
	}

	if (key == GLFW_KEY_KP_ADD)
	{
		bigger = pressRelease(action, bigger);
	}

	if (key == GLFW_KEY_KP_SUBTRACT)
	{
		smaller = pressRelease(action, smaller);
	}

	if (key == GLFW_KEY_O && action == GLFW_PRESS)
	{
		cameraSelecionada = false;
	}
	else if (key == GLFW_KEY_C && action == GLFW_PRESS)
	{
		cameraSelecionada = true;
	}

	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
	{
		cameraSelecionada = false;
		createObject = true;
	}
	else if (key == GLFW_KEY_BACKSPACE && action == GLFW_PRESS)
	{
		objetos.resize((int)objetos.size() - 1);
		selectedObject = (int)objetos.size() - 1;
	}

	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
	{
		if (selectedObject > 0)
		{
			selectedObject--;
		}
		else
		{
			selectedObject = (int)objetos.size() - 1;
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

	if (cameraSelecionada) 
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	else 
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}

bool pressRelease(int action, bool estado)
{
	if (action == GLFW_RELEASE)
	{
		return false;
	}
	else 
	{
		return true;
	}
}

void executarAcaoObjeto(Objeto& objeto, float deltaTime)
{
	if (rotateUp && !rotateDown)
	{
		objeto.RotacionarFrente(deltaTime);
	}
	else if (rotateDown && !rotateUp)
	{
		objeto.RotacionarTras(deltaTime);
	}

	if (rotateLeft && !rotateRight)
	{
		objeto.RotacionarDireita(deltaTime);
	}
	else if (rotateRight && !rotateLeft)
	{
		objeto.RotacionarEsquerda(deltaTime);
	}

	if (moveUp && !moveDown)
	{
		objeto.MoverFrente(deltaTime);
	}
	if (moveDown && !moveUp)
	{
		objeto.MoverTras(deltaTime);
	}

	if (moveRight && !moveLeft)
	{
		objeto.MoverDireta(deltaTime);
	}
	if (moveLeft && !moveRight)
	{
		objeto.MoverEsquerda(deltaTime);
	}

	if (bigger && !smaller)
	{
		objeto.AumentarTamanho(deltaTime);
	}
	else if (smaller && !bigger)
	{
		objeto.DiminuirTamanho(deltaTime);
	}

	if (alterarEstadoMover) 
	{
		if (objeto.EstaAutomatico())
		{
			objeto.Controlar();
		}
		else 
		{
			objeto.AutomatizarMovimento();
		}
	}
}

void executarAcaoCamera(GLFWwindow* window, Camera& camera, float deltaTime) {
	if (moveUp)
	{
		camera.MoverFrente(deltaTime);
	}
	if (moveDown)
	{
		camera.MoverTraz(deltaTime);
	}
	if (moveRight)
	{
		camera.GirarDireita(deltaTime);
	}
	if (moveLeft)
	{
		camera.GirarEsquerda(deltaTime);
	}

	if (rotateUp)
	{
		camera.RotacionarTras(deltaTime);
	}
	if (rotateDown)
	{
		camera.RotacionarFrente(deltaTime);
	}

	if (rotateRight)
	{
		camera.RotacionarDireita(deltaTime);
	}
	if (rotateLeft)
	{
		camera.RotacionarEsquerda(deltaTime);
	}

	camera.MoverComMouse(window);
	camera.AlterarFovComMouse(scrollOffset);

	scrollOffset = 0.0f;
}

void mostrarInstrucoes() {
	system("cls");
	cout << "Instruções:" << endl;
	cout << "'C'\t\t\t\t- Selecionar camera" << endl;
	cout << "'O'\t\t\t\t- Selecionar objetos" << endl;
	cout << "'M'\t\t\t\t- Automatizar movimento objeto selecionado" << endl;
	cout << "'<' '>'\t\t\t\t- Mudar o objeto selecionado" << endl;
	cout << "WASD\t\t\t\t- Mover objeto selecionado" << endl;
	cout << "4,5,6,8 do teclado numerico\t- Rotacionar o objeto selecionado" << endl;
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
		catch (std::invalid_argument) {
			selecionado = -1;
		}
	} while (selecionado <= 0 || selecionado > tiposObjetos.size());

	return --selecionado;
}