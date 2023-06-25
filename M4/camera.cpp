//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera {
	public:
		glm::vec3 Posicao;
		glm::vec3 Frente;
		glm::vec3 Altura;

	public:
		Camera() {
			Posicao = glm::vec3(0.0, 0.0, 3.0);
			Frente = glm::vec3(0.0, 0.0, -1.0);
			Altura = glm::vec3(0.0, 1.0, 0.0);
		}
};