#ifndef VISUALIZATIONCONTEXT_H
#define VISUALIZATIONCONTEXT_H

enum VisualizatorContext
{
	visUnknown,
	visStarting,
	visExecutionLoop,
	visStopping,

	visUserDefinedBreakpoint,
	visFunctionDefinition,
	visProgramEntryPoint,

	visMallocBreakpoint,
	visCallocBreakpoint,
	visReallocBreakpoint,
	visFreeBreakpoint,
};

#endif // VISUALIZATIONCONTEXT_H
