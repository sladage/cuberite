
#pragma once

#include "ItemHandler.h"
#include "../World.h"
#include "../Blocks/BlockBannerStanding.h"
#include "../Blocks/BlockBannerWall.h"

class cItemBannerHandler :
	public cItemHandler
{
	typedef cItemHandler super;
public:
	cItemBannerHandler(int a_ItemType) :
		super(a_ItemType)
	{
	}

	virtual bool IsPlaceable(void) override
	{
		return true;
	}


	virtual bool GetPlacementBlockTypeMeta(
		cWorld * a_World, cPlayer * a_Player,
		int a_BlockX, int a_BlockY, int a_BlockZ, eBlockFace a_BlockFace,
		int a_CursorX, int a_CursorY, int a_CursorZ,
		BLOCKTYPE & a_BlockType, NIBBLETYPE & a_BlockMeta
	) override
	{
		if (a_BlockFace == BLOCK_FACE_TOP)
		{
			a_BlockMeta = cBlockBannerStandingHandler::RotationToMetaData(a_Player->GetYaw());
			a_BlockType = E_BLOCK_STANDING_BANNER;
		}
		else
		{
			a_BlockMeta = cBlockBannerWallHandler::DirectionToMetaData(a_BlockFace);
			a_BlockType = E_BLOCK_WALL_BANNER;
		}
		return true;
	}
} ;
