#ifndef ADJACENCYLIST_H
#define ADJACENCYLIST_H

#include <algorithm>
#include <iterator>
#include <vector>
#include <map>
#include "GL/glew.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

/** Class for adjacency nodes
*/
class AdjacencyNode
{
	public:
		//Ctors
		AdjacencyNode(glm::vec3 pos = glm::vec3());
		AdjacencyNode(const AdjacencyNode& node);

		//Getters and Setters
		glm::vec3 getPos();
		void setPos(glm::vec3 v);
		glm::mat4 getMat();
		std::vector<AdjacencyNode*> getAdjacentNodes();

		//Node manipulation and shortest path
		static std::vector<AdjacencyNode*> getShortestPath(AdjacencyNode &beg, AdjacencyNode &end);
		void addAdjacentNode(AdjacencyNode &n);
		void removeAdjacentNode(AdjacencyNode &n);
		void clearAdjacentNodes();
		void reset();

		void operator=(const AdjacencyNode& node);
		bool operator==(const AdjacencyNode& node);
		friend bool operator==(const AdjacencyNode &lhs, const AdjacencyNode &rhs);
		friend bool operator<(const AdjacencyNode &lhs, const AdjacencyNode &rhs);
		friend bool operator>(const AdjacencyNode &lhs, const AdjacencyNode &rhs);

		//Dtor
		~AdjacencyNode();

	private:
		glm::vec3 pos; //Nodes position
		glm::mat4 transMat; //Nodes mat for rendering
		std::vector<AdjacencyNode*> adjacentNodes; //Vector of additional nodes
		GLfloat gScore = FLT_MAX; //Scores for a*
		GLfloat hScore = FLT_MAX; //Scores for a*
		bool isInQueue = false; //Bool for a*
};

//Used for node pointer comparison
struct NodeCompare
{
	bool operator()(const AdjacencyNode* lhs, const AdjacencyNode* rhs) const;
};

#endif
