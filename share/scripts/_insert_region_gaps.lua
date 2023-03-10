ardour {
	["type"]    = "EditorAction",
	name        = "Insert Gaps",
	license     = "MIT",
	author      = "Ardour Team",
	description = [[Insert gaps between all regions on selected tracks]]
}

function action_params ()
	return
	{
		["gap"] = { title = "Gap size (in sec)", default = "2" },
	}
end

function factory () return function ()
	-- get configuration
	local p = params or {}
	local gap = p["gap"] or 2
	if gap <= 0 then gap = 2 end

	local sel = Editor:get_selection () -- get current selection

	Session:begin_reversible_command ("Insert Gaps")

	-- iterate over all selected tracks
	for route in sel.tracks:routelist ():iter () do
		local track = route:to_track ()
		if track:isnil () then goto continue end

		-- get track's playlist
		local playlist = track:playlist ()
		local offset = 0

		-- iterate over all regions in the playlist
		for region in playlist:region_list():iter() do

			-- preare for undo operation
			region:to_stateful ():clear_changes ()

			-- move region
			region:set_position (region:position() + Temporal.timecnt_t (offset))
			offset = offset + Session:nominal_sample_rate () * gap

			-- create a diff of the performed work, add it to the session's undo stack
			-- and check if it is not empty
			Session:add_stateful_diff_command (region:to_statefuldestructible ())
		end
		::continue::
	end

	-- all done, commit the combined Undo Operation
	if not Session:abort_empty_reversible_command () then
		Session:commit_reversible_command (nil)
	end

end end
