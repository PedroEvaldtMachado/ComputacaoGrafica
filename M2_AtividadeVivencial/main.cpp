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

using namespace std;

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include <vector>

// Prot�tipo da fun��o de callback de teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// Valida se pressionou ou soltou o bot�o
bool pressRelease(int action);

// Prot�tipos das fun��es
int setupGeometry();

// Dimens�es da janela (pode ser alterado em tempo de execu��o)
const GLuint WIDTH = 1000, HEIGHT = 1000;

bool rotateUp = false, rotateDown = false, rotateLeft = false, rotateRight = false, rotateAngleLeft = false, rotateAngleRight = false;
bool moveUp = false, moveDown = false, moveLeft = false, moveRight = false, moveFront = false, moveBack = false;
bool bigger = false, smaller = false;
int objectsRef = 0;
int selectedObject = 0;
bool createObject = true;

int qtdVertices = 0;

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
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "M2|Atividade vivencial -- Pedro Evaldt Machado.", nullptr, nullptr);
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
	Shader shader = Shader("../M2_AtividadeVivencial/shaders/hello.vs", "../M2_AtividadeVivencial/shaders/hello.fs");

	// Gerando um buffer simples, com a geometria de um tri�ngulo
	GLuint VAO = setupGeometry();

	glUseProgram(shader.ID);

	glm::mat4 model = glm::mat4(1); //matriz identidade;
	GLint modelLoc = glGetUniformLocation(shader.ID, "model");
	//
	//model = glm::rotate(model, /*(GLfloat)glfwGetTime()*/glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glEnable(GL_DEPTH_TEST);

	cout << "Maximo de objetos 10, sendo em ordem de cria��o, primeiro fica no '1' (criado junto com a tela), os seguintes nos numeros em ordem, sendo o decimo no 0" << endl;
	cout << "WASD para mover objeto selecionado" << endl;
	cout << "XYZ para rotacionar o objeto selecionado" << endl;
	cout << "'+' e '-' do teclado numerico para aumentar e dimuir objeto" << endl;
	cout << "'ENTER' e 'BACKSPACE' para adicionar e remover objetos" << endl;

	float rotateXObjects[10];
	float rotateYObjects[10];
	float rotateZObjects[10];
	glm::vec3 scales[10];
	glm::vec3 positionObjects[10];

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

		float angle = (GLfloat)glfwGetTime();

		for (int i = 0; i <= objectsRef; i++)
		{
			model = glm::mat4(1);

			if (i == selectedObject)
			{
				if (createObject)
				{
					rotateXObjects[i] = rotateYObjects[i] = rotateZObjects[i] = 0;
					positionObjects[i] = glm::vec3(0.0f, 0.0f, 0.0f);
					scales[i] = glm::vec3(1.0f, 1.0f, 1.0f);

					createObject = false;
				}

				if (rotateUp && !rotateDown)
				{
					rotateXObjects[i] += glm::radians(1.0f);
				}
				else if (rotateDown && !rotateUp)
				{
					rotateXObjects[i] -= glm::radians(1.0f);
				}

				if (rotateLeft && !rotateRight)
				{
					rotateYObjects[i] += glm::radians(1.0f);
				}
				else if (rotateRight && !rotateLeft)
				{
					rotateYObjects[i] -= glm::radians(1.0f);
				}

				if (rotateAngleRight && !rotateAngleLeft)
				{
					rotateZObjects[i] += glm::radians(1.0f);
				}
				else if (rotateAngleLeft && !rotateAngleRight)
				{
					rotateZObjects[i] -= glm::radians(1.0f);
				}

				if (moveUp && !moveDown)
				{
					positionObjects[i] += glm::vec3(0.0f, 0.01f, 0.0f);
				}
				if (moveDown && !moveUp)
				{
					positionObjects[i] -= glm::vec3(0.0f, 0.01f, 0.0f);
				}

				if (moveRight && !moveLeft)
				{
					positionObjects[i] += glm::vec3(0.01f, 0.0f, 0.0f);
				}
				if (moveLeft && !moveRight)
				{
					positionObjects[i] -= glm::vec3(0.01f, 0.0f, 0.0f);
				}

				if (moveFront && !moveBack)
				{
					positionObjects[i] -= glm::vec3(0.0f, 0.0f, 0.01f);
				}
				if (moveBack && !moveFront)
				{
					positionObjects[i] += glm::vec3(0.0f, 0.0f, 0.01f);
				}

				if (bigger && !smaller)
				{
					scales[i] += glm::vec3(0.01f, 0.01f, 0.01f);
				}
				else if (smaller && !bigger)
				{
					scales[i] -= glm::vec3(0.01f, 0.01f, 0.01f);
				}
			}

			model = glm::translate(model, positionObjects[i]);
			model = glm::scale(model, scales[i]);

			model = glm::rotate(model, rotateXObjects[i], glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::rotate(model, rotateYObjects[i], glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::rotate(model, rotateZObjects[i], glm::vec3(0.0f, 0.0f, 1.0f));

			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

			// Chamada de desenho - drawcall
			// Poligono Preenchido - GL_TRIANGLES (12 TRIANGLES * 3 PONTOS)
			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, qtdVertices);
		}

		moveUp = moveDown = moveLeft = moveRight = false;

		// Troca os buffers da tela
		glfwSwapBuffers(window);
	}

	// Pede pra OpenGL desalocar os buffers
	glDeleteVertexArrays(1, &VAO);
	// Finaliza a execu��o da GLFW, limpando os recursos alocados por ela
	glfwTerminate();

	return 0;
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
		if (objectsRef < 9)
		{
			createObject = true;
			objectsRef++;
			selectedObject = objectsRef;
		}
		else
		{
			cout << "M�ximo de objectos criados permitidos" << endl;
		}
	}
	else if (key == GLFW_KEY_BACKSPACE && action == GLFW_PRESS)
	{
		if (objectsRef > 0)
		{
			objectsRef--;
			selectedObject = objectsRef;
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

// Esta fun��o est� bastante harcoded - objetivo � criar os buffers que armazenam a 
// geometria de um tri�ngulo
// Apenas atributo coordenada nos v�rtices
// 1 VBO com as coordenadas, VAO com apenas 1 ponteiro para atributo
// A fun��o retorna o identificador do VAO
int setupGeometry()
{
	//std::ifstream infile("../M2_AtividadeVivencial/objetos/cube.obj");
	std::ifstream infile("../M2_AtividadeVivencial/objetos/suzanneTri.obj");
	std::string line;

	int indexModel = 0;
	std::vector<int> models;

	int indexVectors = 0;
	std::vector<std::vector<GLfloat>> vectors;

	while (std::getline(infile, line))
	{
		//std::istringstream iss(line);

		if (line[0] == 'v')
		{
			if (line[1] == 't')
			{
				//Ainda n�o
			}
			if (line[1] == 'n')
			{
				//Ainda n�o
			}
			else if (line[1] == ' ')
			{
				std::vector<string> tokens;
				std::istringstream tokenizer{ line };
				std::string token;

				// separa as string por espa�o e coloca no vetor destino
				while (tokenizer >> token)
				{
					tokens.push_back(token);
				}
				
				float teste0 = std::stof(tokens[1]);
				float teste1 = std::stof(tokens[2]);
				float teste2 = std::stof(tokens[3]);

				std::vector<GLfloat> vectorLine;

				vectorLine.insert(vectorLine.end(), std::stof(tokens[1]));
				vectorLine.insert(vectorLine.end(), std::stof(tokens[2]));
				vectorLine.insert(vectorLine.end(), std::stof(tokens[3]));

				vectors.insert(vectors.end(), vectorLine);

				indexVectors++;
			}
		}
		else if (line[0] == 'f' && line[1] == ' ')
		{
			std::vector<string> tokens;
			std::istringstream tokenizer { line };
			std::string token;

			// separa as string por espa�o e coloca no vetor destino
			while (tokenizer >> token)
			{
				tokens.push_back(token);
			}

			for (int iToken = 1; iToken < tokens.size(); iToken++)
			{
				std::string delimiter = "/";
				std::string tokenBarra = tokens[iToken].substr(0, tokens[iToken].find(delimiter));

				models.insert(models.end(), (std::stoi(tokenBarra) - 1));
			}

			indexModel++;
		}
	}

	qtdVertices = models.size();
	std::vector<GLfloat> vertices;

	for (int i = 0; i < models.size(); i++)
	{
		vertices.insert(vertices.end(), vectors[models[i]][0]);
		vertices.insert(vertices.end(), vectors[models[i]][1]);
		vertices.insert(vertices.end(), vectors[models[i]][2]);
		vertices.insert(vertices.end(), i % 2 == 0 ? 1.0f : 0.0f);
		vertices.insert(vertices.end(), i % 2 == 1 ? 1.0f : 0.0f);
		vertices.insert(vertices.end(), i % 2 == 0 ? 1.0f : 0.0f);
	}

	GLuint VBO, VAO;

	//Gera��o do identificador do VBO
	glGenBuffers(1, &VBO);

	//Faz a conex�o (vincula) do buffer como um buffer de array
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//Envia os dados do array de floats para o buffer da OpenGl
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);

	//Gera��o do identificador do VAO (Vertex Array Object)
	glGenVertexArrays(1, &VAO);

	// Vincula (bind) o VAO primeiro, e em seguida  conecta e seta o(s) buffer(s) de v�rtices
	// e os ponteiros para os atributos 
	glBindVertexArray(VAO);

	//Para cada atributo do vertice, criamos um "AttribPointer" (ponteiro para o atributo), indicando: 
	// Localiza��o no shader * (a localiza��o dos atributos devem ser correspondentes no layout especificado no vertex shader)
	// Numero de valores que o atributo tem (por ex, 3 coordenadas xyz) 
	// Tipo do dado
	// Se est� normalizado (entre zero e um)
	// Tamanho em bytes 
	// Deslocamento a partir do byte zero 

	//Atributo posi��o (x, y, z)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	//Atributo cor (r, g, b)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// Observe que isso � permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de v�rtice 
	// atualmente vinculado - para que depois possamos desvincular com seguran�a
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Desvincula o VAO (� uma boa pr�tica desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0);

	return VAO;
}
