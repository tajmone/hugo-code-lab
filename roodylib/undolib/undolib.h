!::
! UNDOlib by Roody Yogurt
! Version .5
!::

!\ The obvious scope of this extension is to add a couple options to >UNDO.
	While written specifically to enhance the "newconverse" library extension,
	it also has some basic functions that could be applied to any game.

	Set the show_commands global to true in your game if you'd like >UNDO
	to show the player what command is being undone. It's just an attempt to
	keep the player on the same page, especially when certain xverb and
	unsuccessful commands make it unclear.

	Moves that return false (therefore, not calling main) but aren't xverbs CAN
	be undone. UNDOlib's default behavior in this scenario is to clear the
	"undocommand" word array. If you'd like a specific "unsuccessful" action
	to have its command saved, add the following code to it:

	SaveCommand
	last_turn_true = true

	The main other thing is an attempt to support multiple-undo, where the game
	UNDOs repeatedly until a global doesn't return true. One idea would be a
	game with an unwinnable scenario where it can undo until back when it was
	winnable. Just a warning: some interpreters like Gargoyle have a crazy
	small undo memory buffer, so you may want to code several behaviors into
	your game in such case.

	To use, set continue_undo to true in your code during turns you'd want it
	to UNDO past. replace ContinueUndoFailure and ContinueUndoSuccess with
	routines with messages that better fit your game:

	replace ContinueUndoSuccess
	{
		"UNDOing back to a point where the game was winnable.."
	}

	(UNDOlib uses individual routines for messages to work better with other
	contributions like newconverse.h while still allowing for easy replacement.)

	Special Note Concerning "show_commands" and EndGame-
	If you are using the show_commands feature, be sure to run "main" one last
	time before setting "endflag", as it needs to run an event one last time
	(so be aware of that if you have other events you *don't* want run).
	You *may* need to also set the last_turn_true global to true. Not sure of
	that off the top of my head.
\!

#ifset VERSIONS
#message "UNDOlib.h Version .5"
#endif

#ifset USE_EXTENSION_CREDITING
#ifclear _ROODYLIB_H
#message error "Extension crediting requires \"roodylib.h\". Be sure to include it first!"
#endif
version_obj undolib_version "UNDOlib Version .5"
{
	in included_extensions
	desc_detail
		" by Roody Yogurt";
}
#endif

#ifclear _UNDOLIB_H
#set _UNDOLIB_H

#ifclear _ROODYLIB_H
#message error "Oh no! UNDOlib requires the roodylib.h! Be sure to include it
first."
#endif

#if undefined UNDO_WORDS
constant UNDO_WORDS 1
#endif


array undocommands[UNDO_WORDS]

global continue_undo
global show_commands

property enough_elements alias e_to

object undolib "undolib"
{
	in init_instructions
	enough_elements 0
	save_info
	{
		local a
		if show_commands and self.enough_elements and
			undocommands[0] ~= 1 and not continue_undo and
			undocommands[0] ~= "" and word[1] = "undo"
		{
				Font(ITALIC_ON)
				print "[ undoing ";
					Font(BOLD_ON|ITALIC_OFF)
					print ">";
					while undocommands[a] ~= ""
					{
						if undocommands[a] = "~and"
							print ",";
						else
							print undocommands[a];
						if undocommands[] = a + 1
							break
						elseif undocommands[++a] ~= "~and"
							print " ";
					}
					Font(BOLD_OFF|ITALIC_ON)
	!			}
				print "]"
				Font(DEFAULT_FONT|ITALIC_OFF)
		}
		if continue_undo
			return true
		else
			return false
	}
	execute
	{
		if not CheckWordSetting("undo") or continue_undo
			return false
		ContinueUndoSuccess ! "UNDOing several turns..."
	}
}


event
{
	if not show_commands
		return
	if undocommands[] >= words
		SaveCommand
	else
		undolib.enough_elements = 0
}

routine SaveCommand
{
	local a, b
	a = 1
	while word[a] ~= "" and a <= undocommands[]
	{
		undocommands[b++] = word[a++]
	}
	if a < undocommands[]
		undocommands[b] = ""
	undolib.enough_elements = true
}

object undo_preparse
{
	in preparse_instructions
	execute
	{
		if not last_turn_true
		{
			ClearArray(undocommands)
		}
	}
}


replace DoUndo
{
	if not UNDO_OFF
	{
		if display.statusline_height > 2
		{
			local c
			c = display.statusline_height
			while word[c] ~= ""
			{
				c++
			}
			word[c] = "statusheight"
		}
		SaveWordSetting("undo")
		local i
		for i in init_instructions
		{
			if i.save_info
				SaveWordSetting(i.name)
		}
		if undo
		{
			while continue_undo
			{
				if not undo
				{
					ContinueUndoFailure ! "Unable to UNDO enough."
					break
				}
			}
			for i in init_instructions
			{
				if CheckWordSetting(i.name)
					run i.execute
			}
			c = 1
			while c < 11
			{
				if word[c] = "statusheight"
					break
				c++
			}
			if c ~= 11
				display.statusline_height = c
			PrintStatusline
			DescribePlace(location)
#ifset USE_AFTER_UNDO
			if after_undo[0]
			{
				local a
				while after_undo[a] ~= 0
				{
					call after_undo[a]
					after_undo[a++] = 0
				}
			}
#endif ! USE_AFTER_UNDO
			last_turn_true = true
			ClearWordArray
			return true
		}
		else
			CannotUndo ! "Unable to undo."
	}
	else
		CannotUndo ! "Unable to undo."
	ClearWordArray
}

routine ContinueUndoSuccess
{
	"UNDOing several turns..."
}

routine ContinueUndoFailure
{
	"Unable to UNDO enough."
}

routine CannotUndo
{
	"Unable to undo."
}

replace ProcessKey(entry,end_type)
{
	local r

	if not entry
		return -1

	select entry
		case "restart", "r" : r = &DoRestart
		case "restore", "e" : r = &DoRestore
#ifclear NO_UNDO
		case "undo", "u" : r = &DoUndo
#endif
		case SpecialKey(end_type) : r = &SpecialRoutine
		case "quit", "q"! : r = &DoQuit
		{
			""
			RLibMessage(&DoQuit,1,end_type) ! "Thanks for playing!"
			""
			display.title_caption = PRESS_ANY_KEY
			if not system(61) ! if not simple port simple port
			{
				print PRESS_ANY_KEY;
				HiddenPause
			}
			quit
		}
	if not (call r) : return -1 : else : return true
}

#endif