#ifndef MY_OCTANT
#define MY_OCTANT

#include "Simplex/Simplex.h"

//Class for nodes
class Node
{
	public:
		Node(glm::vec3 center = glm::vec3(0, 0, 0), glm::vec3 size = glm::vec3(0, 0, 0), GLuint subDivisions = 0);
		Node(const Node& newNode);
		Node operator=(const Node& newNode);
		static std::vector<Node*> Traverse(Node* curNode);
		static std::vector<Node*> TraverseNoChild(Node* curNode);
		const Node* getSubNodes();
		const glm::vec3 getCenter();
		const glm::vec3 getSize();
		const glm::mat4 getTransMat();
		~Node();

	private:
		glm::vec3 nCenter; //Center of the node
		glm::vec3 nSize; //Size of the cube around the node
		Node* subNodes = nullptr; //Node* of subnodes
		glm::mat4 transMat; //Used to create the transform box for the node
};

//Octant Class
class MyOctant
{
	public:
		MyOctant(GLuint octantLvl = 1, glm::vec3 size = glm::vec3(0));
		MyOctant(const MyOctant& newOctant);
		MyOctant operator=(const MyOctant& newOctant);
		Node* getRootNode();
		GLuint getOctantCount();
		std::vector<Node*> getNodeLeafs();
		~MyOctant();

	private:
		Node* rootNode = nullptr; //Root Node of the tree
		GLuint octantCount; //Amount of octants in the tree
		std::vector<Node*> nodeLeafs; //Vector of all leaves in the tree
};

#endif