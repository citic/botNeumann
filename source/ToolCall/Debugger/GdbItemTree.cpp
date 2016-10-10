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
