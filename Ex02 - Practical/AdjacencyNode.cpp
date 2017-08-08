#include "AdjacencyNode.h"

/**Node constructor
 *
 * @param (glm::vec3) pos - The position for the node
 */
AdjacencyNode::AdjacencyNode(glm::vec3 pos)
{
	this->pos = pos;
	transMat = glm::translate(transMat, pos); //Set the matrix
}

/**Node copy constructor
 *
 * @param (AdjacencyNode&) node - The node to copy
 */
AdjacencyNode::AdjacencyNode(const AdjacencyNode& node)
{
	this->pos = node.pos;
	this->transMat = node.transMat;
	this->adjacentNodes = node.adjacentNodes;
	this->gScore = node.gScore;
	this->hScore = node.hScore;
}

/**Gets the nodes position
 *
 * @return (glm::vec3) The position for the node
 */
glm::vec3 AdjacencyNode::getPos()
{
	return this->pos;
}

/**Sets the nodes position
 *
 * @param (glm::vec3) v - The new position for the node
 */
void AdjacencyNode::setPos(glm::vec3 v)
{
	this->pos = v;
}

/**Gets the nodes transform mat
 *
 * @return (glm::mat4) The transform matrix for the node
 */
glm::mat4 AdjacencyNode::getMat()
{
	return transMat;
}

/**Gets the adjacent nodes to the current
 *
 * @return (std::vector<AdjacencyNode*>) Vector of the adjacent nodes
 */
std::vector<AdjacencyNode*> AdjacencyNode::getAdjacentNodes()
{
	return adjacentNodes;
}

/**Gets the shortest path from one node to another using a*
 *
 * @return (std::vector<AdjacencyNode*>) Vector of nodes that make up the shortest path
 */
std::vector<AdjacencyNode*> AdjacencyNode::getShortestPath(AdjacencyNode &beg, AdjacencyNode &end)
{
	//Variables
	std::vector<AdjacencyNode*> incomplete; //Vector of nodes that haven't been fully searched
	std::vector<AdjacencyNode*> complete; //Vector of fully searched nodes
	std::map<AdjacencyNode*, AdjacencyNode*> cameFrom; //Map of nodes and where they came from
	std::map<AdjacencyNode*, GLfloat> cost; //Map of nodes and the cost to get to them
	bool skip; //If the iteration should be skipped
	AdjacencyNode* cur = nullptr; //Pointer to current node

	//Set the beginnings gScore and hScore
	beg.gScore = 0;
	beg.hScore = glm::distance(beg.pos, end.pos);
	
	//Push the beginnings to the incomplete list, set the cost to 0, say it's in the queue
	incomplete.push_back(&beg);
	cost[&beg] = 0;
	beg.isInQueue = true;

	//Loop through the list
	while (!incomplete.empty())
	{
		//Get the node at the top of the incomplete list
		cur = incomplete.front();
		
		//Check if the node is the end node
		if (*cur == end)
		{
			std::vector<AdjacencyNode*> toReturn; //Vector of nodes on the path
			toReturn.push_back(&end); //Add the end node to the path
			AdjacencyNode* tmp = &end; //Temp node for backtracking
			while (tmp != &beg) //Loop while tmp isn't the beginning
			{
				//Get where tmp came from and add it to the vector
				tmp = cameFrom[tmp];
				toReturn.push_back(tmp);
			}
			//Reverse the vector and return it
			std::reverse(toReturn.begin(), toReturn.end());
			return toReturn;
		}

		//Remove the node from the incomplete vector, and add it to the complete vector
		incomplete.erase(incomplete.begin());
		complete.push_back(cur);

		//Loop for all the adjacent nodes
		for (GLuint i = 0; i < cur->adjacentNodes.size(); i++)
		{
			skip = false;

			//Check if the node has already been checked
			for (GLuint j = 0; j < complete.size(); j++)
			{
				if (cur->adjacentNodes[i] == complete[j])
				{
					skip = true;
					break;
				}
			}

			//Skip is the node has been checked
			if (skip)
				continue;

			//If the node isn't in the incomplete vector, add it, and mark that it's added
			if (!cur->adjacentNodes[i]->isInQueue)
			{
				incomplete.push_back(cur->adjacentNodes[i]);
				cur->adjacentNodes[i]->isInQueue = true;
				std::sort(std::begin(incomplete), std::end(incomplete), NodeCompare()); //Sort the vector
			}

			//Calculate a new gScore for the adjacent node
			GLfloat newG = cost[cur] + glm::distance(cur->pos, cur->adjacentNodes[i]->pos);

			//Check if the gScore is less than the current gScore for the node
			if (newG < cur->adjacentNodes[i]->gScore)
			{
				//Add the adjacent node to the cameFrom list with the current node, set the adjacent nodes gScore
				// and hScore, and add the cost to the map
				cameFrom[cur->adjacentNodes[i]] = cur;
				cur->adjacentNodes[i]->gScore = newG;
				cur->adjacentNodes[i]->hScore = glm::abs(glm::distance(cur->adjacentNodes[i]->pos, end.pos));
				cost[cur->adjacentNodes[i]] = newG + cur->adjacentNodes[i]->hScore;
			}

			
		}
	}
}

