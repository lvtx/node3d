#include "Monster.h"
#include "CSVFile.h"
#include "Audio.h"
#include "Res.h"
#include "PlayerMe.h"

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CMonster::CMonster()
{
	//m_nType = 0;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CMonster::~CMonster()
{

}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CMonster::setType(unsigned long uType)
{
	m_uType = uType;
	// ----
	CSVMONSTER.seek(0,uType);
	// ----
	const char* szName			= CSVMONSTER.getStr("name","000000");
	const char* szModelFilename	= CSVMONSTER.getStr("model","000000");
	// ----
	float fScale				= CSVMONSTER.getFloat("scale",1.0f);
	// ----
	setRoleName(s2ws(szName).c_str());
	// ----
	load(szModelFilename);
	// ----
	setScale(Vec3D(fScale,fScale,fScale));
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void playMonsterSound(unsigned long uType, const char* szName)
{
	GetAudio().playSound(CSVMONSTER.seek(0,uType).getStr(szName,""));
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

extern bool g_bLocal;
void CMonster::frameMoveRole(const Matrix& mWorld, double fTime, float fElapsedTime)
{
	float fSoundIdleTime = 10.0f;
	// ----
	if(m_uActionState == WALK||m_uActionState == RUN)
	{
		// ----
		if(fElapsedTime < fSoundIdleTime)
		{
			if(rand() % (int)(fSoundIdleTime / fElapsedTime) == 0)
			{
				playMonsterSound(m_uType,"soundIdle");
			}
		}
	}
	// Local Robot
	if (g_bLocal)
	{
		float fDistance = (CPlayerMe::getInstance().getPos() - getPos()).length();
		if (fDistance < 5.0f && (rand()%10)==0 && CPlayerMe::getInstance().getCurRoleCmd().nType != RoleCmd::DEAD)
		{
			if (m_CurRoleCmd.nType == RoleCmd::STAND)
			{
				// ----
				// Can Attact
				// ----
				if (fDistance > 2.0f)
				{
					// ----
					// Add Move Cmd
					// ----
					RoleCmd cmd;
					cmd.nType = RoleCmd::MOVE;
					cmd.nParam1 = CPlayerMe::getInstance().getCellPosX();
					cmd.nParam2 = CPlayerMe::getInstance().getCellPosY();
					cmd.nParam3	= 2;
					addRoleCmd(cmd);
				}
				else
				{
					// ----
					// Add Attact Cmd
					// ----
					RoleCmd cmd;
					cmd.nType = RoleCmd::ATTACT;
					cmd.nParam1 =  CPlayerMe::getInstance().getRoleID();
					addRoleCmd(cmd);
					// Local Robot
					CPlayerMe::getInstance().damage(rand()%10,0,0);
				}
			}
		}
		else
		{
			// 5 sec rand to move
			if(rand() % (int)(1 + 5.0f / fElapsedTime) == 0)
			{
				RoleCmd cmd;
				cmd.nType = RoleCmd::MOVE;
				cmd.nParam1 = getCellPosX()+(rand()%11)-5;
				cmd.nParam2 = getCellPosY()+(rand()%11)-5;
				cmd.nParam3	= 2;
				addRoleCmd(cmd);
			}
		}
	}
	// ----
	CRole::frameMoveRole(mWorld, fTime, fElapsedTime);
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CMonster::setActionState(unsigned char uActionState)
{
	m_uActionState=uActionState;
	// ----
	switch(m_uActionState)
	{
		case STAND:
		{
			setAnimByName("0");
		}
		break;

		case WALK:
		case RUN:
		{
			setAnimByName("2");
		}
		break;

		case HIT:
		{
			playMonsterSound(m_uType,"soundAttack");
			// ----
			setAnimByName("3");
		}
		break;

		case DIE:
		{
			playMonsterSound(m_uType,"soundDie");
			// ----
			setAnimByName("6");
		}
		break;
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------