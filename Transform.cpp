#include "Transform.h";
Transform::Transform(glm::mat4 m)
{
	toParent = m;
}
Transform::~Transform()
{}
void Transform::draw(GLuint shaderProgram, glm::mat4 m)
{
	glUniform3fv(glGetUniformLocation(shaderProgram, "id"),1,&color[0]);
	for (int i = 0; i < children.size(); i++)
	{
		children[i]->draw(shaderProgram,m*toParent);
	}
}
void Transform::translate(float x, float y, float z)
{
	this->toParent = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z))*this->toParent;
}
void Transform::translate(glm::vec3 v) {
	this->toParent = glm::translate(glm::mat4(1.0f), v)*this->toParent;
}

void Transform::rotate(glm::vec3 axis, float deg)
{
	this->toParent = this->toParent*glm::rotate(glm::mat4(1.0f), deg, axis);
}
void Transform::rotateafter(glm::vec3 axis, float deg)
{
	this->toParent = glm::rotate(glm::mat4(1.0f), deg, axis)*this->toParent;
}

void Transform::scale(float x, float y, float z)
{
	this->toParent = this->toParent*glm::scale(glm::mat4(1.0f), glm::vec3(x, y, z));
}
void Transform::addChild(Node* child)
{
	children.push_back(child);
}
void Transform::removeChild(Node * child)
{
	for (int i = 0; i < children.size(); i++)
	{
		if (children[i] == child)
		{
			children.erase(children.begin() + i);
			return;
		}
	}
}
void Transform::update()
{
	for (int i = 0; i < children.size(); i++)
	{
		children[i]->update();
	}
}