/**Adds an adjacent node to the current
 *
 * @param (AdjacencyNode&) n - The adjance node to add
 */
void AdjacencyNode::addAdjacentNode(AdjacencyNode &n)
{
	adjacentNodes.push_back(&n);
}

/**Removes an adjacent node from the current
 *
 * @param (AdjacencyNode&) n - The adjance node to remove
 */
void AdjacencyNode::removeAdjacentNode(AdjacencyNode &n)
{
	for (GLuint i = 0; i < adjacentNodes.size(); i++)
	{
		if (adjacentNodes[i] == &n)
			adjacentNodes.erase(adjacentNodes.begin() + i);
	}
}

/**Clears all the adjacent nodes
 */
void AdjacencyNode::clearAdjacentNodes()
{
	adjacentNodes.clear();
}

/**Resets this nodes isInQueue value, and g and h score values
 */
void AdjacencyNode::reset()
{
	isInQueue = false;
	gScore = FLT_MAX;
	hScore = FLT_MAX;
}

/**Node assignment operator
 *
 * @param (AdjacencyNode&) node - The node to copy
 */
void AdjacencyNode::operator=(const AdjacencyNode& node)
{
	this->pos = node.pos;
	this->transMat = node.transMat;
	this->adjacentNodes = node.adjacentNodes;
	this->gScore = node.gScore;
	this->hScore = node.hScore;
}

/**Equality operator
 *
 * @param (const AdjacencyNode&) node - The node to check for equality
 *
 * @return (bool) If the nodes are equal or not
 */
bool AdjacencyNode::operator==(const AdjacencyNode& node)
{
	if (this->pos == node.pos && this->adjacentNodes == node.adjacentNodes)
		return true;

	return false;
}

/**Equality operator
 *
 * @param (AdjacencyNode&) lhs - The left-hand side of the equation
 * @param (AdjacencyNode&) rhs - The right-hand side of the equation
 *
 * @return (bool) If the nodes are equal or not
 */
bool operator==(const AdjacencyNode& lhs, const AdjacencyNode& rhs)
{
	if (lhs.pos == rhs.pos)
		return true;

	return false;
}

/**Less-than operator
 *
 * @param (AdjacencyNode&) lhs - The left-hand side of the equation
 * @param (AdjacencyNode&) rhs - The right-hand side of the equation
 *
 * @return (bool) If the left hand node is less than the right hand node
 */
bool operator<(const AdjacencyNode& lhs, const AdjacencyNode& rhs)
{
	return ((lhs.hScore + lhs.gScore) < (rhs.hScore + rhs.gScore));
}

/**Greater-than operator
 *
 * @param (AdjacencyNode&) lhs - The left-hand side of the equation
 * @param (AdjacencyNode&) rhs - The right-hand side of the equation
 *
 * @return (bool) If the left hand node is greater than the right hand node
 */
bool operator>(const AdjacencyNode& lhs, const AdjacencyNode& rhs)
{
	return ((lhs.hScore + lhs.gScore) > (rhs.hScore + rhs.gScore));
}

/**Destructor. Clears the adjance node list
 *
 */
AdjacencyNode::~AdjacencyNode()
{
	adjacentNodes.clear();
}

/**Used as a sorting method for pointers
 *
 * @param (AdjacencyNode*) lhs - The left-hand side of the equation
 * @param (AdjacencyNode*) rhs - The right-hand side of the equation
 *
 * @return (bool) If the left hand node is less than the right hand node
 */
bool NodeCompare::operator()(const AdjacencyNode* lhs, const AdjacencyNode* rhs) const
{
	return *lhs < *rhs;
}