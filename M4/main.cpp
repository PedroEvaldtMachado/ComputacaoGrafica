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

#include "Objeto.cpp"

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

// Obtem o modelo dentro das imagens pr�-definidas
string obterArquivoModelo(int imagem);

// Prot�tipo da fun��o de callback de teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// Valida se pressionou ou soltou o bot�o
bool pressRelease(int action);

// Executa a interacao dos comandos com objeto
void executarAcao(Objeto &objeto);

// Print das instru��es de funcionamento
void mostrarInstrucoes();

// Dimens�es da janela (pode ser alterado em tempo de execu��o)
const GLuint WIDTH = 1000, HEIGHT = 1000;

// Diret�rios dos modelos
std::string diretorioModelos = "../M4/objetos/";

// 1 = Cubo, 2 = Bola, 3 = Suzanne
const int imagem = 1;

bool rotateUp = false, rotateDown = false, rotateLeft = false, rotateRight = false, rotateAngleLeft = false, rotateAngleRight = false;
bool moveUp = false, moveDown = false, moveLeft = false, moveRight = false, moveFront = false, moveBack = false;
bool bigger = false, smaller = false;
int qtdObjetos = 0;
int selectedObject = 0;
bool createObject = true;

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

	mostrarInstrucoes();

	vector<Objeto> objetos;

	// Loop da aplica��o - "game loop"
	while (!glfwWindowShouldClose(window))
	{
		// Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as fun��es de callback correspondentes
		glfwPollEvents();

		// Limpa o buffer de cor
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //cor de fundo
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glLineWidth(10);
		glPointSize(20);

		if (createObject)
		{
			Objeto novoObjeto = Objeto();
			string modelo = obterArquivoModelo((qtdObjetos % 3) + 1);

			novoObjeto.IniciarObjeto(diretorioModelos, modelo);
			objetos.insert(objetos.end(), novoObjeto);

			createObject = false;
		}

		qtdObjetos = objetos.size();

		for (int i = 0; i < objetos.size(); i++)
		{
			if (i == selectedObject)
			{
				executarAcao(objetos[i]);
			}

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(objetos[i].ObtetModel()));

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, objetos[i].IdTextura);

			// Chamada de desenho - drawcall
			// Poligono Preenchido - GL_TRIANGLES (12 TRIANGLES * 3 PONTOS)
			glBindVertexArray(objetos[i].VAO);
			glDrawArrays(GL_TRIANGLES, 0, objetos[i].ObterQuantidadeVertices());
		}

		moveUp = moveDown = moveLeft = moveRight = false;

		// Troca os buffers da tela
		glfwSwapBuffers(window);
	}

	for (int i = 0; i < objetos.size(); i++)
	{
		// Pede pra OpenGL desalocar os buffers
		glDeleteVertexArrays(1, &objetos[i].VAO);
	}
	
	// Finaliza a execu��o da GLFW, limpando os recursos alocados por ela
	glfwTerminate();

	return 0;
}

string obterArquivoModelo(int imagem)
{
	string modelo;

	if (imagem == 1)
	{
		modelo = "CuboTextured.obj";
	}
	else if (imagem == 2)
	{
		modelo = "bola.obj";
	}
	else if (imagem == 3)
	{
		modelo = "suzanneTri.obj";
	}

	return modelo;
}

// Fun��o de callback de teclado - s� pode ter uma inst�ncia (deve ser est�tica se
// estiver dentro de uma classe) - � chamada sempre que uma tecla for pressionada
// ou solta via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_X || key == GLFW_KEY_KP_8)
	{
		rotateUp = pressRelease(action);
	}

	if (key == GLFW_KEY_KP_2)
	{
		rotateDown = pressRelease(action);
	}

	if (key == GLFW_KEY_Y || key == GLFW_KEY_KP_4)
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

	if (key == GLFW_KEY_Z || key == GLFW_KEY_KP_9)
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

	if (key == GLFW_KEY_1 && action == GLFW_PRESS)
	{
		selectedObject = 0;
	}
	else if (key == GLFW_KEY_2 && action == GLFW_PRESS)
	{
		selectedObject = 1;
	}
	else if (key == GLFW_KEY_3 && action == GLFW_PRESS)
	{
		selectedObject = 2;
	}
	else if (key == GLFW_KEY_4 && action == GLFW_PRESS)
	{
		selectedObject = 3;
	}
	else if (key == GLFW_KEY_5 && action == GLFW_PRESS)
	{
		selectedObject = 4;
	}
	else if (key == GLFW_KEY_6 && action == GLFW_PRESS)
	{
		selectedObject = 5;
	}
	else if (key == GLFW_KEY_7 && action == GLFW_PRESS)
	{
		selectedObject = 6;
	}
	else if (key == GLFW_KEY_8 && action == GLFW_PRESS)
	{
		selectedObject = 7;
	}
	else if (key == GLFW_KEY_9 && action == GLFW_PRESS)
	{
		selectedObject = 8;
	}
	else if (key == GLFW_KEY_0 && action == GLFW_PRESS)
	{
		selectedObject = 9;
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
		if (qtdObjetos < 9)
		{
			createObject = true;
			selectedObject = qtdObjetos;
		}
		else
		{
			cout << "M�ximo de objectos criados permitidos" << endl;
		}
	}
	else if (key == GLFW_KEY_BACKSPACE && action == GLFW_PRESS)
	{
		if (qtdObjetos > 0)
		{
			selectedObject = qtdObjetos - 2;
		}
		else
		{
			cout << "M�ximo de objectos excluidos permitidos" << endl;
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

void executarAcao(Objeto &objeto) 
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
		objeto.AlterarTamanho(0.01f);
	}
	else if (smaller && !bigger)
	{
		objeto.AlterarTamanho(-0.01f);
	}
}

void mostrarInstrucoes() {
	cout << "Maximo de objetos 10, sendo em ordem de cria��o, primeiro fica no '1' (criado junto com a tela), os seguintes nos numeros em ordem, sendo o decimo no 0" << endl;
	cout << "WASD para mover objeto selecionado" << endl;
	cout << "F e B para mover no eixo Z (frente <> traz) objeto selecionado" << endl;
	cout << "XYZ para rotacionar o objeto selecionado" << endl;
	cout << "2,4,6,7,8,9 do teclado numerico para rotacionar o objeto selecionado" << endl;
	cout << "'+' e '-' do teclado numerico para aumentar e dimuir objeto" << endl;
	cout << "'ENTER' e 'BACKSPACE' para adicionar e remover objetos" << endl;
}