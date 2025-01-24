#include "cTank.h"
#include <iostream>

cTank::cTank()
{
	std::cout << "A regular tank is created" << std::endl;
	this->m_ID = cTank::m_NextID++;
	this->health = 100.0f;
	this->p_CurrentTarget = NULL;
}

/*static*/
unsigned int cTank::m_NextID = 1000;

unsigned int cTank::getID(void)
{
	return this->m_ID;
}


cTank::~cTank()
{

}

//void cTank::Attack(/*another tank*/)
void cTank::Attack(iTank* pEnemyTank)
{

	return;
}

glm::vec3 cTank::getLocation(void)
{
	// TODO: 

	return this->m_location;
}

void cTank::setLocation(glm::vec3 newLocation_)
{
	this->m_location = newLocation_;
	return;
}


void cTank::UpdateTick(double deltaTime)
{
	// TODO:
	std::cout << "Tank# " << this->m_ID << " updating" << std::endl;
	return;
}


