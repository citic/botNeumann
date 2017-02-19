#include "DebuggerBreakpoint.h"
#include "GdbItemTree.h"
#include "LogManager.h"

DebuggerBreakpoint::DebuggerBreakpoint(const GdbTreeNode& breakpointNode, Roles roles)
	: roles(roles)
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

	// Other available fields:
	//	type="breakpoint"
	//	disp="keep"
	//	enabled="y"
	//	thread-groups=["i1"]
	//	times="1"
	//	original-location="main"
}

void DebuggerBreakpoint::print() const
{
	qCDebug(logTemporary, "Breakpoint[number=%d][line=%d][file=%s][function=%s][address=%zx][roles=%zx]"
			, number, lineNumber, qPrintable(filename), qPrintable(functionName), address, (size_t)roles);
}
