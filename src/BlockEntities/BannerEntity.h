// BannerEntity.h

// Declares the cBannerEntity class representing a single sign in the world





#pragma once

#include "BlockEntity.h"





// tolua_begin

class cBannerEntity :
	public cBlockEntity
{
	typedef cBlockEntity super;

public:

	// tolua_end

	BLOCKENTITY_PROTODEF(cBannerEntity)

		/** Creates a new banner entity at the specified block coords and block type (wall or standing). a_World may be nullptr */
		cBannerEntity(BLOCKTYPE a_BlockType, int a_X, int a_Y, int a_Z, cWorld * a_World, NIBBLETYPE a_Meta);

	struct cPattern {
		int Color;
		AString Pattern;
	};

	typedef std::vector<cPattern> PatternList;

	// tolua_begin

	/** Set the base color */
	void SetBaseColor(int a_Base);

	/** Get the base color */
	int GetBaseColor() const;

	/** Add pattern */
	void AddPattern(int a_Color, AString a_Pattern);

	/** Clear patterns */
	void ClearPatterns();

	// tolua_end


	/** Set the patterns */
	void SetPatterns(PatternList & list);

	/** Get the patterns. */
	const PatternList& GetPatterns() const;

	virtual bool UsedBy(cPlayer * a_Player) override;
	virtual void SendTo(cClientHandle & a_Client) override;

private:

	int m_Base;
	PatternList m_Patterns;

};  // tolua_export




