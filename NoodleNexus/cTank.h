#pragma once

#include "iTank.h"
#include <glm/glm.hpp>
#include "sMesh.h"

// 
#include "cSheild.h"

class cTank : public iTank
{
public:
	cTank();
	virtual ~cTank();

//	void Attack(/*another tank*/);
	virtual void Attack(iTank* pEnemyTank);
	virtual glm::vec3 getLocation(void);
	virtual void setLocation(glm::vec3 newLocation_);
	virtual void UpdateTick(double deltaTime);


	sMesh* pTheMesh;

	cSheild* pTheShield;

	virtual unsigned int getID(void);

	float health;

private:
	static unsigned int m_NextID;
	unsigned int m_ID;

	glm::vec3 m_location;

	// If NULL, there isn't a target
	// If ! NULL, then we are attacking something
	iTank* p_CurrentTarget;
};