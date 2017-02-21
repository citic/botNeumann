#ifndef VISUALIZATIONCONTEXT_H
#define VISUALIZATIONCONTEXT_H

enum VisualizationContext
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

	visStandardInputOutputWatch,
};

#endif // VISUALIZATIONCONTEXT_H
