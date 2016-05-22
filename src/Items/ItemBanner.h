
#pragma once

#include "ItemHandler.h"
#include "../World.h"
#include "../Blocks/BlockBannerStanding.h"
#include "../Blocks/BlockBannerWall.h"
#include "../BlockEntities/BannerEntity.h"
#include "../WorldStorage/FastNBT.h"



class cBannerItemMeta : public cItemMeta
{
public:
	virtual void FromNBT(const cParsedNBT & a_NBT)
	{
		int blockent = /*a_NBT.GetFirstChild(a_NBT.GetRoot());*/a_NBT.FindChildByName(a_NBT.GetRoot(), "BlockEntityTag");
		if (blockent < 0) {
			return;
		}
		int base = a_NBT.FindChildByName(blockent, "Base");
		if (base >= 0) {
			m_Base = a_NBT.GetInt(base);
		}
		int patterns = a_NBT.FindChildByName(blockent, "Patterns");
		if (patterns >= 0) {
			int pattern = a_NBT.GetFirstChild(patterns);
			while (pattern >= 0) {
				int ptColor = a_NBT.FindChildByName(pattern, "Color");
				int ptPattern = a_NBT.FindChildByName(pattern, "Pattern");
				if (ptColor >= 0 && ptPattern >= 0) {
					cBannerEntity::cPattern p;
					p.Color = a_NBT.GetInt(ptColor);
					p.Pattern = a_NBT.GetString(ptPattern);
					m_Patterns.push_back(p);
				}
			}
		}
	}

	virtual void FromCopy(const cItemMeta * a_Meta)
	{
		const cBannerItemMeta * meta = dynamic_cast<const cBannerItemMeta*>(a_Meta);
		if (!meta) return;
		m_Base = meta->m_Base;
		m_Patterns = meta->m_Patterns;
	}

	virtual void FromJSON(const Json::Value & a_Value)
	{
		Json::Value banner = a_Value["Banner"];
		m_Base = banner["Base"].asInt();

		Json::Value patterns = banner["Patterns"];
		if (patterns.isNull())
			return;

		for (auto pattern : patterns) {
			cBannerEntity::cPattern p;
			p.Color = pattern["Color"].asInt();
			p.Pattern = pattern["Pattern"].asString();
			m_Patterns.push_back(p);
		}
	}

	virtual void ToNBT(cFastNBTWriter & a_Writer)
	{
		a_Writer.BeginCompound("BlockEntityTag");

		a_Writer.AddInt("Base", m_Base);

		a_Writer.BeginList("Patterns", TAG_Compound);

		for (auto Pattern : m_Patterns) {
			a_Writer.BeginCompound("");
			a_Writer.AddInt("Color", Pattern.Color);
			a_Writer.AddString("Pattern", Pattern.Pattern);
			a_Writer.EndCompound();
		}

		a_Writer.EndList();

		a_Writer.EndCompound();
	}

	virtual void ToJSON(Json::Value & a_OutValue)
	{
		Json::Value banner;
		banner["Base"] = m_Base;

		Json::Value patterns;
		for (auto Pattern : m_Patterns) {
			Json::Value p;
			p["Color"] = Pattern.Color;
			p["Pattern"] = Pattern.Pattern;
			patterns.append(p);
		}
		banner["Patterns"] = patterns;

		a_OutValue["Banner"] = banner;
	}

	virtual bool IsEqual(cItemMeta * a_ItemMeta)
	{
		cBannerItemMeta * meta = dynamic_cast<cBannerItemMeta*>(a_ItemMeta);
		if (!meta) return false;
		if (meta->m_Base != m_Base) return false;
		if (meta->m_Patterns.size() != m_Patterns.size()) return false;
		for (int i = 0; i < m_Patterns.size(); ++i) {
			auto left = m_Patterns[i];
			auto right = meta->m_Patterns[i];
			if (left.Color != right.Color) return false;
			if (left.Pattern != right.Pattern) return false;
		}
		return true;
	}

	int m_Base;
	cBannerEntity::PatternList m_Patterns;
};



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
			cBannerItemMeta * m_Meta;

			virtual bool Item(cBlockEntity * a_BlockEntity)
			{
				if (!(a_BlockEntity->GetBlockType() == E_BLOCK_STANDING_BANNER || a_BlockEntity->GetBlockType() == E_BLOCK_WALL_BANNER))
				{
					return false;
				}
				auto BannerEntity = static_cast<cBannerEntity *>(a_BlockEntity);

				if (m_Meta) {
					BannerEntity->SetBaseColor(m_Meta->m_Base);
					BannerEntity->SetPatterns(m_Meta->m_Patterns);
				} else {
					LOG("Expected metadata for BannerItem.");
				}

				BannerEntity->GetWorld()->BroadcastBlockEntity(BannerEntity->GetPosX(), BannerEntity->GetPosY(), BannerEntity->GetPosZ());
				//m_Player.GetClientHandle()->SendUpdateBlockEntity(*a_BlockEntity);
				return false;
			}

		public:
			cCallback(cPlayer & a_CBPlayer, cBannerItemMeta * a_ItemMeta, NIBBLETYPE a_BlockMeta) :
				m_Player(a_CBPlayer),
				m_Meta(a_ItemMeta),
				m_BlockMeta(a_BlockMeta)
			{}
		};
		cCallback Callback(a_Player, dynamic_cast<cBannerItemMeta*>(a_EquippedItem.GetItemMeta()), static_cast<NIBBLETYPE>(a_BlockFace));
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

	virtual cItemMeta* MakeItemMeta() override
	{
		return new cBannerItemMeta();
	}
} ;