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
	/// @remarks why QVector and not QList?
	QVector<GdbTreeNode*> children;

  public:
	/// Default constructor
	GdbTreeNode();
	/// Copy constructor
	GdbTreeNode(const GdbTreeNode& other);
	/// Move constructor
	GdbTreeNode(GdbTreeNode&& temp);
	/// Assignment operator
	GdbTreeNode& operator=(const GdbTreeNode& other);
	/// Move operator
	GdbTreeNode& operator=(GdbTreeNode&& temp);
	/// Destructor
	~GdbTreeNode() { clearChildren(); }
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
	/// Removes all children and cleans the vector
	void clearChildren();
	/// Copies children from another node
	void copyChildrenFrom(const GdbTreeNode& other);
	/// Builds a description of this item
	/// @param recursive If true, all its children will be included in description
	QString buildDescription(bool recursive) const;
};


class GdbItemTree
{
  protected:
	/// The root item
	GdbTreeNode root;

  public:
	/// Default constructor
	GdbItemTree() { }
	/// Copy constructor
	GdbItemTree(const GdbItemTree& other) : root(other.root) { }
	/// Move constructor
	GdbItemTree(GdbItemTree&& temp) : root( std::move(temp.root) ) { }
	/// Copies the given tree, duplicating node by node
	GdbItemTree& operator=(const GdbItemTree& other);
	/// Move operator
	GdbItemTree& operator=(GdbItemTree&& temp);
	/// Destructor
	~GdbItemTree() { }
	/// Get access to the root element
	inline GdbTreeNode* getRoot() { return &this->root; }
	/// Traverses the item tree and produces a string represeting all the items
	inline QString buildDescription() const { return root.buildDescription(true); }
};

#endif // GDBITEMTREE_H
