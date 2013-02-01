!::
! FOOTNOTES.H by Roody Yogurt
! A library extension for chronologically-ordered footnote announcements
!::

!\
	HOW TO SET UP THIS EXTENSION-

1. #include "footnotes.g" with your grammar and "footnotes.h" after "hugolib.h"
2. before "footnotes.h" is included, define a constant called MAXFOOTNOTES that
tells the game how many possible footnotes there are:

	constant MAXFOOTNOTES 20

3. put the call "FootnoteNotify" in the main routine *after* runevents (not
   necessary if you are using roodylib and a main routine that calls Main_Calls)
4. Add a "PrintFootnote" routine to your game, like the following:

(The cases are the actual footnotes your game will respond with.)

routine PrintFootnote(num)
{
	select num
			case 1 : "Actually, this game was also submitted to the 2011 Hugo Comp. That's TWO comps! Crazy, right?"
			case 2 : "[ Ok, so you converse in different ways with different characters. Get over it. ]"
}

	HOW TO ADD FOOTNOTES IN THE GAME-
	Calling Footnote(case number) will automatically print "(Footnote <next footnote number>)" instantly and add <case number> from PrintFootnote to
	your footnote array.

	If you want the game to announce the footnote with the after-the-turn
	notification system, just call:
	AddFootnote(number)

	If you want to print your own footnote announcement, print it and call:
	AddFootnote(number, 1)

\!

#ifclear _FOOTNOTES_H
#set _FOOTNOTES_H

#ifset VERSIONS
#message "Footnotes.h Version 1.3"
#endif

#ifset USE_EXTENSION_CREDITING
version_obj footnotes_version "Footnotes Version 1.3"
{
	in included_extensions
	desc_detail
		" by Roody Yogurt";
}
#endif

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
#ifset _ROODYLIB_H
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
			select word[(a-1)]
				case "never": self.showfootnotes = 0
				case "once": self.showfootnotes = 1
				case "always": self.showfootnotes = 8
			}
		}
#ifset _NEWMENU_H
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
#endif ! NEWMENU
#endif  ! _ROODYLIB_H
}

object footnotemain
{
#ifset _ROODYLIB_H
	type settings
	in main_instructions
	execute
		{
		FootnoteNotify
		}
#endif  ! _ROODYLIB_H
}

routine DoFootnote
{
	if not object
		{
		FootnoteMessage(&DoFootnote,1) ! "The proper syntax is
												 ! >FOOTNOTE [number]."
		return false
		}
	local a
	a = object
	if a < 1 or a >= MAXFOOTNOTES
		FootnoteMessage(&DoFootnote,2) ! "That isn't a valid footnote number."
	elseif not footnotelib.footnotearray #a
		FootnoteMessage(&DoFootnote,3) ! "You haven't encountered that footnote
												 !   yet."
	else
		PrintFootnote(footnotelib.footnotearray #a)
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
	FootnoteMessage(&FootnoteNotify) ! "\I{"; "Footnote #"; ")\i"
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
		FootnoteMessage(&Footnote,1) ! "("; "Footnote #"; ")";
		}
	elseif a and footnotelib.showfootnotes = 8
		FootnoteMessage(&Footnote,2, a) ! "("; "Footnote #"; ")";
}

routine DoFootnoteMode
{
	if word[2] = "on", "normal"
	{
		if footnotelib.showfootnotes = 1
			FootnoteMessage(&DoFootnoteMode, 1) ! "Footnotes are already on."
		else
		{
			footnotelib.showfootnotes = 1
			FootnoteMessage(&DoFootnoteMode, 2) ! "Footnotes on."
		}
	}
	elseif word[2] = "off", "never"
	{
		if not footnotelib.showfootnotes
			FootnoteMessage(&DoFootnoteMode, 3) ! "Footnotes are already off."
		else
		{
			footnotelib.showfootnotes = 0
			FootnoteMessage(&DoFootnoteMode, 4) ! "Footnotes off."
		}
	}
	elseif word[2] = "always"
	{
		if footnotelib.showfootnotes = 8
			FootnoteMessage(&DoFootnoteMode, 5) ! "Footnotes are already in
															! always-on mode."
		else
		{
			footnotelib.showfootnotes = 8
			FootnoteMessage(&DoFootnoteMode, 6) ! "Footnotes akways on."
		}
	}
	else
		FootnoteMessage(&DoFootnoteMode, 7) ! Footnote instructions
}

routine FootnoteMessage(r, num, a, b)
{
	if NewFootnoteMessages(r, num, a, b):  return

	select r
		case &DoFootnote
			{
			select num
				case 1: "The proper syntax is >FOOTNOTE [number]."
				case 2: "That isn't a valid footnote number."
				case 3: "You haven't encountered that footnote yet."
			}
		case &FootNoteNotify
			{
			print "\I("; "Footnote "; number footnotelib.footnote_notify; ")\i"
			}
		case &FootNote
			{
			select num
				case 1
					print "("; "Footnote "; number footnotelib.totalfootnotes; ")";
				case 2
					print "("; "Footnote ";  number a ; ")";
			}
		case &DoFootnoteMode
			{
			select num
				case 1: "Footnotes are already on."
				case 2: "Footnotes on."
				case 3: "Footnotes are already off."
				case 4: "Footnotes off."
				case 5: "Footnotes are already in always-on mode."
				case 6: "Footnotes always on."
				case 7: "Try \"footnote (number)\" to read a particular footnote.  \
							Normally the indication of a footnote is printed only once;
							to make sure it is always printed, use \"footnotes always\".
							Use \"footnotes never\" to turn footnote printing off or
							\"footnotes normal\" to restore the default mode."
			}
}

!\ The NewFootnoteMessages routine may be REPLACED and should return
true if a replacement message exists for routine <r> \!

routine NewFootnoteMessages(r, num, a, b)
{
   select r
   case else : return false
   return true ! this line is only reached if we replaced something
}

#endif