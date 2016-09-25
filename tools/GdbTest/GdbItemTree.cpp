#include "GdbItemTree.h"

GdbTreeNode::GdbTreeNode()
{
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
