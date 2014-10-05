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
#message "UNDOlib.h Version .6"
#endif

#ifset USE_EXTENSION_CREDITING
#ifclear _ROODYLIB_H
#message error "Extension crediting requires \"roodylib.h\". Be sure to include it first!"
#endif
version_obj undolib_version "UNDOlib Version .6"
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

#ifset CONTINUE_UNDO
global continue_undo

event
{
	if continue_undo
		continue_undo++
}

routine ContinueUndoSuccess
{
	"UNDOing several turns..."
}

routine ContinueUndoFailure
{
	"Unable to UNDO enough."
}
#endif  ! CONTINUE_UNDO

#ifset SHOW_COMMANDS

#if undefined UNDO_WORDS
constant UNDO_WORDS 10
#endif

array undocommands[UNDO_WORDS]

property enough_elements alias e_to

routine CopyArray(arr1,arr2,p,s)
{
	local i = 1, n
	if p
		i = p
	if s
		n = s
	while array arr1[i] ~= ""
	{
		if array arr1[i] = -1
			return false
		array arr2[n++] = array arr1[i++]
	}
	array arr2[n] = ""
	return true
}

routine SaveCommand(arr)
{
	local a, b
	a = 1
	if array arr[] < CurrentCommandWords or verbroutine = &SpeakTo
	{
		undolib.enough_elements = false
		return
	}
	while word[a] ~= "" and a <= undocommands[]
	{
		if word[a] = -1
		{
			undolib.enough_elements = false
			return
		}
		undocommands[b++] = word[a++]
	}
	if a < undocommands[]
		undocommands[b] = ""
	undolib.enough_elements = true
}

routine PrintCommand(arr,n)
{
	while array arr[n] ~= "" and n < array arr[]
	{
		select array arr[n]
			case "~and"
			{
				if array arr[(n-1)] = "~and"
					print " and";
				else
					print ",";
			}
			case "~all"
				print "all";
			case "~except" : print "except";
			case "~oops" : print "oops";  ! hopefully never called
			case "~any" : print "any";
			case else
				print array arr[n];
		if array arr[n+1] ~= "~and", ""
			print " ";
		n++
	}
}
#endif ! SHOW_COMMANDS

object undolib "undolib"
{
	in init_instructions
#ifset SHOW_COMMANDS
	enough_elements 0
#endif
	save_info
	{
		if ContinueUndoCheck
			return true
#ifset SHOW_COMMANDS
		if self.enough_elements and
			undocommands[0] ~= 1 and
			undocommands[0] ~= "" and word[1] = "undo"
		{
			Font(ITALIC_ON)
			print "[ undoing ";
			Font(BOLD_ON|ITALIC_OFF)
			print ">";
			PrintCommand(undocommands)
			Font(BOLD_OFF|ITALIC_ON)
			print "]"
			Font(DEFAULT_FONT|ITALIC_OFF)
			return true
		}
#endif
			return false
	}
	execute
	{
		if word[LAST_TURN] ~= "undo" ! or continue_undo
			return false
#ifset CONTINUE_UNDO
		if continue_undo
			ContinueUndoSuccess ! "UNDOing several turns..."
#endif
		SaveWordSetting("continue")
	}
}

routine ContinueUndoCheck
{
#ifset CONTINUE_UNDO
	if continue_undo > 2
		return true
#endif
	return false
}

replace UndoResponse
{
	local fail
#ifset CONTINUE_UNDO
	while continue_undo
	{
		if not undo
		{
			""
			ContinueUndoFailure ! "Unable to UNDO enough."
			fail = true
			break
		}
	}
#endif
	PrintStatusLine
	if not fail
	{
		local a
		a = CheckWordSetting("continue")
		if not a
			RlibMessage(&UndoResponse,1) ! "Undone."
		else
			word[a] = ""
	}
	if FORMAT & DESCFORM_I
		""
	DescribePlace
}

#endif