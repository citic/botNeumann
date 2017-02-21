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

DebuggerBreakpoint::Role DebuggerBreakpoint::mapVisualizationContext(VisualizatorContext context)
{
	switch ( context )
	{
		case visUserDefinedBreakpoint: return userDefined;
		case visFunctionDefinition: return functionDefinition;
		case visStarting: return programEntryPoint;
		case visProgramEntryPoint: return programEntryPoint;

		case visMallocBreakpoint: return mallocCalled;
		case visCallocBreakpoint: return callocCalled;
		case visReallocBreakpoint: return reallocCalled;
		case visFreeBreakpoint: return freeCalled;

		default: return unknown;
	}
}

DebuggerBreakpoint::Role DebuggerBreakpoint::addRoleFor(VisualizatorContext context)
{
	Role role = mapVisualizationContext(context);
	if ( role != unknown )
		addRole(role);
	return role;
}
