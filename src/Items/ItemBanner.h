
#pragma once

#include "ItemHandler.h"
#include "../World.h"
#include "../Blocks/BlockBannerStanding.h"
#include "../Blocks/BlockBannerWall.h"
#include "../BlockEntities/BannerEntity.h"
#include "../WorldStorage/FastNBT.h"



class cItemBannerHandler :
	public cItemHandler
{
	typedef cItemHandler super;
public:
	cItemBannerHandler(int a_ItemType) :
		super(a_ItemType)
	{
	}

	virtual bool OnPlayerPlace(
		cWorld & a_World, cPlayer & a_Player, const cItem & a_EquippedItem,
		int a_BlockX, int a_BlockY, int a_BlockZ, eBlockFace a_BlockFace,
		int a_CursorX, int a_CursorY, int a_CursorZ
	) override
	{
		// If the regular placement doesn't work, do no further processing:
		if (!super::OnPlayerPlace(a_World, a_Player, a_EquippedItem, a_BlockX, a_BlockY, a_BlockZ, a_BlockFace, a_CursorX, a_CursorY, a_CursorZ))
		{
			return false;
		}

		AddFaceDirection(a_BlockX, a_BlockY, a_BlockZ, a_BlockFace);
		
		// Use a callback to set the properties of the banner entity
		class cCallback : public cBlockEntityCallback
		{
			cPlayer & m_Player;
			NIBBLETYPE m_BlockMeta;
			const Json::Value & m_Meta;

			virtual bool Item(cBlockEntity * a_BlockEntity)
			{
				if (!(a_BlockEntity->GetBlockType() == E_BLOCK_STANDING_BANNER || a_BlockEntity->GetBlockType() == E_BLOCK_WALL_BANNER))
				{
					return false;
				}
				auto BannerEntity = static_cast<cBannerEntity *>(a_BlockEntity);


				Json::Value banner = m_Meta["Banner"];
				if (!banner.isNull()) {
					BannerEntity->SetBaseColor(banner["Base"].asInt());
					for (auto pattern : banner["Patterns"]) {
						BannerEntity->AddPattern(pattern["Color"].asInt(), pattern["Pattern"].asString());
					}
				} else {
					LOG("Expected metadata for BannerItem.");
				}

				BannerEntity->GetWorld()->BroadcastBlockEntity(BannerEntity->GetPosX(), BannerEntity->GetPosY(), BannerEntity->GetPosZ());
				return false;
			}

		public:
			cCallback(cPlayer & a_CBPlayer, const Json::Value & a_ItemMeta, NIBBLETYPE a_BlockMeta) :
				m_Player(a_CBPlayer),
				m_Meta(a_ItemMeta),
				m_BlockMeta(a_BlockMeta)
			{}
		};
		cCallback Callback(a_Player, a_EquippedItem.m_Metadata, static_cast<NIBBLETYPE>(a_BlockFace));
		a_World.DoWithBlockEntityAt(a_BlockX, a_BlockY, a_BlockZ, Callback);

		return true;
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

	virtual void MetadataToNBT(const Json::Value & a_Metadata, cFastNBTWriter & a_Writer)
	{
		Json::Value banner = a_Metadata["Banner"];

		a_Writer.BeginCompound("BlockEntityTag");

		a_Writer.AddInt("Base", banner["Base"].asInt());

		if (banner["Patterns"].size() > 0) {

			a_Writer.BeginList("Patterns", TAG_Compound);

			for (auto pattern : banner["Patterns"]) {
				a_Writer.BeginCompound("");
				a_Writer.AddInt("Color", pattern["Color"].asInt());
				a_Writer.AddString("Pattern", pattern["Pattern"].asString());
				a_Writer.EndCompound();
			}

			a_Writer.EndList();

		}

		a_Writer.EndCompound();
	}

	virtual void MetadataFromNBT(Json::Value & a_Metadata, const cParsedNBT & a_NBT)
	{
		Json::Value banner;
		int blockent = a_NBT.FindChildByName(a_NBT.GetRoot(), "BlockEntityTag");
		if (blockent < 0) {
			return;
		}
		int base = a_NBT.FindChildByName(blockent, "Base");
		if (base >= 0) {
			banner["Base"] = a_NBT.GetInt(base);
		}
		int patternsTag = a_NBT.FindChildByName(blockent, "Patterns");
		if (patternsTag >= 0) {
			Json::Value patterns;
			int patternTag = a_NBT.GetFirstChild(patternsTag);
			while (patternTag >= 0) {
				int ptColor = a_NBT.FindChildByName(patternTag, "Color");
				int ptPattern = a_NBT.FindChildByName(patternTag, "Pattern");
				if (ptColor >= 0 && ptPattern >= 0) {
					Json::Value p;
					p["Color"] = a_NBT.GetInt(ptColor);
					p["Pattern"] = a_NBT.GetString(ptPattern);
					patterns.append(p);
				}
				patternTag = a_NBT.GetNextSibling(patternTag);
			}
			banner["Patterns"] = patterns;
		}
		a_Metadata["Banner"] = banner;
	}
} ;