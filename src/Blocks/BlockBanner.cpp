#include "Globals.h"
#include "BlockBannerStanding.h"
#include "BlockBannerWall.h"
#include "../BlockEntities/BannerEntity.h"
#include "../Items/ItemBanner.h"


void _OnDestroyedByPlayer(cChunkInterface & a_ChunkInterface, cWorldInterface & a_WorldInterface, cPlayer * a_Player, int a_BlockX, int a_BlockY, int a_BlockZ, BLOCKTYPE a_BlockType)
{
	if (a_Player->IsGameModeCreative())
	{
		// No drops in creative mode
		return;
	}

	class cCallback : public cBlockEntityCallback
	{
		BLOCKTYPE m_BlockType;

		virtual bool Item(cBlockEntity * a_BlockEntity)
		{
			if (a_BlockEntity->GetBlockType() != m_BlockType)
			{
				return false;
			}
			cBannerEntity * BannerEntity = static_cast<cBannerEntity*>(a_BlockEntity);

			cItems Pickups;
			cItem banner(E_ITEM_BANNER, 1, 0);

			// set meta data
			cItemMeta * meta = banner.GetHandler()->MakeItemMeta();
			if (meta) {
				cBannerItemMeta * bannerMeta = static_cast<cBannerItemMeta*>(meta);
				bannerMeta->m_Base = BannerEntity->GetBaseColor();
				bannerMeta->m_Patterns = BannerEntity->GetPatterns();
				banner.SetItemMeta(bannerMeta);
			}

			Pickups.Add(banner);
			MTRand r1;

			// Mid-block position first
			double MicroX, MicroY, MicroZ;
			MicroX = BannerEntity->GetPosX() + 0.5;
			MicroY = BannerEntity->GetPosY() + 0.5;
			MicroZ = BannerEntity->GetPosZ() + 0.5;

			// Add random offset second
			MicroX += r1.rand(1) - 0.5;
			MicroZ += r1.rand(1) - 0.5;

			BannerEntity->GetWorld()->SpawnItemPickups(Pickups, MicroX, MicroY, MicroZ);
			return false;
		}

	public:
		cCallback(BLOCKTYPE a_BlockType) : m_BlockType(a_BlockType)
		{

		}
	};

	a_WorldInterface.DoWithBlockEntityAt(a_BlockX, a_BlockY, a_BlockZ, cCallback(a_BlockType));
}

void cBlockBannerStandingHandler::OnDestroyedByPlayer(cChunkInterface & a_ChunkInterface, cWorldInterface & a_WorldInterface, cPlayer * a_Player, int a_BlockX, int a_BlockY, int a_BlockZ)
{
	_OnDestroyedByPlayer(a_ChunkInterface, a_WorldInterface, a_Player, a_BlockX, a_BlockY, a_BlockZ, E_BLOCK_STANDING_BANNER);
}

void cBlockBannerWallHandler::OnDestroyedByPlayer(cChunkInterface & a_ChunkInterface, cWorldInterface & a_WorldInterface, cPlayer * a_Player, int a_BlockX, int a_BlockY, int a_BlockZ)
{
	_OnDestroyedByPlayer(a_ChunkInterface, a_WorldInterface, a_Player, a_BlockX, a_BlockY, a_BlockZ, E_BLOCK_WALL_BANNER);
}