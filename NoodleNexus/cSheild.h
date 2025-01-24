#pragma once

class cSheild
{
public:
	cSheild();
	void Raise(void);
	void Lower(void);
	bool bIsUp(void);

private:
	bool m_bIsRaised;
};