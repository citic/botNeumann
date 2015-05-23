#include "Visualizator.h"
#include "DebuggerCall.h"
#include "UnitPlayingScene.h"

Visualizator::Visualizator(const QFileInfo& executablePath, UnitPlayingScene* unitPlayingScene)
	: QObject(unitPlayingScene)
	, debuggerCall( new DebuggerCall(executablePath) )
	, unitPlayingScene(unitPlayingScene)
{
}

Visualizator::~Visualizator()
{
}

#include <QDebug>

void Visualizator::start()
{
	// ToDo: Connect signals and slots here
	debuggerCall->start();
}
