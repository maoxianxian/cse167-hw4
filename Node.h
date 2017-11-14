#ifndef NODE_H
#define NODE_H
#include <iostream>
#include <math.h>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
class Node {
public:
	virtual void draw(GLuint shaderProgram,glm::mat4 m) = 0;
	virtual void update() = 0;
}; 
#endif
