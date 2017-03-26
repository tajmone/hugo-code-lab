verb "footnote","note","fn","f"
*							DoFootnote
*	number				DoFootnote

xverb "help"
	* "footnotes"						DoFootnoteMode

xverb "footnotes", "notes"
	*										DoFootnoteMode
	* "help"                      DoFootnoteMode
	* "always"							DoFootnoteMode
	* "on"/"normal"					DoFootnoteMode
	* "off"/"never"					DoFootnoteMode