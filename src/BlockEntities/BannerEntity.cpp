
// BannerEntity.cpp

// Implements the cBannerEntity class representing a single sign in the world

#include "Globals.h"  // NOTE: MSVC stupidness requires this to be the same across all modules
#include "json/value.h"
#include "BannerEntity.h"
#include "../ClientHandle.h"
#include "../Entities/Player.h"




cBannerEntity::cBannerEntity(BLOCKTYPE a_BlockType, int a_X, int a_Y, int a_Z, cWorld * a_World, NIBBLETYPE a_Meta) :
	super(a_BlockType, a_X, a_Y, a_Z, a_World), m_Base(E_META_BANNER_BLACK)
{
	ASSERT((a_Y >= 0) && (a_Y < cChunkDef::Height));
}





bool cBannerEntity::UsedBy(cPlayer * a_Player)
{
	UNUSED(a_Player);
	return false;
}


void cBannerEntity::SetBaseColor(int a_Base)
{
	m_Base = a_Base;
}


int cBannerEntity::GetBaseColor() const
{
	return m_Base;
}


void cBannerEntity::SendTo(cClientHandle & a_Client)
{
	cWorld * World = a_Client.GetPlayer()->GetWorld();
	a_Client.SendBlockChange(m_PosX, m_PosY, m_PosZ, m_BlockType, World->GetBlockMeta(m_PosX, m_PosY, m_PosZ));
	a_Client.SendUpdateBlockEntity(*this);
}


const cBannerEntity::PatternList& cBannerEntity::GetPatterns() const
{
	return m_Patterns;
}

void cBannerEntity::AddPattern(int a_Color, AString a_Pattern)
{
	cPattern pattern;
	pattern.Color = a_Color;
	pattern.Pattern = a_Pattern;
	m_Patterns.push_back(pattern);
}

void cBannerEntity::ClearPatterns()
{
	m_Patterns.clear();
}

void cBannerEntity::SetPatterns(cBannerEntity::PatternList & a_Patterns)
{
	m_Patterns = a_Patterns;
}