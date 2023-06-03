/* Hello Triangle - código adaptado de https://learnopengl.com/#!Getting-started/Hello-Triangle
 *
 * Adaptado por Rossana Baptista Queiroz
 * para a disciplina de Processamento Gráfico - Jogos Digitais - Unisinos
 * Versão inicial: 7/4/2017
 * Última atualização em 02/03/2022
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
#include "stb_image.h"
#include <vector>

// Protótipo da função de callback de teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// Valida se pressionou ou soltou o botão
bool pressRelease(int action);

// Protótipos das funções
int setupGeometry();

// Carrega a textura
GLuint carregarTextura();

// Print das instruções de funcionamento
void mostrarInstrucoes();

// Dimensões da janela (pode ser alterado em tempo de execução)
const GLuint WIDTH = 1000, HEIGHT = 1000;

// Diretórios dos modelos
std::string diretorioModelos = "../M4/objetos/";
std::string diretorioImagemModelo = "";

// 1 = Cubo, 2 = Bola, 3 = Suzanne
const int imagem = 3;

bool rotateUp = false, rotateDown = false, rotateLeft = false, rotateRight = false, rotateAngleLeft = false, rotateAngleRight = false;
bool moveUp = false, moveDown = false, moveLeft = false, moveRight = false, moveFront = false, moveBack = false;
bool bigger = false, smaller = false;
int objectsRef = 0;
int selectedObject = 0;
bool createObject = true;

int qtdVertices = 0;

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
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "M4 -- Pedro Evaldt Machado.", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Fazendo o registro da função de callback para a janela GLFW
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

	// Definindo as dimensões da viewport com as mesmas dimensões da janela da aplicação
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	// Compilando e buildando o programa de shader
	Shader shader = Shader("../M4/shaders/hello.vs", "../M4/shaders/hello.fs");

	// Gerando um buffer simples, com a geometria de um triângulo
	GLuint VAO = setupGeometry();
	GLuint texID = carregarTextura();

	glUseProgram(shader.ID);

	glm::mat4 model = glm::mat4(1); //matriz identidade;
	GLint modelLoc = glGetUniformLocation(shader.ID, "model");

	glUniform1i(glGetUniformLocation(shader.ID, "tex_buffer"), 0);

	//model = glm::rotate(model, /*(GLfloat)glfwGetTime()*/glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glEnable(GL_DEPTH_TEST);

	mostrarInstrucoes();

	float rotateXObjects[10];
	float rotateYObjects[10];
	float rotateZObjects[10];
	glm::vec3 scales[10];
	glm::vec3 positionObjects[10];

	// Loop da aplicação - "game loop"
	while (!glfwWindowShouldClose(window))
	{
		// Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as funções de callback correspondentes
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

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texID);

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
	// Finaliza a execução da GLFW, limpando os recursos alocados por ela
	glfwTerminate();

	return 0;
}

// Função de callback de teclado - só pode ter uma instância (deve ser estática se
// estiver dentro de uma classe) - É chamada sempre que uma tecla for pressionada
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
			cout << "Máximo de objectos criados permitidos" << endl;
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
			cout << "Máximo de objectos excluidos permitidos" << endl;
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

// Esta função está bastante harcoded - objetivo é criar os buffers que armazenam a 
// geometria de um triângulo
// Apenas atributo coordenada nos vértices
// 1 VBO com as coordenadas, VAO com apenas 1 ponteiro para atributo
// A função retorna o identificador do VAO
int setupGeometry()
{
	std::string filename = diretorioModelos;
	std::string diretorioMtl;

	if (imagem == 1)
	{
		filename += "CuboTextured.obj";
	}
	else if (imagem == 2)
	{
		filename += "bola.obj";
	}
	else if (imagem == 3)
	{
		filename += "suzanneTri.obj";
	}

	std::vector<std::vector<int>> models;
	std::vector<std::vector<GLfloat>> vectors;
	std::vector<std::vector<GLfloat>> normalVectors;
	std::vector<std::vector<GLfloat>> textureVectors;

	std::ifstream infile(filename);
	std::string line;

	while (std::getline(infile, line))
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

			diretorioMtl = diretorioModelos + tokens[1];
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

			diretorioImagemModelo = diretorioModelos + tokens[1];
		}
	}

	qtdVertices = models.size();
	std::vector<GLfloat> vertices;

	int indexVectors = 0;
	int indexTexture = 1;
	int indexNormals = 2;

	for (int i = 0; i < models.size(); i++)
	{
		vertices.insert(vertices.end(), vectors[models[i][indexVectors]][0]);
		vertices.insert(vertices.end(), vectors[models[i][indexVectors]][1]);
		vertices.insert(vertices.end(), vectors[models[i][indexVectors]][2]);
		vertices.insert(vertices.end(), normalVectors[models[i][indexNormals]][0]);
		vertices.insert(vertices.end(), normalVectors[models[i][indexNormals]][1]);
		vertices.insert(vertices.end(), normalVectors[models[i][indexNormals]][2]);
		vertices.insert(vertices.end(), textureVectors[models[i][indexTexture]][0]);
		vertices.insert(vertices.end(), textureVectors[models[i][indexTexture]][1]);
	}

	GLuint VBO, VAO;

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

	return VAO;
}

GLuint carregarTextura() 
{
	GLuint texID;

	// Gera o identificador da textura na memória 
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	unsigned char* data = stbi_load(diretorioImagemModelo.c_str(), &width, &height, &nrChannels, 0);

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

	return texID;
}

void mostrarInstrucoes() {
	cout << "Maximo de objetos 10, sendo em ordem de criação, primeiro fica no '1' (criado junto com a tela), os seguintes nos numeros em ordem, sendo o decimo no 0" << endl;
	cout << "WASD para mover objeto selecionado" << endl;
	cout << "F e B para mover no eixo Z (frente <> traz) objeto selecionado" << endl;
	cout << "XYZ para rotacionar o objeto selecionado" << endl;
	cout << "2,4,6,7,8,9 do teclado numerico para rotacionar o objeto selecionado" << endl;
	cout << "'+' e '-' do teclado numerico para aumentar e dimuir objeto" << endl;
	cout << "'ENTER' e 'BACKSPACE' para adicionar e remover objetos" << endl;
}