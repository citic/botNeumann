#include "Player.h"

Player::Player(const QString& nickname, QObject *parent)
	: QObject(parent)
	, nickname(nickname)
{
}

Player::~Player()
{
}
