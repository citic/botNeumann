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
};

#endif // VISUALIZATIONCONTEXT_H
