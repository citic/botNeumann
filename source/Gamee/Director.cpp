#include "Director.h"

Director::Director(Stage* stage, QObject *parent)
	: QObject(parent)
	, stage(stage)
{
}

Director::~Director()
{
}

