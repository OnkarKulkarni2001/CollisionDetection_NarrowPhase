#pragma once

//#include "cSword.h"
class cSword;	// Forward declaration

class cMonster
{
public:
	cSword* pSword;
	void Attack(cMonster *EnemyMonster);
	float health;
};
