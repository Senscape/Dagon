require 'journal'

Notes = {
	TUNNEL = "'Tunnel of the Damned'",
	HOSPITAL = "Hospital",
	KEY_MALE = "Key to Male Cells"
}

journal = Journal.create()

journal:addEntry(Notes.TUNNEL, 
	function()
		feed("A wise man is said to live there that may be able provide the answers she seeks.", "dlg_00001.ogg")
	end)
	
journal:addEntry(Notes.HOSPITAL,
	function()
		feed("Myosotis, the Trader of Stories, has travelled far across the land in search...", "dlg_00002.ogg")
	end)

journal:addEntry("Test",
	function()
		feed("For some time now, she has been told tales of a place where the Great Oak stands and that rocks float in the air.", "dlg_00002.ogg")
	end)