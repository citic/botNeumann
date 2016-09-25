#ifndef GDBITEMTREE_H
#define GDBITEMTREE_H

#include <QString>
#include <QVector>

/**
	Gdb generates some recursive items:

		item=value
		item_object={item1=value1,item2=value2,itemN=valueN}
		item_array=[value1,value2,valueN]

	Because relations between items are hierarchical, each item is stored as a Node
**/
class GdbTreeNode
{
  protected:
	/// Name of the item, in the form name=value
	QString name;
	/// If this item has a textual value (i.e item=value)
	QString textValue;
	///
	/// @remarks why QVector and not QList?
	QVector<GdbTreeNode*> children;

  public:
	/// Default constructor
	GdbTreeNode();
	/// Disable copies, move constructor?
	GdbTreeNode(const GdbTreeNode& other) = delete;
	/// Disable copies
	GdbTreeNode& operator=(const GdbTreeNode& other) = delete;
	/// Get access to the name
	inline const QString& getName() const { return this->name; }
	/// Changes the name of the element when it is known
	inline void setName(const QString& name) { this->name = name; }
	/// Get access to the textual value, "" if none
	inline const QString& getTextValue() const { return this->textValue; }
	/// Changes the textual value of this element
	inline void setTextValue(const QString& textValue) { this->textValue = textValue; }
	/// Adds a node to the list of children
	inline void addChild(GdbTreeNode* child) { children.append(child); }
};


class GdbItemTree
{
  protected:
	/// The root item
	GdbTreeNode root;

  public:
	/// Get access to the root element
	inline GdbTreeNode* getRoot() { return &this->root; }
};

#endif // GDBITEMTREE_H
