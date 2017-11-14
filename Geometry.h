#ifndef GEOMETRY_H
#define GEOMETRY_H
#include "Node.h"
#include <vector>
class Geometry : public Node {
public:
	GLuint VBO, VAO, EBO, NBO;
	float xmax;
	float xmin;
	float ymax;
	float ymin;
	float zmax;
	float zmin;
	glm::mat4 centermat;
	std::vector<unsigned int> indices;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	Geometry(const char* filepath);
	~Geometry();
	void parse(const char* filepath);
	void center();
	void draw(GLuint shaderProgram, glm::mat4 m);
	void update();
};
#endif
