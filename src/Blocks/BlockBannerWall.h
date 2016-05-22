
#pragma once

#include "BlockHandler.h"
#include "../Entities/Player.h"
#include "Chunk.h"
#include "ChunkInterface.h"


class cBlockBannerWallHandler :
	public cBlockHandler
{
	typedef cBlockHandler super;

public:
	cBlockBannerWallHandler(BLOCKTYPE a_BlockType) :
		super(a_BlockType)
	{
	}

	virtual void ConvertToPickups(cItems & a_Pickups, NIBBLETYPE a_BlockMeta) override
	{
		// The drop spawn is in the OnDestroyedByPlayer method
	}

	virtual void OnDestroyedByPlayer(cChunkInterface & a_ChunkInterface, cWorldInterface & a_WorldInterface, cPlayer * a_Player, int a_BlockX, int a_BlockY, int a_BlockZ) override;

	virtual bool CanBeAt(cChunkInterface & a_ChunkInterface, int a_RelX, int a_RelY, int a_RelZ, const cChunk & a_Chunk) override
	{
		int BlockX = (a_Chunk.GetPosX() * cChunkDef::Width) + a_RelX;
		int BlockZ = (a_Chunk.GetPosZ() * cChunkDef::Width) + a_RelZ;
		GetBlockCoordsBehindTheSign(a_Chunk.GetMeta(a_RelX, a_RelY, a_RelZ), BlockX, BlockZ);
		BLOCKTYPE Type = a_ChunkInterface.GetBlock(BlockX, a_RelY, BlockZ);

		return ((Type == E_BLOCK_WALL_BANNER) || (Type == E_BLOCK_STANDING_BANNER) || cBlockInfo::IsSolid(Type));
	}

	static void GetBlockCoordsBehindTheSign(NIBBLETYPE a_BlockMeta, int & a_BlockX, int & a_BlockZ)
	{
		switch (a_BlockMeta)
		{
			case 2:  a_BlockZ++; break;
			case 3:  a_BlockZ--; break;
			case 4:  a_BlockX++; break;
			case 5:  a_BlockX--; break;
			default: break;
		}
	}

	static NIBBLETYPE DirectionToMetaData(eBlockFace a_Direction)
	{
		switch (a_Direction)
		{
			case BLOCK_FACE_ZM: return 0x2;
			case BLOCK_FACE_ZP: return 0x3;
			case BLOCK_FACE_XM: return 0x4;
			case BLOCK_FACE_XP: return 0x5;
			case BLOCK_FACE_NONE:
			case BLOCK_FACE_YP:
			case BLOCK_FACE_YM:
			{
				break;
			}
		}
		return 0x2;
	}

	virtual ColourID GetMapBaseColourID(NIBBLETYPE a_Meta) override
	{
		UNUSED(a_Meta);
		return 13;
	}
} ;
