#include "MyOctant.h"

/** Node constructor
 *
 * @param (glm::vec3) center - The center of the node
 * @param (glm::vec3) size - The size of cube drawn around the node
 * @param (GLuint) subDivisions - The amount of subdivisions
 */
Node::Node(glm::vec3 center, glm::vec3 size, GLuint subDivisions)
{
	nCenter = center;
	nSize = size;
	transMat = glm::translate(nCenter) * glm::scale(nSize); //Calculate the trans mat

	if (subDivisions > 0)
	{
		//New center posistions to base subnodes on
		glm::vec3 subCenters[8] = {	 center,
									 glm::vec3( center.x,  center.y, -center.z),
									 glm::vec3( center.x, -center.y,  center.z),
									 glm::vec3(-center.x,  center.y,  center.z),
									 glm::vec3( center.x, -center.y, -center.z),
									 glm::vec3(-center.x, -center.y,  center.z),
									 glm::vec3(-center.x,  center.y, -center.z),
									-center };

		//New size posistions to base subnodes on
		glm::vec3 subSizes[8] = {  size / 2.0f,
								   glm::vec3( size.x,  size.y, -size.z) / 2.0f,
								   glm::vec3( size.x, -size.y,  size.z) / 2.0f,
								   glm::vec3(-size.x,  size.y,  size.z) / 2.0f,
								   glm::vec3( size.x, -size.y, -size.z) / 2.0f,
								   glm::vec3(-size.x, -size.y,  size.z) / 2.0f,
								   glm::vec3(-size.x,  size.y, -size.z) / 2.0f,
								  -size / 2.0f };

		//Make the subnodes
		subNodes = new Node[8] { Node(((subSizes[0] / 2.0f) + subCenters[0]), subSizes[0], subDivisions - 1),
								 Node(((subSizes[1] / 2.0f) + subCenters[1]), subSizes[0], subDivisions - 1),
								 Node(((subSizes[2] / 2.0f) + subCenters[2]), subSizes[0], subDivisions - 1),
								 Node(((subSizes[3] / 2.0f) + subCenters[3]), subSizes[0], subDivisions - 1),
								 Node(((subSizes[4] / 2.0f) + subCenters[4]), subSizes[0], subDivisions - 1),
								 Node(((subSizes[5] / 2.0f) + subCenters[5]), subSizes[0], subDivisions - 1),
								 Node(((subSizes[6] / 2.0f) + subCenters[6]), subSizes[0], subDivisions - 1),
								 Node(((subSizes[7] / 2.0f) + subCenters[7]), subSizes[0], subDivisions - 1) };
	}
}

/** Node copy constructor
 *
 * @param (const Node&) newNode - The node to copy
 */
Node::Node(const Node& newNode)
{
	memcpy(this, &newNode, sizeof(newNode)); //Copy the new node to the old node
}

/** Node assignment operator
 *
 * @param (const Node&) newNode - The node to copy
 *
 * @return (Node) - This node after being assigned
 */
Node Node::operator=(const Node& newNode)
{
	memcpy(this, &newNode, sizeof(newNode)); //Copy the new node to the old node
	return *this; //Return this
}

/** Traverses the tree
 *
 * @param (Node*) curNode - The node to traverse
 *
 * @return (std::vector<Node*>) - Vector of nodes traversed
 */
std::vector<Node*> Node::Traverse(Node* curNode)
{
	//Temp vector to return
	std::vector<Node*> ret;
	ret.push_back(curNode);

	//Check if there are sub-nodes
	if (curNode->subNodes != nullptr)
	{
		std::vector<Node*> tmp; //Vector used to add to ret
		//Loop through subnodes and traverse them, making tmp equal the returned value
		for (GLuint i = 0; i < 8; i++)
		{
			tmp = Traverse(&curNode->subNodes[i]);
			
			//For each Node in the vector, push it back to ret
			for (GLuint j = 0; j < tmp.size(); j++)
			{
				ret.push_back(tmp[j]);
			}
		}
	}

	return ret;
}

/** Traverse's and gets rid of children
 *
 * @param (Node*) curNode - The node to traverse
 *
 * @return (std::vector<Node*>) - Vector of leaf nodes traversed
 */
std::vector<Node*> Node::TraverseNoChild(Node* curNode)
{
	//Vector to return is gained from the traverse method
	std::vector<Node*> ret = Traverse(curNode);

	//For each node in ret, check if there are any children. If so, remove that node from the vector, only keeping
	// leaf nodes
	for (GLuint i = 0; i < ret.size(); i++)
	{
		if (ret[i]->subNodes != nullptr)
		{
			ret.erase(ret.begin() + i);
		}
	}

	return ret;
}

/** Gets the subnodes nodes of the node
 *
 * @return (const Node*) - Pointer to the subnodes
 */
const Node* Node::getSubNodes()
{
	return subNodes;
}

/** Gets the center of the node
 *
 * @return (const glm::vec3) - The center of the node
 */
const glm::vec3 Node::getCenter()
{
	return nCenter;
}

/** Gets the size of the node
 *
 * @return (const glm::vec3) - The size of the node
 */
const glm::vec3 Node::getSize()
{
	return nSize;
}

/** Gets the transformation matrix of the node
 *
 * @return (const glm::mat4) - The transformation matrix of the node
 */
const glm::mat4 Node::getTransMat()
{
	return transMat;
}

/** Node Destructor
 */
Node::~Node()
{
	//If there are subnodes, delete them
	if(subNodes != nullptr)
		delete [] subNodes;
}

/** Octant constructor
 *
 * @param (GLuint) octantLvl - The amount of levels in the octant
 * @param (glm::vec3) size - The size of the overall tree
 */
MyOctant::MyOctant(GLuint octantLvl, glm::vec3 size)
{
	//Create a root node, get the leafs of the tree, get the octant count
	rootNode = new Node(glm::vec3(0, 0, 0), size, octantLvl);
	nodeLeafs = Node::TraverseNoChild(rootNode);
	octantCount = nodeLeafs.size();
}

/** Octant copy constructor
 *
 * @param (const MyOctant&) newOctant - The octant to copy
 */
MyOctant::MyOctant(const MyOctant &newOctant)
{
	memcpy(this, &newOctant, sizeof(newOctant)); //Copy the memory of the new octant
}

/** Octant assignment operator
 *
 * @param (const MyOctant&) newOctant - The octant to copy
 *
 * @return (MyOctant) - This octant after being assigned
 */
MyOctant MyOctant::operator=(const MyOctant &newOctant)
{
	memcpy(this, &newOctant, sizeof(newOctant)); //Copy the memory of the new octant
	return *this;
}

/** Gets the root node
 *
 * @return (Node*) - The root node
 */
Node* MyOctant::getRootNode()
{
	return rootNode;
}

/** Gets the octant count
 *
 * @return (GLuint) - The octant count
 */
GLuint MyOctant::getOctantCount()
{
	return octantCount;
}

/** Gets the leafs of the octant
 *
 * @return (std::vector<Node*>) - The leafs of the octant
 */
std::vector<Node*> MyOctant::getNodeLeafs()
{
	return nodeLeafs;
}

/** Octant Destructor
 */
MyOctant::~MyOctant()
{
	//Deletes the root node if it exists
	if(rootNode != nullptr)
		delete rootNode;
}