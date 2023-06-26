//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Luz.cpp"
#include "Camera.cpp"

class Estrutura {
	private:
		Luz LuzCenario;
		Camera CameraCenario;

	public:
		Estrutura() {
			LuzCenario = Luz();
			CameraCenario = Camera();
		};

	public:
		void CarregarEstrutura(Shader &shader, float width, float height) {
			glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
			shader.setMat4("projection", glm::value_ptr(projection));

			glm::mat4 view = glm::lookAt(CameraCenario.Posicao, CameraCenario.Posicao + CameraCenario.Frente, CameraCenario.Up);
			shader.setMat4("view", glm::value_ptr(view));

			shader.setVec3("cameraPos", CameraCenario.Posicao.x, CameraCenario.Posicao.y, CameraCenario.Posicao.z);

			LuzCenario.SetarLuz(shader);
		}

		void NovaLuz(glm::vec3 novaLuz) {
			LuzCenario = Luz(novaLuz);
		}
};