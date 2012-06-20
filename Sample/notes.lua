require 'journal'

Notes = {
	TUNNEL = "'Tunnel of the Damned'",
	HOSPITAL = "Hospital",
	KEY_MALE = "Key to Male Cells"
}

journal = Journal.create()

journal:addEntry(Notes.TUNNEL, 
	function()
		feed("Scary place.")
	end)
	
journal:addEntry(Notes.HOSPITAL,
	function()
		feed("I must find the hospital.")
	end)
