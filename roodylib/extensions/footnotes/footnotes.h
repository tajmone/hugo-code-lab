!----------------------------------------------------------------------------
!* FOOTNOTES
!----------------------------------------------------------------------------
!\ This section allows Hitchhiker/Guilty Bastards style footnotes.
\!

#if undefined MAXFOOTNOTES
constant MAXFOOTNOTES 10
#endif

! if roodylib.h has been included before footnotes.h, nothing needs to be
! added to the main routine

property footnote_notify alias d_to
property footnotearray alias cant_go
property totalfootnotes alias out_to
property showfootnotes alias e_to

!\ Notes- I made two settings objects. One goes in main_instructions
 to handle the printing of footnotes. The other goes in init_instructions
 and handles the saving of the footnotes setting over the play session
 (so it remains consistent over game restarts and restorations).
\!
object footnotelib "footnote"
{
	footnote_notify 0
	footnotearray #MAXFOOTNOTES
	totalfootnotes 0
	showfootnotes 1
	save_info
	{
		select self.showfootnotes
			case 0 : SaveWordSetting("never")
			case 1 : SaveWordSetting("once")
			case 8 : SaveWordSetting("always")
		return true
	}
	type settings
	in init_instructions
	execute
	{
		local a
		a = CheckWordSetting("footnote")
		if a
		{
			select word[(a+1)]
				case "never": self.showfootnotes = 0
				case "once": self.showfootnotes = 1
				case "always": self.showfootnotes = 8
		}
	}
	usage_desc
	{
		Indent
		"\BFOOTNOTE #- Prints applicable footnote."
		Indent
		"\BFOOTNOTES ON\b- Footnote prompts are shown."
		Indent
		"\BFOOTNOTES OFF/NEVER\b- Footnote prompts are not shown."
		Indent
		"\BFOOTNOTES ALWAYS\b- Footnote prompts show each time (not just the
		first time)."
	}
}

object footnotemain
{
	type settings
	in main_instructions
	execute
	{
		FootnoteNotify
	}
}

routine DoFootnote
{
	if not object
	{
		"The proper syntax is >FOOTNOTE [number]."
		return false
	}
	local a
	a = object
	if a < 1 or a >= MAXFOOTNOTES
		"That isn't a valid footnote number."
#ifclear HITCHHIKER_STYLE
	elseif not footnotelib.footnotearray #a
		"You haven't encountered that footnote yet."
	else
		PrintFootnote(footnotelib.footnotearray #a)
#else
	else
		PrintFootnote(a)
#endif
	return false
}

routine AddFootnote(num,silent)
{
	local a
	a = InList(footnotelib,footnotearray,num)
	if not a and footnotelib.showfootnotes
	{
		footnotelib.footnotearray #(++footnotelib.totalfootnotes) = num
		if not silent
			footnotelib.footnote_notify = footnotelib.totalfootnotes
	}
	elseif a and footnotelib.showfootnotes = 8
	{
		if not silent
			footnotelib.footnote_notify = a
	}
}

routine FootnoteNotify
{
	if not footnotelib.footnote_notify
		return
		""
	print "\I("; "Footnote "; number footnotelib.footnote_notify; ")\i"
	footnotelib.footnote_notify = 0
}

! the Footnote routine is a shortcut for printing
! "(Footnote <whatever is the next footnote number>)"
routine Footnote(num)
{
	local a
	a = InList(footnotelib,footnotearray,num)
	if not a and footnotelib.showfootnotes
	{
		AddFootnote(num,1)
		print "("; "Footnote "; number footnotelib.totalfootnotes; ")";
	}
	elseif a and footnotelib.showfootnotes = 8
		print "("; "Footnote ";  number a ; ")";
}

routine DoFootnoteMode
{
	if word[2] = "on", "normal"
	{
		if footnotelib.showfootnotes = 1
			"Footnotes are already on."
		else
		{
			footnotelib.showfootnotes = 1
			"Footnotes on."
		}
	}
	elseif word[2] = "off", "never"
	{
		if not footnotelib.showfootnotes
			"Footnotes are already off."
		else
		{
			footnotelib.showfootnotes = 0
			"Footnotes off."
		}
	}
	elseif word[2] = "always"
	{
		if footnotelib.showfootnotes = 8
			"Footnotes are already in always-on mode."
		else
		{
			footnotelib.showfootnotes = 8
			"Footnotes always on."
		}
	}
	else
		"Try \"footnote (number)\" to read a particular footnote.  \
		Normally the indication of a footnote is printed only once;
		to make sure it is always printed, use \"footnotes always\".
		Use \"footnotes never\" to turn footnote printing off or
		\"footnotes normal\" to restore the default mode."
}

routine PrintFootNote
{
	"You need to replace the PrintFootNote routine with one that prints
	your own responses!"
}