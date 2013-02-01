!::
!  FOOTNOTES.H GRAMMAR
!::


verb "footnote","note"
*							DoFootnote
*	number				DoFootnote

xverb "help"
	* "footnotes"						DoFootnoteMode

xverb "footnotes", "notes"
	*										DoFootnoteMode
	* "always"							DoFootnoteMode
	* "on"/"normal"					DoFootnoteMode
	* "off"/"never"					DoFootnoteMode