#include "GdbItemTree.h"

GdbTreeNode::GdbTreeNode()
{
}

GdbTreeNode::GdbTreeNode(const GdbTreeNode& other)
	: name( other.name )
	, textValue( other.textValue )
{
	copyChildrenFrom(other);
}

GdbTreeNode::GdbTreeNode(GdbTreeNode&& temp)
	: name( std::move(temp.name) )
	, textValue( std::move(temp.textValue) )
	, children( std::move(temp.children) )
{
}

GdbTreeNode& GdbTreeNode::operator=(const GdbTreeNode& other)
{
	if ( this != &other )
	{
		this->name = other.name;
		this->textValue = other.textValue;

		copyChildrenFrom(other);
	}
	return *this;
}

GdbTreeNode& GdbTreeNode::operator=(GdbTreeNode&& temp)
{
	if ( this != &temp )
	{
		this->name = std::move(temp.name);
		this->textValue = std::move(temp.textValue);
		this->children = std::move(temp.children);
	}
	return *this;
}

void GdbTreeNode::clearChildren()
{
	for ( int childIndex = 0; childIndex < children.size(); ++childIndex )
		delete children[childIndex];

	children.clear();
}

void GdbTreeNode::copyChildrenFrom(const GdbTreeNode& other)
{
	this->clearChildren();
	this->children.reserve( other.children.size() );

	for ( int childIndex = 0; childIndex < other.children.size(); ++childIndex )
		children.append( new GdbTreeNode( * other.children[childIndex] ) );
}

QString GdbTreeNode::buildDescription(bool recursive) const
{
	QString result = '{' + name + '=' + textValue + ':';

	if ( recursive )
		for ( int childIndex = 0; childIndex < children.size(); ++childIndex )
			result += children[childIndex]->buildDescription(recursive);

	result += '}';
	return result;
}

const GdbTreeNode* GdbTreeNode::findNode(QString nodePath) const
{
	// Find the first node separator '/'
	Q_ASSERT( nodePath.length() > 0 );
	int slashPosition = nodePath.indexOf('/');

	// If it has the leading '/', this node must be the root
	if ( slashPosition == 0 )
	{
		// Get the remaining path with some child names: child1/child2/...
		Q_ASSERT( name.isEmpty() );
		nodePath = nodePath.mid(1);

		// Looking for "/"?
		if ( nodePath.isEmpty() )
			return this;

		slashPosition = nodePath.indexOf('/');
	}

	// Extract the first child name in the path
	QString childName;
	// And the remaning nodes of the path
	QString remainingPath;

	// If there is not separator in the path, it must be a child name
	if ( slashPosition == -1 )
		childName = nodePath;
	else
	{
		// The path has the form child/more/children
		childName = nodePath.left(slashPosition);
		remainingPath = nodePath.mid(slashPosition + 1);
	}

	// The searched child index in the array
	int childIndex = -1;

	// If the index is explicitly given in the path, use it, else find it
	if ( childName[0] == '#' )
		childIndex = childName.mid(1).toInt() - 1;
	else
		childIndex = findChildIndex(childName);

	// If children was not found, return null pointer
	if ( childIndex < 0 || childIndex >= children.size() )
		return nullptr;

	// We found the child
	GdbTreeNode* child = children[childIndex];

	// If path asks for more children, continue searching,
	if ( remainingPath.isEmpty() )
		return child;
	else
		return child->findNode(remainingPath);
}

QString GdbTreeNode::findTextValue(const QString& nodePath) const
{
	const GdbTreeNode* childNode = findNode(nodePath);
	return childNode ? childNode->getTextValue() : QString();
}

const GdbTreeNode*GdbTreeNode::getChild(int childIndex) const
{
	Q_ASSERT(childIndex >= 0 && childIndex < children.size() );
	return children[childIndex];
}

int GdbTreeNode::findChildIndex(const QString& childName) const
{
	for ( int childIndex = 0; childIndex < children.size(); ++childIndex )
		if ( childName == children[childIndex]->getName() )
			return childIndex;

	return -1;
}

GdbItemTree& GdbItemTree::operator=(const GdbItemTree& other)
{
	if ( this != &other )
		root = other.root;

	return *this;
}

GdbItemTree& GdbItemTree::operator=(GdbItemTree&& temp)
{
	if ( this != &temp )
		root = std::move(temp.root);

	return *this;
}
