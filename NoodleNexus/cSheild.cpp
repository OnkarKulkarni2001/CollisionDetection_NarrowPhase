#include "cSheild.h"

cSheild::cSheild()
{
	this->m_bIsRaised = false;
}

void cSheild::Raise(void)
{
	this->m_bIsRaised = true;
	return;
}

void cSheild::Lower(void)
{
	this->m_bIsRaised = false;
	return;
}

bool cSheild::bIsUp(void)
{
	return this->m_bIsRaised;
}

