return
{
	HOOK_PLAYER_FISHED =
	{
		CalledWhen = "A player gets a reward from fishing.",
		DefaultFnName = "OnPlayerFished", -- also used as pagename
		Desc = [[
			This hook gets called when a player already has his reward from fishing.
		]],
		Params =
		{
			{ Name = "Player", Type = "{{cPlayer}}", Notes = "The player who pulled the fish in." },
			{ Name = "Reward", Type = "{{cItems}}", Notes = "The reward the player gets. It can be a fish, tressure and junk." },
		},
		Returns = [[
			If the function returns false or no value, the next plugin's callback is called. If the function returns true, no other
			callback is called for this event.
		]],
	}, -- HOOK_PLAYER_FISHED
}	