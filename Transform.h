#ifndef TRANSFORM_H
#define TRANSFORM_H
#include "Node.h"
#include <vector>
class Transform : public Node {
	public:
		int id;
		glm::mat4 toParent;
		std::vector<Node*> children;
		Transform(glm::mat4 m);
		~Transform();
		void draw(GLuint shaderProgram,glm::mat4 m);
		void translate(float x, float y, float z);
		void scale(float x, float y, float z);
		void rotate(glm::vec3 axis, float deg);
		void rotateafter(glm::vec3 axis, float deg);
		void addChild(Node* child);
		void removeChild(Node* child);
		void update();
};
#endif