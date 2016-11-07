#include "DebuggerBreakpoint.h"
#include "GdbItemTree.h"

DebuggerBreakpoint::DebuggerBreakpoint(const GdbTreeNode& breakpointNode)
	: number(-1)
	, address(0x0)
	, lineNumber(-1)
{
	updateFromNode(breakpointNode);
}

DebuggerBreakpoint::~DebuggerBreakpoint()
{
}

void DebuggerBreakpoint::updateFromNode(const GdbTreeNode& breakpointNode)
{
	this->number       = breakpointNode.findTextValue("number").toInt();
	this->address      = breakpointNode.findTextValue("addr").toInt(nullptr, 16);
	this->functionName = breakpointNode.findTextValue("func");
	this->filename     = breakpointNode.findTextValue("file");
	this->filepath     = breakpointNode.findTextValue("fullname");
	this->lineNumber   = breakpointNode.findTextValue("line").toInt();

	qDebug("  Breakpoint[number=%d][line=%d][file=%s][function=%s][address=%zx]"
		   , number, lineNumber, qPrintable(filename), qPrintable(functionName), address);
}
