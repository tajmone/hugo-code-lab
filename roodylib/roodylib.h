!::
! ROODYLIB - A collection of hugolib updates and additions
!::

#ifclear _ROODYLIB_H
#set _ROODYLIB_H

constant ROODYBANNER "RoodyLib Version 4.1.2"
constant ROODYVERSION "4.1.2"

#ifset VERSIONS
#message "roodylib.h version 4.1.2"
#endif

#ifset USE_CONFIG_SYSTEM
#set USE_TIME_SYSTEM
#endif
!----------------------------------------------------------------------------
!* SOME RANDOM CONSTANTS AND PROPERTIES AND STUFF
!----------------------------------------------------------------------------

constant LAST_TURN 31 ! used by SaveSettings/LoadSettings to keep track of
                      ! what xverb is being used
#ifclear NO_ACCESSIBILITY
constant CHEAP_ON 1
constant CHEAP_MENUS 2
#endif

! some verbosity constants since I can never remember which value is which
enumerate step + 1
{
	BRIEF, SUPERBRIEF, VERBOSE
}

! some statustype constants
enumerate step * 2
{
	NO_STATUS, SCORE_MOVES = 1, TIME_STATUS, CUSTOM_STATUS, INFOCOM_STYLE,
	MILITARY_TIME
}

enumerate start = 32, step * 2
{
	FINDOBJECT_LIVING, FINDOBJECT_FOUND , FINDOBJECT_CALLED
}

! if newmenu.h is going to be included, it's easier to just declare the
! usage_desc property now
#if undefined usage_desc
property usage_desc alias short_desc
#endif

!----------------------------------------------------------------------------
!* LIBRARY EXTENSION CREDITING SYSTEM
!----------------------------------------------------------------------------
routine Roodylib_Credits
{
#if defined GAME_TITLE
	Font(BOLD_ON)
	print GAME_TITLE;
	Font(BOLD_OFF)
#endif
#if undefined GAME_TITLE
	print "This game";
#endif
	" uses \IRoodylib\i. RoodyLib is a collection of improvements written and/or
	suggested by Kent Tessman, Mike Snyder, Jason McWright, Robb Sherwin, Rob
	O'Hara, Paul Lee, Juhana Leinonen, and Roody Yogurt."
}

#ifset USE_EXTENSION_CREDITING
property show_author alias long_desc
property is_tall alias short_desc

object included_extensions
{
	show_author true  ! credit extension author?
	is_tall true  ! list tall (true) or wide (false) ?
}


class version_obj
{}

!  version_obj example:
!version_obj new_menu_version "New Menu Version 2.5"
!{
!	in included_extensions
!	desc_detail
!		" by Roody Yogurt and Kent Tessman";
!}

! ListExtensions prints the list of extensions.
! Tall/wide listing and whether extension authors are credited are determined
! by included_extensions object properties

!\ Usage:
	Indent
	Roodylib_Credits
	Indent(true)
   "This game uses the following Hugo library extensions:"
	ListExtensions  \!

routine ListExtensions
{
	local saveformat
	saveformat = FORMAT
	if included_extensions.is_tall
		FORMAT = (FORMAT | LIST_F)  ! force the tall listing
	else
		FORMAT = FORMAT & ~LIST_F   ! or force wide
	if not included_extensions.show_author
		FORMAT = (FORMAT | NOPARENTHESES_F) ! don't show author
	else
		FORMAT = FORMAT & ~NOPARENTHESES_F ! or do
	ListChildren(included_extensions)
	FORMAT = saveformat
}
#endif ! USE_EXTENSION_CREDITING

!----------------------------------------------------------------------------
!* REPLACED HUGOLIB.H ROUTINES
!----------------------------------------------------------------------------

! Roody's note: AfterRoutines and BeforeRoutines called player.react_before
! twice

replace AfterRoutines
{
	local i, r

	r = player.after
	if r
	{
#ifset DEBUG
		if debug_flags & D_PARSE
		{
			print "\B["; player.name;
			if debug_flags & D_OBJNUM
				print " ["; number player; "]";
			print ".after returned "; number r; "]\b"
		}
#endif
	}

	r = player.react_after
	if r
	{
#ifset DEBUG
		if debug_flags & D_PARSE
		{
			print "\B["; player.name;
			if debug_flags & D_OBJNUM
				print " ["; number player; "]";
			print ".react_after returned "; number r; "]\b"
		}
#endif
	}

	r = location.after
	if r
	{
#ifset DEBUG
		if debug_flags & D_PARSE
		{
			print "\B["; location.name;
			if debug_flags & D_OBJNUM
				print " ["; number location; "]";
			print ".after returned "; number r; "]\b"
		}
#endif
	}

	r = location.react_after
	if r
	{
#ifset DEBUG
		if debug_flags & D_PARSE
		{
			print "\B["; location.name;
			if debug_flags & D_OBJNUM
				print " ["; number location; "]";
			print ".react_after returned "; number r; "]\b"
		}
#endif
	}

	for i in location
	{
		if i ~= player
			r = i.react_after
		if r
		{
#ifset DEBUG
			if debug_flags & D_PARSE
			{
				print "\B["; i.name;
				if debug_flags & D_OBJNUM
					print " ["; number i; "]";
				print ".react_after returned "; number r; "]\b"
			}
#endif
		}
	}
}

replace BeforeRoutines(queue)
{
	local r, i

	r = player.react_before
	if r
	{
#ifset DEBUG
		if debug_flags & D_PARSE
		{
			print "\B["; player.name;
			if debug_flags & D_OBJNUM
				print " ["; number player; "]";
			print ".react_before returned "; number r; "]\b"
		}
#endif
		return r
	}
	r = player.before
	if r
	{
#ifset DEBUG
		if debug_flags & D_PARSE
		{
			print "\B["; player.name;
			if debug_flags & D_OBJNUM
				print " ["; number player; "]";
			print ".before returned "; number r; "]\b"
		}
#endif
		return r
	}

	r = location.react_before
	if r
	{
#ifset DEBUG
		if debug_flags & D_PARSE
		{
			print "\B["; location.name;
			if debug_flags & D_OBJNUM
				print " ["; number location; "]";
			print ".react_before returned "; number r; "]\b"
		}
#endif
		return r
	}
	r = location.before
	if r
	{
#ifset DEBUG
		if debug_flags & D_PARSE
		{
			print "\B["; location.name;
			if debug_flags & D_OBJNUM
				print " ["; number location; "]";
			print "before returned "; number r; "]\b"
		}
#endif
		return r
	}

	for i in location
	{
		if i ~= player
			r = i.react_before
		if r
		{
#ifset DEBUG
			if debug_flags & D_PARSE
			{
				print "\B["; i.name;
				if debug_flags & D_OBJNUM
					print " ["; number i; "]";
				print ".react_before returned "; number r; "]\b"
			}
#endif
			return r
		}
	}

	! queue is -1 if the object was a number (i.e., a literal digit)
	if queue ~= -1 and xobject > display
	{
		r = xobject.before
		if r
		{
#ifset DEBUG
			if debug_flags & D_PARSE
			{
				print "\B["; xobject.name;
				if debug_flags & D_OBJNUM
					print " ["; number xobject; "]";
				print ".before returned "; number r; "]\b"
			}
#endif
			return r
		}
	}

	if queue ~= -1 and object > display
	{
		r = object.before
		if r
		{
#ifset DEBUG
			if debug_flags & D_PARSE
			{
				print "\B["; object.name;
				if debug_flags & D_OBJNUM
					print " ["; number object; "]";
				print ".before returned "; number r; "]\b"
			}
#endif
			return r
		}
	}
}

! Roody's note: Replaced AnyVerb because the player's PreParse check can
! confuse it. Also added an argument for disallowing SpeakTo since that has
! been coming up a lot for me.
!
! Also made it ignore MovePlayer since AnyVerb exists mainly for the handling
! of commands.

replace AnyVerb(obj,disallow_speakto)
{
#ifclear NO_XVERBS
	if parser_data[VERB_IS_XVERB]
		return false
#endif
	if (verbroutine = &PreParse,&MovePlayer) or
	(verbroutine = &SpeakTo and disallow_speakto)
		return false
	elseif obj
		return obj
	else
		return true
}

#ifclear NO_FUSES
#ifset NEW_FUSE
!\Roody's note: The "NEW_FUSE" system. The new fuse system is some experimental
code that allows fuse code to run after an UNDO or RESTORE, if applicable.
The timer code now works off of the game counter so there isn't a worry that
it'll progress two steps. Still, proper usage probably requires clever coding
in a lot of cases, so this is going to be an unadvertised feature for a while.

Also, early versions of this used up a lot of game memory. Be warned!
\!
property fuse_length alias e_to

replace fuse
{
	type fuse
	size 0
	timer
	{
		if self.fuse_length
			return (self.fuse_length - counter)
		else
			return 0
	}
	fuse_length 0
	in_scope 0
	tick
	{
		local a
		a = self.timer
		if a <= 0
			self.fuse_length = 0
#ifset DEBUG
		if debug_flags & D_FUSES
		{
			print "[Running fuse "; number self; ":  timer = ";
			print number a; "]"
		}
#endif

		if a = 0
			Deactivate(self)
		return a
	}
}

property timer_start alias fuse_length
property fake_event alias s_to

replace daemon
{
	type daemon
	size 0
	in_scope 0
	timer
	{
		if self.timer_start
		{
			return (counter - self.timer_start)
		}
		else
			return 0
	}
}

object fuse_bucket
{}

routine fake_runevents
{
	local i
	for i in fuse_bucket
	{
		run i.fake_event
	}
}
#endif  ! NEW_FUSE

!\
Activate - added a warning for when it is called before the player global has been set. Also added an option to not run daemons/fuses if the Hugofix
fuse/daemon monitor is on.
\!
replace Activate(a, set)                ! <set> is for fuses only
{
	local err, b

	if a.type = fuse
		b = "fuse"
	else
		b = "daemon"
	if not player
	{
		Font(BOLD_ON)
		print "[WARNING:  The player global must be set before
		daemon (object "; number a;") can be activated.]"
		err = true
	}
#ifset DEBUG
	if debug_flags & D_FUSES
	{
		print "[";
		print b; " "; number a;
		if a.type = fuse
			print " (timer = "; number set; ")";
		" called. Allow activation? (Y/N) > ";
		while true
		{
			pause
			if word[0] = 'y','Y'
			{
				print "Y]"
				break
			}
			elseif word[0] = 'n','N'
			{
				print "N]"
				print "["; b; " "; number a; " canceled.]"
				return false
			}
		}
	}
#endif
#ifset NEW_FUSE
	move a to fuse_bucket
#endif
	a.in_scope = player
	a is active
	if a.type = fuse and not err
	{
		if set
#ifclear NEW_FUSE
			a.timer = set
#else
			a.fuse_length = (counter + set)
#endif

		run a.activate_event
	}
	elseif a.type = daemon and not err
	{
#ifclear NEW_FUSE
		if set and not a.#timer
#else
		if set and not a.#timer_start
#endif
		{
			Font(BOLD_ON)
			print "[WARNING:  Attempt to set nonexistent timer
				property on daemon (object "; number a; ")]"
			err = true
		}
		else
#ifclear NEW_FUSE
			a.timer = set
#else
			a.timer_start = (counter - set)
#endif
		run a.activate_event
	}
	elseif not err
	{
		Font(BOLD_ON)
		print "[WARNING:  Attempt to activate non-fuse/\
		daemon (object "; number a; ")]"
		err = true
	}

#ifset DEBUG
	if debug_flags & D_FUSES and not err
	{
		print "[Activating "; b; " "; number a;
		if a.type = fuse
			print " (timer = "; number a.timer; ")";
		print "]"
	}
#endif
	if err
	{
		Font(BOLD_OFF)
		"\npress a key to continue..."
		HiddenPause
	}
	return (not err)
}

replace Deactivate(a)
{
	local err

#ifset HUGOFIX
	move a to fuses_and_daemons
#else
	remove a
#endif
	a.in_scope = 0
	a is not active

	if a.type ~= fuse and a.type ~= daemon
	{
		print "[WARNING:  Attempt to deactivate non-fuse/\
			daemon (object "; number a; ")]"
		err = true
	}
	else
	{
		run a.deactivate_event
	}

#ifset DEBUG
	if debug_flags & D_FUSES and not err
	{
		print "[Deactivating "; a.name; " "; number a; "]"
	}
#endif

	return (not err)
}
#endif ! ifclear NO_FUSES

! Roody's note: Added a new global variable so it's not so important to add
! a holding property to all containers. Character objects still have a holding
! property by default.

global holding_global

replace Acquire(newparent, newchild)
{
	local p,h

	CalculateHolding(newparent)

	if newparent.#holding
		h = newparent.holding
	else
		h = holding_global

	if h + newchild.size > newparent.capacity
		return false
	else
	{
		p = parent(newchild)
		move newchild to newparent
		CalculateHolding(p)
#ifset MULTI_PCS
		MakeMovedVisited(newchild)
#else
		newchild is moved
#endif
		newchild is not hidden
		if newparent.#holding
			newparent.holding = newparent.holding + newchild.size
		return true
	}
}

! AssignPronoun(object)
! sets the appropriate pronoun global to <object>

!\ Roody's note: This version has added support for animals and things being
! called "it" instead of something gendered, written by Mike Snyder.
! Added a "force" argument. Along with different pronoun handling
! in Parse and other places, just adding a true value to "force"
! should guarantee a pronoun change (no longer requiring the author
! to set last_object to -1.
! Also made it so that if there isn't a current value for the applicable
! pronoun global variable, it sets it anyway.
\!

replace AssignPronoun(obj,force)
{
	local a

	if ExcludeFromPronouns(obj)
		return

	! No use if you can't refer to it
	if not obj.#noun and not obj.#adjective
		return
	select true
		case (obj is plural)
		{
			if not them_obj
				a = true
		}
		case (obj is not living or obj.pronoun #2 = "it")
		{
			if not it_obj
				a = true
		}
		case (obj is female)
		{
			if not her_obj
				a = true
		}
		case else
		{
			if not him_obj
				a = true
		}

!	if not a and NEW_PARSE & PRONOUNS_SET and not force:  return
	if NEW_PARSE & PRONOUNS_SET and not force:  return
	if obj is not living
	{
		if obj is not plural
			it_obj = obj
		else
			them_obj = obj
	}
	else
	{
		if obj is plural
			them_obj = obj
		elseif obj is female
			her_obj = obj
		elseif (obj.pronouns #2) = "it"
			it_obj = obj
		else
			him_obj = obj
	}
	if force
		NEW_PARSE |= PRONOUNS_SET
}

!\ Roody's note: I created a routine for establishing rules for objects
that pronouns should never be set to.  Like the original AssignPronoun,
I've included the player object but I also included direction objects.  Replace
this routine if you'd like special rules for your game. \!

routine ExcludeFromPronouns(obj)
{
#ifset NEW_STYLE_PRONOUNS
	if obj >= it_object and obj <= them_object
		return true
#endif
	if obj = player:  return true
#ifclear NO_OBJLIB
	elseif obj.type = direction : return true
#endif
	return false
}

!\ Roody's note: This has some extra code added to avoid mobile object
confusion (written by Mike Snyder). Also added support for global_holding
so not all containers necessarily need a holding property. \!

replace CalculateHolding(obj)
{
	local i

	if obj.#holding
	{
		obj.holding = 0
		for i in obj
		{
			if not (i is worn and i is clothing and obj = player)
				obj.holding = obj.holding + i.size
		}
	}
	else
	{
		holding_global = 0
		for i in obj
		{
			if not (i is worn and i is clothing and obj = player)
				holding_global = holding_global + i.size
		}
	}
}

! Roody's note: I forget why, but I thought CenterTitle should be available
! even when menus aren't being used.
#ifset NO_MENUS
! if CenterTitle doesn't exist, I declare it here just to be replaced
routine CenterTitle(a, lines,force)
{}
#endif

replace CenterTitle(a, lines,force, do_not_use_menu_colors)
{
#ifclear NO_ACCESSIBILITY
	if cheap and not force
		return
#endif !ifclear NO_ACCESSIBILITY
	local l, b, c

	if not lines:  lines = 1

	if not (SL_TEXTCOLOR or SL_BGCOLOR)
	{
		b = DEF_SL_BACKGROUND
		c = DEF_SL_FOREGROUND
	}
#ifclear NO_MENUS
	elseif (MENU_SELECTCOLOR or MENU_SELECTBGCOLOR) and
		not do_not_use_menu_colors
	{
		b = MENU_SELECTBGCOLOR
		c = MENU_SELECTCOLOR
	}
#endif
	else
	{
		b = SL_BGCOLOR
		c = SL_TEXTCOLOR
	}

	Font(BOLD_OFF|ITALIC_OFF|UNDERLINE_OFF|PROP_OFF)
	l = string(_temp_string, a)
	if not system(61)
		window 0                        ! remove previous window

	if (TERP & GLK_TERP)
	{
		window 1  ! draw an empty window so glk terps determine screenwidth properly
		{}
	}

	while (l + 1) > (display.linelength * lines)
	{
		lines++
	}

	window lines
	{
		if not (TERP & SIMPLE_TERP)
		{
			color c,b
			cls
			locate 1,1
		}
		print "\_";
		if (l+1) < display.linelength
			print to (display.linelength/2 - l/2);

		print a;
	}
	color TEXTCOLOR, BGCOLOR, INPUTCOLOR
	FONT(DEFAULT_FONT)
	if not (TERP & SIMPLE_TERP) and not force
		cls
	if not system(61) and not force
		locate 1, LinesFromTop
}

#ifclear NO_SCRIPTS

! american vs british spelling
replace CancelScript(obj)
{
	local o

	o = FindScript(obj)
	if o = MAX_SCRIPTS
		return
	scriptdata[o * 3] = 0
	if o = number_scripts - 1
		number_scripts--

#ifset DEBUG
		if debug_flags & D_SCRIPTS
		{
			print "[Script for obj. ";
			print number obj; " ("; obj.name; ") ";
#ifset AMERICAN_ENGLISH
			print "canceled]"
#else
			print "cancelled]"
#endif
		}
#endif

	return true
}

! Roody's note: This CharMove version tries to work better with doors.
replace CharMove(char, dir)
{
#ifclear NO_OBJLIB

	local newroom, a

	if dir.type ~= direction
		return

	newroom = parent(char).(dir.dir_to)

	if newroom.type = door
	{
		a = newroom
		newroom = a.between #((parent(char) = \
				a.between #1) + 1)
		if a is not open
		{
			if char in location or newroom = location
			{
				self = a
				""
				OMessage(door, 5)
			}
		}
		elseif newroom = location or char in location
			a = 0
	}

	if char in location and not a
	{
		Message(&CharMove, 1, char, dir)
		event_flag = true
	}

	move char to newroom

#ifset DEBUG
	if debug_flags & D_SCRIPTS
	{
		print "["; CThe(char); IsorAre(char, true); " now in:  ";
		print capital parent(char).name; ".]"
	}
#endif

	if char in location and not a
	{
		Message(&CharMove, 2, char, dir)
		event_flag = true
	}
	elseif char in location
		event_flag = true

#endif  ! ifclear NO_OBJLIB

	run parent(char).after

	return true
}

! Roody's note: This allows looping scripts without losing a turn.
! When calling it, set a true value for 'loop'
replace LoopScript(obj,loop)
{
	local o

	while scriptdata[o * 3] ~= obj and o < MAX_SCRIPTS:  o++
	if o = MAX_SCRIPTS
		return
	scriptdata[o * 3 + 1] = 0

#ifset DEBUG
	if debug_flags & D_SCRIPTS
	{
		print "[Looping script for obj. ";
		print number obj; " ("; obj.name; ")]"
	}
#endif

	if loop
		RunScripts
}

!\ Roody's note: I also replaced CharGet and CharDrop just to add a
make-sure-there's-an-object check. It's probably unnecessary, but my own
experimentation with CharMove and my findpath extension made me think it might
be good to play it safe. \!

!----------------------------------------------------------------------------
! CharGet
! Script usage:  &CharGet, <object>

replace CharGet(char, obj)
{
	if not obj
		return
	if FindObject(obj, parent(char)) = 1
	{
		if Acquire(char, obj)
		{
			if char in location
			{
				Message(&CharGet, 1, char, obj)
				event_flag = true
			}
		}
		return true
	}
}

!----------------------------------------------------------------------------
! CharDrop
! Script usage:  &CharDrop, <object>

replace CharDrop(char, obj)
{
	if not obj
		return
	move obj to parent(char)
	char.holding = char.holding - obj.size
	if char in location
	{
		Message(&CharDrop, 1, char, obj)
		event_flag = true
	}
	return true
}

#endif ! ifclear NO_SCRIPTS

! Roody's note : This has extra code for components and such, a problem
! first noticed by Jason McWright.
! Also added support for transparent, non-container objects
replace CheckReach(obj)
{
	local i

	if not obj or obj = parent(player)
		return true

#ifclear NO_VERBS
! added a check to make sure the player doesn't have the same parent as the
! object before doing "x is closed." messages
	local parentcheck
#ifset USE_SUPERCONTAINER
	if parent(object).type = SuperContainer
	{
		if InList(parent(object), contents_in, object) and
			InList(parent(object), contents_in, player)
			parentcheck = true
	}
	else
#endif
	if parent(object) = parent(player)
		parentcheck = true

	if (verbroutine ~= &DoLook, &DoLookIn) and parent(object) and
		parent(object) ~= player and not parentcheck
	{
	! used to check for transparent here, but FindObject should make that not
	! necessary
		if parent(object) is openable, not open
		{
			VMessage(&DoGet, 5)     ! "X is closed."
			return false
		}
		elseif parent(object) is transparent, not living, not container
		{
#ifset _ROODYLIB_H
			RLibMessage(&CheckReach,1,obj) ! "The <obj> is inside the <parent>."
#else
			print capital The(obj); " is inside "; The(parent(obj)); "."
#endif
			return false
		}
	}
#endif

	if not parent(player).reach or
		Contains(parent(player), obj)
		return true

#ifclear NO_OBJLIB   !  since the component class is part of objlib
	local p
	p = obj.part_of
	while p
	{
	if Contains(parent(player), p)
		return true
	p = p.part_of
	}
#endif

#ifclear NO_VERBS
	if parent(obj) is living
	{
		if verbroutine ~= &DoGet, &DoLook

			! "Except that X has it..."
			Message(&CheckReach, 1, obj)

		elseif verbroutine = &DoGet and parent(obj) is unfriendly

			! "Except that X doesn't want to give it to you..."
			Message(&CheckReach, 2, obj)
	}
#endif

	for (i=1; i<=parent(player).#reach; i++)
	{
		if Contains(parent(player).reach #i, obj) or   ! is the object inside a reach object
          obj = parent(player).reach #i ! is the object a reach object
		{
			return true
		}
	}

#ifclear NO_OBJLIB
	p = obj.part_of
	while p
	{
		for (i=1; i<=parent(player).#reach; i++)
		{
			if Contains(parent(player).reach #i, p) or ! is the object part of an object inside the reach object
				p = parent(player).reach #i ! is the object part of a reach object
			{
				return true
			}
		}
		p = p.part_of
	}
#endif

#ifset USE_ATTACHABLES
	if parent(player).type = attachable
	{
		if InList(parent(player), attached_to, obj)
			return true
	}
	if obj.type = attachable
	{
		if InList(obj, attached_to, parent(player))
			return true
	}
#endif

	! "You can't reach it..."
	Message(&CheckReach, 3, obj)
}

! Roody's note: This routine had a mistake where multiple command lines
! were concerned.
!replace CurrentCommandWords
!{
!	if words = 0
!		return 0
!
!	local i
!	for (i=1; i<words; i++)		! not <=, because of the i++
!	{
!		if word[i] = "","then" ! "then" in case this is called during Parse
!		{
!			if word[(i-1)] = "~and"
!				i--
!			i--
!			break
!		}
!	}
!	return i
!}

#ifset USE_DARK_ROOM
! Roody's note: In games with dark rooms, I didn't like how they are treated
! as non-rooms. This code makes dark rooms look more "room-y" when the player
! moves into them by printing the name of the darkness object as if it were a
! room name.


! the darkness object is not a room because the room class will be replaced
! further down in Roodylib and it doesn't really need to be a room anyway
object darkness "Darkness"
{}

replace DarkWarning
{
	RLibMessage(&DescribePlace,1,darkness)
	Indent
	print CThe(player); " stumble"; MatchSubject(player); \
		" around in the dark."
}
#endif

!\ Roody's note : Replaced so a new line is still printed, even if indenting
is turned off. Also changed it so that when FORMAT & LIST_F is on, it indents
by the same method for consistency.  \!

replace Indent(newl)
{
	local i

	if newl or (display.cursor_column > 1 and (FORMAT & LIST_F))
		print ""
	if override_indent or display.cursor_column > 1
		print AFTER_PERIOD;
	elseif not (FORMAT & NOINDENT_F)
	{
		if (FORMAT & LIST_F)
		{
			print to 2;
		}
		elseif INDENT_SIZE
		{
		! See display.cursor_column > 1, above
		!print newline
		print "\_";
		for (i=2; i<=INDENT_SIZE; i++)
			print " ";
		}
	}
	override_indent = false
}

!\ Roody's note: added "conjunction" argument so authors can use object
 hierarchy as methods of listing things. For example: you put several objects
 representing choices in a container, then use ListObjects with an "or"
 conjunction to list them all, using "or" if there is more than one choice.
 If no conjunction is specified, it defaults to the AND_WORD constant ("and").
\!
replace ListObjects(thisobj, conjunction)
{
	local i, obj, count, pluralcount,d
	local templist_count            ! temporary total of unlisted objs.
	local id_count                  ! for identical (or plural) objects
#ifset USE_PLURAL_OBJECTS
	local j, this_class
#endif

	if not conjunction
		conjunction = AND_WORD
	list_nest++
	for obj in thisobj
	{
		if obj is hidden
		{
			obj is already_listed
#ifclear NO_OBJLIB
			if obj.type = scenery
#ifset MULTI_PCS
				MakeKnown(obj)
#else
				obj is known
#endif  ! MULTI_PCS
#endif  ! NO_OBJLIB
		}
		else
#ifset MULTI_PCS
			MakeKnown(obj)
#else
			obj is known
#endif

#ifset USE_PLURAL_OBJECTS

		! Need to count identical (and possibly plural) objects
		! for grouping in listing

		if obj.identical_to and obj is not already_listed
		{
			this_class = obj.identical_to
			if this_class.type = identical_class or
				FORMAT & GROUPPLURALS_F
			{
				id_count = 1
				for (i=1; i<=this_class.#plural_of; i++)
				{
					j = this_class.plural_of #i
					if j in thisobj and j~=obj and j is not hidden
					{
						id_count++
						pluralcount++
						list_count--
						j is already_listed
					}
				}
			}
		}
#endif
		if obj is not already_listed
		{
			! May have a leading "is" or "are" that needs to
			! be printed at the head of the list

			if (FORMAT & ISORARE_F)
			{
				if list_count = 1 and id_count <= 1 and
					obj is not plural
					RLibMessage(&ListObjects,1, IS_WORD) ! "is"
				else
					RLibMessage(&ListObjects,1, ARE_WORD) ! "are"
				if (FORMAT & LIST_F)
					RLibMessage(&ListObjects,2) ! ":"
				FORMAT = FORMAT & ~ISORARE_F    ! clear it
			}

			need_newline = false
			if obj is plural
				pluralcount++

			AssignPronoun(obj)
			if not (FORMAT & LIST_F)
			{
				if list_count > 2 and count
					print ",";
				if list_count > 1 and count = list_count - 1
					print " "; conjunction;
				if not (FORMAT & FIRSTCAPITAL_F)
					print " ";
			}
			else
			{
				d = list_nest
				if (verbroutine ~= &DoInventory, &DoLookIn,&DoLook)  and
				FORMAT & NOINDENT_F
					d--
				print to ((d ) * 2);	! INDENT_SIZE);
			}

#ifset USE_PLURAL_OBJECTS

			! If a number of identical (or possibly plural)
			! objects are grouped together, they are printed
			! as a single entry in the list
			!
			if obj.identical_to and
				(this_class.type = identical_class or
				FORMAT & GROUPPLURALS_F)
			{
				if id_count = 1
				{
					if (FORMAT & FIRSTCAPITAL_F) or (FORMAT & LIST_F)
						CArt(obj)
					else
						Art(obj)
				}
				else
				{
					if (FORMAT & FIRSTCAPITAL_F) or (FORMAT & LIST_F)
						print NumberWord(id_count, true);
					else
						print NumberWord(id_count);
					print " "; this_class.name;

					if this_class.type = plural_class
					{
						local k

						if (FORMAT & LIST_F)
							print ":";
						else
							print " (";

						k = 0
						for (i=1; i<=this_class.#plural_of; i++)
						{
							j = this_class.plural_of #i
							if parent(j) = thisobj
							{
								if not (FORMAT & LIST_F)
								{
									if id_count > 2 and k
										print ",";
									if k = id_count - 1
										print " "; AND_WORD;
									if k
										print " ";
								}
								else
								{
									print "\n";
									d = list_nest
									if (verbroutine ~= &DoInventory, &DoLookIn,&DoLook) and  (FORMAT & NOINDENT_F)
										d--
									print to ((d ) * 2);	! INDENT_SIZE);
								}
								if (FORMAT & LIST_F) and verbroutine ~= &DoInventory
									CArt(j)
								else
									Art(j)
								if not (FORMAT & NOPARENTHESES_F)
									ObjectIs(j)
								k++
							}
						}
						if not (FORMAT & LIST_F):  print ")";
					}
				}
			}
			else
			{
#endif
				! Regular old non-plural, non-identical
				! objects get listed here:

				if ((FORMAT & FIRSTCAPITAL_F) or (FORMAT & LIST_F)) and
					verbroutine ~= &DoInventory
					CArt(obj)
				else:  Art(obj)
				if not (FORMAT & NOPARENTHESES_F)
					ObjectIs(obj)
#ifset USE_PLURAL_OBJECTS
			}
#endif
			FORMAT = FORMAT & ~FIRSTCAPITAL_F       ! clear it

			count++
		}

		! For itemized listings, list the children of
		! each object immediately after that object (unless
		! it is a SpecialDesc-printed description)

		if obj is not hidden and (FORMAT & LIST_F)
		{
			print newline
			if children(obj)
			{
				if not obj.list_contents
				{
					templist_count = list_count
					WhatsIn(obj)
					list_count = templist_count
				}
			}
		}
	}

	! If not an itemized list, it is necessary to finish off the
	! sentence, adding any necessary words at the end.  Then, the
	! children of all previously objects listed at this level are
	! listed.

	if not (FORMAT & LIST_F)
	{
		if count
		{
			if list_nest = 1 and (FORMAT & ISORAREHERE_F)
			{
				if count + pluralcount > 1
					print " "; ARE_WORD;
				else:  print " "; IS_WORD;
#ifset USE_RELATIVE_DESCRIPTIONS
				if not (DESCRIBEPLACEFORMAT & OVERRIDEHERE_F)  or
				((DESCRIBEPLACEFORMAT & OVERRIDEHERE_F) and
				thisobj = parent(player))
#endif
					print " "; HERE_WORD;
#ifset USE_RELATIVE_DESCRIPTIONS
				if DESCRIBEPLACEFORMAT & OVERRIDEHERE_F
				{
					if DESCRIBEPLACEFORMAT & ALSO_F
					{
						print " also";
						DESCRIBEPLACEFORMAT = DESCRIBEPLACEFORMAT & ~ALSO_F
					}
					print " ";
					RelativeText(thisobj)
					if DESCRIBEPLACEFORMAT & OVERRIDEHERE_F
						DESCRIBEPLACEFORMAT = DESCRIBEPLACEFORMAT & ~OVERRIDEHERE_F
						! clear it
				}
#endif
				FORMAT = FORMAT & ~ISORAREHERE_F  ! clear it
				if not ((FORMAT & LIST_F) or (FORMAT & TEMPLIST_F))
					override_indent = true
			}

			if not (FORMAT & NORECURSE_F)
				print ".";
		}

		i = 0
		local char_count
		for obj in thisobj
		{
			if children(obj) and obj is not hidden and
				(obj is not already_listed or
					thisobj ~= location) and not ClothingCheck(obj)
			char_count++
			if char_count = 2
				break
		}
		for obj in thisobj
		{
			if children(obj) and obj is not hidden and
				(obj is not already_listed or
					thisobj ~= location) and not ClothingCheck(obj)
			{
				if (FORMAT & TEMPLIST_F)
				{
					FORMAT = FORMAT | LIST_F & ~TEMPLIST_F
					i = true
					print newline
				}

#ifclear NO_OBJLIB
				if count > 1 and obj.type = character
				{
					FORMAT = FORMAT | USECHARNAMES_F
					if char_count = 2
					{
						print newline
						override_indent = false
					}
				}
#endif
				templist_count = list_count
				WhatsIn(obj)
				list_count = templist_count
			}
		}
	}

	if --list_nest = 0
	{
		if not (FORMAT & LIST_F) and not (FORMAT & NORECURSE_F)
		{
			print newline
			override_indent = false
			need_newline = false
		}
	}
}

! Roody's note : added conjunction argument like in ListObjects above.
replace PropertyList(obj, prop, artic, conjunction)
{
	local a, b, n, total

	if not conjunction
		conjunction = AND_WORD

	for (a=1; a<=obj.#prop; a++)
	{
		if obj.prop #a:  total++
	}

	for (a=1; a<=obj.#prop; a++)
	{
		b = obj.prop #a
		if b
		{
			if artic
				The(b)
			else
				Art(b)

			if a < total and total > 2
				print ", ";
			elseif a = total - 1:  print " ";

			if a = total - 1
				print conjunction; " ";
			n++
		}
	}
	return n
}

!----------------------------------------------------------------------------
! ObjectIs(object)
! prints a list of parenthetical attributes

replace ObjectIs(obj)
{
	local n

	if not &obj.desc_detail
	{
#ifclear LIST_CLOTHES_FIRST
		if obj is clothing:  n = TestObjectIs(obj, worn, "being worn", n)
#endif
		n = TestObjectIs(obj, light, "providing light", n)

		if n:  print ")";
	}
	else
		run obj.desc_detail
}

! Roody's note: was missing a space
replace TestObjectIs(obj, attr, str, n)
{
	if obj is attr
	{
		if n = 0
			print " (";
		else
			print " "; AND_WORD; " ";
		print str;
		n++
	}
	return n
}

#ifset USE_ELEVATED_PLATFORMS
attribute elevated
#endif

!\ Roody's note: I replaced WhatsIn so printed things like the colon-printing
(":") are sent to RLibMessages for easier message configuring.
  Changed a carriage return to 'print newline' \!

replace WhatsIn(obj,dont_flush)
{
	local i, totallisted
	local initial_list_nest

	if (FORMAT & NORECURSE_F)
		return

	for i in obj
	{
		if not dont_flush
			i is not already_listed
		if i is not hidden and i is not already_listed
			totallisted++
	}

	if totallisted = 0
	{
		print newline
		return
	}
	list_count = totallisted

	if obj is not container or obj is platform or
		(obj is container and (obj is not openable or
			(obj is openable and
				(obj is open or obj is transparent)))) and
		(obj ~= player or (FORMAT & INVENTORY_F)) and obj is not quiet
	{
		SpecialDesc(obj)

		! If list_count is 0 now, but totallisted was not, something must have been
		! printed by SpecialDesc

		if dont_flush and list_count
		{
			list_count = 0
			for i in obj
			{
				if i is not hidden and i is not already_listed
					list_count++
			}
		}

		if list_count = 0
		{
			if (FORMAT & INVENTORY_F) and not (FORMAT & LIST_F) and
				list_nest = 0
			{
				print newline    ! was ' print "" '
			}
			return totallisted
		}

#ifset NEW_DESCRIBEPLACE
		if need_newline and (FORMAT & DESCFORM_D) and
			not (FORMAT & LIST_F)
			""
#endif
		if obj.list_contents
			return totallisted

		initial_list_nest = list_nest

		if (FORMAT & LIST_F)
		{
			if list_nest
			{
				! Indent the first time so that it lines up with
				! paragraph indentation:
				i = list_nest
				if (FORMAT & NOINDENT_F) and
				verbroutine ~= &DoInventory, &DoLook, &DoLookIn
				{
					i--
				}
				print to (i * 2);	! INDENT_SIZE);
			}
		}
		else
			Indent

		if obj.contains_desc    ! custom heading
		{
			if (FORMAT & LIST_F)
				RLibMessage(&WhatsIn, 1 )  !  ":"
#ifset USE_RELATIVE_DESCRIPTIONS
			FORMAT = FORMAT & ~FIRSTCAPITAL_F
			FORMAT = FORMAT & ~ISORAREHERE_F
			DESCRIBEPLACEFORMAT = DESCRIBEPLACEFORMAT & ~ OVERRIDEHERE_F
#endif
		}
		else
		{
			if obj = player
			{
				if (FORMAT & LIST_F) and list_count < totallisted
					print "\n";

				! "You are carrying..."
				Message(&WhatsIn, 1, totallisted)

				if (FORMAT & LIST_F)
					RLibMessage(&WhatsIn, 1 )  !  ":"
			}
			elseif obj is living and not obj.prep
			{
				! "X has..."
				Message(&WhatsIn, 2, obj, totallisted)
				if (FORMAT & LIST_F)
					RLibMessage(&WhatsIn, 1 )  !  ":"
			}
#ifset USE_RELATIVE_DESCRIPTIONS
			elseif not (DESCRIBEPLACEFORMAT & OVERRIDEHERE_F)
#else
			else
#endif
			{
				if list_count < totallisted
					! "Also sitting on/in..."
					RLibMessage(&WhatsIn, 2, obj )
				else
					! "Sitting on/in..."
					RLibMessage(&WhatsIn, 3, obj )
				FORMAT = FORMAT | ISORARE_F
			}
		}

		ListObjects(obj)
		for i in obj
		{
			i is already_listed
		}
		list_nest = initial_list_nest
	}
	return totallisted
}


!\ Roody's note: Added some code to SpecialDesc so that if the obj is the only
child of a parent, it still checks (and lists) any children of that obj. \!
replace SpecialDesc(obj)
{
	local a, c, flag, printed_blankline, d

	if (FORMAT & LIST_F)
		return

	list_count = 0
	for a in obj
	{
		if a is not hidden
		{
			flag = true
			c++
		}
		else
			flag = false

		if (FORMAT & INVENTORY_F) and obj = player and flag
		{
			if &a.inv_desc
			{
				print newline
				Indent
			}
			if a.inv_desc
			{
				if (FORMAT & LIST_F):  print newline
				AddSpecialDesc(a)
			}
		}
#ifset MULTI_PCS
		elseif not ObjectIsMovedVisited(a) and flag
#else
		elseif a is not moved and flag
#endif
		{
			if &a.initial_desc
			{
				d++
#ifset NEW_DESCRIBEPLACE
				if (FORMAT & DESCFORM_D) and d > 1
					""
				elseif not (FORMAT & DESCFORM_D)
#endif
					print newline
				override_indent = false
				if (FORMAT & INVENTORY_F) and (FORMAT & NOINDENT_F) and not printed_blankline
					print ""
				printed_blankline = true
				Indent
				need_newline = true
			}
			if a.initial_desc
				AddSpecialDesc(a)
		}
	}
	list_count = c - list_count
	if not list_count and c
	{
		for a in obj
		{
			if children(a) and a is not quiet and
			(a is platform or a is transparent or
			(a is container and
			(a is not openable or (a is openable and a is open)))) and
			not a.list_contents and not ClothingCheck(a)
			{
#ifset NEW_DESCRIBEPLACE
				if (FORMAT & DESCFORM_D)
					""
#endif
				WhatsIn(a)
				list_count = 0
			}
		}
	}
}

! Roody's note: The following is a routine mainly used by the
! LIST_CLOTHES_FIRST switch, so that containers and platforms that are
! clothing don't have their contents listed twice. For that reason, it's
! called "ClothingCheck" but this routine might be useful to replace
! anytime you want to split parts of the inventory up different ways.

routine ClothingCheck(a)
{
#ifset LIST_CLOTHES_FIRST
	if (a is worn and a is clothing and verbroutine ~= &ListClothesFirst) or
		(a is not worn and verbroutine = &ListClothesFirst)
		return true
	else
#endif
		return false
}

! Roody's note: Changed it so word[1] is cleared when leaving the routine
! so chained YesorNo's don't get confused by the unchanging word array
replace YesorNo
{
	local w, count, val, ret
	w = word[1]
	while true
	{
		select w
			case "yes", "y":ret = ++val
			case "no", "n": ret = true
			case else
			{
!				if (FORMAT & DESCFORM_I) and not count
!					""
				if ++count < 10
					RlibMessage(&YesorNo, 1)	! ask "yes" or "no"
				else
					RlibMessage(&YesorNo, 2)
				GetInput
				w = word[1]
			}
		if ret
		{
			word[1] = "" ! clearing word[1], duh
			return val
		}
	}
}

!----------------------------------------------------------------------------
!* REPLACED HUGOFIX.H ROUTINES
!----------------------------------------------------------------------------

! Roody's note: Changed the parse$-printing code.
#ifset HUGOFIX
enumerate start = 64, step * 2
{
	D_PLAYBACK_HELPER, D_OPCODE_MONITOR
}

replace DoHugoFix
{
	local i, n

	Font(PROP_OFF) ! proportional printing off

	if ((VerbWord ~= "$wo", "$wn") and object > objects and object > 0) or
		((VerbWord ~= "$ac", "$at") and xobject > objects)
	{
		"[Object number(s) out of range]"
		Font(DEFAULT_FONT)
		return
	}

	select VerbWord
	case "$", "$?"
	{
		"HUGOFIX DEBUGGING COMMANDS:\n\
		\_    $?  - Display this help menu\n"

		"Monitoring:\n\
		\_    $on - Toggle object numbers"
		if hugor is switchedon
			"\_    $om - Hugor opcode monitor on/off"
#ifclear NO_SCRIPTS
	       "\_    $sc - Script monitor on/off"
#endif

#ifclear NO_FUSES
	       "\_    $fd - Fuse/daemon monitor on/off"
#endif

               "\_    $fi - FindObject monitoring on/off"
               "\_    $pm - Parser monitoring on/off\n"


	       "Object manipulation:\n\
		\_    $at <obj.> is [not] <attr. #>\n\
		\_                         - set/clear object attribute\n\
		\_    $mo <obj.> to <obj.> - Move object to new parent\n
		\_    $mp <obj.>           - Move player object to new
			parent\n"

		"Object testing:\n\
		\_    $fo [obj.]   - Find object (or player, if no obj.)\n\
		\_    $na <obj. #> - Print name of object number <obj.>\n\
		\_    $nu <obj.>   - Print number of object <obj.>\n"

		"Parser testing:\n\
		\_    $ca                - Check articles for all objects\n\
		\_    $pc [$all] [obj.]  - Check parser conflicts (for <obj.>)\n\
		\_                         (parse_ranked objects only or all)\n\
		\_    $pn                - List current pronouns\n\
		\_    $pr                - parse_rank monitoring\n"

		"Other utilities:"

#ifclear NO_FUSES
	       "\_    $ac <obj.> [timer] - Activate fuse (with timer) or
			daemon\n\
		\_    $de <obj.>         - Deactivate fuse or daemon"
#endif
#ifclear NO_OBJLIB
	       "\_    $di [obj.]         - Audit directions (for room <obj.>)"
#endif
	       "\_    $kn                - Set all objects to 'known'\n\
		\_    $kn <obj. #>       - Make <obj.> 'known'\n\
		\_    $nr                - Normalize random numbers\n\
		\_    $ot [obj. | $loc]  - Print object tree (beginning with
			<obj.>)\n\
		\_    $rp                - Skip in-game pauses in recording playback\n\
		\_    $rr                - Restore \"random\" random numbers\n\
		\_    $uk <obj.>         - Make object unknown\n\
		\_    $wo <val.>         - Print dictionary word entry <val.>\n\
		\_    $wn <word>         - Value/addr. of (lowercase) <word>\n\
		\n\_    $au                - Run HugoFixAudit"
	}


#ifclear NO_FUSES
	case "$ac"
	{
		if object.type ~= fuse and object.type ~= daemon
			"Not a fuse or daemon."
		else
		{
			Activate(object)
			object.timer = xobject
			"Activated."
		}
	}
	case "$de"
	{
		if object.type ~= fuse and object.type ~= daemon
			"Not a fuse or daemon."
		else
		{
			Deactivate(object)
			"Deactivated."
		}
	}
#endif

	case "$at"
	{
		local isnot

		for (i=1; i<=words; i++)
		{
			if word[i] = "": break
			if word[i] = "not": isnot = true
		}

		print "[Setting "; object.name; " is ";

		n = xobject

		if isnot
		{
			object is not n
			"not ";
		}
		else
			object is n

		print "attribute "; number n; "]"
	}
	case "$au"
	{
		HugoFixAudit
	}
	case "$ca"
	{
		"[Checking articles]"
		for (i=last_library_object+1; i<objects; i++)
		{
			if (i.noun or i.adjective) and not i.article
			{
				print "Object \""; i.name; "\" ("; number i; ") has no article"
			}
		}
		"[...Done]"
	}

#ifclear NO_OBJLIB
	case "$di"
	{
		local place

		if object:  place = object
		else:  place = location

		print "DIRECTIONS FROM:  "; capital place.name; " "; "("; \
			number place; ")"
		local printed
		for i in direction
		{
			if &place.(i.dir_to)
			{
				print "\_ "; i.name; ":  (Property routine)"
				printed++
			}
			else
			{
				n = place.(i.dir_to)
				if n > 1
				{
					print "\_ "; i.name; ":  "; \
						capital n.name
					printed++
				}
				elseif n = 0
				{
					if &place.cant_go
					{
						print "\_ "; i.name; ":  ";
						if not place.cant_go
							print newline
						printed++
					}
				}
			}
		}
		if not printed:  "\_ (None)"
	}
#endif

#ifclear NO_FUSES
	case "$fd"
	{
		"[Fuse/daemon monitor";
		OnorOff(D_FUSES)
	}
#endif

	case "$fi"
	{
		"[FindObject monitoring";
		OnorOff(D_FINDOBJECT)
	}
	case "$fo"
	{
		if not object:  object = player
		"[Parent of obj. "; : print number object; " ("; \
		object.name; "):  Obj. ";
		print number (parent(object)); " ("; parent(object).name; ")]"
	}
	case "$kn"
	{
		if object
		{
#ifset MULTI_PCS
			MakeKnown(object)
#else
			object is known
#endif
			print "["; object.name; " now 'known']"
		}
		else
		{
#ifset NO_OBJLIB
			i = 0
#else
			i = out_obj + 1
#endif
			for (; i<objects; i++)
#ifset MULTI_PCS
				MakeKnown(i)
#else
				i is known
#endif
			"[All objects now 'known']"
		}
	}
	case "$mo"
	{
		if word[words] = "$loc":  xobject = location
		print "[Obj. "; number object; " ("; object.name; ") ";
		print "moved to obj. "; number xobject; " (";
		print xobject.name; ")]"
		move object to xobject
		object is not hidden
#ifset MULTI_PCS
		MakeKnown(object)
#else
		object is known
#endif
	}
	case "$mp"
	{
		if (parent(object) and object is not enterable)
		{
			print "[Obj. "; number object; " (";
			print object.name; ") is not a room or enterable object]"
		}
#ifclear NO_OBJLIB
		elseif (not parent(object) and object.type ~= room)
		{
			print "[Obj. "; number object; " (";
			print object.name; ") is not a room or enterable object]"
		}
#endif
		else
		{
			Font(DEFAULT_FONT)
			MovePlayer(object, false, true)
			Font(PROP_OFF)
			"[Player moved to obj. ";
			print number object; " ("; object.name; ")]"
		}
	}
	case "$na", "$nu"
	{
		print "[Obj. "; number object; ": \""; object.name; "\"]"
	}
	case "$nr"
	{
		system(21)
		"[Random numbers now normalized]"
	}
	case "$om"
	{
		if hugor is not switchedon
			"[Hugor not detected. Opcode monitor not turned on.]"
		else
		{
			print "[Hugor opcode monitor";
			OnorOff(D_OPCODE_MONITOR)
		}
	}
	case "$on"
	{
		"[Object numbers";
		OnorOff(D_OBJNUM)
	}
	case "$ot"
	{
		if object = 0 and words = 1
		{
			"[Specify a parent object to draw the tree from, or
			'$ot 0' to draw the entire object tree, including the children
			of the \"object classes\" and \"settings\" objects.]\n"
	!		Font(DEFAULT_FONT)
	!		return
		}
		elseif word[2] = "0"
			"[Skipped object numbers are replaced objects.]\n"

		if word[2] = "$loc":  object = location
		print "["; number object; "] ";
		if object = 1
			print "(display)"
		elseif object.name
			print object.name
#ifset _WINDOW_H
		elseif object.type = window_class
		{
			if object = window_class
				print "(window_class)"
			else
				print "(window object)"
		}
#endif
		else
			print "<replaced object>"
		list_nest = (object ~= 0)
		for (i=0; i<objects; i=i+1)
		{
			local a,b,c
#ifset _WINDOW_H
			if i.type = window_class
				a++
#endif
#ifclear NO_FUSES
			if i.type = fuse and i ~= fuse
				b++
			elseif i.type = daemon and i ~= daemon
				c++
#endif
			if i ~= object and parent(i) = object
				DrawBranch(i,a,b,c)
		}
	}
	case "$pc"
	{
		local parse_rank_only = true

		if word[2] = "$all"
			parse_rank_only = false

		if object
		{
			if not ListParserConflicts(object, parse_rank_only)
			{
				"No parser conflicts for:  ";
				PrintParserInfoForObject(object)
			}
		}
		else
		{
			"NOTE:  You have chosen to check parser conflicts for the
			entire tree";
			if not parse_rank_only
				" for ALL objects";
			".  This may take a long time depending on the number of
			objects/adjectives/nouns.  Do you wish to continue (YES OR NO)? ";
			input
			if not YesOrNo
			{
				Font(DEFAULT_FONT)
				return
			}
			"[Checking parser conflicts...]"

			local count
			for (i=0; i<objects; i++)
			{
				if i.noun and (not parse_rank_only or i.parse_rank)
				{
					if ListParserConflicts(i, parse_rank_only)
						count++
				}
			}
			if not count
				"No parser conflicts found."
		}
	}
	case "$pn"
	{
		n = 1
		while n < 5
		{
			select n
				case 1
				{
					print "it object: ";
					i = it_obj
				}
				case 2
				{
					print "him object: ";
					i = him_obj
				}
				case 3
				{
					print "her object: ";
					i = her_obj
				}
				case 4
				{
					print "them object: ";
					i = them_obj
				}
			if not i
				print "unassigned"
			else
			{
				print i.name; " ("; number i; ")"
			}
			n++
		}
	}
	case "$rp"
	{
		"[In-game pause-skipping in recording playback";
		OnorOff(D_PLAYBACK_HELPER)
	}
	case "$pm"
	{
		"[Parser monitoring";
		OnorOff(D_PARSE)
	}
	case "$pr"
	{
		"[parse_rank monitoring";
		OnorOff(D_PARSE_RANK)
	}
	case "$rr"
	{
		system(22)
		"[\"Random\" random numbers restored]"
	}

#ifclear NO_SCRIPTS
	case "$sc"
	{
		"[Script monitor";
		OnorOff(D_SCRIPTS)
	}
#endif

	case "$uk"
	{
#ifset MULTI_PCS
		MakeUnknown(object)
#else
		object is not known
#endif
		print "["; object.name; " now not 'known']"
	}
	case "$wn"
	{
		print "[\""; word[2]; "\": "; number word[2]; "]"
	}
	case "$wo"
	{
		print "[\""; object; "\": "; number object; "]"
	}

	Font(DEFAULT_FONT)	! restore font (i.e., proportional printing
				! if it was turned off)
}

#ifset MULTI_PCS
routine MakeUnknown(obj)
{
	obj is not known
}
#endif

! Roody's note - added some code so windows and replaced objects
! get listed when printing the object tree
replace DrawBranch(obj,win_count,fuse_count,daemon_count)
{
	local i, nextobj

	if (not word[2] or word[2] = "0") and not obj.name and obj ~= 1
	{
#ifset _WINDOW_H
		if obj.type ~= window_class
#endif
			return  ! skip replaced objects
	}
	for (i=1; i<=list_nest; i++)
		print ". . ";
	print "["; number obj; "] ";
		if obj = 1
			print "(display)"
#ifclear NO_FUSES
		elseif obj.name = "(fuse)" and obj ~= fuse and not list_nest
			print "(fuse #"; number fuse_count;")"
		elseif obj.name = "(daemon)" and obj ~= daemon and not list_nest
			print "(daemon #"; number daemon_count;")"
#endif
		elseif obj.name
			print obj.name
#ifset _WINDOW_H
		elseif obj.type =  window_class and obj ~= window_class
			print "(untitled window #"; number win_count ; ")"
#endif
		else
			print "<replaced object>"

	if (not word[2] and (obj = settings, object_classes))
		return

	for nextobj in obj
	{
		list_nest++
		DrawBranch(nextobj)
		list_nest--
	}
}

replace CheckParserConflicts(obj1, obj2, what)
{
	local n

	for (n=1; n<obj1.#what; n++)
	{
		if obj1.what #n and InList(obj2, what, obj1.what #n)
		{
			if obj1 is not workflag
			{
				"\n\B*** Conflicts with: ";
				PrintParserInfoForObject(obj1)
				Font(BOLD_OFF)
			}
			"\_ ";
			PrintParserInfoForObject(obj2, obj1)
			obj1 is workflag
			return true
		}
	}
}

replace ListParserConflicts(obj, parse_rank_only)
{
	local i, matched, check

	obj is not workflag

	for (i=0; i<objects; i++)
	{

		if parse_rank_only
		{
			if obj.parse_rank or i.parse_rank
				check = true
		}
		else
			check = true

		if i ~= obj and check and i.noun
		{
			if i.adjective
			{
				if CheckParserConflicts(obj, i, adjectives)
					matched = true
			}

			if not matched
			{
				if CheckParserConflicts(obj, i, nouns)
					matched = true
			}
		}
		check = false
	}

	return matched
}

! Roody's note: Added an option to call it silently
replace OnorOff(a,silent)
{
	if not (debug_flags & a)
	{
		if not silent
			print " on.]"
		debug_flags = debug_flags | a
	}
	else
	{
		if not silent
			print " off.]"
		debug_flags = debug_flags & ~a
	}
}

! Roody's note - just got rid of a jump
replace The(obj, a)
{
	AssignPronoun(obj)

	if obj = player and player_person = 1 and a
		print player.pronoun #2;
	else
	{
		if obj.article
			print "the ";
		print obj.name;
	}

	if (debug_flags & D_OBJNUM)
		print " ["; number obj; "]";
}

! Roody's note - just got rid of a jump
replace Art(obj, a)
{
	AssignPronoun(obj)

	if obj = player and player_person = 1 and a
		print player.pronoun #2;
	else
	{
		if obj.article
			print obj.article; " ";
		print obj.name;
	}

	if (debug_flags & D_OBJNUM)
		print " ["; number obj; "]";
}

! Roody's note: just replaced to add a note that a holding property isn't so
! important with Roodylib
replace HugoFixAudit
{
	local obj, err, count

	Font(PROP_OFF)

	"Validating library object setup..."

	! Make sure "nothing" is object 0
	if nothing ~= 0
	{
		"\"nothing\" object is not object 0"
		err++
	}

#ifset USE_PLURAL_OBJECTS
	! Check for proper plural/identical objects initialization
	local i

	"Validating plural/identical objects..."
	count = 0

	for (obj=1; obj<objects; obj++)
	{
		if (obj.type = plural_class, identical_class) and
			(obj ~= plural_class, identical_class)
		{
			count++

			if not obj.plural_of
			{
				err++
				"Plural/identical class ";
				print "\""; obj.name; "\" ("; number obj; ")";
				" has no 'plural_of' property"
			}
			elseif obj.type = identical_class
			{
				for (i=1; i<=obj.#plural_of; i++)
				{
					if obj.plural_of #i.identical_to ~= obj
					{
						err++
						"Identical object ";
						print "\""; obj.plural_of #i.name; "\" ";
						print "("; number obj.plural_of #i; ") ";
						"should have 'identical_class' ";
						print "\""; obj.name; "\" ";
						print "("; number obj; ") ";
						"in 'identical_to' property"
					}
				}
			}
		}
	}

	print number count;
	" plural/identical object(s) checked"

#endif	! ifset USE_PLURAL_OBJECTS

	"Validating containers and platforms..."
	count = 0

	for (obj=1; obj<objects; obj++)
	{
		if obj is container or obj is platform
		{
			count++

			local a
			if obj.capacity and not obj.#holding and not &obj.holding
			{
				if obj is container
					"Container ";
				else
					"Platform ";
				print "\""; obj.name; "\" ("; number obj; ")";
				" has 'capacity' but no 'holding' property"
				err++
				a++
			}
			if a
			{
				"\B(Roodylib containers and platforms should still work even
				without the holding property.)\b"
			}
			if obj.holding and not obj.#capacity and not &obj.capacity
			{
				if obj is container
					"Container ";
				else
					"Platform ";
				print "\""; obj.name; "\" ("; number obj; ")";
				" has 'holding' but no 'capacity' property"
				err++
			}
		}
	}

	print number count;
	" container/platform object(s) checked"

	print "[";
	if err
		print number err;
	else
		print "No";
	" error(s)/warning(s) found by HugoFixAudit]"
	Font(DEFAULT_FONT)
}

!\ DoScope and DoScopeRooms - Here are two debugging routines for reminding you
what objects are in scope of various rooms. "SCOPE" will just list the items
in scope of the current location, while "SCOPE ROOMS" will list all objects
in scope in every room.

IMPORTANT NOTE: All rooms and objects must be defined *after* the
player character object.    \!

routine DoScope
{
	print "Total objects: "; number objects
	print "Highest object number: "; number (objects - 1)
	"Objects in scope:"
	local a
	for (a=player ; a< objects ; a=a+1)
	{
		if a ~= location
		{
			if FindObject(a,location)
			{
				print "\_  ";
				print a.name
			}
		}
	}
}

#ifclear NO_OBJLIB
routine DoScopeRooms
{
	local a, b, c
        print "Total objects: "; number objects
	print "Highest object number: "; number (objects - 1)
	a = player
	do
	{
		if a.type = room
		{
			c = 0
			print ""
			print "Room: "; a.name
			print "In scope:"
			for (b=player ; b< objects ; b=b+1)
			{
				if b.type ~= room and b~= player
				{
					if FindObject(b,a)
					{
						print "\_  "; b.name
						c++
					}
				}
				elseif b = player and a = location
				{
					print "\_  "; b.name
					c++
				}
			}
			if not c
				print "\_  "; "nothing"
		}
		a++
	}
	while a< objects
}
#endif

!\  DoVerbTest is another debugging utility. Type >VERBTEST <OBJECT>
    to see what the responses are to a host of verbs. Hopefully, it'll remind
	 you of important responses to be yet written, like if you have a food
	 object but forgot to code a fitting DoEat response.

	 NOTE: It might act a little wonky with doors and such, as it tests
	 DoGo and DoEnter, among other things.
\!
routine DoVerbTest
{
	local a, b, c, d, t
	a = parent(object)

	while b < 26
	{
		b++
		t = 0
		Font(ITALIC_ON)
		select b
			case 1
			{
				if player ~= parent(object)
				{
					t = "Getting object"
					c = &DoGet
				}
			}
			case 2
			{
				t = "Wearing object"
				c = &DoWear
			}
			case 3
			{
				if parent(object) = player
				{
					t = "Removing object"
					c = &DoTakeOff
				}
			}
			case 4
			{
				if parent(object) = player
				{
					t = "Dropping object"
					c = &DoDrop
				}
			}
			case 5
			{
				if object is openable
				{
					t = "Opening object"
					c = &DoOpen
				}
			}
			case 6
			{
				if object is openable
				{
					t = "Closing object"
					c = &DoClose
				}
			}
			case 7
			{
				if object is lockable
				{
					t = "Locking object"
					c = &DoLock
				}
			}
			case 8
			{
				if object is lockable
				{
					t = "Unlocking object"
					c = &DoUnlock
				}
			}
			case 9
			{
				if object is switchable
				{
					t = "Switching on object"
					c = &DoSwitchOn
				}
			}
			case 10
			{
				if object is switchable
				{
					t = "Switching off object"
					c = &DoSwitchOff
				}
			}
			case 11
			{
				t = "Listening to object"
				c = &DoListen
			}
			case 12
			{
				t = "Eating object"
				c = &DoEat
			}
			case 13
			{
				t = "Drinking object"
				c = &DoDrink
			}
			case 14
			{
				t = "Hitting object"
				c = &DoHit
			}
			case 15
			{
				t = "Hello-ing object"
				c = &DoHello
			}
			case 16
			{
				t = "Examining object"
				c = &DoLook
			}
			case 17
			{
				if object is platform or object is container
				{
					t = "Looking in/on object"
					c = &DoLookIn
				}
			}
			case 18
			{
				t = "Looking through object"
				c = &DoLookThrough
			}
			case 19
			{
				t = "Looking under object"
				c = &DoLookUnder
				word[2] = "under"
			}
			case 20
			{
				t = "Go-ing object"
				c = &DoGo
			}
			case 21
			{
				t = "Entering object"
				c = &DoEnter
			}
			case 22
			{
				t = "Sitting on object"
				c = &DoSit
			}
			case 23
			{
				t = "Exiting object"
				c = &DoExit
			}
			case 24
			{
				t = "Moving object"
				c = &DoMove
			}
			case 25
			{
				t = "Searching object"
				c = &DoSearch
			}
			case 26
			{
				t = "Smelling object"
				c = &DoSmell
			}
		Font(ITALIC_OFF)
		if c
		{
			if d
			{
				""
			}
			print t
			Perform(c, object)
			d = c
		}
		c = 0
		if parent(object) ~= a, player
			move object to location
	}
!	c = 0
#ifset _VERBSTUB_H
	""
	b = 0
	d = 0
	Font(BOLD_ON)
	"VERBSTUB verbs"
	Font(BOLD_OFF)
	""
	while b < 14
	{
		b++
		t = 0
		Font(ITALIC_ON)
		select b
			case 1
			{
				t = "Getting object again:"
				c = &DoGet
			}
			case 2
			{
				t = "Waving to object"
				c = &DoWaveHands
			}
			case 3
			{
				if parent(object) = player
				{
					t = "Waving object"
					c = &DoWave
				}
			}
			case 4
			{
				if parent(object) = player
				{
					t = "Throwing object"
					c = &DoThrowAt
				}
			}
			case 5
			{
				t = "Climbing object"
				c = &DoClimb
			}
			case 6
			{
				t = "Pushing object"
				c = &DoPush
			}
			case 7
			{
				t = "Pulling object"
				c = &DoPull
			}
			case 8
			{
				t = "Kissing object"
				c = &DoKiss
			}
			case 9
			{
				t = "Touching object"
				c = &DoTouch
			}
			case 10
			{
				t = "Tying object"
				c = &DoTie
			}
			case 11
			{
				t = "Untying object"
				c = &DoUntie
			}
			case 12
			{
				t = "Burning object"
				c = &DoBurn
			}
			case 13
			{
				t = "Cutting object"
				c = &DoCut
			}
			case 14
			{
				t = "Digging object"
				c = &DoDig
			}
		Font(ITALIC_OFF)
		if c
		{
			if d
				""
			print t
			Perform(c, object)
			d = c
		}
		c = 0
		if parent(object) ~= a, player
			move object to location
	}
#endif  !ifset USE_VERBSTUB
}

! Roody's note:  The following routines are called at the beginning of the game,
! allowing to start monitoring various things before the game has actually
! started.

routine HugoFixInit
{
	local key

!	Font(BOLD_OFF | ITALIC_OFF | UNDERLINE_OFF | PROP_OFF)
!	color TEXTCOLOR, BGCOLOR, INPUTCOLOR
   while true
	{
		local ret
		while true
		{
			if not (TERP & SIMPLE_TERP)
				cls
			else
				""
			if not (TERP & SIMPLE_TERP)
				CenterTitle("HugoFix Debugging Options")
			if display.needs_repaint
				display.needs_repaint = false
			print newline
			Indent
			print "\_  ";
			Font(BOLD_ON)
			print "HugoFix Debugging Options"
			Font(BOLD_OFF)
			""
			PrintHugoFixOptions
			print ""
			"Select the number of your choice (or ESCAPE to begin the game)"
			key = HiddenPause
			local numb
			if key = 'q','Q', '0', ESCAPE_KEY, ' ', ENTER_KEY
			{
				ret = 0
				break
			}
			else
				numb = key - 48
			if numb and (numb > 0) and (numb < 9)
			{
				if not (numb = 8 and hugor is not switchedon)
				{
					ret = numb
					break
				}
			}
			""
		}
		if not (TERP & SIMPLE_TERP)
			cls
		if ret
		{
			local cheap_mode
			select ret
				case 1: OnOrOff(D_OBJNUM,1)
				case 2: OnOrOff(D_SCRIPTS,1)
				case 3: OnOrOff(D_FUSES,1)
				case 4: OnOrOff(D_FINDOBJECT,1)
				case 5: OnOrOff(D_PARSE,1)
				case 6: OnOrOff(D_PARSE_RANK,1)
				case 7: OnOrOff(D_PLAYBACK_HELPER,1)
				case 8: OnOrOff(D_OPCODE_MONITOR,1)
			window 0 ! only to draw a line in simple interpreters
#ifclear NO_ACCESSIBILITY
			cheap_mode = cheap
#endif
			if not (cheap_mode or (TERP & SIMPLE_TERP))
				cls
		}
		else
			break
	}
}

routine PrintHugoFixOptions
{
	local total, sel = 1
	if hugor is switchedon
		total = 9
	else
		total = 8
	while sel < total
	{
		Indent
		print number sel; ". ";
		select sel
			case 1
			{
				print "Toggle object numbers (";
				if (debug_flags & D_OBJNUM)
					print "ON";
				else
					print "OFF";
				")"
			}
			case 2
			{
				print "Script Monitor (";
				if (debug_flags & D_SCRIPTS)
					print "ON";
				else
					print "OFF";
				")"
			}
			case 3
			{
				print "Fuse/Daemon Monitor (";
				if (debug_flags & D_FUSES)
					print "ON";
				else
					print "OFF";
				")"
			}
			case 4
			{
				print "FindObject Monitoring (";
				if (debug_flags & D_FINDOBJECT)
					print "ON";
				else
					print "OFF";
				")"
			}
			case 5
			{
				print "Parser Monitoring (";
				if (debug_flags & D_PARSE)
					print "ON";
				else
					print "OFF";
				")"
			}
			case 6
			{
				print "Parse Rank Monitoring (";
				if (debug_flags & D_PARSE_RANK)
					print "ON";
				else
					print "OFF";
				")"
			}
			case 7
			{
				print "Recording Playback Helper (";
				if (debug_flags & D_PLAYBACK_HELPER)
					print "ON";
				else
					print "OFF";
				")"
			}
			case 8
			{
				print "Hugor Opcode Monitor (";
				if (debug_flags & D_OPCODE_MONITOR)
					print "ON";
				else
					print "OFF";
				")"
			}
		sel++
	}
}

! Roody's note: This next section is to organize the object tree when HugoFix
! is on to better distinguish between Hugo "guts" and game objects like rooms
! and their contents.

object replaced_objects
{}

object object_classes
{}

object fuses_and_daemons
{}

routine OrganizeTree
{
	local i
	for (i=2;i<=objects ;i++ )
	{
		if parent(i) = nothing
		{
			local a
#ifclear NO_OBJLIB
				a = female_character ! since female_character is of type character
#endif
			if i.type = i or i = a
				move i to object_classes
			elseif i.type = settings
				move i to settings
			elseif i.type = fuse,daemon
				move i to fuses_and_daemons
			elseif not i.name
			{
#ifset _WINDOW_H
				if i.type ~= window_class
#endif
					move i to replaced_objects
			}
		}

	}
}

!\ Roody's note: When I'm testing stuff, I often will code something
in a room's long_desc to print out whether things are true and false.
Here is a helper routine to save people some time.
\!
routine TrueorFalse(arg)
{
	if arg
		print "true";
	else
		print "false";
}

#endif  ! #ifset HUGOFIX


!\ Roody's note: Changed EndGame to make the menu more configurable
  This is the version I had already written that splits EndGame up into
  two routines for both readability and configurability (for adding options
  like (A)musing ).

  As such, I added SpecialKey and SpecialRoutine routines, so people can do
  this:

replace SpecialKey(end_type)
{
   if (end_type = 3) and (word[1] = "s","special")
	{
		return word[1]
	}
   else
		return false
}
 \!

replace EndGame(end_type)
{
	verbroutine = &EndGame     ! something for newmenu and such to check for
	PrintStatusLine                 ! update one last time
	PrintEndGame(end_type)          ! print appropriate ending message

#ifclear NO_VERBS
	local r,n

	RlibMessage(&EndGame, 1,end_type)    ! ask to RESTART, RESTORE, (UNDO), or QUIT

	r = -1
	while r = -1
	{
		GetInput
		n++
		r = ProcessKey(word[1], end_type)
		if r = -1
		{
			if display.needs_repaint
			{
				if RepaintScreen
				{
					InitScreen
					PrintStatusLine
				}
				n = 0
			}
			if word[1] and word[1] = SpecialKey(end_type) or not n
				RlibMessage(&EndGame, 1,end_type)    ! ask to RESTART, RESTORE, (UNDO), or QUIT
			else
				RlibMessage(&EndGame, 2,end_type)    ! ask again (more succinctly)
		}
	}
	return r
#else   ! i.e., #elseif set NO_VERBS
	QuitGameText
	return 0;
#endif
}

!#ifclear NO_VERBS
! Roody's note: Companion routine to EndGame
routine ProcessKey(entry,end_type)
{
   local r,i
   r = -1
	i = 0
	if not entry
		return -1
   select entry
	! we do SpecialKey first since options like RESTART can clear word[1]
	! and trigger SpecialKey (which sometimes equals false)
		case SpecialKey(end_type)
			SpecialRoutine(end_type)
		case "restart", "r"
		{
#ifclear NO_XVERBS
			word[1] = "yes"
			if (call &DoRestart)
				r = 1
#else
			if restart
				r = 1
#endif
		}
! "restore" is not possible in a game without xverbs as the player
! shouldn't have been able to save
#ifclear NO_XVERBS
		case "restore", "e"
		{
			word[1] = "restore"
			if (call &DoRestore)
				r = 1
		}
#endif
#ifclear NO_UNDO
		case "undo", "u"
		{
			word[1] = "undo"
#ifclear NO_XVERBS
			if (call &DoUndo)
				r = 1
#else
				""
				if not UNDO_OFF
				{
					if undo
					{
						r = true
						UndoResponse
					}
					else
						RLibMessage(&UndoResponse,2)  ! "Unable to UNDO."
				}
				else
					RLibMessage(&UndoResponse,2) ! "UNDO is not currently allowed."
#endif
		}
#endif
		case "quit", "q"
		{
			QuitGameText
			r = 0
		}
   return r
}
!#endif  ! ifclear NO_VERBS

routine SpecialKey(end_type)   ! this routine exists to be replaced
{
	! example SpecialKey code
!	if (word[1] = "amusing","a") and end_type = 1
!		return word[1] ! so it matches the select word[1] text
!	return 0
}

routine SpecialRoutine(end_type)  ! also exists to be replace
{
!	You most likely won't need the end_type argument but it's there in case
!	 you want additional responses besides the winning condition

!	ShowPage(amusing_list) ! example of using newmenu's ShowPage routine
}

! Roody's note: A routine for text to be used both by Endgame and DoQuit.
routine QuitGameText
{
#ifset NO_ACCESSIBILITY
	local cheap
#endif
	""
	RLibMessage(&QuitGameText) ! "Thanks for playing!"
	""
	if not cheap
		display.title_caption = PRESS_ANY_KEY
	if not system(61) ! if not simple port
	{
		print PRESS_ANY_KEY  ! was PRESS_ANY_KEY; but that doesn't show up in DOS
		HiddenPause
	}
}

! Roody's note: Future Boy! replacement by Kent Tessman that omits objects
! held by NPCs unless specically mentioned ("GET ALL FROM FRED")
! Also updated by me to ignore non-clothing items for clothing verbs
replace ExcludeFromAll(obj)
{
	if obj is hidden
		return true

! in most cases, disallow >WEAR ALL / REMOVE ALL
	local takeoff_check, wear_check,verb_check
	if verbroutine =&DoGet and (word[1] = "remove" or
	(word[1] = "take" and word[3] = "off")) and
		xobject is living
#ifclear AIF
		return true
#else
	{
		verb_check = true
		if obj is worn
			takeoff_check = true
	}
#endif
	elseif verbroutine =&DoGet and (obj is clothing,worn)
		return true
	elseif verbroutine = &DoGet and parent(obj) = player
		return true
#ifclear USE_CHECKHELD
	if verbroutine = &DoTakeOff
	{
		verb_check = true
		if obj is worn
			takeoff_check = true
	}
	elseif verbroutine = &DoWear
	{
		verb_check = true
		if obj is not worn
			wear_check = true
	}
	if verb_check
#else
	if verbroutine = &DoWear_Checkheld
	{
		verb_check = true
		if obj is not worn
			wear_check = true
	}
	elseif verbroutine = &DoTakeOff_Checkheld
	{
		verb_check = true
		if obj is worn
			takeoff_check = true
	}
	if verb_check
#endif
	{
#ifset AIF
! Make >WEAR ALL and >REMOVE ALL only acknowledge clothes
		if obj is not clothing or (obj is clothing and
		not (takeoff_check or wear_check))
#endif
			return true
	}

	! Exclude things NPCs are carrying unless the NPC is explicitly
	! given as the parent
	if parent(obj) is living
	{
		if IsPossibleXobject(parent(obj)) or parent(obj) = player
			return false
		return true
	}
	return false
}

#ifclear NO_DISAMB_HELP
#if undefined disamb_max
constant DISAMB_MAX 3
#endif

property disamb_suspects alias misc
property disamb_total alias each_turn

object disamb_holder
{
	in settings
	disamb_total 0
	disamb_suspects #DISAMB_MAX
}
#endif  ! NO_DISAMB_HELP

! Roody's note: FindObject tweaked to get rid of jumps, just because
! Added some new checkheld code
replace FindObject(obj, objloc, recurse)
{
	local a, p
	local this_parse_rank
	local found_result = true
	local FindObjectIsFound

	parser_data[PARSER_STATUS] |= FINDOBJECT_CALLED

	if obj = nothing or obj = player
	{
		if obj = nothing and not recurse
		{
#ifset DEBUG
			if debug_flags & D_FINDOBJECT
				print "[Resetting FindObject]"
			if debug_flags & D_PARSE_RANK
				print "[parser_data[BEST_PARSE_RANK] = 0]"
#endif
			parser_data[BEST_PARSE_RANK] = 0
			parser_data[PARSE_RANK_TESTS] = 0
			parser_data[PARSER_STATUS] &= ~FINDOBJECT_KNOWN
			parser_data[PARSER_STATUS] &= ~FINDOBJECT_NONLIVING
		}
		elseif obj = player
		{
			this_parse_rank = obj.parse_rank
			if this_parse_rank > parser_data[BEST_PARSE_RANK]
				parser_data[BEST_PARSE_RANK] = this_parse_rank
		}

		! nothing and player are always available
#ifset MULTI_PCS
		MakeKnown(obj)
#else
		obj is known
#endif
		return true
	}
#ifclear NO_FUSES
	elseif obj.type = fuse or obj.type = daemon
	{
		! Optimize checking of simple fuses and daemons:
		if obj.#in_scope = 1 and not &obj.in_scope and not obj.#found_in and not &obj.found_in
		{
			if obj is active and obj.in_scope = player
				return true
			else
				return false
		}
	}
#endif

	if not recurse and parser_data[PARSER_STATUS] & PARSER_ACTIVE
	{
		if FailsAllChecks(obj,objloc)
			return false

	! Do some parse_rank tweaking to rank lower objects which (probably) don't
	! apply to a particular command, like trying to open something that's
	! already open:
		this_parse_rank = obj.parse_rank

	! And be ready to prefer the last specifically referred to object in
	! the event of disambiguation
		if obj = parser_data[LAST_SINGLE_OBJECT]
			this_parse_rank++

#ifclear NO_VERBS
		this_parse_rank = CheckParseRank(obj, this_parse_rank)

#ifset USE_CHECKHELD
		if checkheld is active
		{
			if DismissUnheldItems(obj,objloc)
				return false
			elseif PrioritizeHeldItems(obj)
				this_parse_rank += 100
		}
#endif	! USE_CHECKHELD
#endif	! #ifclear NO_VERBS
	}

	p = parent(obj)  ! find the parent of the object

	! The objloc argument is equal to 0 if a) the grammar token is
	! "anything", or b) the engine is re-testing object availability
	! given the setting of parser_data[BEST_PARSE_RANK] during the first
	! pass of object disambiguation.

	if objloc = 0 and parser_data[PARSER_STATUS] & PARSER_ACTIVE
	{
#ifset MULTI_PCS
		if location and not ObjectIsKnown(obj)
#else
		if location and obj is not known
#endif
		{
			if FindObject(obj, location, true)
#ifset MULTI_PCS
				MakeKnown(obj)
#else
				obj is known
#endif
		}

		if not ObjectIsKnown(obj)
		{
#ifset DEBUG
			if debug_flags & D_FINDOBJECT and not recurse
			{
				print "[FindObject("; obj.name; " ["; number obj; "], "; \
					objloc.name; " ["; number objloc; "]):  "; \
					"false (object not 'known')]"
			}
#endif
			return false
		}

		if not recurse
		{
			if not AnythingTokenCheck(obj,objloc)
				return false
			elseif this_parse_rank < parser_data[BEST_PARSE_RANK]
			{
#ifset DEBUG
				if debug_flags & D_FINDOBJECT
				{
					print "[FindObject("; obj.name; " ["; number obj; "], "; \
						objloc.name; " ["; number objloc; "]):  "; \
						"false (this_parse_rank = "; number this_parse_rank; \
						" < parser_data[BEST_PARSE_RANK] = "; number parser_data[BEST_PARSE_RANK]; ")]"
				}
#endif
				return false
			}
		}
		FindObjectisFound = true
	}
	elseif obj = objloc or
		(player in obj and obj ~= objloc and (obj~=location or not recurse)) or
		p = obj or p = objloc or p = player
	{
#ifset MULTI_PCS
		MakeKnown(obj)
#else
		obj is known
#endif
		FindObjectIsFound = true
	}
	elseif p  ! does obj have a parent?
	{
		local supercheck
#ifset USE_SUPERCONTAINER
		if p.type = SuperContainer
		{
			if InList(p, contents_in, obj) and p is not open and p is openable
!			and p is not transparent
			{
				if p ~= parent(player) or (p = parent(player) and
					InList(p, contents_on, player))
					supercheck = true
			}
		}
#endif
		if (p is not openable or p is platform) and p is not quiet and
				not supercheck
		{
			if FindObject(p, objloc, true) and ObjectisKnown(p)
			{
#ifset MULTI_PCS
				MakeKnown(obj)
#else
				obj is known
#endif
				FindObjectIsFound = true
			}
		}
		elseif p is openable and p is open and p is not quiet
		{
			if FindObject(p, objloc, true) and ObjectisKnown(p)
			{
#ifset MULTI_PCS
				MakeKnown(obj)
#else
				obj is known
#endif
				FindObjectIsFound = true
			}
		}
		elseif p is transparent, not quiet
		! (p is openable, not open, transparent, not quiet)
		{
			if FindObject(p, objloc, true) and ObjectisKnown(p)
			{
#ifset MULTI_PCS
				MakeKnown(obj)
#else
				obj is known
#endif
				found_result = 2
				FindObjectIsFound = true
			}
		}
	}

	if obj.#found_in and not FindObjectIsFound
	{
		for (a=1; a<=obj.#found_in; a++)
		{
			if obj.found_in #a and (obj.found_in #a = objloc or
				FindObject(obj.found_in #a, objloc, true))
			{
#ifset MULTI_PCS
				MakeKnown(obj)
#else
				obj is known
#endif
				FindObjectIsFound = true
				break
			}
		}
	}

	if obj.#in_scope and not FindObjectIsFound
	{
		for (a=1; a<=obj.#in_scope; a++)
		{
			if obj.in_scope #a
			{
				if obj.in_scope #a=objloc or obj.in_scope #a=actor
					FindObjectIsFound = true
				elseif FindObject(obj.in_scope #a, objloc, true)
					FindObjectIsFound = true
				if FindObjectIsFound
				{
#ifset MULTI_PCS
					MakeKnown(obj)
#else
					obj is known
#endif
					break
				}
			}
		}
	}
	if parser_data[PARSER_STATUS] & PARSER_ACTIVE and not recurse
		MakeParserNotes(obj, FindObjectIsFound)
	if not FindObjectIsFound
	{
#ifset DEBUG
		if debug_flags & D_FINDOBJECT and not recurse
		{
			print "[FindObject("; obj.name; " ["; number obj; "], "; \
				objloc.name; " ["; number objloc; "]):  "; \
				"false]"
		}
#endif
		return false
	}

! FindObjectIsFound area
	if not recurse and parser_data[PARSER_STATUS] & PARSER_ACTIVE
	{
		local new_disamb
		if parser_data[PARSE_RANK_TESTS]++ = 0
		{
#ifset DEBUG
			if debug_flags & D_PARSE_RANK and this_parse_rank > parser_data[BEST_PARSE_RANK]
			{
				print "[parser_data[BEST_PARSE_RANK] = "; number this_parse_rank; "]"
			}
#endif
			parser_data[BEST_PARSE_RANK] = this_parse_rank
			new_disamb = true
		}
		elseif this_parse_rank > parser_data[BEST_PARSE_RANK]
		{
#ifset DEBUG
			if debug_flags & D_PARSE_RANK and this_parse_rank > parser_data[BEST_PARSE_RANK]
			{
				print "[parser_data[BEST_PARSE_RANK] = "; number this_parse_rank; "]"
			}
#endif
			parser_data[BEST_PARSE_RANK] = this_parse_rank
!			new_disamb = true
		}
#ifclear NO_DISAMB_HELP
	if not objloc or (new_disamb and disamb_holder.disamb_suspects)
		FillDisambHolder(obj,new_disamb,this_parse_rank)
#endif
	}
#ifset DEBUG
	if debug_flags & D_FINDOBJECT and not recurse
	{
		print "[FindObject("; obj.name; " ["; number obj; "], "; \
			objloc.name; " ["; number objloc; "]):  "; \
			"true]"
	}
#endif
	return found_result
}

! Roody's note: MakeParserNotes adds some values to PARSER_STATUS.
! Some are from the original FindObject, some are new. Mainly, they
! affect how some things are handled in ParseError.

routine MakeParserNotes(obj,is_found)
{
	! If any object FindObject is attempting to disambiguate
	! is known, make a note of it (if during the parsing phase)
	if obj is not living
		parser_data[PARSER_STATUS] |= FINDOBJECT_NONLIVING
	else
	{
		parser_data[PARSER_STATUS] |= FINDOBJECT_LIVING
		last_actor = obj
	}
	if ObjectisKnown(obj)
		parser_data[PARSER_STATUS] |= FINDOBJECT_KNOWN
	if is_found
		parser_data[PARSER_STATUS] |= FINDOBJECT_FOUND
}

! Roody's note: FillDisambHolder makes "which <blank> did you mean,
! the 1) <first blank> or 2) <second blank>?" messages possible.
! This routine fills the disam_holder object with the possible
! suspects.

#ifclear NO_DISAMB_HELP
routine FillDisambHolder(obj,reset,this_parse_rank)
{
	local x
	if not reset
	{
		x = AddPropValue(disamb_holder, disamb_suspects , obj,true)
		if x > disamb_holder.disamb_total
		{
#ifset DEBUG
			if debug_flags & D_PARSE_RANK
				print "["; obj.name;" added to disamb_holder suspects with
				parse rank "; number this_parse_rank;".]"
#endif
			disamb_holder is special
			disamb_holder.disamb_total = disamb_holder.disamb_total + 1
			return true
		}
		elseif not x
		{
#ifset DEBUG
			if debug_flags & D_PARSE_RANK
				print "[unable to add "; obj.name;" to disamb_holder
				suspects. disamb_holder deactivated.]"
#endif
			disamb_holder is not special
			return
		}
	}
	x = 1 ! 2
#ifset DEBUG
	if debug_flags & D_PARSE_RANK
		print "[disamb_holder suspects cleared.]"
#endif
	disamb_holder.disamb_total = 0 ! 1
!	disamb_holder.disamb_suspects = obj
	while (x <= disamb_holder.#disamb_suspects and
	disamb_holder.disamb_suspects #x)
	{
		disamb_holder.disamb_suspects #x = 0
		x++
	}
	disamb_holder is not special
}

#ifset NEW_STYLE_PRONOUNS
object parse_pronouns
{
	in preparse_instructions
	type settings
	execute
	{
		move it_object to parent(it_obj)
		move him_object to parent(him_obj)
		move her_object to parent(her_obj)
		move them_object to parent(them_obj)
		return false
	}
}
#endif

object parse_disamb
{
	in preparse_instructions
	type settings
	execute
	{
		local a, n, w
		if disamb_holder is not special
			return false
		if words = 1
		{
			if disamb_holder.disamb_total = 2
			{
				select word[1]
					case "former":a = 1
					case "latter":a = 2
			}
			select word[1]
				case "1","first": a = 1
				case "2","second": a = 2
				case "3", "third": a = 3
				case "4", "fourth": a = 4
				case "5", "fifth": a = 5
			if a and a <= disamb_holder.disamb_total
			{
				parser_data[LAST_SINGLE_OBJECT] = disamb_holder.disamb_suspects #a
				RestoreFromOldWord
				return true
			}
		}
		return false
	}
}
#endif

! Roody's note: FailsAllChecks is called by commands with "all" in
! it.

routine FailsAllChecks(obj,objloc)
{
	local a
	if not (word[2] = "~all" or word[3] = "~all")
		return
	if not IsPossibleXobject(obj) or (xobject and xobject ~= obj)
	{
		a = obj.exclude_from_all
		if a < 2
		{
			if a or ExcludeFromAll(obj) = true
			{
#ifset DEBUG
				if debug_flags & D_FINDOBJECT
				{
					print "[FindObject("; obj.name; " ["; number obj; "], "; \
						objloc.name; " ["; number objloc; "]):  "; \
						"false (excluded from \"all\")]"
				}
#endif
				return true
			}
		}
	}
}

! Roody's note: CheckParseRank puts all of FindObject's parse rank
! modifying code into one routine. Replace this if you have other
! verb routines to add or tweak.

routine CheckParseRank(obj, this_parse_rank)
{
	select verbroutine
	case &DoOpen
	{
		if obj is openable and obj is open
			this_parse_rank--
	}
	case &DoClose
	{
		if obj is openable and obj is not open
			this_parse_rank--
	}
	case &DoSwitchOn
	{
		if obj is switchable and obj is switchedon
			this_parse_rank--
	}
	case &DoSwitchOff
	{
		if obj is switchable and obj is not switchedon
			this_parse_rank--
	}
#ifset USE_CHECKHELD
	case &DoWear_Checkheld
	{
		if obj is clothing and obj is worn
			this_parse_rank--
	}
	case &DoTakeOff_CheckHeld
	{
		if obj is clothing and obj is not worn
			this_parse_rank--
	}
#else  ! if clear USE_CHECKHELD
	case &DoWear
	{
		if obj is clothing and obj is worn
			this_parse_rank--
	}
	case &DoTakeOff
	{
		if obj is clothing and obj is not worn
			this_parse_rank--
	}
#endif !  USE_CHECKHELD

! Ok, I wrote this code when I mistakenly thought Hugo didn't
! prioritize nouns over adjectives, but then it turns out I was just
! misled by some game code I didn't take a good look at.  I'll leave
! this code here commented out in case it helps anyone.
!#ifclear NO_NOUN_PRIORITY
!	local a
!	a = CurrentCommandWords
!! in an xobject & object command, the xobject is run through
!! FindObject first, so if there is no xobject, we know the last word
!! is important
!	if not xobject
!	{
!		if ObjWord(word[a],obj) = noun
!			this_parse_rank++
!	}
!	else
!	{
!! if there's an xobject already defined, we know it's an
!! object & xobject command and FindObject is currently findint the
!! object
!! First, we go back from the end of the command until we get to the
!! grammar words
!		while not VerbCheck(word[a]) and a
!		{
!			a--
!		}
!! Then we keep going back past the grammar words to get to the last
!! object word
!		while VerbCheck(word[a]) and a
!		{
!			a--
!		}
!		if ObjWord(word[a],obj) = noun
!			this_parse_rank++
!	}
!#endif !ifclear NO_NOUN_PRIORITY

	return this_parse_rank
}

! Roody's note: When using things like the Routine Grammar Helper, if
! your game has multiple items with similar names, disambiguation messages
! can pop up for items not even in the room (since we're using the anything
! grammar token). AnythingTokenCheck exists to make sure those objects aren't
! even considered. You may need to replace and adapt this if you write any
! code that uses the grammar helper.

routine AnythingTokenCheck(obj,objloc)
{
	local failed_check
	select verbroutine
#ifclear NO_VERBS
#ifset NEW_EMPTY
		case &DoEmpty
		{
			if not parent(obj) or not FindObject(parent(obj),location)
				failed_check = true
#ifset DEBUG
				if debug_flags & D_FINDOBJECT
				{
					print "[FindObject("; obj.name; " ["; number obj; "], "; \
					objloc.name; " ["; number objloc; "]):  "; \
					"false (object not in scope)]"
				}
#endif
		}
#endif
		case &DoGet
		{
			if obj in player
			{
#ifset DEBUG
				if debug_flags & D_FINDOBJECT
				{
					print "[FindObject("; obj.name; " ["; number obj; "], "; \
						objloc.name; " ["; number objloc; "]):  "; \
						"false (object in player)]"
				}
#endif
				failed_check = true
			}
		}
#endif ! ifclear NO_VERBS
		case else: failed_check = false
	return (not failed_check) ! return false if it failed
}

#ifset USE_CHECKHELD
! Roody's note: DismissUnheldItems is called when a command uses
! "all" and the USE_CHECKHELD system is turned on.
routine DismissUnheldItems(obj,objloc)
{
	if checkheld is not plural or obj is checkheld_flag
		return
	elseif CheckHeld_Verb_Check
	{
#ifset DEBUG
		if debug_flags & D_FINDOBJECT
		{
			print "[FindObject("; obj.name; " ["; number obj; "], "; \
				objloc.name; " ["; number objloc; "]):  "; \
				"false (not checkheld_flag)]"
		}
#endif
		return true
	}
}

! Roody's note: PrioritizeHeldItems is called when a command uses
! "all" and the USE_CHECKHELD system is turned on.
routine PrioritizeHeldItems(obj)
{
	if checkheld is plural or obj is not checkheld_flag
		return
	elseif CheckHeld_Verb_Check
		return true
}

! The new code in this routine makes it so items must be held for >WEAR ALL or
! >REMOVE ALL to work
routine CheckHeld_Verb_Check
{
	if (verbroutine = &DoDrop_CheckHeld, &DoPutonGround_CheckHeld) or
		(verbroutine = &DoPutIn_CheckHeld and xobject) or
		(verbroutine = &DoTakeOff_CheckHeld, &DoWear_Checkheld)
	return true
}
#endif

!\ Roody's note: This version of HoursMinutes should work for several
"days" (up to 22) \!
replace HoursMinutes(val)
{
	local hours, minutes, military

	hours = val / 60
	minutes = val - hours * 60
	military = (STATUSTYPE & MILITARY_TIME)

	if not military
	{
		while hours > 12
		{ hours -= 12 }
		elseif hours = 0:  hours = 12
	}
	else
	{
		while hours >= 24
		{
			hours -= 24
		}
		if hours < 10:  print "0";
	}

	print number hours; ":";
	if minutes < 10
		print "0";
	print number minutes; " ";
	if not military
	{
		while val > 1440
		{
			val -= 1440
		}
		if val >= 720
			print "p.m.";
		else
			print "a.m.";
	}
}

! Roody's note: this version doesn't reset event_flag when it's set to 2 and
! also gets rid of jump, also moves PrintStatusLine to end of routine
replace MovePlayer(loc, silent, ignore)
{
	local v, obj, xobj, act, ret, LeavingMovePlayer,real_loc

#ifclear NO_OBJLIB
	if loc.type = direction
	{
		local l
		l = location.(loc.dir_to)
		if l > 1                ! since a message always returns 1
			loc = l
		else
			return
	}
#endif
	if parent(loc) = 0
		real_loc = loc
	else
		real_loc = parent(loc)

#ifset USE_ATTACHABLES
	if ObjectisAttached(player, location, real_loc)
		return
#endif

! Check if there's a before routine for MovePlayer in the new
! location.  Note that the (potential) new location and the old
! location are stored in object and xobject, respectively.
!
	v = verbroutine
	obj = object
	xobj = xobject
	act = actor
	verbroutine = &MovePlayer
	object = real_loc
	xobject = location
	actor = player
	if not ignore
	{
		while (true)
		{
			ret = player.before
			if ret : break
			ret = location.before
			if ret : break
			ret = real_loc.before
			if ret : break
			local i
			for i in location
			{
				if i ~= player
					ret = i.react_before
				if ret
					break
			}
#ifset USE_SCOPE_REACT
			if ret : break
			ret = ScopeReactBefore
#endif
			break
		}
		if ret : LeavingMovePlayer = true
	}

	if (not LeavingMovePlayer)
	{
		move player to loc
		old_location = location
		location = real_loc

#ifset USE_ATTACHABLES
		MoveAllAttachables(player, old_location, location)
#endif
	}

! LeavingMovePlayer area

	if not ret
	{
		local lig
		lig = FindLight(location)
		if not lig
			DarkWarning
		elseif not silent
		{
			DescribePlace(location)
			if not event_flag
				event_flag = true
		}

	! Check if there's an after routine for MovePlayer in the new
	! location:
	!
		if not ignore
		{
			ret = player.after
			if not ret
				ret = location.after
			if not ret
			{
				for i in location
				{
					if i ~= player
						ret = i.react_after
					if ret
						break
				}
			}
#ifset USE_SCOPE_REACT
			if not ret
				ret = ScopeReactAfter
#endif
		}
		if lig and not silent
		{
#ifset NEW_ROOMS
			if not location.first_visit
				location.first_visit = counter + 1
#endif
#ifset MULTI_PCS
			MakeMovedVisited(location)
#else
			location is visited
#endif
		}
	}
	verbroutine = v
	object = obj
	xobject = xobj
	actor = act

	return ret
}

global NEW_PARSE
global last_actor
global last_verbroutine
global previous_object
global previous_xobject

enumerate start = 1, step *2
{
	WORDSSAVED_F            ! locks in the first actor, object, etc. called
	MULTISPEAKTOAGAIN_F = 4 ! continued speakto commands *not* in a multicommand
	PARSE$_F                ! was there a parse$ string in last command?
	MULTICOMMAND_F          ! is a multiple-command line being interpreted?
	LAST_TURN_TRUE_F        ! did the last turn return true?
	HIDE_COMMAND_F          ! set this if you want to temporarily hide
	                        ! commands when SHOW_COMMANDS is set
	SPEAKTO_F               ! order being parsed
}

! Roody's note: Took out Parse's HugoFix code, as it tries to print parse$
! before parse$ is set also got rid of jump
replace Parse
{
	local a
	local reparse
	local temp_verb, temp_obj, temp_xobj
	local JumpToEnd

	ResetParse
	NEW_PARSE = NEW_PARSE & ~SPEAKTO_F

	list_nest = 0
	event_flag = 0

	if nothing ~= 0                   ! best place to check this
		print "[WARNING:  Objects/classes defined before library]"

#ifset DEBUG
	if debug_flags & D_PARSE_RANK
		print "[parser_data[BEST_PARSE_RANK] = 0]"
#endif

	if word[1] = "~oops"
		return
	! save the last turn's stuff which will be used if the current command is
	! >CHARACTER, G (AGAIN)
	previous_object = object
	previous_xobject = xobject
	last_verbroutine = verbroutine

	! Allow the player object to override the parsing cycle completely
	temp_verb = verbroutine
	temp_obj = object
	temp_xobj = xobject
	verbroutine = &PreParse
	object = nothing
	xobject = nothing
	if player.before
	{
		verbroutine = temp_verb
		object = temp_obj
		xobject = temp_xobj
		parser_data[PARSER_STATUS] = 0
		parser_data[LAST_PARSER_STATUS] = 0
		return false
	}
	verbroutine = temp_verb
	object = temp_obj
	xobject = temp_xobj

	if PreParseInstructions: reparse = true
	if PreParse:  reparse = true            ! easily replaceable routine

! The following, devised by Jim Newland, checks to see if the player
! input refers to an unnecessary item of scenery (for example) which is
! nonetheless mentioned in the location description.

	local command_words
	! figure out the words in the current command
	command_words = CurrentCommandWords
	for (a=2; a<=command_words; a++)
	{
		local error
		if word[a] = "then"  ! Allow for "then..." and "and then..."
		{
			! end of this command
			word[a] = "."
			command_words = a - 1
			if word[(a-1)] = "~and"
			{
				DeleteWord(a-1)
				command_words--
			}
			reparse = true
			break
		}
		elseif Inlist(player, extra_scenery, word[a])
		{
			Message(&Parse, 1)
			error = true
		}
		elseif Inlist(location, extra_scenery, word[a])
		{
			Message(&Parse, 1)
			error = true
		}
		elseif word[a] = "~oops"
		{
			ParseError(6) ! "That doesn't make any sense."
			error = true
		}
		elseif word[a] = "~and" and VerbCheck(word[a+1])
		{
		! this checks to make sure a character isn't ordered with an xverb
		! command
			if XVerbCheck(word[a+1])
			{
				local safe
				if last_actor and last_actor ~= player and
					(word[(a+1)] = "g" or word[a+1] = "again") and
					parser_data[PARSER_STATUS] ~= PARSER_RESET
				{
					if ObjWord(word[(a-1)], last_actor) = noun
						safe = true
				}
				if not safe
				{
					ParseError(6) ! "That doesn't make any sense."
					error = true
				}
			}
		! this runs the command through OrdersPreParse
			elseif word[a+1] ~= "then"
			{
				if word[(a+1)] = "~and"
				{
					DeleteWord((a+1),1)
					command_words--
				}
				else
				{
					NEW_PARSE = NEW_PARSE | SPEAKTO_F
					if OrdersPreParse((a+1),command_words)
						reparse = true
				}
			}
		}
		if error
		{
			word[1] = ""            ! force ParseError(0)
			words = 0
			customerror_flag = true
			return true
		}
	}

	! Last player-specified object
	if object > 0 and object < objects and
	not (NEW_PARSE & PRONOUNS_SET)
		AssignPronoun(object)

	! Must do this after AssignPronoun, and reset it to 0
	! after both of Perform and ParseError:
	parser_data[PARSER_STATUS] = PARSER_ACTIVE

	parser_data[LAST_PARSER_STATUS] = 0

!  MultiCommandInstructions returns false if a character is given an xverb
!  command
	local x
	x = MultiCommandInstructions(command_words)
	if not x
	{
		ParseError(6) ! "That doesn't make any sense."
		word[1] = ""            ! force ParseError(0)
		words = 0
		customerror_flag = true
		return true
	}
	! since MultiCommandInstructions calls OrdersPreparse, it returns 2 if
	! that returned true
	elseif x = 2
		reparse = true

	if not (NEW_PARSE & SPEAKTO_F) and last_actor and last_actor ~= player
		AssignPronoun(last_actor)

	! To repeat last command
	if (word[1] = "again" or word[1] = "g") and word[2] = ""
	{
		! if the last command had string grammar in it, the turn can't be
		! repeated
		if (NEW_PARSE & PARSE$_F)
		{
			NEW_PARSE = NEW_PARSE & ~PARSE$_F
			RLibMessage(&Parse,1) ! "You will need to type that in again."
			word[1] = ""            ! force ParseError(0)
			words = 0
			customerror_flag = true
			return true
		}
		RestoreFromOldWord
		! in the instance of CHARACTER, SOME ACTION followed by >AGAIN
		! the next turn
		if (NEW_PARSE & MULTISPEAKTOAGAIN_F) and ! speakto_again
			(NEW_PARSE & WORDSSAVED_F)
		{
			if word[1]
			{
				InsertWord(1,2)
				word[2] = "~and"
			}
			word[1] = last_actor.noun
			NEW_PARSE = NEW_PARSE & ~MULTISPEAKTOAGAIN_F ! clear it
		}
		elseif not oldword[0] or not (NEW_PARSE & WORDSSAVED_F)
			words = 0

		reparse = true
		JumpToEnd = true
	}

#ifclear NO_UNDO
	if not (word[1] = "undo" and word[2] = "")
#endif
		NEW_PARSE = (NEW_PARSE & ~WORDSSAVED_F) & ~PARSE$_F
	if not JumpToEnd
	{
#ifclear NEW_STYLE_PRONOUNS
		local n, number_pronouns
#endif
		local count
		for (count=2; count<=command_words and word[count]~=""; count++)
		{
			select word[count]
				! Rename pronouns to appropriate objects
				case "it", "them", "him", "her"
				{
					select word[count]
						case "it":      a = it_obj
						case "them":    a = them_obj
						case "him":     a = him_obj
						case "her":     a = her_obj

					if a = nothing
					{
						! "...be a little more specific"
						ParseError(13)
						words = 0
						customerror_flag = true
						return
					}
#ifclear NEW_STYLE_PRONOUNS
					! Avoid, e.g., "put it it something"
					elseif word[count] ~= word[count+1]
					{
						! For her_obj, we can run into trouble, since
						! it's both possessive and a pronoun, so only
						! replace "her" where it's the last word:
						if a ~= her_obj or
							(a = her_obj and (count = 2 or count = words))
						{
!   Maybe this is a bad idea, but I was hoping this next bit might
!   help with repeated ASK/TELL about other characters, like:
!   >TELL BOB ABOUT FRED.  >ASK BOB ABOUT HIM
							if count ~= 2 and count = words and
							(object is living and
							(word[count] = "her" and xobject is female) or
							(word[count] = "him" and xobject.pronoun #2 = "him"))
							{
								if ObjWord(word[2],object)
									a = xobject
							}
							n = SetObjWord(count, a)
							replace_pronoun[number_pronouns] = a
							number_pronouns++
							if n > 1
								count = count + (n - 1)
							NEW_PARSE |= PRONOUNS_SET
							reparse = true
						}
					}
#endif
				}
		}

#ifclear NEW_STYLE_PRONOUNS
		if number_pronouns = 2 and replace_pronoun[0] = replace_pronoun[1]
			number_pronouns--

		if number_pronouns > 0
			PrintReplacedPronouns(number_pronouns)
#endif

#ifset USE_PLURAL_OBJECTS
		if ParsePluralObjects:  reparse = true
#endif

#ifset USE_CHECKHELD
		ParseCheckHeld
#endif

		! Store current command for future reference
		! old word storage code. now replaced in Perform
!		if not (NEW_PARSE & NEWPARSE_F) and not XVerbCheck(word[1])
!		{
! 			local ow = 1 ! oldword variable
!			a = 1
!			while word[a] ~= ""
!				oldword[ow++] = word[a++]
!			oldword[ow--] = ""
!			oldword[0] = ow
!		}
	}
	! old "LeaveParse area

#ifset DEBUG
	if debug_flags & D_PARSE
	{
		local w
		Font(BOLD_ON)
		for (w=1; w<=words and word[w]~=""; w++)
		{
			print "[";
			if (word[w] ~= -1)
				print word[w];
			else
				print "\Iparse$\i";
			"] ";
		}
		print newline
		Font(BOLD_OFF)
	}
#endif
	return reparse
}

replace ResetParse
{
	customerror_flag = false
	parser_data[BEST_PARSE_RANK] = 0  ! reset each parser cycle
	parser_data[PARSE_RANK_TESTS] = 0
	parser_data[VERB_IS_XVERB] = 0
}

routine VerbCheck(a)
{
	if a < "nothing" ! nothing.name
		return true
	else
		return false
}

! Roody's note: Since commands to characters get sent straight to SpeakTo
! when parsed, XverbCheck exists to try to stop xverb commands from
! successfully being passed on. A game with additional xverbs may need to
! replace this and add more.

routine XverbCheck(a)
{
	local r
	select a
		case "load","restore","resume","save","suspend","script", \
		"transcript","transcription","quit","q","restart","brief", \
		"normal","superbrief","verbose","short","record","playback", \
		"long","display","wide","tall","score","undo", "g", "again", \
		"version", "about", "hints", "hint"
			r=1

	return r
}

routine MultiCommandInstructions(n) ! n is CurrentCommandWords
{
	if not NEW_PARSE = NEW_PARSE & LAST_TURN_TRUE_F ! last_turn_true
		NEW_PARSE = NEW_PARSE & ~MULTICOMMAND_F !multicommand = false
	NEW_PARSE = NEW_PARSE & ~LAST_TURN_TRUE_F
	if not (NEW_PARSE & MULTICOMMAND_F) ! multicommand
	{
		if n < words
			NEW_PARSE = NEW_PARSE | MULTICOMMAND_F ! multicommand = true
		if last_actor and last_actor ~= player and (word[1] = "g","again") and
			not (NEW_PARSE & MULTICOMMAND_F) ! multicommand
			NEW_PARSE = NEW_PARSE | MULTISPEAKTOAGAIN_F
	}
	else
	{
#ifset SHOW_COMMANDS
		if PrintCurrentCommand(true)
			""
#endif
		if n = words
			NEW_PARSE = NEW_PARSE & ~MULTICOMMAND_F ! multicommand = false
		if last_actor and last_actor ~= player
		{
			NEW_PARSE = NEW_PARSE | SPEAKTO_F
			if XverbCheck(word[1]) and not (word[1] = "g","again")
			{
				NEW_PARSE = NEW_PARSE & ~MULTICOMMAND_F ! multicommand = false
				return false
			}
			elseif OrdersPreparse(1,n)
				return 2
		}
	}
	return true
}

! Roody's note: OrdersPreParse is a PreParse routine for orders given to
! characters.  Since the beginning of the relevant command often won't be
! word[1], b and e are the numbers of the word array where the command
! begins and ends, respectively.

routine OrdersPreParse(b,e)
{}

routine PrintCurrentCommand(print_prompt)
{
	if (NEW_PARSE & HIDE_COMMAND_F)
		return false
	FONT(ITALIC_ON)
	if print_prompt
		print prompt;
	if last_actor and last_actor ~= player and (word[1] ~= "g","again")
		print last_actor.name;", ";
	local a = 1
	while not (word[a] = "" or word[a] = "then")
	{
		select word[a]
			case -1 : print "\"";parse$;"\"";
			case "~all" : print "all";
			case "~any" : print "any";
			case "~and"
			{
				if word[a-1] = "~and" or not VerbCheck(word[a+1])
					print " and";
				else
					print ",";
			}
			case "~except" : print "except";
			case "~oops" : print "oops";  ! hopefully never called
			case else : print word[a];
		if word[a+1] ~= "","~and"
			print " ";
		a++
	}
	FONT(ITALIC_OFF)
	return true
}

routine SaveOldWord
{
	local ow = 1, a
	a = 1
	if word[1] = "g", "again"
		return
	while word[a] ~= ""
	{
		if word[a] = -1
			NEW_PARSE = NEW_PARSE | PARSE$_F
		oldword[ow++] = word[a++]
	}
	oldword[(ow+1)]= ""
	oldword[ow] = "" ! oldword[ow--] = ""  ! old code
	oldword[0] = words  ! oldword[0] = ow ! old code
}

routine RestoreFromOldWord
{
	local ow = 1,a
	while oldword[ow+1] ~= ""
		ow++
	if ow>1
		InsertWord(1,(ow-1))
	for (a=1; a<=ow; a++)
		word[a] = oldword[a]
}

! Roody's note: Now prints parse$ if parser monitoring is on
! and various other little fixes suggested by Mike Snyder
replace ParseError(errornumber, obj)
{
	PrintStatusLine ! redraw PrintStatusLine in case screen size changed
#ifset DEBUG
	if debug_flags & D_PARSE
	{
		local w, p
		Font(BOLD_ON)
		if (errornumber = 0,1,16,17) ! cases where Parse hasn't been called
		{
			for (w=1; w<=words and word[w]~=""; w++)
			{
				print "[";
				if (word[w] ~= -1)
					print word[w];
				else
				{
					print "\Iparse$";
					if p
						print number (p+1);"*";
					p++
					FONT(ITALIC_OFF)
				}
				"] ";
			}
			print newline
			if (p>1)
				"(* Not an \Iactual\i referable token.)"
		}
		! can't remember why I needed this code
		! this section so we print parse$ only once
!		if string(_temp_string,parse$) and
!			not (parser_data[PARSER_STATUS] & 32)
!		{
			print "[parse$ = \""; parse$; "\"]"
!			parser_data[PARSER_STATUS] |= 32
!		}
		print "[ParseError errornumber = "; number errornumber; "]"
		print "[ParseError object = "; obj.name; "]"
		Font(BOLD_OFF)
	}
#endif

	NEW_PARSE = (NEW_PARSE & ~WORDSSAVED_F)

	local r
	r = BeforeParseError(errornumber,obj)
	if word[1] = "*"
	{
		if betalib is special
			RLibMessage(&DoScriptOnOff, 3) ! Comment recorded!
		else
			RLibMessage(&DoScriptOnOff, 4) ! Comment not recorded!
		r = true
	}
	if r
		customerror_flag = true

	if errornumber >= 100
	{
		CustomError(errornumber, obj)
		word[1] = ""            ! force ParseError(0)
		words = 0
		last_actor = 0
		customerror_flag = true
		parser_data[PARSER_STATUS] = PARSER_RESET
		return true
	}
	if customerror_flag
	{
		last_actor = 0
		customerror_flag = 0    ! CustomError already printed error
		parser_data[PARSER_STATUS] = PARSER_RESET
		return true
	}

	r = NewParseError(errornumber, obj)
	if r
	{
		parser_data[PARSER_STATUS] = PARSER_RESET
		return r
	}

	select errornumber
		case 1
		{
			print "You";
			MatchPlural(player, "doesn't", "don't")
			print	" need to use the word \"";
			if obj
				print obj;
			else
				print parse$;
#ifset AMERICAN_ENGLISH
				".\""
#else
				"\"."
#endif
		}
		case 2
		{
			if not (parser_data[PARSER_STATUS] & FINDOBJECT_CALLED)
			{
				if XverbCheck(word[1])
					return ParseError(1,word[1])
				else
					errornumber = 6
			}
			elseif actor = player and
!				not (parser_data[PARSER_STATUS] & FINDOBJECT_NONLIVING) and
				(parser_data[PARSER_STATUS] & FINDOBJECT_LIVING)
			{
				obj = last_actor
				errornumber = 11
			}
			elseif not (parser_data[PARSER_STATUS] & FINDOBJECT_FOUND)
					errornumber = 11
			else
			{
				r = 1
				while word[r] ~= ""
				{
					if word[r] = "~and","former","latter"
					{
						errornumber = 6
						break
					}
					r++
				}
				if errornumber ~= 6
					"Better start with a verb."
			}
		}
		case 3
		{
			local a, b
			 select word[1]
				case "x"
				{
					a = "examine"
					if word[2] ~= "~all"
						b = word[2]
				}
				case "l","look"
				{
					if word[2] = "~all"
						a = "look at"
					else
						a = "look"
					if word[2] ~= "~all"
						b = word[2]
				}
				case "sit","lie","go","walk","watch"
				{
					a = word[1]
					if word[2] ~= "~all"
						b = word[2]
				}
				case "take"
				{
					if word[2] = "off" or word[3] = "off"
					{
						"Please specify which item you'd like to take off."
						return true
					}
					else
						a = word[1]
				}
				case "remove","wear"
				{
					print "Please specify which item you'd like to "; word[1];"."
					return true
				}
				case "get", "step"
				{
					a = word[1]
					if word[2] ~= "~all"
					{
						if (word[2] = "outof")
							b = "out of"
						elseif word[2] = "offof"
							b = "off of"
						else
							b = word[2]
					}
				}
				case else : a = parse$
			print CThe(actor); " can't "; a;
			if b
				print " "; b ;
			" multiple objects like that."
		}

		case 5
		{
			if VerbWord = "give", "offer", "hand", "show"
			{
				print "Not sure what you're referring to--try \""; \
					VerbWord; " (something) \Ito\i (someone)";
	#ifset AMERICAN_ENGLISH
					".\""
	#else
					"\"."
	#endif
			}
			else
			{
!\
			  	print CThe(player); \
					MatchPlural(player, "hasn't", "haven't");
					print " seen any";
#ifclear NEW_STYLE_PRONOUNS
					! If we built an object phrase using
					! OLD_STYLE_PRONOUNS, avoid printing a
					! potential nonsense name:
					if parser_data[PARSER_STATUS] & PRONOUNS_SET
						print "thing like that";
					else
#endif
						print " \""; parse$; "\"";
					print ", nor"; IsorAre(player, true); " "; The(player); \
					" likely to, even if such a thing exists."
\!
			  	print CThe(player); \
					MatchPlural(player, "hasn't", "haven't");
					print " encountered any";
#ifclear NEW_STYLE_PRONOUNS
				if NEW_PARSE & PRONOUNS_SET
					print "thing like that";
				else
#endif
					print " \""; parse$; "\"";
				print ".";
				print "  (If you're sure you need to refer to that, try
					putting it another way.)"
			}
		}
		case 6
			print "That doesn't make any sense."
#ifclear NO_DISAMB_HELP
		case 8
		{
			if disamb_holder is not special
				return false
			SaveOldWord
			print "Which "; parse$ ; " do you mean, ";
			local i
			for (i=1;i<= disamb_holder.disamb_total ;i++ )
			{
				if disamb_holder.disamb_suspects #i.article
				 print "the ";
				print number i; ") "; disamb_holder.disamb_suspects #i.name;
				if i = disamb_holder.disamb_total
					break
				if disamb_holder.disamb_total > 2
					print ",";
				if disamb_holder.disamb_total > 1 and
					i = disamb_holder.disamb_total - 1
					print " or";
				print " ";
			}
			"?"
		}
#endif
		case 9
		{
			local v
!			x = CurrentCommandWords
			if word[1] = "take" and (word[2] = "off" or word[3] = "off")
			{
				v = " off"
			}
#ifclear AIF
			local c
			if verbroutine =&DoGet and (word[1] = "remove" or
				(word[1] = "take" and word[3] = "off")) and
				xobject is living
			{
				verbroutine = &DoTakeOff
				c = true
				if xobject = player
				{
					print "Try \"";parse$;
					if v
						print v;
					" all\" instead."
				}
				else
				{
					print "Please specify which item you'd like to ";parse$;
					if v
						print v;
					"."
				}
				return true
			}
#ifclear USE_CHECKHELD
			if (verbroutine = &DoWear, &DoTakeOff) or c
#else
			if (verbroutine = &DoWear_Checkheld, &DoTakeOff_Checkheld) or c
#endif
			{
				print "Please specify which item you'd like to ";parse$;
!				if player_person ~= 2:  print The(player, true); " ";
!				print "to ";
				if v
					print v;
				"."
				return true
			}
#endif
			if verbroutine = &DoGet and xobject = player and
			not (word[1] = "remove" or
				(word[1] = "take" and word[3] = "off"))
			{
				ParseError(6) ! "That didn't make any sense"
				return true
			}
			print "Nothing to ";
			if verbroutine
			{
				print parse$;
				if v
					print v;
				"."
			}
			else:  print "do."
		}

		case 10
		{
			print CThe(player); \
			MatchPlural(player, "hasn't", "haven't");
			if &obj.long_desc
				" seen ";
			else
				" encountered ";
			if (obj and obj is living and obj.pronoun #2 ~= "it") ! or
!			(parser_data[PARSER_STATUS] & FINDOBJECT_LIVING)
				print "anybody";
			else: print "anything";
			print " like that";
#ifset DEBUG
			if (debug_flags & D_OBJNUM)
				print " ; number obj; ?";
#endif
			print "."
		}

		case 11
		{
			if not ParseError_ObjectIsKnown(obj)
			{
				ParseError(10, obj)
			}
			elseif parent(obj) ~= 0 and FindObject(parent(obj), location) and
				parent(obj) is openable ! and parent(obj) ~= parent(player)
			{
				if CheckReach(parent(obj))
				{
					print CThe(parent(obj)); \
					IsOrAre(parent(obj), true); " closed."
				}
			}
			elseif obj is living
			{
!\
				print CThe(obj); \
					MatchPlural(obj, "isn't", "aren't"); " here."
\!
				print CThe(actor); \
					MatchPlural(actor, "doesn't", "don't"); \
					" see";
#ifset NEW_STYLE_PRONOUNS
				if obj.type = it_object
					print " "; obj.name;
				else
#endif
				if (obj.pronouns #2)
					print " "; obj.pronouns #2;
			  	elseif obj is plural
			  		" them";
			  	elseif obj is female
			  		" her";
			  	else
			  		" him";
#ifset DEBUG
				if (debug_flags & D_OBJNUM)
					print " ["; number obj; "]";
#endif
				print "."
			}
			else
			{
#ifset USE_CHECKHELD
				if verbroutine = &DoDrop_CheckHeld, &DoPutIn_CheckHeld,
					&DoPutonGround_CheckHeld
				{
					ParseError(15, obj)
				}
				else
				{
#endif
					print CThe(actor); \
						MatchPlural(actor, "doesn't", "don't"); \
						" see that"; ! MatchPlural(obj, "that", "those");
#ifset DEBUG
					if (debug_flags & D_OBJNUM)
						print " ["; number obj; "]";
#endif
					print "."
#ifset USE_CHECKHELD
				}
#endif
			}
		}

		case 12
		{
			if not ParseError_ObjectIsKnown(obj)
			{
				ParseError(10, obj)
			}
			else
			{
				print CThe(actor); \
					" can't do that with ";
				if obj ~= actor
					TheOrThat(obj)
				else
					print obj.pronouns #4;
				"."
			}
		}
		case 13
			print "You'll have to be a little more specific."

		case 14
		{
			if not ParseError_ObjectIsKnown(obj)
			{
				ParseError(10, obj)
			}
			elseif xobject is living
			{
				print CThe(xobject); \
					MatchPlural(xobject, "doesn't", "don't"); \
					" have "; TheOrThat(obj); "."
			}
			else
			{
				print CThe(actor); \
					MatchPlural(actor, "doesn't", "don't"); \
					" see "; TheOrThat(obj); " there."
			}
		}

		case 15
		{
			if not ParseError_ObjectIsKnown(obj)
			{
				ParseError(10, obj)
			}
			elseif obj = player
			{
				ParseError(12, obj)
			}
			else
			{
				print CThe(actor); IsorAre(actor); " not
					holding"; MatchPlural(obj, "that", "those"); "."
				AssignPronoun(obj,true)
			}
		}

		case else
		{
			last_actor = 0
			parser_data[PARSER_STATUS] = PARSER_RESET
			return false            ! print the default message
		}

	last_actor = 0
	parser_data[PARSER_STATUS] = PARSER_RESET
	return true                             ! message already printed
}

! Roody's note: This only exists here to have a NewParseError replacement
! routine all set up to be copied and pasted into your ParseError-replacing
! game
replace NewParseError(errornumber,obj)
{
	select errornumber
		case else : return false
	return true
}

!\ Roody's note - BeforeParseError is a routine solely for being replaced.
Use it for any code you want to run before parser error messages are printed.
\!
routine BeforeParseError(errornumber,obj)
{}

! Roody's note: Removes the jump command... just because.
replace Perform(action, obj, xobj, queue, isxverb)
{
	local r
	local objtemp, xobjtemp, verbtemp, actortemp
#ifclear NO_XVERBS
	local restoring
	if verbroutine = &DoRestore:  restoring = true
#endif

#ifset DEBUG
	if debug_flags & D_PARSE
	{
		print "\B[Perform("; number action; ", "; obj.name;
		if (debug_flags & D_OBJNUM) or queue = -1
			print " ["; number obj; "]";
		print ", "; xobj.name;
		if (debug_flags & D_OBJNUM) or queue = -1
			print " ["; number xobj; "]";
		if queue
			print ", "; number queue;
		print ")]\b"
	}
#endif

	if queue
		parser_data[PARSER_STATUS] |= PERFORM_QUEUE

	if not queue and object
		parser_data[LAST_SINGLE_OBJECT] = object
	else
		parser_data[LAST_SINGLE_OBJECT] = 0

	parser_data[VERB_IS_XVERB] = isxverb

	objtemp = object
	xobjtemp = xobject
	verbtemp = verbroutine
	actortemp = actor

	object = obj
	xobject = xobj
	verbroutine = action
	actor = player

#ifset NEW_STYLE_PRONOUNS
	if object.type = it_object or xobject.type = it_object
		ApplyPronouns(obj, xobj)
#endif

	if not (NEW_PARSE & WORDSSAVED_F) and not isxverb
	{
		last_actor = actor
		NEW_PARSE &= ~PRONOUNS_SET
		SetPronouns
		NEW_PARSE |= PRONOUNS_SET
		parser_data[PARSER_STATUS] &= ~PARSER_ACTIVE
		NEW_PARSE = NEW_PARSE | WORDSSAVED_F
		SaveOldWord
#ifset CONTINUE_UNDO
		if continue_undo
			continue_undo++
#endif
	}

#ifclear NO_OBJLIB
	if verbroutine = &DoGo and not object
		SetupDirectionObjects
#endif

	! Itemize each object in a list of multiple objects
	if queue > 0 and object > display
	{
#ifset USE_CHECKHELD
		! Check if an ImplicitTakeForDrop was just done, meaning we
		! need a newline before printing the next "object:"
		if checkheld is workflag
			print ""
		checkheld is not workflag
#endif
		print object.name; ":  ";
	}

	r = BeforeRoutines(queue)
#ifset USE_SCOPE_REACT
	if not r
		r = ScopeReactBefore
#endif
	if not r ! if action not stopped by before routines
	{
		r = call action                 ! object.after and xobject.after
						! run by verbroutine
#ifclear NO_XVERBS
		if restoring
			verbroutine = &DoRestore
#endif
		if r  ! if action was successful, run after routines
		{
			AfterRoutines
#ifset USE_SCOPE_REACT
			ScopeReactAfter
#endif
		}
	}

!:DonePerform area
#ifset USE_CHECKHELD
	ResetCheckHeld
#endif

	last_object = object

	verbroutine = verbtemp
	object = objtemp
	xobject = xobjtemp
	actor = actortemp

	if queue = -1
		last_object = -1
	parser_data[PARSER_STATUS] = PARSER_RESET
	return r
}

routine SetPronouns
{
	AssignPronoun(object)
}

#ifset NEW_STYLE_PRONOUNS
routine ApplyPronouns(obj,xobj)
{
	local number_pronouns = 0
	if object = it_object:		object = it_obj
	elseif object = him_object:	object = him_obj
	elseif object = her_object:	object = her_obj
	elseif object = them_object:	object = them_obj
	if object ~= obj
	{
		replace_pronoun[number_pronouns] = object
		number_pronouns++
	}
	if xobject = it_object:		xobject = it_obj
	elseif xobject = him_object:	xobject = him_obj
	elseif xobject = her_object:	xobject = her_obj
	elseif xobject = them_object:	xobject = them_obj
	if xobject ~= xobj
	{
		replace_pronoun[number_pronouns] = xobject
		number_pronouns++
	}
	if number_pronouns
	{
		NEW_PARSE |= PRONOUNS_SET
		PrintReplacedPronouns(number_pronouns)
	}
}
#endif

! Roody's note: Fixes a print statement, the math for determining rank,
! and allows score/rank for more than just STATUSTYPE 1
replace PrintScore(end_of_game)
{
	if MAX_SCORE
	{
		if end_of_game:  print ""
		print "You ";
		if not end_of_game
			print "have ";
		print "scored a total of ";
		print number score; " out of "; number MAX_SCORE;
		if ranking[0] ~= ""
		{
			print ", giving you the rank of ";

			! A complicated formula, since only
			! integer division is allowed:
			!
			local temp_rank
			temp_rank = (score*MAX_RANK)/MAX_SCORE
			if temp_rank > MAX_RANK
				temp_rank = MAX_RANK
			print ranking[temp_rank];
!\ Changed so that if the score goes over the MAX_SCORE, it continues
to give the highest ranking. \!
		}
		print "."
	}
}

!\ Roody's note: This version of RunScripts allows scripts to be run with a
true/false argument (mainly so LoopScripts can be called with a true
value). \!
replace RunScripts
{

#ifclear NO_SCRIPTS

	local a, obj, step, total, routine, b
	local tempactor, tempverb, tempobject

	for (; a<number_scripts; a++)
	{
		obj = scriptdata[a * 3]         ! this object/character
		step = scriptdata[a * 3 + 1]    ! current step
		total = scriptdata[a * 3 + 2]   ! total steps in script

		if obj and total < 0            ! if skipping this script
			scriptdata[a * 3 + 2] = scriptdata[a * 3 + 2] + 32767

		elseif obj and step < total and step >= 0
		{
			! action
			routine = setscript[a * MAX_SCRIPT_STEPS + step * 2]

			! object being acted upon
			b = setscript[a * MAX_SCRIPT_STEPS + step * 2 + 1]

#ifset DEBUG
			if debug_flags & D_SCRIPTS
			{
				print "[Script for obj. ";
				print number obj; " ("; obj.name; "), step ";
				print number (step + 1); ":  $"; \
				hex routine; ", obj. ";
				print number b; " ("; b.name; ")]"
			}
#endif

			scriptdata[a * 3 + 1] = step + 1
			tempactor = actor
			tempverb = verbroutine
			tempobject = object
			actor = obj
			verbroutine = routine
			object = b
			if not parent(actor).before
			{
				if not actor.before
				{
					if not object.before or object = 1
					{
						call routine(obj, b)
						run actor.after
					}
				}
			}
			if actor in location
#ifset MULTI_PCS
	MakeKnown(actor)
#else
	actor  is known
#endif
			actor = tempactor
			verbroutine = tempverb
			object = tempobject
		}
		elseif step >= 0
		{
			scriptdata[a * 3] = 0   ! clear this object's script
			if a = number_scripts - 1
				number_scripts--
		}
	}

#endif  ! ifclear NO_SCRIPTS

}

! Roody's note: Jump command removed. in superbrief mode, objects with
! short_descs treated like objects without them.
replace ShortDescribe(obj)
{
	local ListContents
#ifset MULTI_PCS
	MakeKnown(obj)
#else
	obj is known
#endif

	if list_nest = 1
		print newline

	AssignPronoun(obj)

#ifset MULTI_PCS
	if not ObjectIsMovedVisited(obj) and &obj.initial_desc
#else
	if obj is not moved and &obj.initial_desc
#endif
	{
		Indent
		run obj.initial_desc
		ListContents = true
	}
	elseif obj is openable
	{
		if obj is open
		{
			if &obj.when_open
			{
				Indent
				run obj.when_open
				ListContents = true
			}
		}
		elseif &obj.when_closed
		{
			Indent
			run obj.when_closed
			ListContents = true
		}
	}

	if (not ListContents)
	{
		if verbosity = 1
			return

		if &obj.short_desc
			Indent

		if not obj.short_desc
			return
	}

!  "ListContents" section
	obj is already_listed
	AssignPronoun(obj)

	list_count = 0

	if children(obj) > 0 and (obj is platform or obj is transparent or
	(obj is container and
	(obj is not openable or (obj is openable and obj is open)))) and
	obj is not quiet
	{
#ifset NEW_DESCRIBEPLACE
		if (FORMAT & DESCFORM_D) and not (FORMAT & LIST_F)
			""
#endif
		list_nest = (LIST_F = (FORMAT & LIST_F))
!		if FORMAT & LIST_F
!			list_nest = 1
!		else
!			list_nest = 0
		WhatsIn(obj)
	}

	! If INDENT_SIZE is 0, formatting may be thrown off when listing
	! the contents of an object:
	if INDENT_SIZE = 0:  need_newline = true

	if need_newline:  print newline
}

! Roody's note: Removes jump command. Only clears/resets speaking global if
! player tries talking to a character.
replace SpeakTo(char)
{
	local a, v, TryOrder, IgnoreResponse, retval, stay, same, different

#ifset USE_CHECKHELD
	if verbroutine = &DoDrop_CheckHeld
		verbroutine = &DoDrop
	elseif verbroutine = &DoPutIn_CheckHeld
		verbroutine = &DoPutIn
	elseif verbroutine = &DoTakeOff_CheckHeld
		verbroutine = &DoTakeOff
	elseif verbroutine = &DoWear_CheckHeld
		verbroutine = &DoWear
#endif

#ifset VERBSTUBS
	if verbroutine = &DoHelpChar and object = player
	{
		verbroutine = &DoHelp
		object = nothing
	}
#endif

	if verbroutine = &DoAgain
	{
		verbroutine = last_verbroutine
		object = previous_object
		xobject = previous_xobject
		RestoreFromOldWord
	}
	else
		SaveOldWord

	NEW_PARSE = NEW_PARSE | WORDSSAVED_F
	parser_data[PARSER_STATUS] &= ~PARSER_ACTIVE

#ifset NEW_STYLE_PRONOUNS
	local objtemp, xobjtemp, actortemp
	objtemp = object
	xobjtemp = xobject
	actortemp = actor

	if object.type = it_object or xobject.type = it_object
		ApplyPronouns(objtemp, xobjtemp)
#endif

#ifset USE_CHECKHELD
	ResetCheckHeld
#endif

#ifset DEBUG
	if debug_flags & D_PARSE
	{
		print "\B[Speakto("; char.name;
		if (debug_flags & D_OBJNUM)
			print " ["; number char; "]";
		print ") verbroutine="; number verbroutine;
		print ", object="; object.name;
		if (debug_flags & D_OBJNUM)
			print " ["; number object; "]";
		print ", xobject="; xobject.name;
		if (debug_flags & D_OBJNUM)
			print " ["; number xobject; "]";
		print "]\b"
	}
#endif

	v = verbroutine
	verbroutine = &SpeakTo
	retval = player.before
	if retval
		a = player
	else
	{
		retval = location.before
		if retval
			a = location
	}
	verbroutine = v

	if not retval
		retval = actor.before

	if retval
	{
#ifset DEBUG
		if debug_flags & D_PARSE
		{
			print "\B["; a.name;
			if debug_flags & D_OBJNUM
				print " ["; number a; "]";
			print ".before returned "; number retval; "]\b"
		}
#endif
		return retval
	}

	if char is not living
	{
		if word[1] = ""
			ParseError(2) ! Please supply a verb.
		else
			ParseError(6)  ! "That doesn't make any sense."
		return
	}

	AssignPronoun(char)

	! Handle player/typist-related ParseError messages:
	if char = player
		Message(&Speakto, 1)    ! "Stop talking to yourself..."
	elseif not ObjectisKnown(object) and not FindObject(object, location)
		ParseError(10, object)
	else
	    stay = true

	if not stay
	{
	   speaking = 0
	   return
	}

	last_actor = actor

	if char is unfriendly
		IgnoreResponse = true
	else
	{
#ifclear NO_OBJLIB
		if verbroutine = &DoGo and not object
			SetupDirectionObjects
#endif
		! In the event of:  >CHARACTER, GO NORTH.  GET THE THING.  GO WEST., etc.
		if not FindObject(char, location)
		{
			speaking = char
			run char.order_response
			return true
		}

		same = (char = speaking)

		select verbroutine
			case 0                  ! Just the character name is given,
						! so just "X is listening."
			{
				if not char.order_response
					Message(&Speakto, 2, char)
				retval = true
			}

#ifclear NO_VERBS
			case &DoHello           ! Note the ampersands ('&')--or else
			{                       ! the routines themselves would run
				if not char.order_response
				{
					if char is not unfriendly
					{
						! "X nods hello."
						Message(&Speakto, 3, char)
						retval = true
					}
					else
						IgnoreResponse = true
				}
				else
					retval = true
			}

			case &DoAskQuestion
				return Perform(&DoAsk, char, object)

			case &DoTalk
			{
				if xobject
					ParseError(6)
				else
					return Perform(&DoAsk, char, object)
			}

			case &DoTell
			{
				if object = player
					return Perform(&DoAsk, char, xobject)
				else
					TryOrder = true
			}
#endif  ! ifclear NO_VERBS

			case else
			{

	! If the character can respond to a request, this should be dealt with by
	! an order_response property routine; order_response--if it exists--should
	! return false if there is no response for the given verbroutine

				TryOrder = true
			}
	}

	if TryOrder
	{
		if (not char.order_response)
			IgnoreResponse = true
		else
			retval = true
	}

	different = (speaking ~= char)

!	This same/different local variable stuff allows for certain
!	orders to end conversations. If your order_response code clears
!	the speaking global, this code prevents it being reset.

	if retval and not (same and different)
		speaking = char

	if IgnoreResponse
	{
		if not char.ignore_response
			Message(&Speakto, 4, char)      ! "X ignores you."
		speaking = 0  ! clear the speaking global
	}
	else
		SetPronouns
	return retval
}

#ifset USE_CHECKHELD
! Roody's note: new VerbHeldMode to work with checkheld wear/remove system
replace VerbHeldMode(w)
{
	if w = "get", "take", "grab", "pick" ! , "remove"
	{
		return -1			! explicitly notheld
	}
	elseif w = "drop", "put", "leave", "place", "throw", "insert",
		"give", "offer" , "wear" ! , "remove"
	{
		return 1			! explicitly held
	}
	return 0
}
#endif  ! ifset USE_CHECKHELD

!----------------------------------------------------------------------------
!* "SHOW_COMMANDS"
!----------------------------------------------------------------------------
!\
Roody's note: With the SHOW_COMMANDS flag on, lines with multiple commands get
the current command printed at each step.  Also, >UNDO prints the action being
undone.  Now, since Hugo's parser uses a combination of synonyms and removals,
printing the command is an imperfect science.  Any author who turns this on has
to be okay with the fact that the output will look pretty dumb from time to
time.  Here are some examples of things that wouldn't print right:

"get apple and orange" will become "get apple, orange"
"get all from box but rat" will become "get all from box except rat"

Well, I'm sure there are plenty of other examples but that's all I can think
of right now.

Anyhow, the following commands are called by the player_character object
in the "replace objlib.h code" section.
\!

#ifset SHOW_COMMANDS


#endif ! ifset SHOW_COMMANDS
!----------------------------------------------------------------------------
!* "NEW_STYLE_PRONOUNS"
!----------------------------------------------------------------------------
!\
Roody's note: hugolib.h has a pronoun system that is automatically turned off
(due to Kent being unhappy with certain behaviors). Here in Roody-land, though,
we like to live on the EDGE and dance with danger.

It is different from the normal system as it uses additional objects that
redirect actions to the applicable pronoun global variable (it_obj, him_obj,
etc.).

Originally, I thought that one of the perks of this system was that it didn't
always print "(Assuming you mean the so-and-so)" text (which can be nice in a
game with more behind-the-scenes trickery to hide), but that turned out to be
due to some mishandled code.

So, the nicest thing about this system is that it avoids all of the word
array manipulation the regular system uses, a step-saving method that might
be especially useful in games where game state memory is tight.

The main thing to watch out if you use the new style pronouns is that
verb definitions that use object-specific grammar tokens will not allow
referring to those objects with pronouns.

So, if you use the new style pronouns, you should replace those grammar tokens
with routine grammar tokens, like:

The box1-object-specific verb definition:
verb "test"
*  (box1)                   DoTest

would be replaced with:
verb "test"
*  (TestRoutine)                   DoTest

with this accompanying grammar routine:
routine TestRoutine(obj)
{
	if CheckObject(obj, box1)
		return true
}

\!

#ifset NEW_STYLE_PRONOUNS
! If NEW_STYLE_PRONOUNS is not set, the Parse routine will actually change
! the input line by inserting the object name.  This is a different
! approach using actual stand-in objects.

object it_object "that" ! to match "You don't see that." ParseError message.
{
	is known, hidden
	nouns "it"
	type it_object
	misc { return it_obj }
	parse_rank 1000
	found_in
	{
		local obj
		obj = self.misc
		if FindObject(obj, location)
			return location ! parent(obj)
		else
			return false
	}
}

it_object him_object "him"
{
	is living
	nouns "him"
	misc { return him_obj }
}

it_object her_object "her"
{
	is living, female
	nouns "her"
	misc { return her_obj }
}

it_object them_object "them"
{
	is plural
	nouns "them"
	misc { return them_obj }
}

!\ ObjectisLight replacement - Ran into a problem if Hugo thinks the it_object
  is providing light, so this extra code redirects ObjectisLight to the
  applicable object. \!

replace ObjectisLight(obj)
{
	local nextobj

	if obj = it_object:		obj = it_obj
	elseif obj = him_object:	obj = him_obj
	elseif obj = her_object:	obj = her_obj
	elseif obj = them_object:	obj = them_obj

	if obj is light
	{
		light_source = obj
		return light_source
	}
	if obj is not container or
		(obj is container and obj is not openable) or
		(obj is container and obj is openable and
			(obj is open or obj is transparent))
	{
		for nextobj in obj
		{
			if ObjectisLight(nextobj)
				return light_source
		}
	}
}

#endif	! NEW_STYLE_PRONOUNS

!----------------------------------------------------------------------------
!* "NEW_EMPTY" system
!----------------------------------------------------------------------------
!\ Roody's note- I found the pre-existing DoEmpty grammar and verb routines
a bit troublesome, so I designed this replacement system. To use, #set NEW_EMPTY
before any grammar is included.

Then give your containers and platforms the following properties to inherit
various behaviors:
	#inherits held-to-player ! requires the object to be held and empties
                            ! contents to player
	#inherits unheld-to-player ! requires object to not be held and empties
	                           ! contents to player
	#inherits held-to-ground ! requires object to be held and empties to ground
	#inherits no-empty    ! platform or container does not allow emptying

	Objects that don't require being held and dump to the ground should work fine
	with the default code.
	\!

#ifset NEW_EMPTY

#ifclear ROUTINE_GRAMMAR_HELPER
#set ROUTINE_GRAMMAR_HELPER
#endif

property empty_type
constant NO_EMPTY_T 16 ! new grammar helper token for "no emptying" containers

class unheld_to_player
{
	empty_type NOTHELD_T
!	parse_rank
!	{
!		if GrandParent(self) = location
!			return 2
!		else
!			return -1
!	}
	before
	{
		object DoEmpty
		{
			local a, b, obj, xobj
			local thisobj, nextobj

			CalculateHolding(object)

			if object is container, openable, not open
			{
				VMessage(&DoEmpty, 1)           ! "It's closed."
				return true
			}

			if object is not container, platform
			{
!				ParseError(12, object)  ! so the message isn't printed twice
                                    ! (even though this class should never
                                    ! be applied to a
                                    ! non-container/platform)
				return false
			}

			if not children(object)
			{
				VMessage(&DoEmpty, 2)           ! "It's already empty."
				return true
			}

			thisobj = child(object)
			while thisobj
			{
				nextobj = sibling(thisobj)

				print thisobj.name; ":  ";

				if thisobj is static
					VMessage(&DoEmpty, 3)    ! "You can't move that."
				else
				{
					a = player.holding
					b = thisobj
					obj = object
					xobj = xobject

					if Perform(&DoGet, b)
						player.holding = a + b.size
					else
						player.holding = a
					object = obj
					xobject = xobj

					if object.#holding
					{
						if b not in object
							object.holding = object.holding - b.size
					}
				}

				thisobj = nextobj
			}

			run object.after
			return true
		}
	}
}

class held_to_player "(held_to_player)"
{
	empty_type HELD_T
!	parse_rank
!	{
!		if GrandParent(self) = location
!			return 2
!		else
!			return -1
!	}
	inherits unheld_to_player
}

class held_to_ground
{
	empty_type HELD_T
}

class no_empty
{
	empty_type NO_EMPTY_T
}

routine CheckEmpty(obj)
{
	if obj.empty_type
	{
		TOKEN = obj.empty_type
		if not CheckObject(obj)
			return false
		elseif obj.empty_type = NO_EMPTY_T
		{
			ParseError(12, obj)
			return false
		}
		return true
	}
	elseif not CheckObject(obj)
		return false
	return true
}
#endif NEW_EMPTY

!----------------------------------------------------------------------------
!* ROUTINE GRAMMAR HELPER
!----------------------------------------------------------------------------
!\ Roody's note- Using routines as grammar tokens is one of my favorite unsung
features of Hugo. It's a nice way to do things you can't normally do with
regular grammar tokens, like only accept objects of a certain type.

Since the objects sent to grammar token routines are the same that would be
accepted by the "anything" token, authors might spend a fair amount of time
weeding out unwanted objects.

To this end, the code below attempts to make it easy to disallow various types
of objects, giving responses consistent to normally-called ParseError messages.
Its mask usage even allows objects that require multiple grammar tokens
or attributes
\!

#ifset ROUTINE_GRAMMAR_HELPER
global TOKEN

enumerate step *2
{
	TOKEN_RESET,		! 0, for reset
	ANYTHING_T = 1,   ! accept anything objects
	HELD_T,	     	! force held
	NOTHELD_T,	! force unheld
	ATTRIBUTE_T,	! force attribute
}

!  sample grammar routine:
!routine SampleRoutineGrammar(obj)
!{
!	TOKEN = ATTRIBUTE_T | HELD_T
!	return CheckObject(obj, 0, openable)
!}

!\ set goal_obj if the routine requires a special object
	(goal_obj is the object you would have used if you were doing
	object-as-grammar-token)
 att1 through att3 can be used for necessary attributes\!

routine CheckObject(obj,goal_obj, att1, att2, att3)
{
	local ret
	if not FindObject(obj,location) and not (TOKEN & ANYTHING_T)
		ParseError(11,obj)  ! You don't see that.
	elseif not Contains(player, obj) and (TOKEN & HELD_T)
		ParseError(15, obj) ! You're not holding that.
	elseif Contains(player, obj) and (TOKEN & NOTHELD_T)
		ParseError(11, obj)  ! You don't see that.
#ifset NEW_STYLE_PRONOUNS
	elseif ((obj = it_object and it_obj = goal_obj) or obj = goal_obj)
		ret = true
#else
	elseif (obj = goal_obj)
		ret = true
#endif
	elseif goal_obj
		ParseError(12,obj)  ! You can't do that with that.
	elseif (TOKEN & ATTRIBUTE_T)
	{
		if (att1 and obj is not att1) or (att2 and obj is not att2) or
		(att3 and obj is not att3)
			ParseError(12, obj) ! "you can't do that with that."
		else
			ret = true
	}
	else
		ret = true
	TOKEN = TOKEN_RESET
	return ret
}
#endif  ! ifset ROUTINE_GRAMMAR_HELPER

!----------------------------------------------------------------------------
!* NEW PRINTSTATUSLINE
!----------------------------------------------------------------------------
!\
Like the other "settings" stuff further down, Roodylib's PrintStatusLine
uses objects to keep track of its settings. The intended benefits of using it
are:
- built in support for varying screen widths and screen size changes
- "out-of-the-box" compatibility with several status-line-changing extensions,
  like newconverse and newautomap.
- makes it easy to have non-standard statuses that opt for player state
  instead of turns/score
\!
#ifclear NO_STRING_ARRAYS
#ifclear NO_FANCY_STUFF

! PrintStatusLine object properties
property find_height alias u_to ! property that points to routines for
                                !   determining status window height
property draw_window alias e_to ! routines with instructions for drawing the
                                !   status window
property bottom_justified alias d_to ! have this return true for status windows
                                    !   where regular status information shares
												!  a window with other information and you
												!	 want the regular status information to
												!	 be printed at the bottom of the window
!property terp_type alias w_to   ! this property gets set to the current
!                                !   interpreter type automatically
property status_override alias nw_to ! status_override - normally, the status
                                     !   window object with the highest
                                     !   find_height number gets drawn (so have
												!	 those properties return 0 when not in
                                    !    use), but sometimes more than one
												!	 extension *could* be drawn, so we use
                                    !   status_override to have one override the
												!	 other (have it return true to initiate
                                    !    such override)
property status_top alias sw_to     ! top line number of the status window, if
                                    ! applicable
property chosen_window alias nw_to  ! set to the window instructions object
                                    !  whose draw_window property will be
												!	executed. authors can ignore this.

! "terp_type" values  2, 4, 8
enumerate step * 2
{
    NORMAL_TERP = 2, GLK_TERP, SIMPLE_TERP
}

!\ a PrintStatusLine object in which we will put our instruction objects in (by
inclusion of extensions and what not. PrintStatusLine will call this directly).
\!
object printstatuslib
{
	in settings
	type settings
	find_height
	{
		local highest, i, a
		if children(self) ~= 1
		{
			i = child(self)
			while i
			{
				a = i.find_height
				if i.status_override or a > highest
				{
					self.chosen_window = i
					highest = a
					if i.status_override
						break
				}
				i = younger(i)
			}
		}
		else
		{
			i = child(self)
			self.chosen_window = i
			highest = i.find_height
		}

		self.status_top = (self.chosen_window).status_top
		return highest
	}
	draw_window
	{
		run (self.chosen_window).draw_window
	}
	chosen_window 0
!	terp_type NORMAL_TERP
	bottom_justified 1
	status_top 0
}

! Roody's note: This object checks for a big change in the status line size
! that'd require the window to be cleared.
object statuslinelib "statuslinelib"
{
	in init_instructions
	type settings
#ifclear NO_FANCY_STUFF
	save_info
	{
		if display.statusline_height > 2 and not system(61)
		{
			SaveWordSetting(display.statusline_height)
			return true
		}
		return false
	}
#endif
	execute
	{
		if word[LAST_TURN] = "undo","restore"
		{
#ifclear NO_FANCY_STUFF
			local a
			a = CheckWordSetting(self.name)
			display.statusline_height = word[(a+1)]
#endif
		}
!		if word[LAST_TURN] ~= "undo"
!		{
!	! set the "terp_type" value
!			if IsGlk
!				printstatuslib.terp_type = GLK_TERP
!			elseif system(61) ! minimal port
!				printstatuslib.terp_type = SIMPLE_TERP
!			else
!				printstatuslib.terp_type = NORMAL_TERP
!		}
	}
}

replace PrintStatusline
{
	local newstatusheight, window_top
#ifset NO_ACCESSIBILITY
	local cheap
#endif

	! have to redraw the "cheap" statusline each time just in case
	! the screensize has changed
!	if cheap = 1 and printstatuslib.terp_type ~= SIMPLE_TERP
!	{
!		CenterTitle(CheapTitle,0,1,1)
!		display.needs_repaint = false
!		return
!	}
!	else
	if cheap = 1
		return

	! figure out the size our window will be
	newstatusheight = printstatuslib.find_height
	window_top = printstatuslib.status_top

	if not window_top
		window_top = 1

	! clear/remove the window if the status window height has changed
	if (newstatusheight < display.statusline_height) and not system(61) and
	display.statusline_height
	{
		window 1, window_top, display.screenwidth,
		(display.statusline_height + window_top - 1)
		{cls} ! clear whatever's there
		window 0
	}

	display.statusline_height = newstatusheight

	Font(BOLD_OFF | ITALIC_OFF | UNDERLINE_OFF | PROP_OFF)
	if not (TERP & NORMAL_TERP)
	{
		window display.statusline_height
		{
			if not (TERP & SIMPLE_TERP)
			{
				cls
				locate 1,1
			}
			run printstatuslib.draw_window
		}
	}
	else
	{
		window 1, window_top, display.screenwidth,
		(display.statusline_height + window_top - 1)
		{
			color SL_TEXTCOLOR, SL_BGCOLOR
			cls
			locate 1,1
			run printstatuslib.draw_window
		}
	}
	color TEXTCOLOR, BGCOLOR, INPUTCOLOR
	Font(DEFAULT_FONT)
}

property counter_length alias se_to

! Here is an example status window instruction object. It and its routines
! draw a regular status window
object statuswindow
{
	in printstatuslib
	find_height
	{
		return (call &FindStatusHeight)
	}
	draw_window
	{
		return (call &WriteStatus)
	}
	counter_length 0
}
!\ Note: These properties *could* just say "return FindStatus". I just used
the above syntax to give a clue as to how one would change a value "mid-game".
If you want statuswindow.find_height to point to *another* routine, you could
have a line like this:
	statuswindow.find_height = call &FindNewStatusHeight  \!


! routine for finding the height of the regular status info
routine FindStatusHeight
{
	local len1, b
	if not light_source
		len1 = 11
	else
		len1 = string(_temp_string,location.name)
	if STATUSTYPE
	{
		b = GetStatusLength(len1)
		statuswindow.counter_length = b
	}
	else
	{
		b = 1
		while display.screenwidth < (b*len1)
		{
			b++
		}
		return b
	}
	if (b + len1 + 4)<display.screenwidth ! let's force a 4 character gap between
	{                              ! the two fields
		return 1
	}
	else
		return 2
}

! Roody's note: Replace this if you want to use the top right area
! for something else ("HUNGRY", "TIRED", or whatever)
routine STATUSTYPE4
{}

! routine for drawing the regular status
routine WriteStatus
{
	local row,a
	if printstatuslib.bottom_justified and
		not (TERP & SIMPLE_TERP)
	{
		if display.statusline_height > 1
			row = display.statusline_height - 1
		else
			row = display.statusline_height
		locate 1, row
	}
	else
		row = 1
	RlibMessage(&WriteStatus,1,location) ! "in the dark" or location name
	if display.statusline_height = 1 and STATUSTYPE
	{
		a = display.linelength
		print to (a - \
			( statuswindow.counter_length + \
			(TERP = SIMPLE_TERP)* 2 - \
			(TERP = GLK_TERP)));
	}
	elseif STATUSTYPE and display.statusline_height = 2
	{
		if not (TERP & SIMPLE_TERP)
			locate 1, ++row
		else
			""
		print "\_";
	}
	PrintStatusType
}
#endif  !ifclear NO_FANCY_STUFF

routine CountDigits(num)
{
	local sum
	do
	{
		sum++
		num /= 10
	}
	while num
	return sum
}

routine GetStatusLength(len)
{
	if STATUSTYPE = 1,INFOCOM_STYLE, 9
	{
		local a,b, sum
		a = CountDigits(score)
		b = CountDigits(counter)
		if STATUSTYPE = 1
		{
			sum = a + b + 4
		}
		else
		{
			sum = a + b + 17
			if (STATUSTYPE & INFOCOM_STYLE) and
				((len + sum + 6) > display.screenwidth)
			{
				if (sum - 8 + len) < display.screenwidth
				{
					sum = a + b + 9
				}
			}
		}
	}
	elseif STATUSTYPE = 2,16, 18
	{
		if (STATUSTYPE & MILITARY_TIME)
			sum = 6
		else
		{
			local num
			num =  counter / 60
			while num > 12
			{
				num -= 12
			}
			if not num or num > 9
				sum = 11
			else
				sum = 10
		}
	}
	elseif STATUSTYPE = 4
	{
		text to _temp_string
		StatusType4
		text to 0
		sum = StringLength(_temp_string) + 1
	}
	return sum
}

routine PrintStatusType
{
	if (STATUSTYPE & INFOCOM_STYLE)
	{
		if (STATUSTYPE & INFOCOM_STYLE) and
			((display.linelength - display.cursor_column) <  17)
		{
				print "S: "; number score; "\_ "; "M: "; number counter;
				return
		}
		print "Score: "; number score; "\_ "; "Moves: "; number counter;
	}
	elseif (STATUSTYPE & SCORE_MOVES)
			print number score; " / "; number counter;
	elseif (STATUSTYPE & TIME_STATUS) or (STATUSTYPE & MILITARY_TIME) or
		STATUSTYPE = 2
	{
		print HoursMinutes(counter);
	}
	elseif (STATUSTYPE = 4)
		StatusType4 ! routine for configurable statusline
}

#ifset NO_FANCY_STUFF
replace PrintStatusline
{
	local a
!	g = isGlk
!	simple = (not g and system(61))
	if not display.statusline_height
		display.statusline_height = 1

	Font(BOLD_OFF | ITALIC_OFF | UNDERLINE_OFF | PROP_OFF)
	color SL_TEXTCOLOR, SL_BGCOLOR
	window display.statusline_height
	{
		if not (TERP & SIMPLE_TERP) ! simple
		{
			cls
			locate 1, 1
		}
		print "\_";
		if not light_source
			print "In the dark";
		else
			print location.name;
		a = GetStatusLength(display.cursor_column)
		if (a + display.cursor_column + 4) < display.linelength
		{
			print to (display.linelength - \
			( a + \
			((TERP = SIMPLE_TERP)* 2) - (TERP = GLK_TERP)));
			PrintStatusType
		}
	}
	color TEXTCOLOR, BGCOLOR
	Font(DEFAULT_FONT)
}

! Roody's note: Replace this if you want to use the top right area
! for something else ("HUNGRY", "TIRED", or whatever)
routine STATUSTYPE4
{}

#endif ! NO_FANCY_STUFF
#endif ! NO_STRING_ARRAYS
!----------------------------------------------------------------------------
!* "SETTINGS" CLASS AND ROUTINES
!----------------------------------------------------------------------------

! A new "settings" class and some new properties to use for library extension
! settings (so I don't have to keep track of which individual library uses
! what aliased property for what.

class settings
{}

property save_info

!\ A couple routines used by routines like DoRestore and DoRestart
to save certain game state things. \!

routine SaveSettings(w)
{
	word[LAST_TURN] = w
#ifclear NO_FANCY_STUFF
	local i,n = 30
	for i in init_instructions
	{
		if i.save_info
		{
			n = SaveWordSetting(i.name, n)
			n--
		}
	}
#endif
}

routine SaveWordSetting(w,start_val)
{
	local i
	if not start_val
		start_val = 30
	for (i=start_val ; i>0 ; i-- )
	{
		if word[i] = "" ! found an open slot
			word[i] = w  ! add word to word array
		if word[i] = w ! word is already in word array
			return i
	}
	return false ! didn't find anything

}

routine LoadSettings
{
	local i, n, v, s
	for i in init_instructions
	{
		v = CheckWordSetting(i.name, s)
		if v
		{
			run i.execute
			s = v - 1
		}
	}
	n = LAST_TURN
	while word[n] ~= ""
	{
		word[n--] = ""
	}
}

routine CheckWordSetting(w,start_val)
{
	local i
	if not start_val
		start_val = 30
	for (i=start_val ; i>0 ; i-- )
	{
		if word[i] = ""
			return false
		elseif word[i] = w
			return i
	}
	return false ! didn't find anything
}

!----------------------------------------------------------------------------
!* INIT / MAIN INSTRUCTION OBJECTS
!----------------------------------------------------------------------------

property execute alias misc
property did_print alias extra_scenery

routine Init_Calls
{
	local i
	for i in init_instructions
	{
		run i.execute
		if i.did_print
			InitScreen
	}
	ClearWordArray
}

object init_instructions
{
	in settings
}

object roodylib "roodylib"
{
	in init_instructions
	type settings
#ifclear NO_FANCY_STUFF
	save_info
	{
		select verbosity
			case 0 : SaveWordsetting("brief")
			case 1 : SaveWordSetting("superbrief")
			case 2 : SaveWordSetting("verbose")
		return true
	}
#endif
	execute
	{
		if word[LAST_TURN] ~= "undo"
		{
			if word[LAST_TURN] ~= "restore" ! new game
			{
				CalculateHolding(player)
#ifset USE_PLURAL_OBJECTS
				InitPluralObjects
#endif
#ifset NEW_STYLE_PRONOUNS
				! Only need to set up all attributes once, so that a given
				! pronoun will satisfy any grammatical attribute requirements
				if it_object is not 127
				{
					local i
					for (i=0; i<128; i++)
					{
						it_object is i
						him_object is i
						her_object is i
						them_object is i
					}
				}
#endif
			}
#ifclear NO_FANCY_STUFF
			local a
			a = CheckWordSetting("roodylib")
			select word[(a+1)]
				case "brief" : verbosity = 0
				case "superbrief" : verbosity = 1
				case "verbose" : verbosity = 2
#endif
#ifset USE_SCOPE_REACT
			OrganizeScopeObjects
#endif
			if system(61)
			{
				if IsGLK
					TERP = GLK_TERP
				else
					TERP = SIMPLE_TERP
				hugor is not switchedon
			}
			else
			{
				local hugor_check
				TERP = NORMAL_TERP
				readfile "HrCheck"
				{
					hugor_check = readval
				}
				if hugor_check
					hugor is switchedon
				else
					hugor is not switchedon
				if hugor is switchedon
				{
					ExecOpcode(getversion)
					ExecOpcode(get_os)
					ExecOpcode(full_opacity)
					self.did_print = 1
				}
			}
		}
	}
	did_print 0
}

#ifset HUGOFIX
! Roody's note: Made a HugoFix object so that all monitor commands remain
! active after a restart
object hugofixlib "hugofixlib"
{
	in init_instructions
	type settings
	did_print true
#ifclear NO_FANCY_STUFF
	save_info
	{
		if debug_flags = 0 ! if absolutely no debug flags are turn on,
			return          !  just return right away
		local b
		if debug_flags & D_FUSES
			b = SaveWordSetting("fuses",b)
		if debug_flags & D_OBJNUM
			b = SaveWordSetting("objnum",b)
		if debug_flags & D_PARSE
			b = SaveWordSetting("parse",b)
		if debug_flags & D_SCRIPTS
			b = SaveWordSetting("scripts",b)
		if debug_flags & D_FINDOBJECT
			b = SaveWordSetting("findobject",b)
		if debug_flags & D_PARSE_RANK
			b = SaveWordSetting("parserank",b)
		if debug_flags & D_PLAYBACK_HELPER
			b = SaveWordSetting("playbackhelper",b)
		if debug_flags & D_OPCODE_MONITOR
			b = SaveWordSetting("opcodemonitor",b)
		if b
			return true
		else
			return false
	}
#endif
	execute
	{
		if word[LAST_TURN] ~= "undo","restore"
		{
#ifclear NO_FANCY_STUFF
				local a,b
				a = CheckWordSetting(self.name)
				while true
				{
					select word[(a+1)]
						case "fuses" : b = D_FUSES
						case "objnum" : b = D_OBJNUM
						case "parse" : b = D_PARSE
						case "scripts" : b = D_SCRIPTS
						case "findobject" : b = D_FINDOBJECT
						case "parserank" : b = D_PARSE_RANK
						case "playbackhelper" : b = D_PLAYBACK_HELPER
						case "opcodemonitor" : b = D_OPCODE_MONITOR
						case else : break
					debug_flags = debug_flags | b
					a++
				}
#endif
				OrganizeTree
				HugoFixInit
		}
	}
}
#endif

!\ Roody's note: Main_Calls is called last in Roodylib's shell's main
routines. This is nice for game system messages, like score notifications or
footnotes or what have you.\!

routine Main_Calls
{
	local i
	i = child(main_instructions)
	while i
	{
		run i.execute
		i = younger(i)
	}
}

object main_instructions
{
	in settings
}

!----------------------------------------------------------------------------
!* INTERPRETER THINGY
!----------------------------------------------------------------------------
global TERP

!routine HugorCheck()
!{
!	readfile "HrCheck"
!	{
!		return readval = 16962
!	}
!}

property version alias n_to
property os alias ne_to

object hugor "Hugor interpreter monitor"
{
	os 0
	version 0 0 0
	type settings
}

property opcode_results
property opcode_value alias size

class opcode
{
	type opcode
	execute 0
	opcode_value 0
}

opcode getversion "hugor version opcode"
{
	in hugor
	opcode_value 100
	save_info
	{
		hugor.version = readval
		hugor.version #2 = readval
		hugor.version #3 = readval
	}
}

opcode get_os "hugor OS opcode"
{
	nearby
	opcode_value 200
	save_info
		hugor.os = readval
}

opcode op_abort "hugor op abort"
{
	nearby
	opcode_value 300
}

property block alias n_to ! (true to stop action, false: code continues while
                          !  running)
property duration alias ne_to ! in milliseconds
property start_opacity alias e_to ! -1 -> 255 (-1 to keep at current opacity)
property end_opacity alias se_to ! 0... 255

opcode fade_screen "hugor fade screen opcode"
{
	nearby
	opcode_value 400
	block 1
	duration 0
	start_opacity -9999
	end_opacity -9999
	execute
	{
		! This just checks to make sure that somebody didn't accidentally
		! run ExecOpcode on this class object as it would hide all text on
		! the screen
		if self = fade_screen
		{
#ifset DEBUG
			if debug_flags & D_OPCODE_MONITOR
			{
				print "[Do not execute opcode
				\""; self.name;"\" itself. Create a fade_screen
				object with timing and fade values you want. Fade canceled.]"
			}
#endif
			return
		}
		local start_alpha
		start_alpha = self.start_opacity
		if self.start_opacity < 0
			start_alpha = -9999
		writeval self.duration , start_alpha, self.end_opacity, self.block
	}
}

fade_screen full_opacity "restore full opacity opcode"
{
	in fade_screen
	block 1
	duration 1
	start_opacity 255
	end_opacity 255
}

opcode open_URL "hugor open URL opcode"
{
	in hugor
	opcode_value 500
}

opcode fullscreen "hugor fullscreen opcode"
{
	nearby
	opcode_value 600
	execute
		writeval 1
}

opcode windowed "hugor windowed opcode"
{
	nearby
	opcode_value 600
	execute
		writeval 0
}

opcode clipboard "hugor clipboard opcode"
{
	nearby
	opcode_value 700
}

opcode is_music_playing "hugor music detector"
{
	nearby
	opcode_value 800
	save_info
	{
		local ret
		ret = readval
#ifset _RESOURCE_H
		if audio.current_music and not ret
		{
			audio.current_music = 0
			audio.current_music_resfile = 0
			audio.current_music_looping = 0
		}
#endif
		return ret
	}
}

opcode is_sample_playing "hugor sample detector"
{
	nearby
	opcode_value 900
	save_info
	{
		local ret
		ret = readval
#ifset _RESOURCE_H
		if audio.current_sound and not ret
		{
			audio.current_sound = 0
			audio.current_sound_resfile = 0
			audio.current_sound_looping = 0
		}
#endif
		return ret
	}
}

routine ExecOpcode(op_file,str)
{
	if op_file.type ~= opcode
	{
#ifset DEBUG
		if debug_flags & D_OPCODE_MONITOR
		{
			print "[ExecOpcode run with non-opcode type object
			\""; op_file.name;"\"]"
		}
#endif
		return
	}
	writefile "HrCtlAPI"
	{
		writeval op_file.opcode_value
		if str
			writeval str
		if &op_file.execute
			run op_file.execute
	}
	local failure, result
	readfile "HrCtlAPI"
	{
		failure = readval
		if failure
		{
#ifset DEBUG
			if debug_flags & D_OPCODE_MONITOR
			{
				select failure
					case 10 : print "[Incorrect number of parameters for \
					opcode "; op_file.name; " ["; number op_file;"]"
					case 20 : print "[Hugor returned unexpected byte count \
					for opcode "; op_file.name; " ["; number op_file;"]"
					case 30
					{
						local num, i
						num = readval
						print "[unrecognized opcode "; number num ; \
						" attempted";
						num = readval
						if num
						{
							" with ";
							print number num; " argument";
							if num > 1
								print "s";
							print " (";
							for (i=1;i<=num ;i++ )
							{
								failure = readval
								print number failure;
								if failure < num
									print ",";
							}
							print ")";
						}
						print "]"
					}
			}
#endif
			return
		}
#ifset DEBUG
		if debug_flags & D_OPCODE_MONITOR
			print "[\""; op_file.name;"\" success.]"
#endif
		if &op_file.save_info
			result = op_file.save_info
	}
	return result
}

!----------------------------------------------------------------------------
!* NEW MENU
!----------------------------------------------------------------------------

#ifset USE_NEWMENU
object menulib "menu"
{
	type settings
	in init_instructions
	execute
	{
		local i

		if word[LAST_TURN] ~= "undo","restore"
		{
			for (i=(menu_category + 1);i<=objects ;i++ )
			{
				if i.type = menu_category and parent(i) = nothing and
				i ~= menu_category
					move i to menu_pages
			}
		}
	}
}

! menu_category properties
property title_gap alias initial_desc
property options_gap alias reach
property page_text_color alias holding
property page_bg_color alias exclude_from_all
property title_color alias list_contents
property title_bg  alias capacity
property menu_link alias door_to

! option/hint_option properties

property menu_text alias long_desc
property hint1 alias n_to
property hint2 alias ne_to
property hint3 alias e_to
property hint4 alias se_to
property hint5 alias s_to
property hint6 alias sw_to
property hint7 alias w_to
property hint8 alias nw_to
property hint9 alias u_to
property hint10 alias d_to
property alt_title alias misc
property alt_name alias misc ! just because I think I'm going to forget the
                             ! name of the property from time to time

property option_available alias in_to
property hints_available alias in_to
property hints_revealed alias out_to

object menu_pages
{}

class menu_category
{
	type menu_category
	title_gap  0 ! lines between menu title and "[N]ext key"
	options_gap 1 ! lines between "[N]ext key" and menu options
	option_available true
	page_bg_color 0
	page_text_color 0
	title_color 0
	title_bg 0
}

class option
{
	type option
	option_available true
}

option hint_option "(hint_option)"
{
	type hint_option
	menu_text
		Help_Hints(self)
	hints_available 1
	hints_revealed 0
}

routine MakeMenu(menu_title, width, recurse)
{
	local category, old_category, testlink
#ifset NO_ACCESSIBILITY
	local cheap
#endif

	if not menu_title
		menu_title = child(menu_pages) ! main_menu

	if not recurse
	{
		if verbroutine = &EndGame
			""
		if not (system(61) or cheap)
			color BGCOLOR, BGCOLOR
		RlibMessage(&MakeMenu,1,menu_title) ! "Opening menu..."
		""
		color TEXTCOLOR, BGCOLOR, INPUTCOLOR
	}

   while true
	{
		if not (cheap or system(61))
			SetPageColors(menu_title)

		if not (cheap or (TERP & SIMPLE_TERP))
		{
			window 0
			cls
		}
		category = Menu(menu_title, width, old_category)
		old_category = category
		testlink = category.menu_link
		if testlink
			category = testlink
!		old_category = category.menu_link
!		if old_category
!			category = old_category
!		else
!			old_category = category
		if category
		{
			if category.type = menu_category
				MakeMenu(category,width,(recurse + 1))
			else
			{
				if not (cheap or system(61))
					color MENU_BGCOLOR, MENU_BGCOLOR
				if not cheap
					RlibMessage(&MakeMenu,2,category.name)
				if not (cheap or system(61))
				{
					SetPageColors(category)
					color MENU_TEXTCOLOR, MENU_BGCOLOR
				}
				do
				{
					if display.needs_repaint and not cheap
					{
						color MENU_BGCOLOR, MENU_BGCOLOR
						RlibMessage(&MakeMenu,3) ! "[WINDOW RESIZED]"
						""
						RlibMessage(&MakeMenu,2,category.name)
						color TEXTCOLOR, BGCOLOR, INPUTCOLOR
					}
					display.needs_repaint = false
					color TEXTCOLOR, BGCOLOR, INPUTCOLOR
					if not (cheap or (TERP & SIMPLE_TERP))! simple_port
					{
						if category.alt_title
							CenterTitle(category.alt_title)
						else
							CenterTitle(category.name)
					}
					else
					{
						Indent
						print "\_  ";
						Font(BOLD_ON)
						if category.alt_title
							print category.alt_title
						else
							print category.name
						Font(BOLD_OFF)
						""
					}
					if not (cheap or system(61))
						locate 1,LinesFromTop
					run category.menu_text
					print newline
					""
					if category.type ~= hint_option
						CoolPause(0,category)
				}
				while (display.needs_repaint = true  )
			}
		}
		else
		{
			if not recurse
			{
				if not (cheap or (TERP & SIMPLE_TERP))
					InitScreen
				if not verbroutine ~= &EndGame
					AfterMenu
				else
					PrintStatusLine
			}
			return
		}
	}
}

routine AfterMenu
{
#ifclear NO_ACCESSIBILITY
	if not (cheap & CHEAP_ON)
#endif
		PrintStatusline
	Font(DEFAULT_FONT) ! just in case
	RlibMessage(&AfterMenu,1) ! "...and now returning to the story."
	if (FORMAT & DESCFORM_I)
		""
	DescribePlace(location, true)
#ifset NEW_FUSE
	fake_runevents
#endif
}

replace Menu(menu_par, width, selection)
{
	local i, column, mouseclick, titlegap, optionsgap, num
	local n
	local sel = 1

#ifset NO_ACCESSIBILITY
	local cheap
#endif

	titlegap = menu_par.title_gap
	optionsgap = menu_par.options_gap

	if system(61)
	{
		titlegap = 0
		optionsgap = 1
	}
	if width = 0:  width = 20
	for i in menu_par
	{
		if i.option_available
		{
			num++
			n = string(_temp_string,i.name)
			if n > width : width = n
		}
	}

	if width > (display.linelength-1):  width = display.linelength-1

	! Default selection is first child if not otherwise given
	if selection = 0:  selection = child(menu_par)

	Font(BOLD_OFF | ITALIC_OFF | UNDERLINE_OFF | PROP_OFF)
	while true
	{
		if not (cheap or system(61)) ! system(61)
		{
			color MENU_BGCOLOR, MENU_BGCOLOR
			if display.needs_repaint
			{
				window 0
				display.needs_repaint = false
			}
			cls
		}
		if cheap or (TERP & SIMPLE_TERP) or
		( (TERP & GLK_TERP) and
		( num + 5 + titlegap + optionsgap ) >= (display.screenheight/3*2))
			return CheapMenu(menu_par,num)
		column = display.linelength/2 - width/2
		window ( num + 5 + titlegap + optionsgap )
		{
			if (TERP & GLK_TERP) ! system(61) and not simple_port
				cls

			! This section "fakes" CenterTitle, so it appears to be
			! a regular title window, when in actuality, the whole menu
			! is in the status window
			if not (TERP & SIMPLE_TERP)
				locate 1,1
			Font(PROP_OFF)
			n = string(_temp_string, menu_par.name)
			color MENU_SELECTCOLOR, MENU_SELECTBGCOLOR ! shouldn't affect glk
			print to (display.linelength/2 - n/2);
			print menu_par.name;
			print to display.linelength ! make sure we color the line completely
			for (i=0; i<titlegap;i++)
			{
				""
			}
			color MENU_TEXTCOLOR, MENU_BGCOLOR
			! end of fake CenterTitle
			Font(BOLD_OFF|ITALIC_OFF|UNDERLINE_OFF|PROP_OFF) ! shouldn't affect
																			 ! glk
			RlibMessage(&Menu, 1)	! print key commands

			for (i=0; i<optionsgap;i++)
			{
				""
			}

			sel = 1
			i = child(menu_par)
			while i
			{
				if i.option_available
				{
					if i = selection
					{
						if system(61) ! glk or minimum port
							print to (column - 2);"* ";
						else
							locate (column + 1), display.cursor_row
						color MENU_SELECTCOLOR, MENU_SELECTBGCOLOR  ! shouldn't affect
																				  ! glk?
						print i.name; to (column+width)
						color MENU_TEXTCOLOR, MENU_BGCOLOR
					}
					else
					{
						locate (column + 1), display.cursor_row !print to column;
						print i.name ; to (column+width)
					}
					sel++
				}
				i = younger(i)
			}
		}
		word[0] = HiddenPause
		if word[0] = MOUSE_CLICK and display.pointer_x >= column and
					display.pointer_x <= column+width
		{
			mouseclick = (num + 1) + display.pointer_y
			i = PickNthChild(mouseclick, menu_par, &IsOptionAvailable )
			if i.name
			{
					selection = i
					word[0] = ENTER_KEY
			}
			mouseclick = 0
		}
		elseif word[0] = MOUSE_CLICK
		{
			if (display.pointer_y + num + 5 + titlegap + optionsgap ) = 3 + titlegap
			{
				if display.pointer_x >= (display.screenwidth - 10)
					word[0] = ESCAPE_KEY
				elseif display.pointer_x <= 11
					word[0] = 'n'
			}
			elseif (display.pointer_y + num + 5 + titlegap + optionsgap ) = 4 + titlegap
			{
				if display.pointer_x >= (display.screenwidth - 16)
					word[0] = ENTER_KEY
				elseif display.pointer_x <= 15
					word[0] = 'p'
			}
		}
		if word[0] >= '0' and word[0] <= '9'
		{
			i = word[0] - '0'
			if i = 0:  i = 10
			if i <= num
			{
				i = PickNthChild(i, menu_par, &IsOptionAvailable )
				if i.name
					selection = i
			}
		}
		select word[0]
			case 'N', 'n', DOWN_ARROW, RIGHT_ARROW
			{
				while true
				{
					if not younger(selection)
						selection = child(menu_par)
					else
						selection = younger(selection)
					if selection.option_available and selection.name ~= ""
						break
				}
			}
			case 'P', 'p', UP_ARROW, LEFT_ARROW
			{
				while true
				{
					if not elder(selection)
						selection = youngest(menu_par)
					else
						selection = elder(selection)
					if selection.option_available and selection.name ~= ""
						break
				}
			}
			case 'Q', 'q', ESCAPE_KEY
			{
				if not (cheap or system(61))! cos ! system(61)
				{
					window !0
					{cls}
				}
				return 0
			}
			case ENTER_KEY
			{
				if not (cheap or system(61))! cos ! system(61)
				{
					window !0
					{cls}
				}
				return selection
			}
	}
!	}
}
#if undefined PickNthChild
routine PickNthChild(num, par, qualify_routine)
{
	local x, n, qual
	x = child(par)
	while x
	{
		if qualify_routine
		{
			qual = call qualify_routine(x)
			if qual
				n++
		}
		else
			n++
		if n = num
			return x
		x = younger(x)
	}
	return false
}
#endif

routine IsOptionAvailable(obj)
{
	if obj.option_available
		return true
	return false
}

routine CheapMenu(menu_par,num)
{
	Font(PROP_ON)
	local sel,i
#ifset NO_ACCESSIBILITY
	local cheap
#endif
	while true
	{
		if not (cheap or (TERP & SIMPLE_TERP))
			cls
		if not (TERP & SIMPLE_TERP)
			CenterTitle(menu_par.name)
		if display.needs_repaint
			display.needs_repaint = false
		print newline
		Indent
		print "\_  ";
		Font(BOLD_ON)
		print menu_par.name
		Font(BOLD_OFF)
		""
		sel = 1
		i = child(menu_par)
		while i
		{
			if i.option_available
			{
				Indent
				print number sel++;". ";
				print i.name
			}
			i = younger(i)
		}
		print ""
		RlibMessage(&Menu, 2)		! "Select the number of your choice"
		if num > 9
		{
			GetInput
			if word[1] = "q"
			{
				""
				return 0
			}
			if word[1]
				sel = StringToNumber(word[1])
			else
				sel = StringToNumber(parse$)
		}
		else
		{
			pause
			if word[0] = 'q','Q', '0', ESCAPE_KEY
			{
				printchar word[0]
				print newline
				if not (cheap or (TERP & SIMPLE_TERP))
					cls
				""
				return 0
			}
			else
				sel = word[0] - 48
		}
		if num <= 9
		{
			printchar word[0]
			print newline
		}
		""
		if sel > 0 and (sel <= num) ! sel)
		{
			if not (cheap or (TERP & SIMPLE_TERP))
				cls
			return PickNthChild(sel, menu_par, &IsOptionAvailable)
		}
		else
			"Not a valid option.\n"
	}
}

routine Help_Hints(obj)
{
	local i
#ifset NO_ACCESSIBILITY
	local cheap
#endif
	for (i = 0; i<=obj.hints_revealed; i++)
	{
		if i
			""
		run obj.(hint1+i)
		print newline
	}
	""
	while &obj.(hint1+i) ~= 0 ! i.e., no more topics
	{
		Font(BOLD_ON)
		RlibMessage(&Help_Hints,1) ! "[Press 'H' for another hint, or 'Q' to
										!	quit]";
		Font(BOLD_OFF)
		print newline
		word[0] = HiddenPause
		if word[0] = 'Q', 'q', ESCAPE_KEY
			return
		if word[0] = 'H', 'h'
		{
			obj.hints_revealed++
			run obj.(hint1+i++)
			print newline
			""
		}
	}
	Font(BOLD_ON)
	if not cheap
	{
		RlibMessage(&Help_Hints,2) ! "[No more hints.  Press any
	                           !  key...]";
		Font(BOLD_OFF)
		HiddenPause
	}
	else
	{
		RlibMessage(&Help_Hints,3) ! "[No more hints.  Leaving page.]";
		Font(BOLD_OFF)
	}

	return
}

routine ShowPage(page)
{
#ifset NO_ACCESSIBILITY
	local cheap
#endif
 	local reset_indent
	if not (cheap or system(61))
		window 0
	if verbroutine = &EndGame
		""
	if not (cheap or (TERP & SIMPLE_TERP))
	{
		color BGCOLOR, BGCOLOR
		RlibMessage(&ShowPage,1,page) ! "[OPENING PAGE]"
		""
		cls
	}
	do
	{
		if display.needs_repaint and not cheap
		{
			color BGCOLOR, BGCOLOR
			RlibMessage(&MakeMenu,3) ! "[WINDOW RESIZED]"
			""
		}
		display.needs_repaint = false
		color TEXTCOLOR, BGCOLOR, INPUTCOLOR
		if not (cheap or (TERP & SIMPLE_TERP))
		{
			if page.alt_title
				CenterTitle(page.alt_title)
			else
				CenterTitle(page.name)
		}
		if not cheap and not system (61)
			locate 1,LinesFromTop
		elseif cheap
		{
			if not (FORMAT & NOINDENT_F)
			{
				reset_indent = true
				FORMAT = FORMAT | NOINDENT_F
			}
		}
		run page.menu_text
		if reset_indent
			FORMAT = FORMAT & ~NOINDENT_F
		print newline
		if not cheap and
			not (page.type = hint_option and verbroutine = &EndGame)
			""
		if page.type ~= hint_option and not (cheap or (TERP & SIMPLE_TERP))
			CoolPause(0,page,(verbroutine = &EndGame))
	}
	while (display.needs_repaint = true  )

	if not (cheap or (TERP & SIMPLE_TERP))
	{
		InitScreen
		if verbroutine ~= &EndGame
			AfterMenu
	}
	else
		PrintStatusLine
}

routine SetPageColors(page)
{
	if not (menu_category.page_text_color or menu_category.page_bg_color)
	{
		if not (MENU_TEXTCOLOR or MENU_BGCOLOR) ! must not have been set
		{
			MENU_TEXTCOLOR = TEXTCOLOR
			MENU_BGCOLOR = BGCOLOR
		}
		menu_category.page_text_color = MENU_TEXTCOLOR
		menu_category.page_bg_color = MENU_BGCOLOR
	}

	if not (menu_category.title_color or menu_category.title_bg)
	{
		if not (MENU_SELECTCOLOR or MENU_SELECTBGCOLOR)
		{
			MENU_SELECTCOLOR = SL_TEXTCOLOR
			MENU_SELECTBGCOLOR = SL_BGCOLOR

		}
		menu_category.title_color = MENU_SELECTCOLOR
		menu_category.title_bg = MENU_SELECTBGCOLOR
	}

	if page.page_text_color or page.page_bg_color
	{
		MENU_TEXTCOLOR = page.page_text_color
		MENU_BGCOLOR = page.page_bg_color
	}
	else
	{
		MENU_TEXTCOLOR = menu_category.page_text_color
		MENU_BGCOLOR = menu_category.page_bg_color
	}
	if page.title_color or page.title_bg
	{
		MENU_SELECTCOLOR = page.title_color
		MENU_SELECTBGCOLOR = page.title_bg
	}
	else
	{
		MENU_SELECTCOLOR = menu_category.title_color
		MENU_SELECTBGCOLOR = menu_category.title_bg
	}
}
#endif ! USE_NEWMENU

!----------------------------------------------------------------------------
!* FOOTNOTES
!----------------------------------------------------------------------------
!\ This section allows Hitchhiker/Guilty Bastards style footnotes.
\!

#ifset USE_FOOTNOTES

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
		RLibMessage(&DoFootnote,1) ! "The proper syntax is
												 ! >FOOTNOTE [number]."
		return false
	}
	local a
	a = object
	if a < 1 or a >= MAXFOOTNOTES
		RLibMessage(&DoFootnote,2,a) ! "That isn't a valid footnote number."
#ifclear HITCHHIKER_STYLE
	elseif not footnotelib.footnotearray #a
		RLibMessage(&DoFootnote,3) ! "You haven't encountered that footnote
												 !   yet."
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
	RLibMessage(&FootnoteNotify) ! "\I{"; "Footnote #"; ")\i"
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
		RLibMessage(&Footnote,1) ! "("; "Footnote #"; ")";
	}
	elseif a and footnotelib.showfootnotes = 8
		RLibMessage(&Footnote,2, a) ! "("; "Footnote #"; ")";
}

routine DoFootnoteMode
{
	if word[2] = "on", "normal"
	{
		if footnotelib.showfootnotes = 1
			RLibMessage(&DoFootnoteMode, 1) ! "Footnotes are already on."
		else
		{
			footnotelib.showfootnotes = 1
			RLibMessage(&DoFootnoteMode, 2) ! "Footnotes on."
		}
	}
	elseif word[2] = "off", "never"
	{
		if not footnotelib.showfootnotes
			RLibMessage(&DoFootnoteMode, 3) ! "Footnotes are already off."
		else
		{
			footnotelib.showfootnotes = 0
			RLibMessage(&DoFootnoteMode, 4) ! "Footnotes off."
		}
	}
	elseif word[2] = "always"
	{
		if footnotelib.showfootnotes = 8
			RLibMessage(&DoFootnoteMode, 5) ! "Footnotes are already in
															! always-on mode."
		else
		{
			footnotelib.showfootnotes = 8
			RLibMessage(&DoFootnoteMode, 6) ! "Footnotes akways on."
		}
	}
	else
		RLibMessage(&DoFootnoteMode, 7) ! Footnote instructions
}

routine PrintFootNote
{
	"You need to replace the PrintFootNote routine with one that prints
	your own responses!"
}
#endif ! ifset USE_FOOTNOTES

!----------------------------------------------------------------------------
!* SCORE NOTIFY
!----------------------------------------------------------------------------
!\
Provides text like "You score has gone up by [x] points!"
\!

#ifset SCORE_NOTIFY
attribute notify_on alias special

property points alias e_to

object scorenotifylib "scorenotify"
{
	points 0
	is notify_on
	save_info
	{
		select self is notify_on
			case 0 : SaveWordSetting("score_off")
			case 1 : SaveWordSetting("score_on")
		return true
	}
	type settings
	in init_instructions
	execute
	{
		local a
		a = CheckWordSetting("scorenotify")
		if a
		{
			select word[(a+1)]
				case "score_off": self is not notify_on
				case "score_on": self is notify_on
		}
	}
	usage_desc
	{
		"\BSCORE NOTIFICATION ON\b- Be notified when you score points."
		Indent
		"\BSCORE NOTIFICATION OFF\b- Play without score notifications."
	}
}

object scorenotifymain
{
	type settings
	in main_instructions
	execute
	{
		ScoreNotify
	}
}

!\ Set the NOTIFY_FONT global to whatever font you want the notification to be
    in. You can even combine fonts with sometime like:
	NOTIFY_FONT = BOLD_ON | ITALIC_ON

	(any font styles other than BOLD_ON, ITALIC_ON, or UNDERLINE_ON will
	probably cause trouble} \!
global NOTIFY_FONT = BOLD_ON

routine ScoreNotify
{
	if scorenotifylib.points and scorenotifylib is notify_on
	{
		""
		Font(NOTIFY_FONT)
		if scorenotifylib.points > 0
			RLibMessage(&ScoreNotify, 1, scorenotifylib.points )
			! "[Your score has gone up.]"
		else
			RLibMessage(&ScoreNotify, 2, -scorenotifylib.points )
		! "[Your score has gone down.]"
		Font((NOTIFY_FONT*2))
	}
	if scorenotifylib.points
	{
		scorenotifylib.points = 0    ! reset the point counter
!		PrintStatusLine  ! update status bar with new score
	}

}

! call this routine to add to the game score
routine AddScore(a)
{
	scorenotifylib.points += a
	score += scorenotifylib.points   ! add the points to the score
}

routine SubtractScore(a)
{
	scorenotifylib.points -= a
	score += scorenotifylib.points   ! add the points to the score
}

routine DoScoreNotifyOnOff
{
	if scorenotifylib is notify_on
		Perform(&DoScoreNotifyOff)
	else
		Perform(&DoScoreNotifyOn)
}

routine DoScoreNotifyOn
{
	if scorenotifylib is notify_on
		RLibMessage(&DoScoreNotifyOn, 1 )
		! "[Score notification already on.]"
	else
	{
		RLibMessage(&DoScoreNotifyOn, 2 )
		! "[Score notification on.]"
		scorenotifylib is notify_on
	}
}

routine DoScoreNotifyOff
{
	if scorenotifylib is not notify_on
		RLibMessage(&DoScoreNotifyOff, 1 )
		! "[Score notification already off.]"
	else
	{
		RLibMessage(&DoScoreNotifyOff, 2 )
		! "[Score notification off.]"
		scorenotifylib is not notify_on
	}
}
#endif  ! ifset SCORE_NOTIFY

!----------------------------------------------------------------------------
!* BOXDRAW
!----------------------------------------------------------------------------
#ifset USE_BOXDRAW
property line alias misc
global nextepigram
attribute centered alias special
property simplefont alias capacity

object boxlib
{
	type settings
	in main_instructions
	execute
	{
		if nextepigram
			Epigram(nextepigram)
	}
}

class quote
{
	line 0
	simplefont ITALIC_ON  ! note: gargoyle will only honor italic OR bold, not
	                      ! both
}

routine Epigram(quotefile, pauseflag)
{
	nextepigram = quotefile
	if not system(61) and parser_data[PARSER_STATUS] ~= PARSER_RESET
		return
	Box(quotefile, pauseflag)
}

routine TitleEpigram(quotefile)
{
	InitScreen
	nextepigram = quotefile
	Box(quotefile, true)
	""
	InitScreen
}

routine QuoteboxPosition
{
	return 3
}

routine Box(quotefile, pauseflag,force_simple)
{
	local a,i, l, lng, pos_start, pos_end, start_row, old_lng, current_pos
#ifclear NO_ACCESSIBILITY
	if not force_simple
		force_simple = (cheap = 1)
#endif
	if system(61) or force_simple
	{
		a = quotefile.#line
		if quotefile.simplefont
			Font(quotefile.simplefont)
		for (i=1;i<=a ;i++ )
		{
			Indent
			if i = 1 and not nextepigram
				print "[";
			print quotefile.line #i;
			if i = a and not nextepigram
				print "]"
			else
				""
		}
		nextepigram = 0
		if quotefile.simplefont
			Font(quotefile.simplefont * 2)
		ExtraText(quotefile)
		if pauseflag
		{
			""
			CoolPause
		}
		return
	}
	for (i = 1;i<= quotefile.#line ;i++ )
	{
		old_lng = lng
		l = string(_temp_string, quotefile.line #i )
		lng = higher(old_lng,l)
	}
	if lng >= (display.linelength - 4)
	{
		Box(quotefile,pauseflag,true)
		return
	}
	""
	FONT(PROP_OFF)
	if nextepigram
	{
		start_row = display.statusline_height + QuoteboxPosition
		current_pos = display.cursor_row
	}
	else
	{
		start_row = display.cursor_row
	}
	pos_start = (display.screenwidth / 2 - (lng/2) - 2)
	pos_end = pos_start + lng + 3
	for (i = 1;i<= quotefile.#line ;i++ )
	{
		locate pos_start, (start_row + i - 1)
		if i = 1
		{
			color TEXTCOLOR, TEXTCOLOR
			print "[ ";
			color BGCOLOR, TEXTCOLOR
		}
		else
			print "\_ ";
		if quotefile is centered
		{
			l = string(_temp_string, quotefile.line #i )
			if l < lng
				print to (display.linelength / 2 - (l / 2));
		}
		print quotefile.line #i;
		if i = quotefile.#line
		{
			print to (pos_end - 1);
			color TEXTCOLOR, TEXTCOLOR
			print "]"
		}
		else
		{
!			local x
!			for (x = (i+1);x<= quotefile.#line ;x++ )
!			{
!				if quotefile.line #x
!				{
					print to pos_end
!					break
!				}
!			}
		}
	}
	color TEXTCOLOR, BGCOLOR
	ExtraText(quotefile)
	if nextepigram
		locate 1, current_pos
	FONT(DEFAULT_FONT)
	if pauseflag
		TopPause
	nextepigram = 0
}

routine ExtraText(quotefile)
{}

#endif ! ifset USE_BOXDRAW
!----------------------------------------------------------------------------
!* INIT ROUTINES
!----------------------------------------------------------------------------

routine SimpleIntro
{
	local g
	g = IsGlk
	if (not g and system(61)) and
		not CheckWordSetting("restart") ! non-glk, simple port
	{
"\_          ___\_      ___\_  ___\_      ___\_  _________\_    _________"
"\_         (__ |\_    | __) (__ |\_    | __)\_| _______ |\_  | _______ |"
"\_           | |\_    | |\_    | |\_    | |\_  | |\_    | |\_  | |\_    | |"
"\_           | |\_    | |\_    | |\_    | |\_  | |\_    |_|\_  | |\_    | |"
"\_           | |_____| |\_    | |\_    | |\_  | |\_   ___\_   | |\_    | |"
"\_           |\_ _____\_ |\_    | |\_    | |\_  | |\_  (__ |\_  | |\_    | |"
"\_           | |\_    | |\_    | |\_    | |\_  | |\_    | |\_  | |\_    | |"
"\_           | |\_    | |\_    | |\_    | |\_  | |\_    | |\_  | |\_    | |"
"\_          _| |\_    | |_\_   | |_____| |\_  | |_____| |\_  | |_____| |"
"\_         (___|\_    |___)\_  |_________|\_  |_________|\_  |_________|"
""
		PRINT to (display.screenwidth / 2 - 11) ; "I N T E R A C T I V E"
		PRINT to (display.screenwidth / 2 - 7) ; "F I C T I O N"
		pause
	}
}
!\ old version of the SimpleIntro code- just cleared the screen
			local i  ! hack-y code to clear the screen
			for (i=1;i<display.screenheight ;i++ )
				{
				if i = display.screenheight / 2
					{
					print to (display.screenwidth/2 - 10);
					print "PRESS A KEY TO BEGIN"
					}
				else
					""
				}
\!
routine InitScreen
{
	local simple_port
#ifset NO_ACCESSIBILITY
	local cheap
#endif
#ifclear NO_ACCESSIBILITY
	if cheaplib is not special
	{
		run cheaplib.execute
		cheaplib is special
		if cheap = 1
			return
	}
#endif
	color TEXTCOLOR, BGCOLOR, INPUTCOLOR
	Font(DEFAULT_FONT)
	simple_port = (not IsGlk and system(61)) ! non-glk simple port
	if not system(61)
		window 0
#ifclear NO_ACCESSIBILITY
!	if not cheap and location is not visited
!	{
!		local a
!		a = CheckWordSetting("cheap")
!		select word[(a+2)]
!			case "off" : cheap = 0
!			case "on" : cheap = 1
!			case "menus" : cheap = 2
!!		if CheckWordSetting("cheap")
!!			cheap = true
!	}
	if not simple_port and cheap ~= 1
		cls
#else
	if not simple_port ! not non-glk simple port
		cls
#endif
	if not system(61) and cheap ~= 1 ! CheckWordSetting("cheap")
		CenterTitle("",0,1,1) ! Draw an empty window
	else
		CenterTitle(CheapTitle,0,1,1) ! See CheapTitle routine
#ifclear NO_ACCESSIBILITY
	if not system(61) and not (cheap & CHEAP_ON)
		locate 1, LinesFromTop
	elseif (cheap & CHEAP_ON) or simple_port ! non-glk simple port
		""
#else
	if not system(61)
		locate 1,LinesFromTop
	elseif simple_port
		""
#endif ! CHEAP
	if display.needs_repaint
		display.needs_repaint = false
}

routine CheapTitle
{
	if display.title_caption
		return display.title_caption
	return "Hugo Interactive Fiction"
}

routine LinesFromTop
{
	if not (display.hasvideo or hugor is switchedon)
		return display.windowlines
	else
		return 2
}

!\ IsGlk
	Roody's note- I decided roodylib should have its own glk-detecting routine.
	Note: WILL NOT WORK FROM NON-MAIN WINDOW
\!
routine IsGlk
{
	return ((display.screenheight + 100) < display.windowlines)
}

! Added ClearWindow as a quick way to clear and reset the cursor
! without clearing the status (and other) windows

routine ClearWindow
{
	local a
#ifclear NO_ACCESSIBILITY
	a = cheap
#endif
	if not (a = 1)
	{
		cls
		if (display.hasvideo or hugor is switchedon)
			locate 1, LinesFromTop
	}
}

!----------------------------------------------------------------------------
!* BETA compiles
!----------------------------------------------------------------------------

! Roody's note: This used to be its own extension, but I figured I'd just throw
! it into Roodylib.  This prompts players with a "start transcripts now?"
! question as soon as they open the game and keeps track of whether a
! transcript is in progress despite restarts or restores.

#ifset BETA
#ifset USE_EXTENSION_CREDITING
version_obj beta_version "Beta Version 2.7"
{
	in included_extensions
	desc_detail
		" by Bert Byfield, Mike Snyder, and Roody Yogurt";
}
#endif
#endif ! ifset BETA

attribute playback_on alias switchedon
attribute record_on alias workflag
attribute transcript_on alias special
!attribute skip_pauses alias light

object betalib "betalib"
{
	type settings
	in init_instructions
	did_print 0
#ifset BETA
	usage_desc
	{
		Indent
		"Betatesters: When a transcript is on (";
		if system(61) ! simple
			print "\"";
		else
		 print "\#147";
		 print "\BSCRIPT ON\b\"), preface your
		notes with an asterisk (";
		if system(61) ! simple
			print "\"";
		else
		 print "\#147";
		 "\B*\b\") to have your comments accepted."
	}
#endif ! ifset BETA
	save_info
	{
		local a, b,c
		if self is workflag
			a = SaveWordSetting("record")
		if self is special
			b = SaveWordSetting("beta")
		if self is switchedon
			c = SaveWordSetting("playback")
		if a or b or c
			return true
		else
			return false
	}
	execute
	{
		BetaInit
	}
}

routine BetaInit
{
	if CheckWordSetting("record")
		betalib is workflag
	if CheckWordSetting("beta")
		betalib is special
	if CheckWordSetting("playback")
		betalib is switchedon
#ifset BETA
	if not (word[LAST_TURN] = "undo","restore") and betalib is not special
	{
		RLibMessage(&BetaInit,1) ! Would you like to start a transcript?
!: fancy pause stuff below
		""
		local a
		a = GetKeyPress
		if a = 'b','B'
		{
			if (not scripton)
				VMessage(&DoScriptOnOff, 1)      ! "Unable to begin..."
			else
			{
				betalib is special
				RlibMessage(&BetaInit, 2)      ! "Starting beta transcript for
				                               !  <GAME TITLE>"
			}
		}
		else
			RlibMessage(&BetaInit,3) ! "No transcript started."
		""
		CoolPause(0,true)
		betalib.did_print = true
	}
#endif ! ifset BETA
}

!----------------------------------------------------------------------------
!* "CHEAPLIB"
!----------------------------------------------------------------------------

! Roody's note: Turning on the cheap system in a game forces a limited display.
! The intention is that this would be useful for games playing with crappy
! displays, like the DOS simple port or the Floyd bot on ifMUD.
! "cheap on" stops the statusline from print altogether, while "cheap menus"
! forces only newmenu's simple menu printing system.

#ifclear NO_ACCESSIBILITY
global cheap

object cheaplib "cheap"
{
	type settings
	in init_instructions
	save_info
	{
		local a
		select cheap
			case 0 : a = SaveWordsetting("off")
			case 1 : a = SaveWordSetting("on")
			case 2 : a = SaveWordSetting("menus")
		word[(a-1)] = prompt
		return true
	}
	execute
	{
		local a,b
		b = cheap
		a = CheckWordSetting("cheap")
		if a
		{
			if word[(a+1)]
				prompt = word[(a + 1)]
			select word[(a+2)]
				case "off" : cheap = 0
				case "on" : cheap = 1
				case "menus" : cheap = 2
		}
		if (cheap & CHEAP_ON) and not b ! and word[LAST_TURN] ~= "restart"
			DrawCheap
	}
	usage_desc
	{
		local simple_port
		simple_port = (not IsGlk and system(61)) ! non-glk simple port
		if simple_port
		{
			"\BPROMPT\b - Toggles between the > prompt and a \"What now? \"
			prompt."
			"\BSTATUS\b - Prints the information in the status bar in the main
			window."
			return
		}
		Indent
		"\BPROMPT\b - Toggles between the > prompt and a \"Your command... \"
		prompt."
		Indent
		"\BSTATUS\b - Prints the information in the status bar in the main
		window."
#ifset USE_NEWMENU
		Indent
		"\BSIMPLE MENUS\b - Displays in-game menus as numerical lists in the
		main window. All other behavior (screen clearing, status lines) are the
		same."
	!	"The following commands automatically display menus as numerated lists:"
#endif
		Indent
		"\BCLEARSCREEN OFF\b - Any in-game screen-clearing is ignored. This
		might work with some screen readers and also targets console interpreters
		where screen clearing doesn't work well anyway. Menu options are
		numbered."
		Indent
		"\BNORMAL\b - Reverts to allow in-game screen sclearing."

	}
}


routine DoAccessibility
{
	local b, simple_port
	b = cheap
	simple_port = (not IsGlk and system(61)) ! non-glk simple port
	if simple_port
	{
		"Hugo detects that this is a simple interpreter. As such, screen
		clearing is already turned off and menus (if any exist) are written
		as numerical lists."
		return
	}
	select word[1]
		case "normal"
		{
			if cheap
			{
				cheap = false
				RLibMessage(&DoAccessibility, 1) ! "Cheap mode off."
				if (b & CHEAP_ON)
					PrintStatusLine
			}
			else
				RLibMessage(&DoAccessibility, 2) ! "Cheap mode already off."
		}
		case "clear","clearscreen"
		{
			select word[2]
				case "normal"
				{
					if cheap
					{
						cheap = false
						RLibMessage(&DoAccessibility, 1) ! "Cheap mode off."
						if (b & CHEAP_ON)
							PrintStatusLine
					}
					else
						RLibMessage(&DoAccessibility, 2) ! "Cheap mode already off."
				}
				case "never","off"
				{
					if not (cheap & CHEAP_ON)
					{
						cheap = true
						RLibMessage(&DoAccessibility, 3) ! "Cheap mode on."
						DrawCheap
					}
					else
						RLibMessage(&DoAccessibility, 4) ! "Cheap mode already on."
				}
		}
}

routine DoCheapOnOff
{
	local a = 2
	local b
	b = cheap
	while a < 4
	{
		select word[a]
			case "off"
			{
				if cheap
				{
					cheap = false
					RLibMessage(&DoAccessibility, 1) ! "Cheap mode off."
					if (b & CHEAP_ON)
						PrintStatusLine
				}
				else
					RLibMessage(&DoAccessibility, 2) ! "Cheap mode already off."
			}
			case "on"
			{
				if not (cheap & CHEAP_ON)
				{
					cheap = true
					RLibMessage(&DoAccessibility, 3) ! "Cheap mode on."
					DrawCheap
				}
				else
					RLibMessage(&DoAccessibility, 4) ! "Cheap mode already on."
			}
			case "menu","menus"
			{
				if cheap ~= CHEAP_MENUS
				{
					cheap = CHEAP_MENUS
					RLibMessage(&DoCheapOnOff, 5) !"Cheap menus on."
					if b
						PrintStatusLine
				}
				else
					RLibMessage(&DoCheapOnOff, 6) !"Cheap menus already on."
			}
		a++
	}
}

routine DoCheapHelp
{
	RLibMessage(&DoCheapHelp,1) ! "'Cheap' help spiel."
}

routine DoCheapToggle
{
	local b
	b = cheap
	if cheap
	{
		cheap = 0
		RLibMessage(&DoCheapToggle,1) ! "'Cheap' mode toggled off."
		if (b & CHEAP_ON)
			PrintStatusLine
	}
	else
	{
		cheap = true
		RLibMessage(&DoCheapToggle,2) ! "'Cheap' mode toggled on."
		DrawCheap(true)
	}
}

routine DrawCheap ! this is basically InitScreen, slightly modified
{
	if not system(61)
	{
		color TEXTCOLOR, BGCOLOR, INPUTCOLOR
		Font(DEFAULT_FONT)
		window 0
	}
	CenterTitle(CheapTitle,0,1,1) ! See RoodyLib's CheapTitle routine
}

routine DoInfo
{
	DoAccessibilityHelp
}

routine DoAccessibilityHelp
{
	local simple_port
	simple_port = (not IsGlk and system(61)) ! non-glk simple port
	if simple_port
	{
		"Hugo detects that this is a simple interpreter. As such, screen
		clearing is already turned off and menus (if any exist) are written
		as numerical lists.\n"
		"The following commands may still be useful:\n"
	"\BPROMPT\b - Toggles between the > prompt and a \"What now? \" prompt."
	"\BSTATUS\b - Prints the information in the status bar in the main
	window."
		return
	}
	"The following commands exist to make playing this game with screen
	readers or in console windows more convenient:\n"
	Indent
	"\BPROMPT\b - Toggles between the > prompt and a \"Your command... \" prompt."
	Indent
	"\BSTATUS\b - Prints the information in the status bar in the main
	window."
#ifset USE_NEWMENU
	Indent
	"\BSIMPLE MENUS\b - Displays in-game menus as numerical lists in the
	main window. All other behavior (screen clearing, status lines) are the
	same."
!	"The following commands automatically display menus as numerated lists:"
#endif
	Indent
	"\BCLEARSCREEN OFF\b - Any in-game screen-clearing is ignored. This
	might work with some screen readers and also targets console interpreters
	where screen clearing doesn't work well anyway. Menu options are numbered."

	"\nTo revert back to regular screen-clearing and menus, type \BNORMAL\b."
}

!\ Roody's note: DoPrompt exists so that players using screenreaders can
change the prompt to something that sounds better when spoken aloud. \!

global promptbackup

routine DoPrompt
{
	if not promptbackup
		promptbackup = prompt
	"Prompt toggled."
	if prompt = "Your command... "
		prompt = promptbackup
	else
		prompt = "Your command... "
}

!\ Roody's note: DoStatus is a command to relay the information in the
status line to the player; it's functionality has the vision-impaired and
people using limited-function interpreters where status lines are bothersome
or not printed.

You may very well want to replace this routine and write some better sounding
text that suits your game. \!

routine DoStatus
{
	print "You are in the location, \"";
	print location.name;
#ifset AMERICAN_ENGLISH
	print ".\"";
#else
	print "\".";
#endif
	if STATUSTYPE or max_score
		print AFTER_PERIOD;
	if statustype
	{
		if STATUSTYPE = 2, 16,18
		{
			print "It is ";
			HoursMinutes(counter)
			if MAX_SCORE
			{
				if (STATUSTYPE & MILITARY_TIME)
					print ".";
				print AFTER_PERIOD ; "You ";
			}
			elseif (STATUSTYPE & MILITARY_TIME)
				print "."
			else
				""
		}
		elseif STATUSTYPE = 1,9,8
		{
			print "In "; number counter; " turn";
			if counter ~= 1
				print "s";
			print ", you ";
		}
		if MAX_SCORE or (STATUSTYPE = 1,9,8)
		{
			print "have scored "; number score; " point";
			if score ~= 1
				print "s";
			"."
		}
	}
	elseif MAX_SCORE
	{
		print AFTER_PERIOD;
		print "Your score is "; number score;"."
	}
	else
	{
		print AFTER_PERIOD;
		print "There ";
		if counter ~= 1
			print "have";
		else
			print "has";
		print " been "; number counter; " move";
		if counter ~= 1
			print "s";
		print "."
	}
	print newline
}
#endif
!----------------------------------------------------------------------------
!* PREPARSE CODE
!----------------------------------------------------------------------------

routine PreParseInstructions
{
	local i , p, r
	for i in preparse_instructions
	{
		p = i.execute
		r = r | p
	}
	return r
}

object preparse_instructions
{
	in settings
}

#ifclear NO_FANCY_STUFF
! Roody's note: setting the AIF switch allows wearing and removing "all"
! (haha not specifically promoting AIF just thought the switch name would
!  be funny)

! Anyhow, we use preparse stuff here because "remove"'s grammar is a little
! unforgiving when it using "all" and held items, so we switch it to "take
! off", which expects the item to be held.
#ifset AIF
object parse_remove
{
	in preparse_instructions
	type settings
	execute
	{
		if word[1] = "remove" and word[2] = "~all"
		{
			if CurrentCommandWords ~= 2
				return false
			word[1] = "take"
			InsertWord(2,1)
			word[2] = "off"
			return true
		}
		return false
	}
}
#endif

object parse_orders
{
	in preparse_instructions
	type settings
	execute
	{
		local a
		if word[1] = "tell", "order", "instruct", "ask", "command"
		{
			for (a=3; a<=words; a++)
			{
				if word[a] = "to"
				{
					word[a] = "~and"
					DeleteWord(1)   ! "tell", "order", etc.
!					OrdersPreParse(a,command_words)
					return true
				}\
				if word[a] = "", "then"
					break
			}
		}
		elseif betalib is switchedon and word[1] = "record" and
			word[2] = "off"
		{
			word[1] = "playback"
			return true
		}
		return false
	}
}

object parse_redraw
{
	in preparse_instructions
	type settings
	execute
	{
		if display.needs_repaint
		{
			if RepaintScreen
				RedrawScreen
		}
		return false
	}
}
#endif
!\ Roody's note: RedrawScreen is a generic routine for redrawing the screen
after a screen-size change. Ideally, it should be called in PreParse.
\!

!global oldwidth

routine RedrawScreen
{
	! if the screen size has changed, we'll clear the screen,
	! print the player's command, and redraw the status line before
	! proceeding to interpret the command

	InitScreen
	PrintStatusLine
	ShowCommand

}

!\ Roody's note: Split the screen-clearing stuff into another routine
   so EndGame and such could use it, too.   \!

#if undefined REPAINT_TIME
constant REPAINT_TIME 30
#endif

routine RepaintScreen
{
	display.needs_repaint = false
#ifclear NO_ACCESSIBILITY
!\ This little section is a little useless since it really only applies to
	non-simple interpreters with cheap mode turned on for testing purposes. \!
	if (cheap & CHEAP_ON)
	{
		CenterTitle(CheapTitle,0,1,1)
		return
	}
#endif ! CHEAP
	RLibMessage(&RepaintScreen) ! "[Detected screen size change;
	                            !  redrawing screen...]"
	""
	local get_key, a
	get_key = system(11) ! READ_KEY
	if not (system_status or system(61)) ! 61 = MINIMAL_PORT
	{
		while a < REPAINT_TIME ! 100
		{
			if system(11)
				break
			system(32) ! wait 1/100th second
			a++
		}
	}
	return true
}

!\ Roody's note: ShowCommand retypes the typed (valid)  command after the
 screen has been cleared during a screen size.  \!

routine ShowCommand
{
	print prompt;
	local i = 1, showfullstops
	if INPUTCOLOR ~= MATCH_FOREGROUND
		color INPUTCOLOR
	while word[i] ~= ""
	{
		print word[i];
		if word[++i] ~= ""
			print " ";
		elseif word[++i] ~= ""
		{
			showfullstops = true
			print ". ";
		}
		if word[i] = "" and showfullstops
			print ".";
		color TEXTCOLOR, BGCOLOR, INPUTCOLOR
	}
	print ""
}

!----------------------------------------------------------------------------
!* SCOPE_REACT
!----------------------------------------------------------------------------

#ifset USE_SCOPE_REACT
object scope_objects
{}

routine OrganizeScopeObjects
{
	local i, n
	for (i=1;i<=objects ;i++ )
	{
#ifset USE_PLURAL_OBJECTS
		n = 0
		if i.type = plural_class, identical_class
			n = 1
#endif
		if i.#found_in or i.#in_scope and not n
		{
			if i.type ~= i and parent(i) = nothing and i.type ~= fuse and
				i.type ~= daemon
			{
				move i to scope_objects
			}
		}
	}
}

routine ScopeReactBefore
{
	local i,r
	for i in scope_objects
	{
		if InList(i, found_in, location)
			r = i.react_before
		if not r and InList(i, in_scope, player)
			r = i.react_before
		if r
		{
#ifset DEBUG
			if debug_flags & D_PARSE
			{
				print "\B["; i.name;
				if debug_flags & D_OBJNUM
					print " ["; number i; "]";
				print ".react_before returned "; number r; "]\b"
			}
#endif
			break
		}
	}
	return r
}

routine ScopeReactAfter
{
	local i,r
	for i in scope_objects
	{
		if InList(i, found_in, location)
			r = i.react_after
#ifset DEBUG
		if debug_flags & D_PARSE and r
		{
			print "\B["; i.name;
			if debug_flags & D_OBJNUM
				print " ["; number i; "]";
			print ".react_after returned "; number r; "]\b"
		}
#endif
		if InList(i, in_scope, player)
			r = i.react_after
#ifset DEBUG
		if debug_flags & D_PARSE and r
		{
			print "\B["; i.name;
			if debug_flags & D_OBJNUM
				print " ["; number i; "]";
			print ".react_after returned "; number r; "]\b"
		}
#endif
	}
}
#endif

!----------------------------------------------------------------------------
!* SUPERCONTAINER CLASS
!----------------------------------------------------------------------------

!\ Roody's note: Added Kent Tessman's Supercontainer class to Roodylib so
player_character code could be automatically added to supercontainer-using
games. Just #set USE_SUPERCONTAINER to use. \!

!\---------------------------------------------------------------------------
	SuperContainer class by Kent Tessman

	The SuperContainer class can be a platform and/or a container,
	and has its own routines for handling objects, for describing
	its contents, etc.

	Always use MoveInto() and MoveOnto() to move objects into or
	onto a SuperContainer.

	Version 1.01 - Robb Sherwin. Added code to check if an item is worn, in addition to being clothing.
	Version 1.00 - Kent Tessman. An implementation of a new, relatively untested SuperContainer container/platform combo class.

---------------------------------------------------------------------------\!
#ifset USE_SUPERCONTAINER

! These can be modified in objects derived from SuperContainer:
property capacity_in alias n_to
property capacity_on alias ne_to

! attribute actually_transparent     ! this attribute is never actually used

! You can change define this constant beforehand if you want more (or less)
! than the default maximum:
#if undefined MAX_SUPERCONTAINER_CONTENTS
constant MAX_SUPERCONTAINER_CONTENTS 32
#endif


!----------------------------------------------------------------------------
! The SuperContainer class itself
!----------------------------------------------------------------------------

! For SuperContainer internal use:
property contents_in
property count_in
property contents_on
property count_on
property reset_contents
attribute supposed_to_be_closed

class SuperContainer
{
	type SuperContainer

	capacity_in 100
	capacity_on 100

	preposition "in", "on"	! can be changed, but both are needed

	! To disable either container or platform behavior, use
	! "is not [container or platform]" in the object definition
	is container
	is platform

! Contents display:

	list_contents
	{
		if not (object = self or xobject = self)
		{
			run self.reset_contents
		}
		if self.count_in and self is container and
			(self is open or self is transparent)
		{
			local temp_word
			! So we can hijack object.before:DoLookIn
			temp_word = word[1]
			word[1] = "in"
			Indent
			Perform(&DoLookIn, self)
			word[1] = temp_word
		}
		if self.count_on
		{
			Indent
			Perform(&DoLookIn, self)
		}
	}

! SuperContainer internals:

	contents_in #MAX_SUPERCONTAINER_CONTENTS
	count_in 0
	contents_on #MAX_SUPERCONTAINER_CONTENTS
	count_on 0

	react_before
	{
		if self = parent(object)
		{
			if InList(self, contents_on, object) and self is not open
			{
				self is open
				self is supposed_to_be_closed
			}
		}
		return false
	}

	before
	{
		xobject		! anytime we're an xobject
		{
			run self.reset_contents
			return false
		}
		object		! anytime we're an object
		{
			run self.reset_contents
			return false
		}
		xobject DoPutIn
		{
			! "in"
!			if PrepWord("in") or PrepWord("into") or PrepWord("inside")
			if PrepWord("in","into","inside")
			{
				if self is openable and self is not open
				{
				!	CThe(self)  ! figured, might as well use the official DoPutIn
				!	" is closed."   ! message
					VMessage(&DoPutIn, 2)    ! "It's closed."
					return true
				}
				elseif object is clothing and object is worn
				{
					VMessage(&DoDrop, 1)     ! "Have to take it off first..."
				}
				elseif not MoveInto(xobject, object)
				{
					"There is no room for ";
					print The(object); " ";
					print self.prep #1; " "; The(self); "."
				}
				else
				{
					print CThe(player); " put"; MatchSubject(player); \
						" "; The(object);
					print " "; xobject.prep #1; " ";
					print The(xobject); "."
				}
			}

			! "on"
			else
			{
				if not MoveOnto(xobject, object)
				{
					"There is no room for ";
					print The(object); " ";
					print self.prep #2; " "; The(self); "."
				}
				elseif object is clothing and object is worn
				{
					VMessage(&DoDrop, 1)     ! "Have to take it off first..."
				}
				else
				{
					print CThe(player); " put"; MatchSubject(player); \
						" "; The(object);
					print " "; xobject.prep #2; " ";
					print The(xobject); "."
				}
			}
		}
		object DoLookIn
		{
			! "in"
!			if PrepWord("in") or PrepWord("into") or PrepWord("inside")
			if PrepWord("in","into","inside")
			{
				if self is openable and self is not open and
				self is not transparent
				{
					CThe(self)
					" is closed."
					return true
				}

				CThe(self)
				if self.count_in = 0
				{
					" is empty."
				}
				else
				{
					" contains ";
					PropertyList(self, contents_in)
					print "."
				}
			}

			! "on"
			else
			{
				CThe(self)
				" has ";
				if self.count_on = 0
				{
					"nothing";
				}
				else
					PropertyList(self, contents_on)
				print " "; self.prep #2; " it."
			}
		}
	}
	after
	{
		xobject		! anytime we're an xobject
		{
			run self.reset_contents
			return false
		}
		object		! anytime we're an object
		{
			run self.reset_contents
			return false
		}
	}

	reset_contents
	{
		local i, obj
		self.count_in = 0
		for (i=1; i<=self.#contents_in; i++)
		{
			obj = self.contents_in #i
			if obj and obj not in self
			{
				self.contents_in #i = 0
			}
			elseif obj
			{
				self.count_in++
				if obj is plural
					self.count_in++
			}
		}
		self.count_on = 0
		for (i=1; i<=self.#contents_on; i++)
		{
			obj = self.contents_on #i
			if obj and obj not in self
			{
				self.contents_on #i = 0
			}
			elseif obj
			{
				self.count_on++
				if obj is plural
					self.count_on++
			}
		}
		if self is supposed_to_be_closed
			self is not open
		self is not supposed_to_be_closed
	}
}


!----------------------------------------------------------------------------
! Always use MoveInto() and MoveOnto() to move objects into or onto a
! SuperContainer.  Returns false if there's no room to do so.
!----------------------------------------------------------------------------

routine MoveInto(p, obj)
{
	if not p.#contents_in:  return false
	run p.reset_contents

	! Search for an empty "in" slot
	local i, slot, bulk
	for (i=1; i<=p.#contents_in; i++)
	{
		if p.contents_in #i = 0
		{
			if not slot
				slot = i
		}
		else
			bulk += (p.contents_in #i).size
	}

	! If we didn't find an empty slot, there's no room
	if not slot:  return false
	if bulk > p.capacity_in:  return false

	! Put the object in the contents_in list
	p.contents_in #slot = obj
	move obj to p
	return true
}

routine MoveOnto(p, obj)
{
	if not p.#contents_on:  return false
	run p.reset_contents

	! Search for an empty "on" slot
	local i, slot, bulk
	for (i=1; i<=p.#contents_on; i++)
	{
		if p.contents_on #i = 0
		{
			if not slot
				slot = i
		}
		else
			bulk += (p.contents_in #i).size
	}

	! If we didn't find an empty slot, there's no room
	if not slot:  return false
	if bulk > p.capacity_on:  return false

	! Put the object in the contents_on list
	p.contents_on #slot = obj
	move obj to p
	return true
}


! PrepWord(str) return true if the word "str" is the preposition used
! in the player input.

! Roody's note: modified this to take care of all possibilities in one go.

routine PrepWord(str1,str2,str3)
{
	local i
	for (i=1; i<=words; i++)
	{
		if not ObjWord(word[i], object)
		{
			select word[i]
				case "": break
				case str1, str2, str3 : return true
		}
	}
}
!routine PrepWord(str)
!{
!	local i
!	for (i=1; i<=words; i++)
!	{
!		if not ObjWord(word[i], object)
!		{
!			if word[i] = str
!				return true
!			if word[i] = ""
!				break
!		}
!	}
!}
#endif ! USE_SUPERCONTAINER

!----------------------------------------------------------------------------
!* LAST TURN SUCCESS GLOBAL
!----------------------------------------------------------------------------
! Roody's note: Some code occasionally needs to know if the last turn was
! successful (for example: the #SHOW_COMMANDS code used for printing commands
! from multi-command lines.  The following makes it possible.

!global last_turn_true

event
{
	NEW_PARSE = NEW_PARSE | LAST_TURN_TRUE_F
!	last_turn_true = true
}

!----------------------------------------------------------------------------
!* REPLACED OBJLIB.H CODE
!----------------------------------------------------------------------------
#ifclear NO_OBJLIB

! Roody's note: if NEW_ROOMS is set, Roodylib sets the "first_visit" property
! to the game counter when a room is first visited.  This allows Roodylib
! to check if it's the first turn in a room after an UNDO or RESTORE and give
! the initial_desc instead of a long_desc.

#ifset NEW_ROOMS
property first_visit
replace room
{
	first_visit 0
	type room
	is static, light, open
}
#endif

! Roody's note: characters now default to being excluded from >ALL
!  (suggested by Paul Lee)
!\
	Also: I added a default response for when you grab objects from friendly
	characters. Change for your characters if you want a different message.
	If you want to not allow grabbing objects from friendly characters
	altogether, stop the action with a "parent(object) DoGet" before routine.

	This default code also allows >GET ALL FROM <character>

	To disallow that, replace the ExcludeFromAll routine so it always returns
	true when the parent is living. You'll also want to provide NewParseError
	case 9 with a special message that checks that the parent is living
	and returns with something like "You'll have to specify one object at a
	time."
\!

replace character
{
	type character
	pronouns "he", "him", "his", "himself"
	capacity 50
	holding 0
	is living, transparent, static
	exclude_from_all true
	after
	{
		xobject DoGet
		{
			RLibOMessage(character, 1) ! "so-and-so allows you to take the <object>"
		}
	}
}

!\Roody's note: Now, doors should automatically open if you try to enter them
and they are locked and you have a key. Doors are also assumed to be hinged
(see before.DoPush/DoPull section). Also some parse rank stuff added, although
I wonder if I'm playing with fire with that one.

\!
replace door "door"
{
	type door
	door_to                            ! depends on actor location
	{
		local currentroom
		if actor = player
			currentroom = location
		else
			currentroom = parent(actor)

		! sitting on or in something
#ifclear USE_VEHICLES
		if actor = player and player not in location
#else
		if actor = player and player not in location and
			parent(player).type ~= vehicle:
#endif
		{
			! "You'll have to get out first..."
			RlibMessage(&DoGo, 1)      ! "You'll have to get up..."
			return true
		}

		if self is not open and actor = player
		{
#ifclear NO_VERBS
			local verb_check
#ifset USE_VEHICLES
			if verbroutine = &DoMoveInVehicle
				verb_check = true
#endif
			if verbroutine = &DoGo : verb_check = true

			if verb_check
			{
				if player not in location
				{
					OMessage(door, 4)       ! "It is closed."
					return true
				}

				if self is not locked and self is openable:
				{
#ifclear SKIP_DOORS
					! check to see if Perform has already been called
					if parser_data[PARSER_STATUS] = 128
						RlibOMessage(door,1) ! "(and then opening it)"
					else
						OMessage(door, 2)       ! "(opening it first)"
					Perform(&DoOpen, self)
					Main    ! counts as a turn
					if self is not open
	!					Perform(&DoGo, self)
						return true
					local d
					if currentroom = self.between #1
						d = self.between #2
					else
						d = self.between #1
					if not FindLight(d) or (FORMAT & DESCFORM_I)
						""
#endif   ! SKIP_DOORS
				}

				if self is lockable and self is locked
				{
#ifclear NO_AUTOMATIC_DOOR_UNLOCK
					local k,a
					if self.key_object
					{
						for (a=1; a <= self.#key_object ; a++)
						{
							if Contains(player, self.key_object #a)
							{
								k = self.key_object #a
								if k is not quiet
									break
							}
						}
					}
					if (k and k is quiet) or not k
					{
						OMessage(door, 3)      ! "It is locked."
						return true
					}
					RlibOMessage(door, 2,k)      ! "(unlocking <the blank> first))"
					Perform(&DoUnlock, self,k)
					if self is not locked
					{
						Main
						""
						Perform(&DoGo, self)
						return true
					}
					else
#else
					OMessage(door, 3)      ! "It is locked."
#endif
						return true
				}
				elseif self is not open
				{
#endif
#ifclear SKIP_DOORS
					OMessage(door, 4)       ! "It is closed."
					return true
#endif ! SKIP_DOORS
#ifclear NO_VERBS
				}
			}
#endif
		}

		if self is not open and (actor in location or
			location = self.between #((parent(actor) = \
				self.between #1) + 1))
		{
			! Notify the player that the player or a character
			! has gone through the door:
			!
			OMessage(door, 5)
			general = 2
		}

		if currentroom = self.between #1
			return self.between #2
		else
			return self.between #1
	}
	long_desc
	{
		if self is open
			OMessage(door, 6)       ! when open
		else
			OMessage(door, 7)       ! when closed
	}
	is openable, static

#ifset VERBSTUBS
!\ DoPush, DoPull - this version of the door class has normal hinged doors.
between #1 is the "exterior door" position; pushing opens the door, while #2
is the interior, where pushing closes. If you want your door to emulate
other behavior (swinging doors, whatever), you might want to change this.
\!
	before
	{
! DoOpen and DoClose always return true
		object DoPush
		{
			if location = self.between
				return Perform(&DoOpen, self)
			else
				return Perform(&DoClose, self)
		}
		object DoPull
		{
			if location = self.between
				return Perform(&DoClose, self)
			else
				return Perform(&DoOpen, self)
		}
	}
#endif
! Not sure if this next section is necessary. Added it to deal with a
! weirdly-coded game that messed with scope a lot.
!
! Doors in your location now have a higher priority than other doors
! this might turn out to be really bad design, though. we'll see.
!	parse_rank
!	{
!		if Inlist(self,between,location)
!		{
!			return 1
!		}
!		else
!			return -1
!	}
}

!\ Roody's note: replaced the female_character character class so it is of
type "character" (like a regular character), as you can always check for the
female attribute if you are specifically looking for a female character\!

replace female_character "(female_character)"
{
	inherits character
	pronouns "she", "her", "her", "herself"
	is female
}


!\ Roody's note: replaced the player_character object so it
can be of type "player_character", as I feel authors will often
want to distinguish playable characters from NPC's \!

#ifset AUTOMATIC_EXAMINE
attribute examined
#endif

replace player_character
{
	! 'nouns' and 'pronouns' as defined below are the defaults
	! for second-person, i.e., when player_person is 2--'name'
	! should be "you"
	!
	nouns "me", "myself"
	pronouns "you", "you", "your", "yourself"
	type player_character
	long_desc
		print "Looking good."
#ifset LIST_CLOTHES_FIRST  ! list worn items before listing rest of inventory
	list_contents
		return ListClothesFirst(self)
#endif
	before
	{
#ifset HUGOFIX
		actor DoVerbTest
		{
			DoVerbTest
			return true
		}
#endif
	}
	capacity 100
	holding 0
	is hidden, living, transparent, static
	is plural       ! because player_person defaults to 2
}

#ifset LIST_CLOTHES_FIRST

! Roody's note: If this switch is set, inventory listings begin with worn
! items. To use, set the character's list_contents property to like it is
! in the player_character object above.

routine ListClothesFirst(char)
{
	local x,w,c, list_save, v
	v = verbroutine
	verbroutine = &ListClothesFirst
	list_save = list_count
	list_count = 0
	for x in char
	{
		if x is worn and x is clothing
		{
			w++
			list_count++
		}
		elseif x is not hidden
		{
			x is already_listed
			c++
		}
	}
	if w and c
		list_nest = 1
	if list_count
	{
		if v ~= &DoInventory
			Indent
		if v = &DoLook
			FORMAT = FORMAT | USECHARNAMES_F
		RLibMessage(&ListClothesFirst,1,char) ! "You are wearing"
		ListObjects(char)
		if (FORMAT & USECHARNAMES_F)
			FORMAT = FORMAT & ~USECHARNAMES_F
		else
			FORMAT = FORMAT & ~USECHARNAMES_F
		list_nest = 0
	}
	for x in char
	{
		if x is not worn or (x is not clothing and x is worn)
			x is not already_listed
		else
		{
			AddSpecialDesc(x) ! tags worn items as already_listed
		}
	}
	verbroutine = v
	list_count = list_save - w
	return (not c and w)
}
#endif ! #ifset LIST_CLOTHES_FIRST

! Roody's note: Fixes a bug where talking to 2 NPCs (by Mike Snyder).
replace self_class
{
	type self_class
	before
	{
		object
		{
			Perform(verbroutine, self.self_object, xobject)
		}
		xobject
		{
			if object > themselves
			{
				! Rule out numbers used as xobjects:
				local i
				for (i=words; i>2; i--)
				{
					if word[i] <= 0 or StringToNumber(word[i]) = xobject
					{
						return false
					}
				}

				if not self.self_resolve
				{
					OMessage(self_class, 1)
				}
				else
				{
                                !       (the old code)
				!	Perform(verbroutine, object, self.self_object)
					Perform(verbroutine, object, object)
				}
			}
			else
				return false
		}
	}
	is known
}

!\ We also have to replace the himself, herself, itself, and themself classes,
as they all inherit from the self class we have replace about. \!

replace himself "himself"
{
	in self_class
	inherits self_class
	noun "himself"
	self_object:	return him_obj
	self_resolve
	{
		if object is plural or object is female or object is not living
			return false
	}
}

replace herself "herself"
{
	in self_class
	inherits self_class
	noun "herself"
	self_object:	return her_obj
	self_resolve
	{
		if object is plural or object is not female or object is not living
			return false
	}
}

replace itself "itself"
{
	in self_class
	inherits self_class
	noun "itself"
	self_object:	return it_obj
	self_resolve
	{
		if object is plural or object is living
			return false
	}
}

replace themselves "themselves"
{
	in self_class
	inherits self_class
	noun "themselves"
	self_object:	return them_obj
	self_resolve
	{
		if object is not plural
			return false
	}
}

#ifset USE_VEHICLES

! Roody's note: New vehicle class for making it easier to have vehicles where
! DOWN or OUT is not the standard direction for exiting; works with new DoGo
! routine

! Example of a vehicle that allows >DOWN and >OUT to exit
!	before
!	{
!		parent(player) DoGo
!		{
!			if object = d_obj, out_obj
!				return object
!			return false
!		}
!	}

replace vehicle
{
	type vehicle
	vehicle_verb "drive"            ! default verb
	prep "in", "out"                !   "     prepositions
	vehicle_move true               ! by default, always ready to move

#ifclear NO_VERBS
	before
	{
		parent(player) DoGo
		{
#ifset SMART_PARENT_DIRECTIONS
			local a
			a = CanGoDir
			if not a
				VMessage(&DoGo, 2)      ! "You can't go that way."
			elseif a = 1
				RlibMessage(&DoGo, 1)      ! "You'll have to get up..."
			else
#endif
			! "To walk, you'll have to get out..."
			OMessage(vehicle, 1, self)
		}
	}
#endif
	is enterable, static
}

replace DoMoveinVehicle
{
	local v, moveto

	if player in location
	{
		RlibOMessage(vehicle, 2)    ! "You aren't in anything."
		return false
	}

	v = parent(player)              ! the vehicle
	if v.type ~= vehicle
	{
		RlibOMessage(vehicle,3) ! "Good luck with that."
		return false
	}
	if v.before:  return true

	! Match the verb
	if not InList(v, vehicle_verbs, VerbWord)
	{
		OMessage(vehicle, 3, v)        ! wrong verb for this vehicle
		return false
	}

	if not object or (object = v and player in object)
	{
		OMessage(vehicle, 4)   ! "Specify a direction as well..."
		return false
	}

	if not v.vehicle_move:  return true

	if obstacle
	{
		OMessage(vehicle, 5, v) ! "X stops you from moving."
		return true
	}

	if object.type ~= direction
	{
		moveto = object.door_to
		if not moveto
			! "You can't (drive) in there..."
			OMessage(vehicle, 6, v)

		if moveto <= 1
			return
	}
	else
		moveto = location.(object.dir_to)

	if not InList(moveto, vehicle_path, v) and moveto ~= 1
	{
		OMessage(vehicle, 7, v) ! "You can't (drive) that way."
		return false
	}
	elseif moveto = 0
	{
		if not location.cant_go
			OMessage(vehicle, 7, v)
		return false
	}
	elseif moveto = 1               ! already printed message
		return false            !   (moveto is never 1)

#ifset USE_ATTACHABLES
	if ObjectisAttached(v, location, moveto)
		return false
#endif

	! Finally, the vehicle can move
	move v to moveto
#ifset MULTI_PCS
	MakeMovedVisited(v)
#else
	v is moved
#endif
	old_location = location
	location = moveto

#ifset USE_ATTACHABLES
	MoveAllAttachables(v, old_location, location)
#endif

	if not FindLight(location)
		DarkWarning
	else
	{
		if not event_flag
			event_flag = true
		DescribePlace(location)
#ifset MULTI_PCS
		MakeMovedVisited(location)
#else
		location is visited
#endif
	}

	run parent(player).after

	return true
}
#endif ! #ifset USE_VEHICLES

#ifset USE_PLURAL_OBJECTS

!\ Roody's note: Added two properties for plural_class scenery objects.
imaginary_plurals is the number of objects you want the player to think there
are.
over_max is the message they see if they refer to more than that number. \!
property imaginary_plurals alias se_to
property over_max alias sw_to

replace plural_class "(plural_class)"
{
	type plural_class
	in_scope 0                      ! may be an actor at some point
	size 0                          ! must be weightless
	pluralobj_number 0
	imaginary_plurals 0 ! number at which we want to change responses
	over_max 0 ! message to print when player asks for too much
	plural_verbs
	{
#ifclear NO_VERBS
		if verbroutine = &DoLook, &DoGet, &DoDrop, &DoPutin
			return true
#ifset USE_CHECKHELD
		elseif verbroutine = &DoDrop_CheckHeld, &DoPutin_CheckHeld
			return true
#endif
		else
#endif
			return false
	}

	found_in
	{
		if self = plural_class, identical_class
			return parent(player)
		else
			return false
	}

	! Set the parse_rank of this object to the (highest) parse_rank of
	! its member objects
	plural_parse_rank -100
	parse_rank
	{
		if self.plural_parse_rank = -100
		{
			local i, p, pr = 1
			for (i=1; i<=self.#plural_of; i++)
			{
				p = (self.plural_of #i).parse_rank
				if p > pr
					pr = p
			}
			self.plural_parse_rank = pr
		}
		return self.plural_parse_rank
	}

	before
	{
		object                  ! intercepts all plural_object verbs
		{
			local i, j, n, obj, count, flag

! Count the number of workflagged member objects, to see if there are
! less than specified (or any at all, for that matter).

			for (i=1; i<=self.#plural_of; i++)
			{
				if self.plural_of #i and self.plural_of #i is workflag
				{
					if ++n = 1
						obj = self.plural_of #i
!					AssignPronoun(obj)
!					n++
				}
			}

			if n > 1:  AssignPronoun(self)
			elseif n = 1 : AssignPronoun(obj)
			else ! n = 0
			{
				! "There are none..."
!				OMessage(plural_class, 1)

				if pluralobj_heldmode = 1
					! "You're not holding..."
					ParseError(15, self)
				else
					! "You don't see that..."
					ParseError(11, self)

				return true
			}

			if not self.imaginary_plurals
			{
				if (n < self.pluralobj_number) or
					(n = 1 and self.pluralobj_number = 0):
				{
					! "There are only x things..."
					OMessage(plural_class, 2, n)
					return true
				}
			}
			elseif self.pluralobj_number > self.imaginary_plurals
			{
				run self.over_max
				return true
			}
			else
			{	! not sure about this but I *think* it'll be okay
				self.pluralobj_number =1
			}

! If it's not an allowable plural-objects action, disqualify it:

			if self.pluralobj_number ~=1 and not self.plural_verbs
			{
				! "You'll have to do that one at a time."
				OMessage(plural_class, 3)
				return true
			}

! Handle examining objects as a group specially:
#ifclear NO_VERBS
			if verbroutine=&DoLook
			{
				if self.pluralobj_number ~= 1
				{
					if self.type = identical_class
						return false

					if self.long_desc
						return true
				}

				flag = 0
				for (i=1; i<=self.#plural_of; i++)
				{
					j = self.plural_of #i
					if j and &j.long_desc
						flag = true
				}
				if flag = 0:  return false
			}
#endif	! ifclear NO_VERBS

! Finally, process the action on workflagged individual objects:

			for (i=1; i<=self.#plural_of; i++)
			{
				obj = self.plural_of #i
				if obj and obj is workflag
				{
					if n~=1 and self.pluralobj_number~=1
					{
						if self.type = identical_class and
							obj.article
						{
							print CountWord(count+1);
							print " ";
						}
						print obj.name; ":  ";
					}

					Perform(verbroutine, obj, xobject)
					count++

					AssignPronoun(obj)
				}

				! number of objects specified (if specified)
				if count=self.pluralobj_number and count>0
					break
			}
		}
	}
	is plural, known
}

! Roody's Note: Since this inherits from pural_class, we need to declare it
! again
replace identical_class "(identical_class)"
{
	inherits plural_class
	type identical_class
	plural_parse_rank -100
	parse_rank
	{
		if self.plural_parse_rank = -100
			self.plural_parse_rank = (self.plural_of).parse_rank + 1
		return self.plural_parse_rank
	}
}

! ParsePluralObjects is called by the Parse routine.
! Roody's note- just got rid of a jump
replace ParsePluralObjects
{
	local i, j, k, n, w, wn
	local any, plist, pobj, plural_type, loc, xobj, pobj_number
	local 2ndpass

! Before doing anything, move all plural/identical object classes back to
! their respective holding classes:

	for (i=0; i<plural_count; i++)
		move plurals[i] to plurals[i].type

	if actor = 0:  actor = player

	pluralobj_heldmode = 0  ! i.e. not explicitly held
	wn = 2                  ! starting word number
	i = 3                   ! make sure we run the following once

	while wn < i
	{
		pobj_number = 0         ! default explicit count (pluralobj_number)

	! The first step is to scan the phrase for a potential plural object word:

		for (w=wn; w<=words; w++)
		{
			if word[w] = 0 or w>words
				break           ! end of phrase

			if word[w] = "my"
			{
				pluralobj_heldmode = 1
			}

			for plist in plural_class
			{
				if InList(plist, single_noun, word[w])
				{
					pobj = plist
					plural_type = single_noun
					break
				}

				if InList(plist, noun, word[w])
				{
					pobj = plist
					plural_type = noun
					break
				}
			}
			if pobj:  break

			for plist in identical_class
			{
				pobj = 0

				if InList(plist, single_noun, word[w])
				{
					pobj = plist
					pobj_number = 1
					plural_type = single_noun
					break
				}

				if InList(plist, noun, word[w])
				{
					pobj = plist
					plural_type = noun
					break
				}
			}
			if pobj:  break
		}
		if not pobj:  return false      ! didn't find anything

	! At this point, <w> points to the word and <pobj> is the object number of
	! the plural class.  <plural_type> is either noun or single_noun.  What
	! follows is the actual parsing of the plural structure:

	! Determine (roughly) if object is explicitly held or notheld; the
	! SetVerbHeldmode routine checks to see if the verb necessarily implies a
	! specific heldmode by checking the first 6 words--i.e., even if the first
	! five represent an object name in an actor-directed command.

		if not 2ndpass
		{
			for (i=1; i<=6 and i<=w; i++)
			{
				if pluralobj_heldmode:  break

				if word[i] = "":  break         ! end of phrase

				SetVerbHeldmode(word[i])
			}

	! If it is a single_noun (i.e. "any thing" or "either thing"), the object
	! global will have to be appropriately determined.  <pobj> will hold the
	! selected object (if any).  In any case, it will be necessary to know if
	! at least one object (in the case of multiple plural object specification)
	! is available, in order to move the plural_class into scope.

	! To boot, it could also get ugly if a container or platform xobject is
	! specified as the location for the object(s) being acted upon.  There is
	! no polite way of figuring out what the xobject might be at this point.

			for (i=w; i<words; i++)
			{
				k = word[i]             ! for shorthand purposes only
				if k = 0:  break        ! end of phrase

				if (k = "from", "outof", "offof", "off") or
					(pluralobj_heldmode ~= 1 and
					(k = "in" or k = "inside" or k = "on"))
				{
					xobj = 0        ! nothing selected yet
					do
					{
						i=i+1   ! get next word

						for (j=1; j<=pobj.#plural_of; j++)
						{
							n = parent(pobj.plural_of #j)
							if ObjWord(word[i], n)
								xobj = n
						}
					}
					while i<=words and word[i]~=0

					break
				}
			}

			n = 0
			if xobj
				loc = xobj
			elseif actor = player
				loc = location
			else
				loc = parent(actor)
		}

	! Remove the old plural_class adjective(s), if any

		while w > 1 and (ObjWord(word[w-1], pobj) = adjective or word[w-1] = "my")
		{
			if word[w-1] = "my"
			{
				! Try to differentiate between, for instance, "my door"
				! out of a set of door, and "my key", meaning the one
				! the player is holding:
				for (i=1; i<=pobj.#identical_to; i++)
				{
					if Contains(player, pobj.identical_to #i)
						break

				}
				if i > pobj.#identical_to
					break
			}

			DeleteWord(w-1)
			w--
		}

	! Then, find out if there is a preceding number

		if w > 1
		{
!#ifclear NO_DISAMB_HELP
!! This might cause trouble with similarly named plural classes, but
!! this is the best fix in the meantime for a clash between the disamb
!! system and the plural class checking for number words
!			if disamb_holder is not special
!#endif
			k = WordisNumber(word[w-1])
			if k >= 1                       ! e.g., "one (or more) things"
			{
				DeleteWord(w-1)
				w--
				pobj_number = k
			}

			if WordisNumber(word[w-1])>=1   ! "two of the three"
			{
				pobj_number = WordisNumber(word[w-1])
				DeleteWord(w-1)
				w--
			}
		}

	! Remove a preceding "all" or "any"; i.e. "all things" is the same as "things"

		if w>1 and (word[w-1] = "~all" or word[w-1] = "~any")
		{
			if word[w-1] = "~any":  any = true
			DeleteWord(w-1)
			w--
		}

	! Now see if the object (or a member of the plural_class) is available given
	! the restrictions of pluralobj_heldmode:

		n = 0
		j = 0

		if pluralobj_heldmode = -1              ! explicitly notheld object
		{
			for (i=1; i<=pobj.#plural_of; i++)
			{
				k = pobj.plural_of #i
				k is not workflag

				if k in loc
				{
					n = k
					k is workflag
					j++
				}
				if n and plural_type = single_noun:  break
			}
		}

		if not n or pobj_number = 0 or j < pobj_number
		{
			for (i=1; i<=pobj.#plural_of; i++)
			{
				k = pobj.plural_of #i

				! explicitly notheld object
				if pluralobj_heldmode = -1 and k is not workflag
				{
					if FindObject(k, loc) and k not in actor
					{
						n = k
						k is workflag
						j++
					}
				}
				elseif pluralobj_heldmode ~= -1
					k is not workflag

				! explicitly held object
				if pluralobj_heldmode = 1
				{
					if k in actor
					{
						n = k
						k is workflag
						j++
					}
				}

				! or neither
				elseif pluralobj_heldmode = 0
				{
					if FindObject(k, loc)
					{
						n = k
						k is workflag
						j++
					}
				}

				if n and plural_type = single_noun:  break
				if j and j = pobj_number:  break
			}
		}

	! Even if no object is found, it will be necessary to insert one in the
	! input line to generate a "You don't see that"-esque parser message in
	! the case of a single_noun specification.

		if plural_type = single_noun and not n
			n = pobj.plural_of #1

	! If it's not a plural specification--i.e. it is an "any" or "either"
	! phrasing instead--change the plural object word to represent <n>.

		i = words

		if plural_type = single_noun
		{
			if pobj.type ~= identical_class and any
			{
				SetObjWord(w, n)
				if pobj.type = plural_class
					print "("; The(n); ")"
			}
			elseif pobj.type = identical_class
			{
				SetObjWord(w, pobj)

				if pluralobj_heldmode = 1       ! in inventory
					move pobj to actor
				elseif xobj                     ! in or on something
					move pobj to xobj
				else                            ! anything else
					PutinScope(pobj, actor)
			}
		}

	! If it is a plural specification--plural_class.noun instead of single_noun--
	! move the plural class into an accessible position, but only if one of
	! the component members is available.

		elseif n and plural_type = noun
		{
			! Insert the name of the identical_class, if necessary
			if pobj.type = identical_class
				SetObjWord(w, pobj)

			if pluralobj_heldmode = 1       ! in inventory
				move pobj to actor
			elseif xobj                     ! in or on something
				move pobj to xobj
			else                            ! anything else
				PutinScope(pobj, actor)
		}

		pobj.pluralobj_number = pobj_number     ! # of plural objects

		wn = w + pobj.#adjectives + (pobj.noun > 0)
		if wn < i
		{
			pobj = 0
			2ndpass = true
		}
	}
	NEW_PARSE |= PRONOUNS_SET  ! since we rebuilt object(s)

	return true
}

! fixes a typo in original
replace AddIdentical(obj, i)
{
	local j

	for (j=1; j<=i.#plural_of; j++)
	{
		if i.plural_of #j = 0           ! a blank slot
		{
			i.plural_of #j = obj    ! add it
			return j
		}
	}
	return false
}

replace WordisNumber(w)
{
	select w
	case "zero", "0":       return 0
	case "one", "1":        return 1
	case "two", "2":        return 2
	case "three", "3":      return 3
	case "four", "4":       return 4
	case "five", "5":       return 5
	case "six", "6":        return 6
	case "seven", "7":      return 7
	case "eight", "8":      return 8
	case "nine", "9":       return 9
	case "ten", "10":       return 10
	case -1
	{
		local a
		a = StringToNumber(parse$)
		if a
			return a
	}
	return -1
}
#endif ! #ifset USE_PLURAL_OBJECTS

#ifset USE_ATTACHABLES
!\ Roody's note: I had to add the "k not in newloc" check so that it doesn't
try to re-move objects that have already been moved.  Maybe this will break
something since it's hard to keep track of everything that's going on, but
cursory testing seems to work ok.  \!
replace Attachable_MoveChildren(obj, oldloc, newloc)
{
	local i, k, first

	for i in obj
	{
		k = Attachable_MoveAttached(i, obj, oldloc, newloc)
		if not first and k not in newloc:  first = k
	}
	return first
}

!\ This routine only has support for one object being pushed which I figure
   works for most games, but I still gave the system its own routine in case
	anyone needs to code a more complex system. \!
routine CheckForAttachments(obj,loc)
{
	local i, j

	for i in loc
	{
		for (j=1; j<=i.#attached_to; j++)
		{
			if i.attached_to #j = obj or
				Contains(obj, i.attached_to #j):
			{
				return i
			}
		}
	}
	for i in player
	{
		for (j=1; j<=i.#attached_to; j++)
		{
			if i.attached_to #j = obj or
				Contains(obj, i.attached_to #j):
			{
				return i
			}
		}
	}
	return 0
}

! Roody's note: Just replaced so that >TIE OBJECT TO ME gets a "you can't tie
! that to yourself." message instead of "you can't tie that to you."
! message
replace DoAttachObject
{
	if object.type ~= attachable
	{
		OMessage(attachable, 6)        ! "You can't (attach) that."
		return false
	}

	! Match the verb
	if not InList(object, attach_verbs, VerbWord)
	{
		OMessage(attachable, 7, object.attach_verb)    ! wrong verb
		return false
	}

	if not xobject
	{
		OMessage(attachable, 8)        ! "Be more specific..."
		return false
	}

	if not CheckReach(xobject)
		return false

#ifclear NO_VERBS
	if object.attach_take and object not in player
	{
		OMessage(attachable, 9)         ! "(taking it first)"
		Perform(&DoGet, object)
		if object not in player
			return false
		Main                    ! counts as a turn
	}
#endif

	! See if the attach-to object is appropriate
	if object.attachable_to and not InList(object, attachable_to, xobject)
	{
		! "You can't (attach) it to that."
		RLibOMessage(attachable, 2)
		return false
	}

	! Make sure it's not already attached
	if InList(object, attached_to, xobject)
	{
		! "Except that it is already (attached) to that."
		OMessage(attachable, 11)
		return false
	}

	if not ObjectAttach(object, xobject)
	{
		! "Except that it is already (attached) to (list)..."
		OMessage(attachable, 12)
		return false
	}

	if not xobject.after
	{
		if not object.after
			! "You (attach) it to..."
			OMessage(attachable, 13)
	}

	return true
}

! ObjectisAttached returns true if something in <oldloc> is keeping
! <obj> from moving to <newloc>.

! Roody's note- got rid of a couple jumps and added a special response for
! pushed-objects-with-attachments
replace ObjectisAttached(obj, oldloc, newloc)
{
	local check

	if oldloc = newloc
		return false

! Check the children of the object in question:

	check = Attachable_CheckChildren(obj, newloc)
	if not check
! Failing finding any attachments there, use the roundabout method of
! checking the location:
		check = Attachable_CheckAttachedTo(obj, oldloc, newloc)
	if not check
		return false                    ! nothing is stopping it


	if check = -1                   ! something already printed
		return true

#ifclear NO_VERBS

#ifset USE_VEHICLES
	if verbroutine = &DoGo or verbroutine = &DoMoveinVehicle
#else
	if verbroutine = &DoGo
#endif
	{
		! "Not while it is still (attached) to..."
		OMessage(attachable, 18, check)
	}
#ifset USE_ROLLABLES
	elseif verbroutine = &DoPushDirTo
		RLibOMessage(attachable, 1, check, object)
	! "You can't move the <blank> while it's attached to the <thing>"
#endif
	else
	{
#endif
		! "You can't move whatever is (attached) to it..."
		OMessage(attachable, 19, check)
#ifclear NO_VERBS
	}
#endif

	return true
}
#endif ! USE_ATTACHABLES
#endif ! ifclear NO_OBJLIB

!----------------------------------------------------------------------------
!* REPLACED RESOURCE.H CODE
!----------------------------------------------------------------------------
#ifset USE_JUKEBOX
#set USE_TIME_SYSTEM
#include "resource.h"
#endif

#ifset _RESOURCE_H
!\ Roody's note: We replace some of the resource.h routines so multimedia is
never played under "minimal" glk interpreters like Gargoyle. Now, our
resources won't be ripped from our games!

If you want to be benevolent and allow pictures and music under Gargoyle,
set the allow_gargoyle global to true.
\!

global allow_gargoyle

replace LoadPicture(resfile, pic)
{
	if not display.hasgraphics or (system(61) and not allow_gargoyle):  return false

	if pic = ""
	{
		pic = resfile
		resfile = last_resourcefile
	}

	picture resfile, pic

	last_resourcefile = resfile

	if system_status:  return false

	return true
}

!\ PictureinText doesn't work at all in Gargoyle. If you DO want pictures
in Gargoyle, use LoadPicture \!

replace PictureinText(resfile, pic, width, height, preserve)
{
	local i

	if (not display.hasgraphics) or system(61):  return false

	Font(PROP_OFF)

	! Print blank lines the height of the picture, but only if images
	! aren't being routed to an external destination
	!
	if (display.hasgraphics ~= 2)
	{
		for (i=1; i<=height; i++)
			print ""
	}

	if width = 0:  width = 1
	if height = 0: height = 1

	! Create a properly sized window at the bottom-left corner of the
	! screen to load the picture in
	!
	window 1, (display.screenheight-height), width,
		(display.screenheight)
	{
		LoadPicture(resfile, pic)
	}

	! If any number of lines was given to preserve, temporarily define
	! a window of that height in order to make it the lowest-defined
	! window--i.e., the top of the main scrolling region
	!
	if preserve
	{
		window 0
		window preserve
		{}
	}

	print ""
	Font(DEFAULT_FONT)

	if system_status:  return false

	return true
}

replace PlayMusic(resfile, song, volume, loop, force)
{
	if (system(61) and not allow_gargoyle)
		return false
	! If the given song is already playing:
	if audio.current_music_resfile = resfile and
		audio.current_music = song
	{
		if not force
			return
		else
			music 0
	}

	local vol
	vol = volume
	if vol = 0
		vol = 100
	elseif vol < 0
		vol = 0

	if resfile = MUSIC_STOP and not song
	{
		music 0
		audio.current_music = 0
	}
	else
	{
		if not loop
		{
			music resfile, song, vol
#ifset USE_JUKEBOX
			if not system_status and not hugor is switchedon
				GetCurrentTime(song_start)
#endif
		}
		else
			music repeat resfile, song, vol
		audio.current_music = song
		audio.current_music_looping = loop
	}

	audio.current_music_resfile = resfile
	last_resourcefile = resfile
	audio.current_music_volume = volume

	if system_status:  return false

	return true
}

replace PlaySound(resfile, s, volume, loop, force)
{
	if (system(61) and not allow_gargoyle)
		return false
	! Due to the nature of samples, always restart the sound
	force = true

	! If the given sound is already playing:
	if audio.current_sound_resfile = resfile and
		audio.current_sound = s
	{
		if not force
			return
		else
			sound 0
	}

	local vol
	vol = volume
	if vol = 0
		vol = 100
	elseif vol < 0
		vol = 0

	if resfile = SOUND_STOP and not s
	{
		sound 0
		audio.current_sound = 0
	}
	else
	{
		if not loop
			sound resfile, s, vol
		else
			sound repeat resfile, s, vol
		audio.current_sound = s
		audio.current_sound_looping = loop
	}

	audio.current_sound_resfile = resfile
	last_resourcefile = resfile
	audio.current_sound_volume = volume

	if system_status:  return false

	return true
}
#endif ! _RESOURCE_H

!----------------------------------------------------------------------------
!* REPLACED WINDOW.H CODE
!----------------------------------------------------------------------------
#ifset _WINDOW_H
! Roody's note: added the window_class type to help out object-tree printing
replace window_class
{
	win_title ""
	type window_class
	win_left	0		! default coordinates
	win_top		0
	win_right	0
	win_bottom	0

	win_textcolor
		return TEXTCOLOR
	win_backcolor
		return BGCOLOR
	win_titlecolor
		return SL_TEXTCOLOR
	win_titleback
		return SL_BGCOLOR

	win_active false

	win_init
	{
		local x, y, titlelen, width, height

		titlelen = string(_win_title_string, self.win_title, 64)

		height = self.win_bottom - self.win_top
		if height++ = 0:  height = 10
		if self.win_title:  height++

		width =  self.win_right - self.win_left
		if width++ = 0:  width = 20
		if titlelen > width
			width = titlelen+2

		! The default position is centered in the middle of
		! the screen
		!
		x = self.win_left
		if x = 0:  x = display.screenwidth/2 - width/2 + 1
		y = self.win_top
		if y = 0:  y = display.screenheight/2 - height/2 + 1

		! Check to make sure the window will actually fit on the
		! screen--and if it doesn't, make it
		!
		if x + width > display.screenwidth + 1
		{
			if width > display.screenwidth
				width = display.screenwidth
			x = display.screenwidth - width + 1
		}
		if y + height > display.screenheight + 1
		{
			if height > display.screenheight
				height = display.screenheight
			y = display.screenheight - height + 1
		}

		! Draw the window, in the supplied color(s), with or
		! without a static title:
		!
		window x, y, (x+width-1), (y+height-1)
		{
			color self.win_textcolor, self.win_backcolor
			cls

			if self.win_title ~= ""
			{
				locate 1, 1
				color self.win_titlecolor,
					self.win_titleback

				print to ((width-titlelen)/2); \
					self.win_title; to width
			}

		}

		! If necessary, redefine the window below the static
		! title:
		!
		if self.win_title ~= ""
		{
			window x, (y+1), (x+width-1), (y+height-1)
			{}			! empty window body
		}

		color self.win_textcolor, self.win_backcolor
		self.win_active = true
	}

	win_clear
	{
		if self.win_active
		{
			color self.win_textcolor, self.win_backcolor
			cls
		}
	}

	win_end
	{
		color TEXTCOLOR, BGCOLOR
		self.win_active = false
	}
}
#endif

!----------------------------------------------------------------------------
!* REPLACED CHECKHELD OBJECT
!----------------------------------------------------------------------------
! Roody's note: Made a checkheld_holder object to keep the checkheld object in
! so its parent is not the nothing object (as ResetCheckheld gets easily
! confused by unintended objects sharing a parent with checkheld.
!----------------------------------------------------------------------------
#ifset USE_CHECKHELD
object checkheld_holder
{}
! The checkheld limit constant as defined by verblib.h is not replaceable, so
! here is a replaceable version
#if undefined NEW_CHECKHELD_LIMIT
constant NEW_CHECKHELD_LIMIT 64
#endif

replace checkheld	! 'active' when active; 'plural' for "~all", etc.
{
	misc #NEW_CHECKHELD_LIMIT
	size 0		! # of managed objects being managed
	is hidden
	in checkheld_holder
}

replace ResetCheckHeld
{
	local i, obj
	for (i=1; i<=checkheld.size; i++)
	{
		obj = checkheld.misc #i
		if obj is checkheld_flag and obj in parent(checkheld)
			move obj to player
		obj is not checkheld_flag
	}
	checkheld.size = 0
	checkheld is not plural
	checkheld is not active
	move checkheld to checkheld_holder
}
#endif  ! #ifset USE_CHECKHELD

!----------------------------------------------------------------------------
!* REPLACED VERBLIB.H ROUTINES
!----------------------------------------------------------------------------

! Roody's note: This DoAskQuestion uses the DetermineSpeaking routine to
! guess the speaking global when it is not set
!----------------------------------------------------------------------------
replace DoAskQuestion
{
	if speaking = 0
	{
		DetermineSpeaking
		if speaking = 0
		{
			VMessage(&DoAskQuestion)        ! "Not talking to anybody..."
			return false
		}
	}
	if xobject
	{
		ParseError(6)
		return false
	}

	return Perform(&DoAsk, speaking, object)
}

! Roody's note: If there's only one person to speak to, make that person the
! obvious recipient of, e.g., a non-specific "ask about...".
! From Future Boy! (Kent Tessman)

routine DetermineSpeaking
{
	local i, spk, count

	for i in location
	{
		if i is living and i is not hidden
		{
			spk = i
			count++
		}
	}

	if count = 1:  speaking = spk
}

! Roody's note: added a special message call for the player object
replace DoDrink
{
	if not CheckReach(object):  return false

	if not object.after
	{
		if object = player
			RlibMessage(&DoDrink)   ! "You can't drink yourself."
		else
			VMessage(&DoDrink)         ! "You can't drink that."
	}
	else
		return true
}

! Roody's note: added a special message call for the player object
replace DoEat
{
	if not CheckReach(object):  return false

	if not object.after
	{
		if object = player
			RlibMessage(&DoEat)   ! "You can't eat yourself."
		else
			VMessage(&DoEat)         ! "You can't eat that."
	}
	else
		return true
}

! Roody's note: replaced so the container/platform check comes before the
! children check.
replace DoEmpty
{
	local a, b, obj, xobj
	local thisobj, nextobj

	CalculateHolding(object)

	if object is container, openable, not open
	{
		VMessage(&DoEmpty, 1)           ! "It's closed."
		return true
	}
	if object is not container, platform
	{
		ParseError(12, object)
		return false
	}
	if not children(object)
	{
		VMessage(&DoEmpty, 2)           ! "It's already empty."
		return true
	}

	thisobj = child(object)
	while thisobj
	{
		nextobj = sibling(thisobj)

		print thisobj.name; ":  ";

		if thisobj is static
			VMessage(&DoEmpty, 3)    ! "You can't move that."
		else
		{
			a = player.holding
			b = thisobj
			obj = object
			xobj = xobject

			if player not in location and
				(parent(player) is platform or
					parent(player) is container) and
				not xobject:

				Perform(&DoPutIn, b, parent(player))
			else
				Perform(&DoDrop, b)

			object = obj
			xobject = xobj
			player.holding = a
			if object.#holding
			{
				if b not in object
					object.holding = object.holding - b.size
			}
		}

		thisobj = nextobj
	}

	run object.after
	return true
}

#ifset NEW_EMPTY
! Since the NEW_EMPTY system gives several new behaviors for emptying,
! I changed the DoEmpty call to not use Perform so authors can do
! specific checks for DoEmptyGround

replace DoEmptyGround                   ! to override vehicles, etc.
{
	local val
	xobject = 1
	val = call &DoEmpty
!	return Perform(&DoEmpty, object, xobject)
	return val
}
#endif

! Roody's note: Mostly not changed. Just commented out the part where word[1]
! is cleared and made the for loop break earlier. Also tweaked one of the
! messages in RlibMessage so that >ENTER ME has a better response.
replace DoEnter
{
#ifclear NO_OBJLIB
	if not object or object in direction
#else
	if not object
#endif
	{
		local i, obj, count
		for i in location
		{
			if i is enterable
			{
				obj = i
				count++
				if count = 2
					break
			}
		}
		if count = 1
		{
			object = obj
		}
		else
		{
			VMessage(&DoEnter, 1)    ! "Be a little more specific..."
			return false
		}

		return Perform(&DoEnter, object)
	}

	if object = player
	{
		RlibMessage(&DoEnter, 1) ! "You can't enter yourself."
		return
	}
	! To prevent endless loops if the player_character class
	! automatically resets the object to in_obj if word[1] = "in"
!	word[1] = ""    ! causing problems

	if &object.door_to
		return Perform(&DoGo, object)   ! routine
	elseif object.door_to
		return Perform(&DoGo, object)   ! object

	if object is not enterable or Contains(player, object)
		RlibMessage(&DoEnter, 1)    ! "You can't enter that."
	elseif player in object
		VMessage(&DoEnter, 3)    ! already in it
	elseif player not in location
		RlibMessage(&DoGo, 1)       ! "You'll have to get up..."
	elseif object is openable, not open
		VMessage(&DoLookIn, 1)   ! "X is closed."
	else
	{
		move player to object
		if not object.after
			VMessage(&DoEnter, 4)    ! "You get in..."
		if (FORMAT & DESCFORM_I)
			""
		object is not quiet
		DescribePlace(location)
		return true
	}
	return
}

! Roody's note: Fixed a bug Juhana Leinonen found where "X is closed."
! messages weren't properly printing the parent's name.
! Also added support for enterable supercontainers.

replace DoExit
{
	local p
	if object = player
	{
		RlibMessage(&DoExit, 1) ! "Trying to get out of your head? Futile."
		return
	}
#ifclear NO_OBJLIB
	! >GO OUT winds up calling DoExit with object = out_obj, thanks to
	! the direction-parsing code in Perform().  English ambiguities being
	! what they are, we correct that interpretation of "out" here, and
	! treat the command as a generic instruction to exit whatever
	! container context we may be in.
	if object = out_obj
		object = nothing

	if object = nothing or object = location
	{
		if player in location and out_obj in direction
		{
			word[1] = out_obj.noun
			word[2] = ""
			return Perform(&DoGo)
		}
	}
	elseif object = d_obj and player in location
	{
		return Perform(&DoGo, object)
	}
#endif

	p = parent(player)

	local parentcheck

#ifset USE_SUPERCONTAINER
	if p.type = SuperContainer
	{
		if InList(p, contents_on, player)
			parentcheck = true
	}
#endif

#ifclear NO_OBJLIB
	if object and player not in object
#else
	if object and player not in object
#endif
		VMessage(&DoExit, 1)             ! "You aren't in that."
	elseif (p is openable, not open) and not parentcheck
	{
		object = p
		VMessage(&DoLookIn, 1)           ! "X is closed."
	}
	else
	{
		if object = nothing
			object = p
		move player to location
		if not object.after
			RlibMessage(&DoExit, 2)      ! "You get out.."
	}
	return true
}

! Roody's note: Juhana found a bug where "X is closed." messages were
! triggered when trying to grab things from a closed enterable container
! the player ALSO is in. Added a quick fix but it's possible DoGet could
! be more efficient, as some of the instances will also be checked by
! CheckReach.
! Changed up the order of things being checked, redirected some clothing
! items to DoTakeOff, added a default "so-and-so is wearing that" message
! for taking other clothes, added optional Anchorhead-style automatic
! examining. Code could probably be cleaner but it should work.

replace DoGet
{
	local b, p

	if Contains(object, player)
	{
		if object is static
			VMessage(&DoGet, 7)	! "You can't take that."
		else
			VMessage(&DoGet, 6)     ! "Not while you're in/on it..."
		return false
	}
	if xobject
	{
		while true
		{
			if not CheckReach(xobject)
				return false
			elseif xobject = object and object ~= player
			{
				ParseError(6) ! That doesn't make any sense.
			}
			elseif object is clothing,worn ! assumes xobject is living because
			                               ! both clothing and worn are used
				return Perform(&DoTakeOff,object)
			else
				break
			return false
		}
	}
	if object in player
	{
		VMessage(&DoGet, 1)      ! "You already have that."
		return true
	}
	elseif object = player
	{
		VMessage(&DoGet, 2)     ! player trying to get player
		return false
	}
	elseif object is living and object is static
	{
		VMessage(&DoGet, 3)     ! player trying to get character
		return false
	}

	p = parent(object)

	if p ~= location
	{
		if not CheckReach(p)
			return false
		if (p is openable, not open) and p is container and p ~= parent(player)
		{
			VMessage(&DoGet, 5)     ! "X is closed."
			return false
		}
		if p is living
		{
			if (object is clothing,worn)
			{
				RLibMessage(&DoGet,1,p) ! "so-and-so is wearing that."
				return true
			}
			if p is unfriendly
			{
				VMessage(&DoGet, 4)      ! "X doesn't want to give it to you."
				return true
			}
		}
	}
	if not CheckReach(object)
		return false
	elseif object is static
	{
		VMessage(&DoGet, 7)      ! "You can't take that."
		return true
	}

	! Because the engine calls location.before
	if (parent(object)~=location)
		b = parent(object).before

	if not b
	{
		xobject = parent(object)

		if object not in location
		{
			CalculateHolding(xobject)
			p = xobject
		}

		if Acquire(player, object)
		{
			object is not hidden

			if not object.after
			{
				! Again, to avoid duplication or
				! echoing:
				!
				b = 0
				if xobject ~= location
					b = xobject.after

				if b = false
				{
#ifset AUTOMATIC_EXAMINE ! unexamined objects are automatically examined
					if object is not examined and &object.long_desc
					{
						! "You pick up the <object>.";
						RLibMessage(&DoGet,2)
						print AFTER_PERIOD;
						Perform(&DoLook, object)
					}
					else
#endif
					! "Taken."
						VMessage(&DoGet, 8)
				}
			}
		}
		else
			! "You're carrying too much to take that."
			VMessage(&DoGet, 9)
	}
	return true
}

! Roody's note: I created a global to be set by DoGo called "exit_type"
! It keeps track of whether the successful direction used uses a door
! or direction. Mainly, this just exists so DescribePlace can have some
! kind of info saved in case you want to write code for specifying
! when pronouns should be assigned to room items without extensive
! word array checking.
global exit_type
constant non_door_portal 3

! Roody's note: Makes "you'll have to get up" message more container/platform
! specific also has some code to work with new vehicle replacement. Also got
! rid of jump. Also changed the answer for trying to go to a nearby,
! non-enterable object from "You can't enter that" to "<The object> is right
! here."
! Also, if USE_ELEVATED_PLATFORMS is set, enterable objects with the "elevated"
! attribute allow >DOWN as an exit.

replace DoGo
{
	local moveto, JumpToEnd, skip_ahead ! , vehicle_check
#ifset NO_OBJLIB
	local wordnum, m
#endif

	if object = parent(player) or ! make sure player isn't already in object
		not object ! or (not parent(player) and word[2] = "out")
		skip_ahead = true

	if player not in location and   ! sitting on or in an obj.
		not skip_ahead
	{
		local a, b

		b = parent(player).before	! i.e., a vehicle, etc.
		if b = 1
			return false ! so the error message doesn't take up a turn
#ifclear NO_OBJLIB
		elseif not b
		{
#ifset USE_ELEVATED_PLATFORMS
			if parent(player) is elevated and object = d_obj
				a++
			elseif parent(player) is platform and object = u_obj and
			parent(player) is not elevated
#else
			if parent(player) is platform and object = u_obj
#endif
				a++
			elseif parent(player) is container and object = out_obj
				a++
		}
#endif

		if	not a and not b
		{
#ifset SMART_PARENT_DIRECTIONS
			if not CanGoDir
				VMessage(&DoGo, 2)      ! "You can't go that way."
			else
#endif
				RlibMessage(&DoGo, 1)      ! "You'll have to get up..."
			return false
		}
		else
			return Perform(&DoExit,parent(player))
	}


	if obstacle and not skip_ahead
	{
#ifclear NO_OBJLIB
		VMessage(&DoGo, 1)		! "X stops you from going..."
		return true
#endif
	}

#ifclear NO_OBJLIB
	! See if the object is one of the current direction set

	if object.type = direction and object in direction
	{
		moveto = object
		JumpToEnd = true
	}
#endif

if not JumpToEnd
{
#ifclear NO_OBJLIB
	if not moveto
	{
#else
	wordnum = 1                             ! various phrasings
	if words > 1 and word[2] ~= ""
		wordnum = 2
	if word[2] = "to", "through"
		wordnum = 3
	elseif (word[2] = "in" or word[2] = "inside") and words > 2
		wordnum = 3

	select word[wordnum]
		case "north", "n":      m = n_to
		case "south", "s":      m = s_to
		case "east", "e":       m = e_to
		case "west", "w":       m = w_to
		case "northwest", "nw": m = nw_to
		case "northeast", "ne": m = ne_to
		case "southeast", "se": m = se_to
		case "southwest", "sw": m = sw_to
		case "up", "u":         m = u_to
		case "down", "d":       m = d_to
		case "in", "inside":    m = in_to
		case "out", "outside":  m = out_to

	if not m
	{
#endif
		if not object
		{
			! ParseError(6)  ! doesn't make any sense
			VMessage(&DoGo, 4)	! "Which way...?"
			return
		}

		if not &object.door_to and (word[2] ~= "to","toward","towards")
		{
			if word[2] = "out"
			{
				ParseError(6)  ! doesn't make any sense
				return
			}
			if not object.door_to and object is enterable
				return Perform(&DoEnter, object)
		}

		moveto = object.door_to
		if not moveto
		{
			if object is container and (word[2] ~= "to","toward","towards")
				VMessage(&DoEnter, 2)  ! "You can't enter..."
			else
				RLibMessage(&DoGo, 2) ! "The <object> is right here."
			return
		}
		if moveto = 1
			return true

#ifclear NO_OBJLIB
		if object.type = door
			exit_type = door
		else
			exit_type = non_door_portal
#endif
	}
}  !  if not JumpToEnd bracket

#ifclear NO_OBJLIB

!  may need to add some code for elevated platforms. not sure yet.

	if moveto.type = direction and moveto in direction
	{
		if player not in location and object = out_obj
		{
			if parent(player) is platform
			{
				RlibMessage(&DoGo, 1) ! "You'll have to get up."
				return false
			}
			else
				return Perform(&DoExit)
		}
		elseif player not in location and object = u_obj
		{
			if parent(player) is container
			{
				RlibMessage(&DoGo, 1) ! "You'll have to get out."
				return false
			}
			else
				return Perform(&DoExit, parent(player))

		}

		if not object
		{
			object = moveto
			if object.before
				return true
		}
!		else:  object = moveto
		moveto = location.(moveto.dir_to)

		if &moveto.door_to or moveto.type = door
		{
			moveto = moveto.door_to
			exit_type = door
		}
		elseif moveto.type ~= room and moveto.door_to
		{
			moveto = moveto.door_to
			exit_type = non_door_portal
		}
		elseif moveto is enterable and moveto.type ~= room
			return Perform(&DoEnter, moveto)
	}
#else   ! ifset NO_OBJLIB
	if m
	{
		if player not in location and m = out_to
		{
			if parent(player) is platform
			{
				RlibMessage(&DoGo, 1) ! "You'll have to get up."
				return false
			}
			else
				return Perform(&DoExit)
		}
		elseif player not in location and m = u_to
		{
			if parent(player) is container
			{
				RlibMessage(&DoGo, 1) ! "You'll have to get out."
				return false
			}
			else
				return Perform(&DoExit, parent(player))
		}

		moveto = location.m
		if moveto.door_to
			moveto = moveto.door_to
		exit_type = non_door_portal
	}
#endif  !  end of ifset NO_OBJLIB

	if moveto = false
	{
		exit_type = 0
		if not location.cant_go
			RlibMessage(&DoGo, 3)      ! "You can't go that way."
		return false
	}
	elseif moveto = true                    ! already printed message
	{
		exit_type = 0
		return true                     ! (moveto is never 1)
	}
	elseif player not in location ! and           ! sitting on or in an obj.
!		not vehicle_check  ! make sure it's not a vehicle that can go through
		                   ! this door
	{
		exit_type = 0
		if parent(player) = moveto  ! does the direction lead to parent(player)?
			VMessage(&DoEnter, 3)    ! already in it message
		else
			RlibMessage(&DoGo, 1)              ! "You'll have to get up..."
	}
	else
	{
!\ Recursive call issues
		m = verbroutine                 ! check room exiting
		verbroutine = &DoExit
		if location.before
			return true
		verbroutine = m
\!
#ifclear NO_OBJLIB
		if not exit_type
			exit_type = direction
#endif
		MovePlayer(moveto)

		return true
	}
}

! Roody's note: added a special message call for the player object
replace DoHit
{
	if not CheckReach(object):  return false

	if not object.after
	{
		if object = player
			RlibMessage(&DoHit)   ! "Venting your frustration on yourself..."
		else
			VMessage(&DoHit)         ! "Doesn't accomplish much..."
	}
	else
		return true
}

! Roody's note: Made it so the verboutine is always DoInventory so this routine
!  can be called for inventory windows in graphical games.
replace DoInventory
{
	local tempformat,v

	if word[2] = "tall"
		INVENTORY_MASK = INVENTORY_MASK | LIST_F
	elseif word[2] = "wide"
		INVENTORY_MASK = INVENTORY_MASK & ~LIST_F
	v = verbroutine
	verbroutine = &DoInventory
	tempformat = FORMAT
	FORMAT = FORMAT | INVENTORY_F | GROUPPLURALS_F | NOINDENT_F
	FORMAT = FORMAT | INVENTORY_MASK
	list_nest = 0
	if not WhatsIn(player)
		VMessage(&DoInventory)   ! "You aren't carrying anything."
	FORMAT = tempformat
	verbroutine = v

	return true
}

! Roody's note: This version of DoListen is more friendly to room-listening.
replace DoListen
{
	if not object
	{
		if not location.after
		{
			RLibMessage(&DoListen, 1)  ! "You hear nothing unexpected."
			return true
		}
		verbroutine = ""
		return true
	}
	elseif not object.after
!		VMessage(&DoListen, 2)   ! "Not making any sound..."
		RLibMessage(&DoListen, 2)  ! "That would serve no purpose."
	! we have to clear verbroutine or else location.after.DoListen will run again
	verbroutine = ""
	return true
}

!\ Roody's note: The following DoUnlock and DoLock routines allow for multiple
key objects per locked item. A more "feature-full" version of this kind of
thing is Jason McWright's multikey.h extension.
\!
replace DoUnlock
{
	local a, list_key, cant_reach
	if not CheckReach(object):  return false

	!\ because setupdirectionobjects can sometimes dictate an xobject where we
	don't want it, we'll get rid of directional xobjects here \!
#ifclear NO_OBJLIB
	if xobject and xobject.type = direction
		xobject = 0
#endif

	if xobject ~= 0
	{
		a = InList(object,key_object,xobject)
		if not a
		{
			VMessage(&DoUnlock, 1)  ! "Doesn't seem to work..."
			return true
		}
	}
	if object is not locked
	{
		VMessage(&DoUnlock, 3)           ! already unlocked
		return true
	}
	if object.key_object and not xobject
	{
		for (a=1; a <= object.#key_object ; a++)
		{
			if Contains(player, object.key_object #a)
			{
				if FindObject(object.key_object #a,location) = 1
				{
					xobject = object.key_object #a
					list_key = true
					break
				}
				else
					cant_reach = object.key_object #a
			}
		}
		if not xobject or xobject is quiet
		{
			if xobject is quiet
				RLibMessage(&DoUnlock, 1) ! "It is unclear what you'd like to
				                          !  unlock the <blank> with."
			elseif cant_reach
				RLibMessage(&DoUnLock, 2, cant_reach)     ! "You can't reach the..."
			else
				VMessage(&DoUnlock, 2)           ! no key that fits
			return true
		}
	}

	if object is not locked
		VMessage(&DoUnlock, 3)           ! already unlocked
	else
	{
		object is not locked
		if xobject
			xobject is not quiet
		if not object.after
		{
			if not xobject.after
				RLibMessage(&DoUnlock, 3, list_key)   ! "Unlocked."
		}
	}
	return true
}

replace DoLock
{
	local a,cant_reach,list_key
	if not CheckReach(object):  return false

	if xobject ~= 0
	{
		a = InList(object,key_object,xobject)
		if not a
		{
			VMessage(&DoUnlock, 1)  ! "Doesn't seem to work..."
			return true
		}
	}
	if object is locked
	{
		VMessage(&DoLock, 1)             ! already locked
		return true
	}
	if object.key_object and not xobject
	{
		for (a=1; a <= object.#key_object ; a++)
		{
			if Contains(player, object.key_object #a)
			{
				if FindObject(object.key_object #a,location) = 1
				{
					xobject = object.key_object #a
					list_key = true
					break
				}
				else
					cant_reach = object.key_object #a
			}
		}
		if not xobject or xobject is quiet
		{
			if xobject is quiet
			{
				RLibMessage(&DoLock, 1) ! "It is unclear what you'd like to
								  !  lock the <blank> with."
			}
			elseif cant_reach
				RLibMessage(&DoUnLock, 2, cant_reach)     ! "You can't reach the..."
			else
				VMessage(&DoUnlock, 2)           ! no key that fits
			return true
		}
	}

	if object is locked
		VMessage(&DoLock, 1)             ! already locked
	elseif object is open
		VMessage(&DoLock, 2)             ! "Have to close it first..."
	else
	{
		if xobject
			xobject is not quiet
		object is locked
		if not object.after
		{
			if not xobject.after
				RLibMessage(&DoLock, 2, list_key)     ! "Locked."
		}
	}
	return true
}

! Replaced LOOK routines\

!\ Roody's note: I added a flag called NO_LOOK_TURNS. #set it if you want your
game to not count LOOK moves (LOOK, EXAMINE <object>, LOOK THROUGH <object)
as turns.

Added children-listing to all transparent objects.

Added code for smarter children listing. Added Zork-style default
responses for containers with no long_desc. Added "object = -1" so
pronouns are set.\!

replace DoLook
{
	local i,skip_ahead, no_fullstop, has_children, count

	if not light_source
		VMessage(&DoLook, 1)     ! "It's too dark to see anything."
	else
	{
		if ( object is transparent or !(object is living, transparent) or
			object is platform or (object is container and
			(object is open or object is not openable))) and
			object is not quiet ! and object is not already_listed
		{
			for i in object
			{
				i is not already_listed
				if i is not hidden
				{
					has_children = true
					count++
				}
			}
		}

		if not object.long_desc
		{
#ifclear FORCE_DEFAULT_MESSAGE
			if object is container and
			object is not quiet and object is not living
			{
				if (object is openable,open)
					print "It's open.";
				Perform(&DoLookIn,object) ! so we get "it is closed." if closed
				skip_ahead = true
			}
			elseif has_children
				no_fullstop = true
			else
#endif
				! "Looks just like you'd expect..."
				VMessage(&DoLook, 2)
		}

		if (object is transparent or !(object is living, transparent) or
			object is platform or (object is container and
			(object is open or object is not openable))) and
			object is not quiet ! and object is not already_listed
		{
			has_children = false
			for i in object
			{
				if i is not hidden and i is not already_listed
				{
					has_children = true
					break
				}
			}
		}

		if i and object ~= player and not skip_ahead
		{
			if count = 1
				NEW_PARSE &= ~PRONOUNS_SET
			local tempformat
			tempformat = FORMAT
			FORMAT = FORMAT | NOINDENT_F
			list_nest = 0
			if not no_fullstop
				print ""
			WhatsIn(object,has_children)
			FORMAT = tempformat
			NEW_PARSE |= PRONOUNS_SET
		}

		run object.after
#ifset AUTOMATIC_EXAMINE   ! objects are examined automatically when picked up
		if object is not examined
			object is examined
#endif
#ifclear NO_LOOK_TURNS
		return true
#endif
	}
}

replace DoLookAround
{
	DescribePlace(location, true)
#ifclear NO_LOOK_TURNS
	return true
#endif
}

!\ Roody's note: I was unsure whether to have LOOK IN work like other LOOK
verbs when the NO_LOOK_TURNS flag is set, as it could be argued that LOOK IN
is more of an action-oriented verb, like SEARCH. For instance, I have LOOK UNDER
still take a turn (so if you DON'T want that, you'll have to replace it).

Just the same, I decided that with NO_LOOK_TURNS, LOOK IN defaults to
not-taking-a-turn, so if you want it to take a turn, you'll have to replace it.

Also, non-container, transparent items are now allowed.

The original Hugo routine only allowed "look in" to work on objects within
reach.  I've added code so ALL transparent objects can be looked in even if
the player is in an enterable object (like a chair).

Of course, there might be some openable objects that you'd want the player
to be able to look in even if they're in a parent object.  My best advice is
to give the object the transparent attribute when it is opened and remove
the attribute when it's closed.

Conversely, if there's a transparent object you DON'T want the player to be
able to look inside without being near (like a pool of water several yards
away), you might want to remove the transparent attribute when the player
enters a parent object (like a lifeguard chair) and give it back when the
player exits it.\!

replace DoLookIn
{
	local tempformat

	if object is not transparent
	{
		if not CheckReach(object):  return false
	}


	if not light_source
		VMessage(&DoLook, 1)     ! "It's too dark to see anything."
	elseif object is not container and object is not transparent and
		object is not platform
		ParseError(12,object) ! "You can't do that with the thing."
	elseif object is living
	{
	!\ I don't know how useful this will be, but I put in an after
	check for characters so you can have character-specific responses to
	LOOK IN <PERSON>. \!
		if object.after
			return true
		else
			ParseError(12,object) ! "You can't do that with so-and-so."
	}
	else
	{
		if object is container and ((object is openable and
			object is not open) and object is not transparent):
			VMessage(&DoLookIn, 1)           ! "It's closed."
		else
		{
			if not object.after
			{
				object is not quiet

				tempformat = FORMAT
				FORMAT = FORMAT | NOINDENT_F
				list_nest = 0

				NEW_PARSE &= ~PRONOUNS_SET
				local i
				i = WhatsIn(object)
				if not i
					VMessage(&DoLookIn, 2)   ! "It's empty."
				elseif i = 1
					NEW_PARSE |= PRONOUNS_SET

				FORMAT = tempformat
			}
		}
#ifclear NO_LOOK_TURNS
		return true
#endif
	}
}

! Roody's note- Replaced to disallow looking through characters
replace DoLookThrough
{
	if object is not static
	{
		if not CheckReach(object)
			return false
	}

	if object is transparent and object is not living
	{
		if object is container or children(object)
			return Perform(&DoLookIn, object)
		else
			return Perform(&DoLook, object)
	}

#ifclear NO_OBJLIB
	elseif object.type = door
	{
		if object is not open
			VMessage(&DoLookIn, 1)   ! "It's closed."
		else
		{
			! "Through it you can see..."
			VMessage(&DoLookThrough, 1)

			if location = object.found_in #1
				print The(object.found_in #2); "."
			else
				print The(object.found_in #1); "."
		}
#ifclear NO_LOOK_TURNS
		return true
#endif
	}
#endif  ! NO_OBJLIB
	else
		VMessage(&DoLookThrough, 2)  ! "You can't see through that."
}

! Roody's note: Added a special message call for the player object
replace DoLookUnder
{
	if not CheckReach(object):  return false

	if object = player
		RlibMessage(&DoLookUnder)
	else
		VMessage(&DoLookUnder, 1)        ! "You don't find anything."
	return true
}

!\ Roody's note: roodylib, by default, supports PUSH and PULL. It redirects them
to DoMove if verbstub hasn't been included.  \!

replace DoMove
{
	if not CheckReach(object):  return false

	if object = player
	{
		RlibMessage(&DoMove, 1) ! "You can't move yourself."
		return
	}
#if defined DoPush
	if word[1] = "push", "shove", "press"
		return Perform(&DoPush, object)
#endif

#if defined DoPull
	if word[1] = "pull", "yank", "tug"
		return Perform(&DoPull, object)
#endif

	VMessage(&DoMove)                ! "You can't move that."
	return true
}

!\ Roody's note: Fixes a bug found by Juhana Leinonen where, if the player is
the only thing in the object being opened, an extra line is printed. This has a
fix for it although I haven't decided if it's an optimal solution.
\!
! Added Zork-style content listing to objects without object.after's
! and added "object = -1" so new pronoun settings stick
! and smarter content-listing

replace DoOpen
{
	local tempformat, light_check, skip_ahead, force_def

#ifset FORCE_DEFAULT_MESSAGES
	force_def = true
#endif
	if not CheckReach(object):  return false

	if object is not openable
	{
		VMessage(&DoOpen, 1)             ! "You can't open that."
		return
	}
	elseif object is open
		VMessage(&DoOpen, 2)             ! "It's already open."
	elseif object is locked
		VMessage(&DoOpen, 3)             ! "It's locked."
	else
	{
		object is open
#ifset MULTI_PCS
		MakeMovedVisited(object)
#else
		object is moved
#endif
		local i
		if not Contains(object,player) and object is not transparent
		{
			for i in object
			{
				if i is not hidden
					break
			}
		}
		if i
		{
			if not light_source
				light_check = FindLight(location)  ! in case the light source
                                               ! has been revealed
		}
		if not object.after
		{
			if not i or object is quiet or force_def
				VMessage(&DoOpen, 4)     ! "Opened."
			else
			{
				local x
				list_count = 0
				for x in object
				{
					if x is not hidden
						list_count++
				}
				if list_count = 1
					NEW_PARSE &= ~PRONOUNS_SET
				RLibMessage(&DoOpen,1) ! "opening the <blank> reveals"
				ListObjects(object)
				NEW_PARSE |= PRONOUNS_SET
				skip_ahead = true
			}
		}
		else
			skip_ahead = true ! object.after has to list the contents
			                  ! if it exists

		if i and object is not quiet and
			not skip_ahead
		{
			print ""
			if children(object) = 1
				NEW_PARSE &= ~PRONOUNS_SET
			tempformat = FORMAT
			FORMAT = FORMAT | NOINDENT_F
			list_nest = 0
			WhatsIn(object)
			FORMAT = tempformat
			NEW_PARSE |= PRONOUNS_SET
		}
	}
	if light_check
		Perform(&DoLookAround)
	return true
}

! Roody's note: First off, I changed the grammar for DoPutIn so &DoPutIn
! message one gets called at all, but I'm also replacing that message here
! since I didn't especially like the original wording.

replace DoPutIn
{
	if not xobject
		RlibMessage(&DoPutIn, 1)    ! "Put it in what?"
	elseif object is clothing and object is worn
		VMessage(&DoDrop, 1)     ! "Have to take it off first..."
	elseif xobject = location
		return Perform(&DoDrop,object)
	elseif xobject is container, openable, not open
		VMessage(&DoPutIn, 2)    ! "It's closed."
	elseif object = xobject
		VMessage(&DoPutIn, 3)    ! putting something in itself
	elseif xobject is not container and xobject is not platform
		VMessage(&DoPutIn, 7)    ! "Can't do that with..."
	elseif Contains(object, xobject)
		VMessage(&DoPutIn, 4)    ! putting a parent in its child
	elseif object in xobject
		VMessage(&DoPutIn, 8)	! "It's already in..."
	elseif CheckReach(object)
	{
		if CheckReach(xobject)
		{
			if Acquire(xobject,object)
			{
				if not object.after
				{
					if not xobject.after
						! "You put X in/on Y."
						VMessage(&DoPutIn, 5)
				}
			}
			else
				! "There's no room..."
				VMessage(&DoPutIn, 6)
		}
	}
	return true
}

!----------------------------------------------------------------------------
!\ Note about xverbs- I cleaned up the code to a bunch of these and tweaked
them so they can be called by EndGame/ProcessKey for more consistent
behavior. \!

! Roody's note: Changed DoQuit to give one final ending message

#if undefined PRESS_ANY_KEY
constant PRESS_ANY_KEY "[ press any key to exit ]"
#endif

#ifclear NO_XVERBS
replace DoQuit
{
	PrintScore
	VMessage(&DoQuit)                        ! "Are you sure?"
	GetInput
	if YesorNo = true
	{
		QuitGameText
		quit
	}
	else
		RLibMessage(&DoQuit,1) ! "Continuing on."
}

replace DoRestart
{
	if word[1] ~= "yes"
	{
		if MAX_SCORE
			PrintScore
		VMessage(&DoRestart, 1)                  ! "Are you sure?"
		GetInput
	}
	if YesorNo = true
	{
		SaveSettings("restart")
		if not restart
			VMessage(&DoRestart, 2)  ! "Unable to restart."
		else
			return true
	}
	else
	{
		RlibMessage(&DoRestart, 1) ! "\nContinuing on."
		if (FORMAT & DESCFORM_I)
			""
#ifclear USE_DARK_ROOM
		elseif not FindLight(location)
			""
#endif
		DescribePlace
#ifset NEW_FUSE
		fake_runevents
#endif
	}
}

!----------------------------------------------------------------------------
replace DoRestore
{
	SaveSettings("restore")
	if verbroutine = &EndGame
		""
	if restore
	{
		LoadSettings
		RestoreResponse
#ifset NEW_FUSE
		fake_runevents
#endif
		return true
	}
	else:  VMessage(&DoRestore, 2)           ! "Unable to restore."
}

!\ Roody's note: Mainly to match the new DoUndo behavior, I added a similar
 response routine for restore. I also added a screen clear since I figure
 it's sort of like jumping to another part of the book. \!

routine RestoreResponse
{
#ifset NO_ACCESSIBILITY
	local cheap
#endif
	if not (system(61) or cheap = 1)
		InitScreen
	PrintStatusLine
	VMessage(&DoRestore, 1)         ! "Restored."
	if (FORMAT & DESCFORM_I)
		""
#ifclear USE_DARK_ROOM
	elseif not FindLight(location)
		""
#endif
	DescribePlace(location, true)
}

!----------------------------------------------------------------------------
! Roody's note: Modified to work with any statustype with a MAX_SCORE
replace DoScore
{
	if not MAX_SCORE
		VMessage(&DoScore)               ! no scorekeeping
	else
		PrintScore
}

replace DoScriptOnOff
{
	if word[2] = "on" or words = 1
	{
		if betalib is special
			RLibMessage(&DoScriptOnOff, 1) ! "Transcription is already on."
		elseif (not scripton)
			VMessage(&DoScriptOnOff, 1) ! "Unable to begin transcription."
		else
		{
			betalib is special
			VMessage(&DoScriptOnOff, 2) ! "Transcription on."
		}
	}
	elseif word[2] = "off"
	{
		if betalib is not special
			RLibMessage(&DoScriptOnOff, 2) ! "Transcription is not currently on."
		elseif (not scriptoff)
			VMessage(&DoScriptOnOff, 3) ! Unable to end transcription."
		else
		{
			betalib is not special
			VMessage(&DoScriptOnOff, 4) ! "Transcription off."
		}
	}
}

#ifclear NO_RECORDING
replace DoRecordOnOff
{
	if VerbWord = "playback"
	{
		if word[2] = "off"
		{
			if betalib is not playback_on
				RLibMessage(&DoRecordOnOff,1) ! "No playback in progress."
			else
			{
!				betalib is not skip_pauses
				betalib is not playback_on
				RLibMessage(&DoRecordOnOff,2) ! "Playback completed."
			}
		}
		elseif betalib is record_on
			RLibMessage(&DoRecordOnOff,3) ! "No playback while recording."
		elseif betalib is playback_on
			RLibMessage(&DoRecordOnOff,4) ! "Playback already in progress."
		else
		{
			if not system(61) ! not a minimal port
			{
				RLibMessage(&DoRecordOnOff,5) ! "MORE skipping spiel"
				TopPause
				PrintStatusLine
				""
			}
			if not playback
				VMessage(&DoRecordOnOff, 1)  ! "Unable to begin..."
			else
			{
				VMessage(&DoRecordOnOff, 2)  ! "Playback beginning..."
				betalib is switchedon
			}
		}
	}
	elseif word[2] = "on" or words = 1
	{
		if betalib is playback_on
			RLibMessage(&DoRecordOnOff,7) ! "No recording during playback."
		elseif betalib is record_on
			RLibMessage(&DoRecordOnOff,8) ! "Recording already in progress."
		elseif not recordon
			VMessage(&DoRecordOnOff, 3)  ! "Unable to begin..."
		else
		{
			betalib is record_on
			VMessage(&DoRecordOnOff, 4)  ! "Recording on."
		}
	}
	elseif word[2] = "off"
	{
		if betalib is not record_on
			RLibMessage(&DoRecordOnOff,9) ! "No recording in progress."
		elseif not recordoff
			VMessage(&DoRecordOnOff, 5)  ! "Unable to end..."
		else
		{
			betalib is not record_on
			VMessage(&DoRecordOnOff, 6)  ! "Recording off."
		}
	}
}
#endif

replace DoUndo
{
	if verbroutine = &EndGame
		""
	if not UNDO_OFF
	{
		SaveSettings("undo")
		NEW_PARSE = NEW_PARSE & ~PARSE$_F
		if undo
		{
			LoadSettings
			UndoResponse
			parser_data[LAST_SINGLE_OBJECT] = 0
			return true
		}
		else
			RLibMessage(&UndoResponse,2)  ! "Unable to UNDO."
		return
	}
	else
		RLibMessage(&UndoResponse,2) ! "UNDO is not currently allowed."
}

#ifset CONTINUE_UNDO
global continue_undo

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

routine PrintCommand(arr,n)
{
	while array arr[n] ~= "" and n < array arr[]
	{
		select array arr[n]
			case "~and"
			{
!				if array arr[(n-1)] = "~and"
				if not VerbCheck(array arr[n+1])
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
#endif ! ifclear NO_XVERBS

object undolib "undolib"
{
	in init_instructions
	save_info
	{
		if word[LAST_TURN] ~= "undo"
			return false
		if ContinueUndoCheck
			return true
		if NEW_PARSE & PARSE$_F
			return false
#ifset SHOW_COMMANDS
		if oldword[1] or (last_actor and last_actor ~= player)
		{
			Font(ITALIC_ON)
			print "[ undoing ";
			Font(BOLD_ON|ITALIC_OFF)
			print ">";
			if last_actor and last_actor ~= player
			{
				print last_actor.name;
				if oldword[1] ~= ""
					print ", ";
			}
			PrintCommand(oldword,1)
			print " ";
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
!		if word[LAST_TURN] ~= "undo" ! or continue_undo
!			return false
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

!\ Roody's note: I thought it'd be better if there were a separate routine
  for what gets printed after a successful routine, so authors can easily
  configure the response without replacing the whole routine. Since this
  new replaceable routine should cover the same uses as the USE_AFTER_UNDO
  routine address array system, I have gotten rid of that. \!

routine UndoResponse
{
	local fail, v
	PrintStatusLine
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
	if not fail
	{
		local a
		a = CheckWordSetting("continue")
		if not a
			RlibMessage(&UndoResponse,1) ! "Undone."
		else
			word[a] = ""
	}
	if (FORMAT & DESCFORM_I)
		""
#ifclear USE_DARK_ROOM
	elseif not FindLight(location)
		""
#endif
	NEW_PARSE |= PRONOUNS_SET
	v = verbroutine
	verbroutine = &DoLookAround
	DescribePlace
	verbroutine = v
#ifset NEW_FUSE
	fake_runevents
#endif
}

! Roody's note: Fixes a typo from verblib.h version
replace DoStand
{
	if parent(parent(player)) = nothing     ! i.e., a room
		VMessage(&DoEnter, 3)           ! "A bit redundant..."
	else
		return Perform(&DoExit , (parent(player)))
}

! Roody's note: Added some other situations where >GET could be called.
replace DoTakeOff
{
!	if not Contains(player, object)
!	{
!		! So >REMOVE LOCK will >TAKE LOCK, etc.
!		return Perform(&DoGet, object)
!	}
!	else
	if parent(object) ~= player and
		not (object is worn and object is clothing)
	{
		! So >REMOVE LOCK will >TAKE LOCK, etc.
		return Perform(&DoGet, object)
	}
	if object is not clothing
	{
		VMessage(&DoTakeOff, 1)         ! "Can't do that..."
		return false
	}

	if object is not worn
		VMessage(&DoTakeOff, 2)          ! "You're not wearing that."
	else
	{
		if parent(object) ~= player
		{
			xobject = parent(object)
			if not CheckReach(xobject)
				return false
		}
		if xobject and not xobject.after
		{
			RLibMessage(&DoTakeOff, 1) ! "So-and-so doesn't let you."
		}
		else
		{
			object is not worn
			if not object.after
				VMessage(&DoTakeOff, 3)  ! "You take it off."
		}
	}
	return true
}

!\ Roody's note: Replaced so "keep waiting?" prompts can be ignored during
recording playback. \!
replace DoWait(count)                   ! count argument is from DoWaitUntil
{
	if object = 0
		count = WAIT_TURN_COUNT
	elseif count = 0
		count = object

	VMessage(&DoWait)                ! "Time passes..."
	event_flag = 0
	while --count
	{
		Main
		system(32) !(PAUSE_100TH_SECOND)  ! potential updating
		local a
#ifset DEBUG
		a = (debug_flags & D_PLAYBACK_HELPER)
#endif
		if event_flag and not a
			if not KeepWaiting
				return
	}
	event_flag = 0
	return true
}

!----------------------------------------------------------------------------
!* REPLACED VERBSTUB.H ROUTINES
!----------------------------------------------------------------------------
#ifclear _VERBSTUB_H
! Roody's note: SEARCH is a pretty common verb. Roodylib adds it regardless.
routine DoSearch
{}
#endif
! Roody's note: Fixes some pronoun stuff. Suggested by Mike Snyder.
replace DoSearch
{
	if object = player
	{
		"Search ";
		The(player)
		" indeed."
	}
	elseif object is container and child(object)
		Perform(&DoLookIn, object)
	elseif object is living
	{
		print CThe(object); MatchPlural(object, "doesn't", "don't");
		" let ";
		The(player)
		if object.pronouns
		{
			" search ";
			if object.pronouns #2
				print object.pronouns #2;
			else
				print object.pronoun;
		}
		print "."
	}
	else
	{
		CThe(player)
		MatchPlural(player, "doesn't", "don't")
		" find anything new."
	}
	return true
}

! Roody's note: Applies "DoListen" logic to DoSmell. Makes it a standard verb.
#ifclear _VERBSTUB_H
routine DoSmell
{}
#endif
replace DoSmell
{
	if not object
	{
		if not location.after
		{
			RLibMessage(&DoSmell,1) ! "You smell nothing unusual."
			return true
		}
		verbroutine = ""
		return true
	}
	elseif not object.after
		RLibMessage(&DoSmell,2) ! "Why do that?"
	! we have to clear verbroutine or else location.after.DoListen will run again
	verbroutine = ""
	return true
}

#ifset _VERBSTUB_H
! Roody's note: DoSwim now supports objects, but we'll assume that the object
! given doesn't make any sense.
replace DoSwim
{
	if object
		ParseError(6) ! "That doesn't make any sense."
	else
		"Not here, you won't."
}
#endif

!----------------------------------------------------------------------------
!* MULTI_PCS
!----------------------------------------------------------------------------
!\ Roody's note - Games with multiple player characters may have situtations
where you want to keep the characters' "knowledge" separate from eachother,
where an object seen by one still gets a "You haven't seen anything like that."
response from another.  Set #MULTI_PCS to be able to do that.

(You will have to replace the MakeKnown, ObjectIsKnown, MakeMovedVisited, and
ObjectIsMovedVisited routines to configure them for your game.)\!
#ifset MULTI_PCS
replace DoClose
{
	if not CheckReach(object):  return false

	if object is not openable
	{
		VMessage(&DoClose, 1)            ! "You can't close that."
		return
	}
	elseif object is not open
		VMessage(&DoClose, 2)            ! "It's already closed."
	else
	{
		object is not open
		MakeMovedVisited(object)
		if not object.after
			VMessage(&DoClose, 3)    ! "Closed."

		if not FindLight(location)      ! in case the light source
						! has been concealed

			VMessage(&DoClose, 4)    ! "Everything goes dark."
	}
	return true
}

replace DoDrop
{
	if object is clothing and object is worn
		VMessage(&DoDrop, 1)     ! "You'll have to take it off first."

	! For the following, xobject is true only
	! when called by DoPutonGround:
	elseif player not in location and (parent(player) is container or
		parent(player) is platform) and not xobject
	{
		Perform(&DoPutIn, object, parent(player))
	}
	else
	{
		move object to location
		MakeMovedVisited(object)
		player.holding = player.holding - object.size
		if not object.after
			VMessage(&DoDrop, 2)     ! "Dropped."
	}
	xobject = 0
	return true
}

replace TheorThat(obj)
{
	if ObjectIsKnown(obj)
		print The(obj);
	else
		print "that";
}

routine MakeMovedVisited(obj)
{
	obj is moved
}

routine ObjectIsMovedVisited(obj)
{
	if obj is moved
		return true
	return false
}

routine MakeKnown(obj)
{
	obj is known
}

replace AddSpecialDesc(a)
{
	a is already_listed
#ifset MULTI_PCS
	MakeKnown(a)
#else
	a is known
#endif
	list_count++
	AssignPronoun(a)
}

#ifset USE_ATTACHABLES
replace Attachable_MoveAttached(att, obj, oldloc, newloc)
{
	local i, j, first

	if att.#attached_to
	{
		for (i=1; i<=att.#attached_to; i++)
		{
			j = att.attached_to #i

			if j
			{
				if Contains(oldloc, j) and j not in obj
				{
					if obj is not workflag
					{
						first = att
						obj is workflag
					}
					move j to newloc
					MakeMovedVisited(j)
					Attachable_MoveChildren(j, oldloc, newloc)
					Attachable_MoveAttachedTo(j, oldloc, newloc)
				}
			}
		}
	}
	return first
}


! Attachable_MoveAttachedTo scans <oldloc> for anything attached to <obj>, moving
! any valid matches to <newloc>.  Not particularly graceful, since it must
! recurse inward for each object, since attachments are explicit only from
! attachable to object, and not vice-versa.

replace Attachable_MoveAttachedTo(obj, oldloc, newloc)
{
	local i, j, k, first

	for i in oldloc
	{
		for (j=1; j<=i.#attached_to; j++)
		{
			if i.attached_to #j = obj or
				Contains(obj, i.attached_to #j):
			{
				move i to newloc
				MakeMovedVisited(j)
				if not first:  first = i
				k = Attachable_MoveAttached(i, obj, oldloc, newloc)
				if not first:  first = k
				k = Attachable_MoveChildren(i, obj, oldloc, newloc)
				if not first:  first = k
			}
		}

		if child(i)
		{
			k = Attachable_MoveAttachedTo(obj, child(i), newloc)
			if not first:  first = k
		}
	}
	return first
}
#endif ! USE_ATTACHABLES

#endif ! MULTI_PCS


!----------------------------------------------------------------------------
!* NEW_DESC
!----------------------------------------------------------------------------
!\ Roody's note - new_desc is an additional property that allows grouping
of short_desc-like properties (since short_descs are often one line, I figure
that in some cases, it'd be nice to all applicable items into on paragraph).

This requires the usage of NEW_DESCRIBEPLACE. If you change the order of
DescribePlace routines, you'll need to make sure that CharsWithNewDescs
and ObjsWithNewDescs are called before CharsWithoutDescs and ObjsWithoutDescs,
respectively.
\!

#ifset NEW_DESC

#ifclear NEW_DESCRIBEPLACE
#set NEW_DESCRIBEPLACE
#endif

property new_desc
#endif ! NEW_DESC
!\ To use, define a new_desc as a regular property element, not a
property routine:

	new_desc "A newspaper is lying in the corner."
\!

!----------------------------------------------------------------------------
!* NEW DESCRIBEPLACE
!----------------------------------------------------------------------------
!\ Roody's note - Regardless, Roodylib replaces DescribePlace to fix a couple
things and add a little more functionality. If you set the #NEW_DESCRIBEPLACE
flag, though, you get the much larger DescribePlace version that allows
changing the order of how things are listed.

In both versions, initial descriptions are still run in SUPERBRIEF mode, just
in case important code is executed there. The DESCFORM_I FORMAT mask, if set,
prevents a blank line from being printed before the room title. Fixed the
verbroutine check so that DescribePlace can be used for rooms that the player
is not in (for faking player movement).

Also, I sent the room name printing to RLibMessages so it is easier to
configure them.

In the "robust" (NEW_DESCRIBEPLACE) version, setting DESCFORM_F will "double
space" between every grouping of items. To change the order of object listing,
add something like the following to SetupGlobalsandFillArrays:

	DescribePlaceArray[0] = &ParentofPlayerScenery,
	 &ObjsWithDescs,&ObjsWithoutDescs,
	&CharsWithDescs, &CharsWithoutDescs,
	&AttachablesChildren, &ListHeldAttachables

\!

enumerate start = 4096, step * 2
{
	DESCFORM_I, DESCFORM_D
}

#ifset USE_RELATIVE_DESCRIPTIONS
global DESCRIBEPLACEFORMAT

enumerate start = 1 , step * 2
{
	OVERRIDEHERE_F, ALSO_F
}
#endif

! Roody's note: Here is the routine to replace if you'd like there to
! be some instances where pronouns are not set to room objects. Have
! it return false for pronouns not to be set.
routine AssignPronounsToRoom
{
!	An example of a check. In this case, pronouns would not be set
!   when the player enters doors or "non_door_portals"
!	if (word[1] = "examine","x","l","look","watch") or
!	exit_type = direction
!	{
!		return true
!	}
	if (verbroutine = &MovePlayer, &DoLookAround) or ! not word[1] or
		XverbCheck(word[1]) !(word[1] = "restart","restore")
		return true
	else
		return false
}

#ifset USE_RELATIVE_DESCRIPTIONS
! Roody's note:  This routine establishes the "rules" for when DescribePlace
! should alter text based on the player being in a container or platform
routine RelativeParent(obj)
{
!#ifset USE_RELATIVE_DESCRIPTIONS
	if player not in location and parent(player) is container
		return true
	else
		return false
!#endif
}

! Roody's note:  This routine allows changing the word used for where an object
! is relative to whatever the player is in.
routine RelativeText(obj)
{
!#ifset USE_RELATIVE_DESCRIPTIONS
	if obj = location and player not in location and
		parent(player) is container
	{
		print "outside ";
	}
	elseif obj is container
		print "inside ";
	else
		print "on ";
	The(parent(player))
!#endif
}
#endif

#ifclear NEW_DESCRIBEPLACE
replace Describeplace(place, long)
{
	local obj, count, notlisted, tempformat, charcount

	if not place
		place = location

	if AssignPronounsToRoom
		NEW_PARSE &= ~PRONOUNS_SET

   ! Since, for example, a room description following entering via
   ! DoGo does not trigger before/after properties tied to looking
   ! around:
   !
#ifclear NO_VERBS
	if verbroutine = &MovePlayer
	{
#ifset MULTI_PCS
		if not ObjectIsMovedVisited(place) and verbosity ~= 1
#else
		if place is not visited and verbosity ~= 1
#endif
			return Perform(&DoLookAround)
		elseif long = true or verbosity = 2
			return Perform(&DoLookAround)
	}
#endif

	exit_type = 0  ! clear the exit_type global

	if not light_source
	{
		Message(&DescribePlace, 1)     ! "It's too dark to see..."
		return
	}

#ifset MULTI_PCS
	MakeKnown(place)
#else
	place is known
#endif

   ! Print the name of the location as a heading
	RLibMessage(&DescribePlace,1,place)

	override_indent = false

#ifset MULTI_PCS
	if not ObjectIsMovedVisited(place) and verbosity ~= 1
#else
	if place is not visited and verbosity ~= 1
#endif
	{
		if &place.initial_desc or &place.long_desc
			Indent
		if not place.initial_desc
			run place.long_desc
	}
	elseif long = true or verbosity = 2
	{
		if &place.long_desc or &place.initial_desc
			Indent
#ifset NEW_ROOMS
		if &place.initial_desc and place.first_visit = counter and
			(parser_data[VERB_IS_XVERB] or word[1] = "undo")
			run place.initial_desc
		else
#endif
			run place.long_desc
	}
#ifset MULTI_PCS
	elseif not ObjectIsMovedVisited(place) and verbosity = 1
#else
	elseif place is not visited and verbosity = 1
#endif
	{
		if &place.initial_desc
			Indent
		run place.initial_desc
	}

	if &place.list_contents and (FORMAT & DESCFORM_F)
		print ""        ! for double-space-after-heading formatting

   ! A location may contain an overriding listing routine, as may any
   ! parent, in the list_contents property
   !
	if not place.list_contents
	{
		list_nest = 0

      ! For double-space-after-heading formatting:
		if (FORMAT & DESCFORM_F)
		{
			for obj in place
			{
				if obj is not hidden and
					(player not in obj or children(obj) > 1)
				{
					print ""
					break
				}
			}
		}

      ! List contents of chair, vehicle, etc. player is in
		if player not in location
		{
#ifset USE_RELATIVE_DESCRIPTIONS
			if RelativeParent(parent(player)) and children(parent(player)) > 1 and
				player not in place and place = location and
				not (FORMAT & LIST_F)
			{
				tempformat = FORMAT
				FORMAT = FORMAT | FIRSTCAPITAL_F | ISORAREHERE_F
				DESCRIBEPLACEFORMAT = DESCRIBEPLACEFORMAT | OVERRIDEHERE_F
				if (FORMAT & LIST_F)
				{
					FORMAT = FORMAT & ~LIST_F       ! clear it
					FORMAT = FORMAT | TEMPLIST_F
				}
				list_nest = 0
				! called WhatsIn and not ListObjects so the parent's listcontents
				! property will be checked
				WhatsIn(parent(player))
				FORMAT = tempformat
			}
			else
			{
#endif
				list_nest = 1
				WhatsIn(Parent(player))
#ifset USE_RELATIVE_DESCRIPTIONS
			}
#endif
		}

      ! List all characters, if any
		count = 0
		for obj in place
		{
			if obj is hidden or obj is not living or
				player in obj
				obj is already_listed
			else
				obj is not already_listed
		}
		for obj in place
		{
			if obj is not already_listed
			{
				print newline
				ShortDescribe(obj)
				if obj is not already_listed
					count++
			}
		}

		list_count = count
		count = 0

		if list_count           ! if characters are to be listed
		{
			charcount++
			tempformat = FORMAT
			FORMAT = FORMAT | FIRSTCAPITAL_F | ISORAREHERE_F
#ifset USE_RELATIVE_DESCRIPTIONS
			if RelativeParent
				DESCRIBEPLACEFORMAT = DESCRIBEPLACEFORMAT | OVERRIDEHERE_F
#endif
			if list_count > 1
				FORMAT = FORMAT | USECHARNAMES_F
			Indent
			if (FORMAT & LIST_F)
			{
				FORMAT = FORMAT & ~LIST_F       ! clear it
				FORMAT = FORMAT | TEMPLIST_F
			}
			list_nest = 0
			ListObjects(place)
			FORMAT = tempformat
		}

		for obj in place
		{
#ifset USE_ATTACHABLES
         ! Exclude all attachables for now (and characters)

			if obj is living or obj.type = attachable or
				player in obj
#else
			if obj is living or player in obj
#endif
				obj is already_listed
			else
				obj is not already_listed
		}

		for obj in place
		{
#ifset USE_PLURAL_OBJECTS
         ! ...And don't list identical objects yet, either

			if (obj.identical_to).type = identical_class
			{
				if obj is not hidden
				count++
			}
			elseif player not in obj
#else
			if player not in obj
#endif
			{
				if obj is not already_listed and
					obj is not hidden
				{
					ShortDescribe(obj)
					if obj is not already_listed
						notlisted++
				}
			}
		}

		if notlisted or count
		{
			list_count = notlisted + count
			tempformat = FORMAT
			FORMAT = FORMAT | FIRSTCAPITAL_F | ISORAREHERE_F
#ifset USE_RELATIVE_DESCRIPTIONS
			if RelativeParent
			{
				DESCRIBEPLACEFORMAT = DESCRIBEPLACEFORMAT | OVERRIDEHERE_F
				if charcount
					DESCRIBEPLACEFORMAT = DESCRIBEPLACEFORMAT | ALSO_F
			}
#endif
			Indent
			if (FORMAT & LIST_F)
			{
				FORMAT = FORMAT & ~LIST_F       ! clear it
				FORMAT = FORMAT | TEMPLIST_F
			}
			list_nest = 0
			ListObjects(place)
			FORMAT = tempformat
		}

#ifclear NO_OBJLIB
#ifset USE_ATTACHABLES
		for obj in place
		{
         ! Print attachables last
			if obj.type = attachable and obj is not hidden
			{
				ShortDescribe(obj)
				if obj is not already_listed
					Message(&DescribePlace, 2, obj)
			}
		}
#endif

		print newline
		override_indent = false

      ! Finally, list contents of scenery objects (unless we've
      ! already done so as the parent of the player)
      !
		for obj in place
		{
			if obj.type = scenery
			{
#ifset MULTI_PCS
				MakeKnown(obj)
#else
				obj is known
#endif
				if player not in obj and
           !    (obj is open or obj is not openable)
			!	((obj is container and (obj is open or obj is transparent))  or
			!	obj is platform) and obj is not quiet
				(obj is open or obj is not openable or obj is platform or
				obj is transparent) and obj is not quiet
				{
					list_nest = 1
					WhatsIn(obj)
				}
			}

         ! For scenery-derived objects that may change the type
			elseif obj is static, hidden
				obj is known
		}
#ifset USE_ATTACHABLES
		print newline
		count = 0
		local no_good, good,a
		for obj in player
		{
			no_good = 0
			good = 0
			if obj.type = attachable
			{
				if Inlist(obj,attached_to,player)
					no_good = true
				if not no_good
				{
					for (a=1; a<=obj.#attached_to; a++)
					{
						if obj.attached_to #a
						{
							if not Contains(player,obj.attached_to #a)
							{
								if FindObject(obj.attached_to #a,place)
								{
									good = true
									break
								}
							}
						}
					}
					if good
					{
						if count++
							print AFTER_PERIOD;
						else
							Indent
						RLibMessage(&DescribePlace,2, obj) ! "The <blank> you are
						                                   !  holding is tied to the
                                                     !  <blank>."
					}
				}
			}
		}
#endif  ! ifset USE_ATTACHMENTS
#endif  ! ifclear NO_OBJLIB

		print newline
		need_newline = false
		NEW_PARSE |= PRONOUNS_SET
	}
}
#else
attribute already_printed
property describe_loc alias misc

property first_pass alias long_desc
property second_pass alias short_desc
property list_order alias n_to

object describeplace_settings
{
	in settings
	type settings
	describe_loc 0
	list_order ListParentofPlayerScenery \
	           ListCharsWithDescs \
	           ListCharsWithoutDescs \
	           ListObjsWithDescs \
	           ListObjsWithoutDescs \
	           ListAttachablesChildren \
	           HeldAttachables
}

replace Describeplace(place, long)
{
	local obj,  didprint, i
	local need_carriage

	if not place
		place = location
	describeplace_settings.describe_loc = place

	if AssignPronounsToRoom
		NEW_PARSE &= ~PRONOUNS_SET

	! Since, for example, a room description following entering via
	! DoGo does not trigger before/after properties tied to looking
	! around:
	!
#ifclear NO_VERBS
	if verbroutine = &MovePlayer ! was "if verbroutine ~= &DoLookAround"
	{
#ifset MULTI_PCS
		if not ObjectIsMovedVisited(place) and verbosity ~= 1
#else
		if place is not visited and verbosity ~= 1
#endif
			return Perform(&DoLookAround)
		elseif long = true or verbosity = 2
			return Perform(&DoLookAround)
	}
#endif

	exit_type = 0  ! clear the exit_type global

	if not light_source
	{
		Message(&DescribePlace, 1)     ! "It's too dark to see..."
		return
	}

#ifset MULTI_PCS
	MakeKnown(place)
#else
	place is known
#endif

   ! Print the name of the location as a heading
	RLibMessage(&DescribePlace,1,place)

	override_indent = false

#ifset MULTI_PCS
	if not ObjectIsMovedVisited(place) and verbosity ~= 1
#else
	if place is not visited and verbosity ~= 1
#endif
	{
		if &place.initial_desc or &place.long_desc
		{
			didprint = true
			Indent
		}
		if not place.initial_desc
			run place.long_desc
	}
	elseif long = true or verbosity = 2
	{
		if &place.long_desc
		{
			Indent
			didprint = true
		}
#ifset NEW_ROOMS
		if &place.initial_desc and place.first_visit = counter and
			(parser_data[VERB_IS_XVERB] or word[1] = "undo")
			run place.initial_desc
		else
#endif
		run place.long_desc
	}
#ifset MULTI_PCS
	elseif not ObjectIsMovedVisited(place) and verbosity = 1
#else
	elseif place is not visited and verbosity = 1
#endif
	{
		if &place.initial_desc
		{
			Indent
			didprint = true
		}
		run place.initial_desc
	}

	if &place.list_contents and (FORMAT & DESCFORM_F) and didprint
		print ""        ! for double-space-after-heading formatting

	! A location may contain an overriding listing routine, as may any
	! parent, in the list_contents property
	!
	for obj in place
		obj is not already_printed

	for obj in describeplace_settings
		obj is not already_printed

	if not place.list_contents
	{
		list_nest = 0
		local a
		for (i = 1 ;i <= describeplace_settings.#list_order ;i++)
		{
			a = describeplace_settings.list_order #i
			need_carriage = a.first_pass
			if ((FORMAT & DESCFORM_F) and i=1 and
				(not (FORMAT & DESCFORM_D) or (FORMAT & LIST_F)))
			{
				for obj in place
				{
					if obj is not hidden and
						(player not in obj or children(obj) > 1)
					{
						print ""
						break
					}
				}
			}
			elseif (FORMAT & DESCFORM_D) and not (FORMAT & LIST_F)
			{
				if need_carriage and didprint
					""
			}

			if need_carriage
			{
				need_carriage = false
				didprint = a.second_pass
				a is already_printed
			}
		}
		print newline
		need_newline = false
	}
}

! routine for listing the player's siblings when he or she is in or on an
! object in the room; also lists children of scenery (hopefully already
! mentioned in the room's long_desc

object ListParentofPlayerScenery
{
	in describeplace_settings
	first_pass
	{
		local obj,ret, place
		place = describeplace_settings.describe_loc
		if player not in place and place = location
		{
			for obj in (Parent(player))
			{
				if obj is not hidden and obj ~= player
					return true
			}
		}
		for obj in place
		{
			if obj.type = scenery
			{
				if player not in obj and
				(obj is open or obj is not openable or obj is platform or
				obj is transparent) and obj is not quiet
				{
					local a
					obj is not already_listed
					for a in obj
					{
						if a is not hidden
							return true
					}
				}
			}
		}
		return false
	}
	second_pass
	{
		local obj, ret, place
		place = describeplace_settings.describe_loc
#ifset USE_RELATIVE_DESCRIPTIONS
		if RelativeParent(parent(player)) and children(parent(player)) > 1 and
			player not in place and place = location and
			not (FORMAT & LIST_F)
		{
			local tempformat
			tempformat = FORMAT
			FORMAT = FORMAT | FIRSTCAPITAL_F | ISORAREHERE_F
			DESCRIBEPLACEFORMAT = DESCRIBEPLACEFORMAT | OVERRIDEHERE_F
			list_nest = 2
			! called WhatsIn and not ListObjects so the parent's listcontents
			! property will be checked
			ret = WhatsIn(parent(player))
			FORMAT = tempformat
		}
		else
		{
#endif
			list_nest = 1
			if player not in place and place = location
				ret = WhatsIn(Parent(player))
#ifset USE_RELATIVE_DESCRIPTIONS
		}
#endif
		local ret2
		for obj in place
		{
			if obj.type = scenery
			{
				obj is known
				if player not in obj and
					((obj is container and ((obj is openable and obj is open)  or
					obj is not openable or obj is transparent))  or
					obj is platform) and obj is not quiet
				{
					if not (FORMAT & DESCFORM_D) or (FORMAT & LIST_F)
						print newline
					list_nest = 1
					if WhatsIn(obj)
						ret2 = true
				}
			}
			! For scenery-derived objects that may change the type
			elseif obj is static, hidden
#ifset MULTI_PCS
				MakeKnown(obj)
#else
				obj is known
#endif
		}
		print newline
		list_nest = 0
		return (ret or ret2)
	}
}

! routine for listing characters with short_desc descriptions
object ListCharsWithDescs
{
	in describeplace_settings
	first_pass
	{
		local obj,place
		place = describeplace_settings.describe_loc
		for obj in place
		{
			if (obj is not hidden and obj is living and
				 obj ~= player and ((&obj.short_desc and verbosity ~= 1) or
				(obj is not moved and &obj.initial_desc)) and
				obj is not already_printed )
			{
				return true
			}
		}
		return false
	}
	second_pass
	{
		local obj, place
		place = describeplace_settings.describe_loc
		! List all characters, if any
!		count = 0
		for obj in place
		{
			if obj is hidden or obj is not living or
				player in obj
			{
				obj is already_listed
			}
			else
			{
				obj is not already_listed
			}
		}
		local ret, count
		for obj in place
		{
			if obj is not already_listed
			{
				if count++ and (FORMAT & DESCFORM_D) and not (FORMAT & LIST_F)
					""
				if verbosity ~= 1 or (obj is not moved and &obj.initial_desc)
					ShortDescribe(obj)
				if obj is already_listed  ! did we print something?
				{
					ret = true
					obj is already_printed
				}
			}
		}
		return ret
	}
}

! routine for listing characters without short_desc descriptions
object ListCharsWithoutDescs
{
	in describeplace_settings
	first_pass
	{
		local obj,place
		place = describeplace_settings.describe_loc
		for obj in place
		{
			if (obj is not hidden and obj is living and
			obj ~= player and
			(not &obj.short_desc or
			(&obj.short_desc  and verbosity = 1)) and
			obj is not already_printed)
				return true
		}
		return false
	}
	second_pass
	{
		local tempformat, count, obj, place
		place = describeplace_settings.describe_loc
#ifset USE_RELATIVE_DESCRIPTIONS
		local also_check
		if not (FORMAT & LIST_F)
		{
			if ListObjsWithoutDescs is already_printed
				also_check = true
!			if IsYoungerThan(self, ListObjsWithoutDescs)
!				also_check = ListObjsWithoutDescs.first_pass
		}
#endif
		for obj in place
		{
			if (obj is not hidden and obj is living and
			obj ~= player and
			(not &obj.short_desc or
			(&obj.short_desc  and verbosity = 1)) and
			obj is not already_printed)
			{
				obj is not already_listed
				count++
			}
			else
				obj is already_listed
		}
		if count ! list_count      ! if characters are to be listed
		{
			list_count = count
			tempformat = FORMAT
			FORMAT = FORMAT | FIRSTCAPITAL_F | ISORAREHERE_F
#ifset USE_RELATIVE_DESCRIPTIONS
			if RelativeParent and not (FORMAT & LIST_F)
			{
				DESCRIBEPLACEFORMAT = DESCRIBEPLACEFORMAT | OVERRIDEHERE_F
				if also_check
					DESCRIBEPLACEFORMAT = DESCRIBEPLACEFORMAT | ALSO_F
			}
#endif
			Indent
			if list_count > 1
				FORMAT = FORMAT | USECHARNAMES_F
			if (FORMAT & LIST_F)
			{
				FORMAT = FORMAT & ~LIST_F       ! clear it
				FORMAT = FORMAT | TEMPLIST_F
			}
			list_nest = 0
			ListObjects(place)
			FORMAT = tempformat
			return true
		}
		return false
	}
}

! routine for listing objects with short_desc descriptions
object ListObjsWithDescs
{
	in describeplace_settings
	first_pass
	{
		local obj, ret, place
		place = describeplace_settings.describe_loc
		for obj in place
		{
	#ifset USE_ATTACHABLES
		! Exclude all attachables for now (and characters)
			if obj is not living and not (obj.type = attachable) and
				player not in obj and obj is not hidden and
				((verbosity ~= 1 and &obj.short_desc) or
	!			(&obj.initial_desc and verbosity = 1)) and
				(&obj.initial_desc and obj is not moved)) and
				obj is not already_printed
	#else
			if obj is not living and player not in obj and
			obj is not hidden and
			((verbosity ~= 1 and &obj.short_desc) or
	!			(&obj.initial_desc and verbosity = 1)) and
				(&obj.initial_desc and obj is not moved)) and
				obj is not already_printed
	#endif
			{
#ifset USE_PLURAL_OBJECTS
         ! ...And don't list identical objects yet, either

				if (obj.identical_to).type = identical_class
					obj is already_listed
				else
				{
#endif
					ret = true
					obj is not already_listed
#ifset USE_PLURAL_OBJECTS
				}
#endif
			}
			else
				obj is already_listed
		}
		return ret
	}
	second_pass
	{
		local obj, ret
		local count, place
		place = describeplace_settings.describe_loc
		for obj in place
		{
			if player not in obj
			{
				if obj is not already_listed and
					obj is not hidden and obj is not already_printed
				{
					if count++ and (FORMAT & DESCFORM_D) and not (FORMAT & LIST_F)
						""
					if verbosity ~= 1 or (verbosity = 1 and
						(obj is not moved and &obj.initial_desc))
						ShortDescribe(obj)
					if obj is already_listed ! did we print something?
					{
						ret = true
						obj is already_printed
					}
				}
			}
		}
		return ret
	}
}

! routine for listing objects without short_desc descriptions
object ListObjsWithoutDescs
{
	in describeplace_settings
	first_pass
	{
		local obj, place
		place = describeplace_settings.describe_loc
		for obj in place
		{
#ifset USE_ATTACHABLES
			! Exclude all attachables for now (and characters)

			if obj is not living and not (obj.type = attachable) and
				player not in obj and obj is not hidden and
				obj is not already_printed and
				(not &obj.short_desc or (&obj.short_desc and verbosity = 1))
#else
			if obj is not living and player not in obj and
			obj is not hidden and obj is not already_printed and
			(not &obj.short_desc or (&obj.short_desc and verbosity = 1))
#endif
			{
				return true
			}
		}
		return false
	}
	second_pass
	{
		local obj, tempformat, count, place
		place = describeplace_settings.describe_loc
#ifset USE_RELATIVE_DESCRIPTIONS
		local also_check, a, b, i
		if not (FORMAT & LIST_F)
		{
			if ListCharsWithoutDescs is already_printed
				also_check = true
!			if IsYoungerThan(self,ListCharsWithoutDescs)
!				also_check = ListCharsWithoutDescs.first_pass
		}
#endif
		for obj in place
		{
#ifset USE_ATTACHABLES
			! Exclude all attachables for now (and characters)

			if obj is living or (obj.type = attachable) or
				player in obj or (&obj.short_desc and verbosity ~= 1) or
				obj is already_printed or obj is hidden
#else
			if obj is living or player in obj or (&obj.short_desc and verbosity ~= 1) or obj is already_printed or obj is hidden
#endif
				obj is already_listed
			else
			{
				count++
				obj is not already_listed
			}
		}

		if count
		{
			list_count = count
			tempformat = FORMAT
			FORMAT = FORMAT | FIRSTCAPITAL_F | ISORAREHERE_F
			Indent
			if (FORMAT & LIST_F)
			{
				FORMAT = FORMAT & ~LIST_F       ! clear it
				FORMAT = FORMAT | TEMPLIST_F
			}
#ifset USE_RELATIVE_DESCRIPTIONS
			if RelativeParent and not (FORMAT & TEMPLIST_F)
			{
				DESCRIBEPLACEFORMAT = DESCRIBEPLACEFORMAT | OVERRIDEHERE_F
				if also_check
					DESCRIBEPLACEFORMAT = DESCRIBEPLACEFORMAT | ALSO_F
			}
#endif
			list_nest = 0
			ListObjects(place)
			FORMAT = tempformat
			return true
		}
		return false
	}
}

! routine for listing attachables and contents of scenery objects
object ListAttachablesChildren
{
	in describeplace_settings
#ifclear NO_OBJLIB
#ifset USE_ATTACHABLES
	first_pass
	{
		local obj, ret, place
		place = describeplace_settings.describe_loc
		for obj in place
		{
			! Print attachables last
			if obj.type = attachable and obj is not hidden and
				obj is not already_printed
			{
				obj is not already_listed
				ret = true
			}
		}
		return ret
	}
	second_pass
	{
		local obj, ret, count, place
		place = describeplace_settings.describe_loc
		for obj in place
		{
			! Print attachables last
			if obj.type = attachable and obj is not hidden and
				obj is not already_printed
			{
				if count++ and (FORMAT & DESCFORM_D) and not (FORMAT & LIST_F)
					""
				ShortDescribe(obj)
				if obj is not already_listed
					Message(&DescribePlace, 2, obj)
				obj is already_printed
				ret = true
			}
		}

		print newline
		override_indent = false
		return ret
	}
#endif  ! ifset USE_ATTACHABLES
#endif  ! ifclear NO_OBJLIB
}

object HeldAttachables
{
	in describeplace_settings
#ifset USE_ATTACHABLES
	first_pass
	{
		local i,a,no_good, good, place
		place = describeplace_settings.describe_loc
		for i in player
		{
			no_good = 0
			good = 0
			if i.type = attachable
			{
				if Inlist(i,attached_to,player)
					no_good = true
				if not no_good
				{
					for (a=1; a<=i.#attached_to; a++)
					{
						if i.attached_to #a
						{
							if not Contains(player,i.attached_to #a)
							{
								if FindObject(i.attached_to #a,place)
								{
									good = true
									break
								}
							}
						}
					}
					if good
						return true
				}
			}
		}
		return false
	}
	second_pass
	{
		local i,a,no_good, good, count, place
		place = describeplace_settings.describe_loc
		for i in player
		{
			no_good = 0
			good = 0
			if i.type = attachable
			{
				if Inlist(i,attached_to,player)
					no_good = true
				if not no_good
				{
					for (a=1; a<=i.#attached_to; a++)
					{
						if i.attached_to #a
						{
							if not Contains(player,i.attached_to #a)
							{
								if FindObject(i.attached_to #a,place)
								{
									good = true
									break
								}
							}
						}
					}
					if good
					{
						if count++
							print AFTER_PERIOD;
						else
							Indent
						RLibMessage(&DescribePlace,2, i) ! "The <blank> you are
																	  !  holding is tied to the
																	  !  <blank>."
					}
				}
			}
		}
		print newline
		return count
	}
#endif
}
#endif  ! NEW_DESCRIBEPLACE
!----------------------------------------------------------------------------
!* ADDITIONAL VERB AND UTILITY ROUTINES
!----------------------------------------------------------------------------

! Roody's note: This only exists for CHARACTER, G. Never actually called.
routine DoAgain
{}

!\ Roody's note - If you want your game to not allow verbosity changes, #set
NO_MODE_CHANGE before Roodylib grammar is included. Then the grammar will call
the following routine which fakes a "I don't understand that" message.

You can use DoFakeRefuse for any other standard verb you want to fake a refusal,
too.
\!

routine DoFakeRefuse
{
	print CThe(player); \
				! " can't use the word \""; \
		MatchPlural(player, "doesn't", "don't"); \
		" need to use the word \""; \
		word[1] ;
#ifset AMERICAN_ENGLISH
		".\""
#else
		"\"."
#endif
}

! Roody's note - Mike Snyder pointed out that it's not optimal that "kick", by
! default, points to DoHit.  Sending the command to its own verb routine makes
! it easier to replace or catch with before routines.
routine DoKick
{
	return Perform(&DoHit, object)
}

!\ Roody's note - DoPushDir figures out what direction the player is trying to
push something, for accurate response messages (after finding the direction,
it calls DoPushDirTo.
\!
#ifclear NO_OBJLIB
routine DoPushDir
{
!	local a,m
!
!	! the for loop finds the last word before the direction
!	! which should be the object's noun
!	for (a=2; ; a=a+1)
!	{
!		if ObjWord(word[a],object) = noun
!			break
!	}
!	! Let's add one to find the direction words
!	a = a + 1
!
!	! Let's identify what we've got and point to the right direction object
!	select word[a]
!		case "north", "n":      m = n_obj
!		case "south", "s":      m = s_obj
!		case "east", "e":       m = e_obj
!		case "west", "w":       m = w_obj
!		case "northwest", "nw": m = nw_obj
!		case "northeast", "ne": m = ne_obj
!		case "southeast", "se": m = se_obj
!		case "southwest", "sw": m = sw_obj
!		case "up", "u":         m = u_obj
!		case "down", "d":       m = d_obj
!		case "in", "inside":    m = in_obj
!		case "out", "outside":  m = out_obj
!
!	! Let's run it
!	return Perform(&DoPushDirTo, object, m)
	SetupDirectionObjects
	return Perform(&DoPushDirTo, object, xobject)
}
#endif

!\ Roody's note- DoPushDirTo either prints a "can't do that" message or
processes an object-pushing, depending on whether USE_ROLLABLES is set. \!

#ifclear USE_ROLLABLES ! if we are not allowing object-rolling
routine DoPushDirTo
{
	RLibMessage(&DoPushDirTo,1) ! "That would not help the present situation."
	return false ! let's not use up a turn
}
#else ! if we are allowing object-rolling

!: Some new rolling properties
property outofbounds ! description property for rooms object can't be pushed in
!\ example:
   outofbounds
		print "You can't push "; art(object); " up the stairs." \!

property pushstart ! desc. property to be printed during successful push
!\ example:
	pushstart
		 "You roll the giant onion to the..." \!

property pushend ! desc. property to be printed after successful push
!\ example:
	pushend
		 "The onion rolls to a stop." \!

! Roody's note- This routine might not work if a game also has "attachable"
! objects.

!\ DoPushDirTo uses pretty much the first method I ever learned for
exit-checking.  It probably won't work great for directions that lead to doors.
Maybe I'll perfect it at some point. \!
routine DoPushDirTo
{
	local moveto
	moveto = xobject

 ! if the item is not rollable, we'll just act like the player just tried to
 ! move it. since mobile = worn, we make sure the item is not clothing.
	if not (object is mobile and  object is not clothing)
#if defined DoPush
		return Perform(&DoPush, object)
#endif
#if undefined DoPush
		return Perform(&DoMove, object)
#endif
	elseif xobject.type ~= direction
	{
		! possibly, you might want to add code that checks to see
		! if the xobject is a container and in such cases, interpreters
		! >PUSH OBJECT INTO XOBJECT as DoPutIn. To avoid the checkheld headache
		! that introduces, we will ignore that possiblity for now
		RLibMessage(&DoPushDirTo,2) ! "Try pushing the object in a direction."
		return false
	}

	! Let's check to see if we're pushing to a location
	! we don't want the player pushing to
	local k
	k = location.(moveto.dir_to)
	if k > 1  ! means the direction we are moving in is a viable direction
	{
		! is the direction a room we can walk to but can't push objects to?
		if k.outofbounds
			return false
#ifset USE_ATTACHABLES
		if ObjectisAttached(object, location, k) or
			ObjectisAttached(player, location, k)
			return false
		local a
		a = CheckForAttachments(object,location)
		if a and player in location
		{
! We'll move the objects silently if it's attached to the pushed object AND
! being held.  Otherwise, we'll print a message.
			if a not in player
				MoveAllAttachables(object, location, k)
			else
				MoveAllAttachables(object, location, k,true)
		}
#endif
		! Let's print a success message before the move "You start pushing..."
		if not object.pushstart and player in location
		{
			RLibMessage(&DoPushDirTo,3) ! "You push the object over to the..."
!			move object to location
			object is hidden
		}
		if (FORMAT & DESCFORM_I)
			""
	}

	! If DoGo works, then move the object to the location and print
	! a followup success message.
	if Perform(&DoGo, xobject)
	{
		print ""
		move object to location
		object is moved
		object is not hidden
		if not object.pushend
			RLibMessage(&DoPushDirto,4) ! "The [x] slows to a stop."
		return true
	}
	elseif player in location
		MoveAllAttachables(object, k, location, true)
	! Hopefully, it didn't get to Perform(&DoGo) only to have the command
	! fail, but if it does, we silently move the attachables back to the
	! first room
}
#endif ! USE_ROLLABLES

! Roody's note: Checkheld versions of DoWear and DoTakeOff
#ifset USE_CHECKHELD
routine DoTakeOff_CheckHeld		! See above re: ImplicitTakeForDrop
{
	if parent(object) ~= player and
	not (object is worn and object is clothing)
	{
		! So >REMOVE LOCK will >TAKE LOCK, etc.
		return Perform(&DoGet, object)
	}
	if (word[1] = "remove" and word[2] ~= "~all") or
	(word[1] = "take" and not (word[2] = "~all" or word[3] = "~all"))
	{
		return Perform(&DoTakeOff, object)
	}
	return CallVerbCheckHeld(&DoTakeOff, location)
}

routine DoWear_CheckHeld		! See above re: ImplicitTakeForDrop
{
	return CallVerbCheckHeld(&DoWear, location)
}
#endif

#ifclear NO_XYZZY
routine DoXYZZY
{
	RLibMessage(&DoXYZZY)
}
#endif

!\ Roody's note: ROTATE_DESC and NEW_ROTATE. Both allow a nicer way to quickly
add multiple descriptions to an object. ROTATE_DESC is quicker to set up but is
limited to 5 descriptions per object. NEW_ROTATE uses as many as you'd like. \!
#ifset ROTATE_DESC
! Examples:
!door red_door "red door"
!{
!	noun "door"
!	adjective "red"
!	article "the"
!	rotations 0
!	long_desc
!	{
!		rotate( "It's a door.", "It's still a door.", "Stop looking at it.", \
!		"I mean it.", "I really do.")
!	}
!	between startlocation east_room
!	!is recycle  ! uncomment if you want descs to cycle
!}
!
!object rock "rock"
!{
!	article "a"
!	noun "rock"
!	in STARTLOCATION
!	long_desc
!	{
!		RandomDesc("A rock.","2nd rock desc.", "3rd rock.","4th rock.")
!	}
!	rotations 0
!}
property rotations ! objects with rotating descriptions need a property to hold
                   ! a value
attribute recycle ! give this attribute to cycle back to the beginning when end
                  ! is reached

!\ My Rotate routine can handle up to five descriptions. Of course, it could
   be expanded to do more. Unfortunately, there's no cool way to get the routine
	to just adapt to whatever arguments you throw it so it basically has to check
	every expected argument by hand.\!

routine Rotate(arg1, arg2, arg3,arg4,arg5)
{
	local n
	object.rotations += 1
	n = object.rotations
	select n
		case 1: print arg1 ! we'll assume this routine won't be used with one
		                   ! "rotation" so we don't need to check for arg2
		case 2
		{
			print arg2
			if not arg3 ! if there are only 2 descriptions
			{
				if object is recycle  ! should it cycle?
					object.rotations = 0  ! start back at the beginning
				else
					object.rotations -= 1 ! or just subtract one so it stays the
					                      ! same next time
				return
			}
		}
		case 3
		{
			print arg3
			if not arg4
			{
				if object is recycle
					object.rotations = 0
				else
					object.rotations -= 1
				return
			}
		}
		case 4
		{
			print arg4
			if not arg5
			{
				if object is recycle
					object.rotations = 0
				else
					object.rotations -= 1
				return
			}
		}
		case 5
		{
			print arg5
			if object is recycle
				object.rotations = 0
			else
				object.rotations -= 1
			return
		}
}

!\ I sometimes like random descriptions, too, so I threw in a random description
   version \!
routine RandomDesc(arg1, arg2, arg3,arg4,arg5)
{
	local x,n

	if arg1: x++
	if arg2: x++
	if arg3: x++
	if arg4: x++
	if arg5: x++

	do
		n = random(x)
	while (n = object.rotations)
	object.rotations = n
	select n
		case 1:print arg1
		case 2: print arg2
		case 3: print arg3
		case 4: print arg4
		case 5: print arg5
}
#endif ! endif ROTATE_DESC
#ifset NEW_ROTATE
! Examples:
!door red_door "red door"
!{
!	noun "door"
!	adjective "red"
!	article "the"
!	rotations 0
!	long_desc
!		Rotate
!	rotate_desc "It's a door." "It's still a door." "Stop looking at it." \
!		"I mean it." "I really do."
!	between startlocation east_room
!	!is recycle  ! uncomment if you want descs to cycle
!}
!
!object rock "rock"
!{
!	article "a"
!	noun "rock"
!	in STARTLOCATION
!	long_desc
!		RandomDesc
!	random_desc "A rock." "2nd rock desc." "3rd rock." "4th rock."
!	rotations 0
!}

property rotations ! objects with rotating descriptions need a property to hold
                   ! a value
attribute recycle ! give this attribute to cycle back to the beginning when end
                  ! is reached
property rotate_desc ! holds descriptions to rotate
property random_desc alias rotate_desc

routine Rotate(obj)
{
	if not obj
		obj = object
	local n
	obj.rotations += 1
	n = obj.rotations
	print obj.rotate_desc #n
	if n = obj.#rotate_desc
	{
		if obj is recycle
			obj.rotations = 0
		else
			obj.rotations -= 1
	}
}

!\ I sometimes like random descriptions, too, so I threw in a random description
   version \!
routine RandomDesc(obj)
{
	local x,n
	if not obj
		obj = object
	x = obj.#random_desc
	do
		n = random(x)
	while (n = object.rotations)
	object.rotations = n
	print obj.random_desc #n
}
#endif ! endif NEW_ROTATE

!----------------------------------------------------------------------------
!* OBJECT SORTING
!----------------------------------------------------------------------------
!\
	This next bit of routines were written mainly to be used with the
	ListChildren routine in the "Other Routines" section, but it's possible
	it could have other uses, too. ListChildren lists the children of a
	parent, and these routines allow you to change the order of the children
	in several ways.

	Read their individual descriptions for usage info.

	NOTE: This section was inspired by Christopher Tate's array sorting
	routines which were originally included in Roodylib.  As Roodylib
	has largely moved to object hieararchy based behavior, those
	routines were moved to the "extraroutines.hug" file
\!

#ifset USE_JUKEBOX
#set OBJECT_SORTING
#endif

#ifset OBJECT_SORTING
!object temp_bowl
!{}

!\ MovePosition - moves object "obj" in parent "par" to position "pos"
( pos = 1 makes it the oldest child, bigger numbers make it younger) \!
routine MovePosition(obj,par,pos)
{
	local n = 1
	while child(par)
	{
		move child(par) to display
	}

	while child(display)
	{
		if n++ = pos
			move obj to par
		else
		{
			if child(display) = obj
				move obj to display
			move child(display) to par
		}
	}
}

! MixObjects - Randomizes order of children of parent "par"
routine MixObjects(par)
{
	local n, obj

	while child(par)
	{
		move child(par) to display
	}

	while child(display)
	{
		n = children(display)
		n = random(n)
		n--
		obj = child(display)
		while n
		{
			obj = sibling(obj)
			n--
		}
		move obj to par
	}
}

! MoveFromTo - with object-based designs, I sometimes need to move all
! children from one object to another. Here is a routine for convenience.
routine MoveFromTo(first,second)
{
	while child(first)
	{
		move child(first) to second
	}
}

! ReverseOrder - Reverses the order of parent's (par's) children
routine ReverseOrder(par)
{
	while child(par)
	{
		move child(par) to display
	}

	while child(display)
	{
		move youngest(display) to par
	}
}

!\ SortChildren- Sorts children of parent "par" by numerical criteria "crit".
If reverse argument is true, puts them in reverse order (lowest to highest).
See BySize example after this for example criteria routine (which would be
called like SortChildren(jellybean_jar, &BySize)
\!

routine SortChildren(par, crit, reverse)
{
	local x, p, next
	local best_choice = -32767
	while child(par)
	{
		for x in par
		{
			p = call array crit(x)
			if p > best_choice
				best_choice = p
		}
		x = child(par)
		while x
		{
			next = younger(x)
			if best_choice = call array crit(x)
				move x to display
			x = next
		}
		best_choice = -32767
	}

	while child(display)
	{
		if reverse
			move youngest(display) to par
		else
			move child(display) to par
	}
}

!\
Example criteria routine

routine BySize(obj)
{
	return obj.size
}
\!

! TakeRandom - returns a random child from parent "par"
routine TakeRandom(par)
{
	local n, obj
	n = children(par)
	n = random(n)
	n--
	obj = child(par)
	while n
	{
		obj = sibling(obj)
		n--
	}
	return obj
}

!\ "floats" objects in parent "par" to the top as determined by rules
   returned by routine "arg" (see sample below)  \!
routine Float(par,arg)
{
	local obj, qualify
	while child(par)
	{
		move child(par) to display
	}

	obj = youngest(display)

	while elder(obj)
	{
		qualify = call arg(obj)
		if qualify
		{
			move obj to par
			obj = youngest(display)
		}
		else
			obj = elder(obj)
	}

	while child(display)
	{
		move child(display) to par
	}
}

!\  Sample routine call and argument routine:
routine DoFloat
{
	Float(basket, &IsEdible)
}

routine IsEdible(obj)
{
	return (obj is edible)
}  \!

#endif ! OBJECT_SORTING
!----------------------------------------------------------------------------
!* EXTRA STRING MANIPULATION ROUTINES
!----------------------------------------------------------------------------

! Some string manipulation routines
#ifclear NO_STRING_ARRAYS
#ifset USE_STRING_MANIPULATION

! CapitalizeEveryFirstLetter - Capitalizes every first letter of a string array
! (specify a "newarray" array if you want the results copied to a different
!  array than the source)
routine CapitalizeEveryFirstLetter(stringarray,newarray)
{
	local len, i, new, finalarray
	finalarray = stringarray

	if newarray
		finalarray = newarray

	len = StringLength(array stringarray)
	for (i=0; i<len; i=i+1) ! the actual case-conversion loop
	{
		if new or i=0
		{
			if array stringarray[i] >= 'a' and array stringarray[i] <= 'z'
				array finalarray[i] = array stringarray[i] - ('a'-'A')
			new = 0
		}
		elseif array stringarray[i] >= 'A' and array stringarray[i] <= 'Z'
			array finalarray[i] = array stringarray[i] + ('a'-'A')
		else
			array finalarray[i] = array stringarray[i]
		if array stringarray[i] = ' '          ! a space
			new = 1
	}
	array finalarray[i] = 0
}

! Convert To All Caps - Capitalizes every letter of a string array
! (specify a "newarray" array if you want the results copied to a different
!  array than the source)
routine ConvertToAllCaps(stringarray,newarray)
{
	local len, i, finalarray
	finalarray = stringarray
	if newarray
		finalarray = newarray

	len = StringLength(array stringarray)
	for (i=0; i<len; i=i+1) ! the actual case-conversion loop
	{
		if array stringarray[i] >= 'a' and array stringarray[i] <= 'z'
			array finalarray[i] = array stringarray[i] - ('a'-'A')
		else
			array finalarray[i] = array stringarray[i]
	}
	array finalarray[i] = 0
}

! Lowercase- a routine for changing every letter in a string array to
! lowercase
! (specify a "newarray" array if you want the results copied to a different
!  array than the source)
routine Lowercase(stringarray,newarray)
{
	local len, i, finalarray
	finalarray = stringarray
	if newarray
		finalarray = newarray

	len = StringLength(array stringarray)
	for (i=0; i<len; i=i+1) ! the actual case-conversion loop
	{
		if array stringarray[i] >= 'A' and array stringarray[i] <= 'Z'
			array finalarray[i] = array stringarray[i] + ('a'-'A')
		else
			array finalarray[i] = array stringarray[i]
	}
	array finalarray[i] = 0
}

! StringAddTo - Adds second string array to first string array, with optional
! space between them
! (specify a "newarray" array if you want the results copied to a different
!  array than the original array)
routine StringAddTo(original_array, addition_array, add_space,newarray)
{
	local f
	if newarray
	{
		text to (array newarray)
		StringPrint(original_array)
	}
	else
	{
		f = StringLength(array original_array)
		text to (array original_array + f)
	}
	if (add_space)
		print " ";
	StringPrint(array addition_array)
	text to 0
}
#endif ! ifset USE_STRING_MANIPULATION
#endif ! ifclear NO_STRING_ARRAYS

!----------------------------------------------------------------------------
!* OTHER ROUTINES
!----------------------------------------------------------------------------

!\ Roody's note: Future Boy!'s AddPropValue and RemovePropValue are like
the routines AddToScope and RemoveFromScope except they aren't limited to
one property (like in those cases, "scope").
As such, they are very useful routines. Written by Kent Tessman.
\!

! AddPropValue(obj, prop, value)
! Adds 'value' as a property value for obj, if possible

routine AddPropValue(obj, prop, val,no_repeats)
{
	local i
	for (i=1; i<=obj.#prop; i++)
	{
		if obj.prop #i = val and no_repeats
			return i
		elseif obj.prop #i = 0
		{
			obj.prop #i = val
			return i
		}
	}
	return false
}


! RemovePropValue(obj, prop, value)
! Removes 'value' as a property value for obj, if applicable

routine RemovePropValue(obj, prop, val)
{
	local i
	for (i=1; i<=obj.#prop; i++)
	{
		if obj.prop #i = val
		{
			obj.prop #i = 0
			return i
		}
	}
}

! Roody's note: Since the end of the game is called just by changing the
! value of the endflag global variable, that can be a little unintuitive to
! new authors. Here is a dummy routine for calling the end of the game.

routine CallFinish(end_type)
{
	endflag = end_type
}

! Roody's note: CanGoDir is an exit-checking routine so you can check to make
! sure the direction the player is trying to go in is valid before printing
! a relevant message. It hasn't had a lot of use and may not work in all
! instances.

#ifclear NO_OBJLIB
routine CanGoDir
{
	local m
	if object.type ~= direction and object is not enterable and
		object.type ~= door
		return false
	select true
		case (object in direction)
			m = location.(object.dir_to)
		case (object.type = door)
		{
			if location = object.between #1
				m = object.between #2
			else
				m = object.between #1
		}
		case else
		{
			if object.door_to
				m = object.door_to
			else
				return true
		}
	return m
}
#endif !  ifclear NO_OBJLIB

!\ Roody's note: ClearArray, as you might guess, clears arrays of values.
I made it mainly to stop having to write array-clearing loops each time.

	Originally, it looped through the entire array, but doing so in a large
	string array like _temp_string is a real memory-killer. Now it clears
	until it gets two empty slots.\!

routine ClearArray(array_to_be_cleared)
{
	local n,t
	for (n=0;n< array array_to_be_cleared[] ; n++ )
	{
		if	array array_to_be_cleared[n] = 0
			t++
		else
		{
			array array_to_be_cleared[n] = 0
			t = 0
		}
		if t = 2
			break
	}
}
! Roody's note: The previous array won't let us use it on the word array so
!  here is a word-array-specific one
routine ClearWordArray
{
	local n
	for (n=LAST_TURN;n> 0 ; n-- )
	{
		if word[n] = ""
			break
		else
			word[n] = ""
	}
}

!\ Roody's note: ClearPronoun is a routine for clearing pronouns (like if you
have a disappearing fake object). Feed it the object and it will clear every
pronoun global variable set to that value. \!

routine ClearPronoun(obj)
{
	select obj
		case it_obj: it_obj = 0
		case her_obj : her_obj = 0
		case him_obj : him_obj = 0
		case them_obj : them_obj = 0
}

! Roody's note: CoolPause is a routine I wrote for newmenu.h, but I find it
! so useful that I have decided to add it to Roodylib. Anyhow, it's a routine
! for doing "press a key" text. The page and no_newline arguments are mainly
! for the new menu system so that different pause text can be given to menu
! pages (and to avoid an extra line if called from EndGame)

!\ ALSO: This used to have code by default to change display.title_caption
 to some pause text. I decided to take that code out and put it into a wrapper
 routine in "extraroutines.hug" so go look there if you want it. \!

routine CoolPause(pausetext,page,no_newline)
{
#ifset NO_ACCESSIBILITY
	local cheap
#endif

#ifclear NO_ACCESSIBILITY
	if (cheap & CHEAP_ON)
	{
!			if there is a pausetext value, prints pausetext.
!			 otherwise prints default.
		RlibMessage(&CoolPause,1,pausetext,page) ! "[PRESS A KEY TO CONTINUE]" !;
		pause
	}
	else
	{
#endif
!			if pausetext exists, prints pausetext. otherwise prints default.
!			 text is italicized
		RlibMessage(&CoolPause,2,pausetext,page) ! "press a key to continue"
		HiddenPause
#ifclear NO_ACCESSIBILITY
	}
#endif
!	print newline
	if not no_newline
		""
}

!\Roody's note: TopPause used to be a part of CoolPause (allowing the option
to put pause text in the status line) but I gave it its own routine just
for code readability.

(I find it nice sometimes to put the text in the status bar because then the
pause text does not interrupt game transcripts) \!
routine TopPause(pausetext)
{
	local force_bottom
#ifclear NO_ACCESSIBILITY
	if (cheap & CHEAP_ON)
		force_bottom = true
#endif
#ifset NO_STRING_ARRAYS
	force_bottom = true
#endif
	if force_bottom
		return CoolPause(pausetext)

	Font(BOLD_OFF | ITALIC_OFF | UNDERLINE_OFF | PROP_OFF)
	if not system(61)
	{
		window display.statusline_height
		{
			cls
		}
	}
	window  1 ! display.statusline_height
	{
		local y
		y = display.linelength
		color SL_TEXTCOLOR, SL_BGCOLOR
		cls			! make sure we've drawn the entire status bar in the
						!  proper colors
!		locate 1,1
		text to _temp_string
		if pausetext
			print pausetext;
		else
			RLibMessage(&TopPause,1) ! "PRESS A KEY TO CONTINUE";
		text to 0

		local alength
		alength = StringLength(_temp_string)
		print to (display.linelength/2 - alength/2);
		StringPrint(_temp_string)
	}
	color TEXTCOLOR, BGCOLOR, INPUTCOLOR
	Font(DEFAULT_FONT)
	HiddenPause ! PauseForKey
}

! Roody's note: GetNumber just exists so authors don't have to remember
! the word[]/parse$ thing when converting player's input to a number. Change
! the wordnum value if for some reason you want to check something other than
! word[1]

routine GetNumber(wordnum)
{
	local ret
	if not wordnum
		wordnum = 1
	if word[wordnum]
		ret = StringToNumber(word[wordnum])
	else
		ret = StringToNumber(parse$)
	return ret
}

! Roody's note: EnterNumber is an example routine using the above routine.
! Call with text t1 and t2 arguments if you want to change the prompt text
! without changing RlibMessage messages

routine EnterNumber(low,high,t1,t2)
{
	local ret
	if t1
		print t1;
	else
		RlibMessage(&EnterNumber,1) ! "Enter a number >";
	while true
	{
		input
		""
		if words = 1
			ret = GetNumber
		else
			word[1] = ""
		if ret
		{
			if high
			{
				if ret <= high and ret >= low
					return ret
			}
			elseif low
			{
				if ret >= low
					return ret
			}
			else
				return ret
		}
		elseif not low and word[1] = "0"
			return ret
		if t2
			print t2;
		else
		! "Enter a valid number between X and Y >";
			RlibMessage(&EnterNumber,2,low,high)
	}
}

!\ Roody's note: GrandParent is like Contains except it returns the
"grandparent" object. Written by Kent Tessman for Future Boy!. \!

! GrandParent returns an object's room object (presuming the room object is
! the last object in the tree before nothing)

routine GrandParent(obj)
{
	local gp
	gp = obj
	while parent(gp)
	{
		gp = parent(gp)
	}
	return gp
}

!\
Roody's note: This is an adapted version of Kent Tessman's PauseForKey routine
from Future Boy!. It uses trickery so the cursor doesn't show up
during a pause. This version, unlike the original, does not have code for
handling prompts, since I figured you can just handle that separately.
\!
routine HiddenPause
{
	local key

#ifset DEBUG
	if (debug_flags & D_PLAYBACK_HELPER) and betalib is playback_on
		return
#endif
	key = system(11) ! READ_KEY
	if system_status or system(61)  or ! MINIMAL_INTERFACE
	hugor is switchedon
	{
		pause
		key = word[0]
	}
	else
	{
		while true
		{
			key = system(11) ! READ_KEY
			if key: break
			system(32) ! PAUSE_100TH_SECOND
		}
	}
	return key
}


!\ IsNear - new approximation routine. returns true if
value fir is within value range of value sec \!

#ifclear NO_AUX_MATH
routine IsNear(fir,sec,range)
{
	if (abs(fir - sec) <= range)
		return true
	else
		return false
}
#endif

!\ Roody's note: ListChildren provides an object-based way to list objects,
like PropertyList, except using object hierarchy. Likewise, it assumes that
all children objects are not hidden. \!

routine ListChildren(obj,conjunction)
{
	local x
	list_count = children(obj)
	for x in obj
	{
		x is not already_listed
	}
	if list_count
		ListObjects(obj,conjunction)
}

! Roody's note: This routine serves as a reminder that when you change
! the player_person global variable, you also need to change the pronouns
! for your player object to match.

routine MakePlayer(player_object, playerperson)
{
	if not player_object
		return  ! just to avoid setting the player as the nothing object or
			     ! some other nonsense
#ifclear NO_OBJLIB
#ifset DEBUG
	if player_object.type ~= player_character
	{
		print newline
		Font(BOLD_ON)
		print "[WARNING:  The player global is set to an object that is
		not of type \"player_character\".]"
		Font(BOLD_OFF)
	}
#endif
	player = player_object
	if playerperson
	{
		player_person = playerperson
		select player_person
			case 1
			{
				player.pronouns #1 = "I"
				player.pronouns #2 = "me"
				player.pronouns #3 = "my"
				player.pronouns #4 = "myself"
			}
			case 2
			{
				player.pronouns #1 = "you"
				player.pronouns #2 = "you"
				player.pronouns #3 = "your"
				player.pronouns #4 = "yourself"
			}
			case 3
			{
				if player is plural
				{
					player.pronouns #1 = "they"
					player.pronouns #2 = "them"
					player.pronouns #3 = "their"
					player.pronouns #4 = "themselves"
				}
				if player is female
				{
					player.pronouns #1 = "she"
					player.pronouns #2 = "her"
					player.pronouns #3 = "her"
					player.pronouns #4 = "herself"
				}
				else
				{
					player.pronouns #1 = "he"
					player.pronouns #2 = "him"
					player.pronouns #3 = "his"
					player.pronouns #4 = "himself"
				}
			}
	}
#else
	player = player_object
#endif
}

! Roody's note: This is based on Kent Tessman's PauseForKey routine where
! it has the option of printing a prompt, but it doesn't have the built-in
! cursor-locating and font adjustment of the original.

routine GetKeyPress(p)	! Where p is a prompt, if it ends up being used
{
	if p
		print p;
	return HiddenPause
}

! Roody's note: SpeakerCheck just replaces one of the common bits of code
!  in the main routine, mostly so the main routine becomes something authors
!  don't really need to touch (and just looks nicer).  You can replace this
!  routine if your game requires a more intensive check, like calling
!  the GrandParent routine or FindObject.

routine SpeakerCheck
{
	if speaking not in location
		speaking = 0
}

!----------------------------------------------------------------------------
!* "OPPORTUNITY" SYSTEM
!----------------------------------------------------------------------------

! Roody's note: Sometimes, it's cute to have special responces to actions or
! words right after something has happened.  This system is just a simple
! way of setting up for games that do a lot of this.
#ifset USE_OPPORTUNE
global current_opp ! current opportunity

! A fuse so that each question is only answerable for one turn
! opp_window = short for "opportunity window"
fuse opp_window "(opp_window fuse)"
{}

event opp_window
{
	if not self.tick
		current_opp = 0
}

class Question ! for Ballyhoo "egress" type questions
{
	type question
}

routine NewQuestion(new_question)
{
	current_opp = new_question
	if not new_question.long_desc
		print new_question.name
	Activate (opp_window, 2) ! Starts the new timer
}

routine NewOpp(opportunity)
{
	current_opp = opportunity
	Activate (opp_window, 2)
}

class opp
{}
#endif ! USE_OPPORTUNE


!----------------------------------------------------------------------------
!* TIME-KEEPING RESOURCES
!----------------------------------------------------------------------------

! Roody's note: Most of this code is from Kent Tessman's Future Boy! with
! just a couple of additions of my own. Now, Hugo doesn't do real-time games
! (like Infocom's Border Zone), but it can catch the current time and the
! following code allows for figuring out the time difference betwen this
! or that time (useful for music jukebox extensions or other misc uses).
!
! Since it's rare for games to use this, set the #USE_TIME_SYSTEM flag
! to enable it.
!----------------------------------------------------------------------------
#ifset USE_TIME_SYSTEM

#ifclear _SYSTEM_H
#include "system.h" ! these routines use this extension
#endif

! we alias these time object properties just because we can
property tm_year alias u_to
property tm_month alias d_to
property tm_day alias s_to
property tm_hour alias n_to
property tm_minute alias e_to
property tm_second alias w_to

! Roody's note : An object class to hold times.
class time_object
{
	tm_year 0
	tm_month 0
	tm_day 0
	tm_hour 0
	tm_minute 0
	tm_second 0
}

#ifset _RESOURCE_H
time_object song_start
{}

time_object current_time
{}

time_object time_difference
{}
#endif

! Roody's note: Sets the current time to a time_object provided as an argument.
routine GetCurrentTime(current)
{
	current.tm_year   = GetSystemTimeValue(TIME_YEAR)
	if current.tm_year = 0
		return false

	current.tm_month  = GetSystemTimeValue(TIME_MONTH)
	current.tm_day    = GetSystemTimeValue(TIME_DAY)
	current.tm_hour   = GetSystemTimeValue(TIME_HOURS)
	current.tm_minute = GetSystemTimeValue(TIME_MINUTES)
	current.tm_second = GetSystemTimeValue(TIME_SECONDS)

	return true
}

! Roody's note: Checks if the year provided is a leap year.
routine IsLeapYear(year)
{
	! Years not divisible by 4 are not leap years
	if mod(year, 4)
		return false

	! Years divisible by 400 are leap years
	if mod(year, 400) = 0
		return true

	! Years divisible by 100 are not leap years
	if mod(year, 100) = 0
		return false

	! If we get here, it's a leap year
	return true
}

! Roody's note: Determines difference between "current" and "previous"
! time_objects, saving to "result" time_object.
routine CalculateTimeDifference(current, previous, result)
{
	local years, months, days, hours, minutes, seconds

	years = current.tm_year - previous.tm_year
	if years < 0
	{
		! current must be later than previous
		return false
	}

	months = current.tm_month - previous.tm_month
	if months < 0
	{
		months = months + 12
		years--
	}

	days = current.tm_day - previous.tm_day
	if days < 0
	{
		local d
		if previous.tm_month = 4, 6, 9, 11
			d = 30
		elseif previous.tm_month = 2
		{
			if IsLeapYear(previous.tm_year)
				d = 29
			else
				d = 28
		}
		else
			d = 31

		days = days + d
		months--
	}

	hours = current.tm_hour - previous.tm_hour
	if hours < 0
	{
		hours = hours + 24
		days--
	}

	minutes = current.tm_minute - previous.tm_minute
	if minutes < 0
	{
		minutes = minutes + 60
		hours--
	}

	seconds = current.tm_second - previous.tm_second
	if seconds < 0
	{
		seconds = seconds + 60
		minutes--
	}

	result.tm_year = years
	result.tm_month = months
	result.tm_day = days
	result.tm_hour = hours
	result.tm_minute = minutes
	result.tm_second = seconds

	return true
}

! Roody's note: Adds time_object "time1" to time_object "time2", saving it to
! time_object "result"
routine AddTimes(time1, time2, result)
{
	local d
	if time1.tm_month = 4, 6, 9, 11
		d = 30
	elseif time1.tm_month = 2
	{
		if IsLeapYear(time1.tm_year)
			d = 29
		else
			d = 28
	}
	else
		d = 31

	result.tm_year = 0
	result.tm_month = 0
	result.tm_day = 0
	result.tm_hour = 0
	result.tm_minute = 0
!	result.tm_second = 0

	result.tm_second = time1.tm_second + time2.tm_second
	if result.tm_second >= 60
	{
		result.tm_second = result.tm_second - 60
		result.tm_minute++
	}

	result.tm_minute = result.tm_minute + time1.tm_minute + time2.tm_minute
	if result.tm_minute >= 60
	{
		result.tm_minute = result.tm_minute - 60
		result.tm_hour++
	}

	result.tm_hour = result.tm_hour + time1.tm_hour + time2.tm_hour
	if result.tm_hour >= 24
	{
		result.tm_hour = result.tm_hour - 24
		result.tm_day++
	}

	result.tm_day = result.tm_day + time1.tm_day + time2.tm_day
	if result.tm_day >= d
	{
		result.tm_day = result.tm_day - d
		result.tm_month++
	}

	result.tm_month = result.tm_month + time1.tm_month + time2.tm_month
	if result.tm_month >= 12
	{
		result.tm_month = result.tm_month - 12
		result.tm_year++
	}

	result.tm_year = result.tm_year + time1.tm_year + time2.tm_year
}

! Roody's note: Copies one time_object to another one
routine CopyTimeValue(time_orig, time_copy)
{
	time_copy.tm_year   = time_orig.tm_year
	time_copy.tm_month  = time_orig.tm_month
	time_copy.tm_day    = time_orig.tm_day
	time_copy.tm_hour   = time_orig.tm_hour
	time_copy.tm_minute = time_orig.tm_minute
	time_copy.tm_second = time_orig.tm_second

	return true
}

! Roody's note: Prints a time_object, with or without seconds.
routine PrintTimeValue(time, no_seconds)
{
	local printed

	if time.tm_year
	{
		NumberWord(time.tm_year)
		" year";
		if time.tm_year > 1
			print "s";
		printed++
	}

	if time.tm_month
	{
		if printed
			print ", ";
		NumberWord(time.tm_month)
		" month";
		if time.tm_month > 1
			print "s";
		printed++
	}

	if time.tm_day
	{
		if printed
			print ", ";
		NumberWord(time.tm_day)
		" day";
		if time.tm_day > 1
			print "s";
		printed++
	}

	if time.tm_hour
	{
		if printed
			print ", ";
		NumberWord(time.tm_hour)
		" hour";
		if time.tm_hour > 1
			print "s";
		printed++
	}

	if time.tm_minute
	{
		if printed
			print ", ";
		NumberWord(time.tm_minute)
		" minute";
		if time.tm_minute > 1
			print "s";
		printed++
	}

	if time.tm_second and (no_seconds = false or printed = 0)
	{
		if printed
		{
			if printed > 1
				",";
			" and ";
		}
		NumberWord(time.tm_second)
		" second";
		if time.tm_second > 1
			print "s";
	}
}

! A routine that counts the seconds since the "then_object" time object
routine SecondsSince(then_object)
{
	local a
	if then_object.tm_year or then_object.tm_day or then_object.tm_month
		return 1000
	a = then_object.tm_second + (then_object.tm_minute * 60) \
	    + (then_object.tm_hour * 60 * 60)
	return a
}

! A simple routine for comparing two time objects
routine IsTimeLonger(first, second)
{
	if first.tm_year > second.tm_year
		return true
	elseif first.tm_month > second.tm_month
		return true
	elseif first.tm_day > second.tm_day
		return true
	elseif first.tm_hour > second.tm_hour
		return true
	elseif first.tm_second > second.tm_second
		return true
	else
		return false
}
#endif ! #ifset USE_TIME_SYSTEM

!----------------------------------------------------------------------------
!* Music Jukebox
!----------------------------------------------------------------------------
!----------------------------------------------------------------------------
#ifset USE_JUKEBOX
object jukebox "music manager"
{}

time_object current_song_length
{}

global current_song

property length alias u_to
property file alias d_to
property artist alias s_to

class song
{}

routine PlayJukebox
{
	local a,success_play, retry
	while true
	{
		a = child(jukebox)
		if not a or retry > 5
		{
			current_song = 0
			music 0
			audio.current_music = 0
			jukebox is not switchedon
			return
		}
		if not hugor is switchedon
		{
			current_song_length.tm_minute = a.length
			current_song_length.tm_second = a.length #2
		}
#if defined MUSIC_RESOURCE_FILE
		success_play = PlayMusic(MUSIC_RESOURCE_FILE, a.file,100,0,1)
#endif
#if undefined MUSIC_RESOURCE_FILE
		success_play = PlayMusic(0, a.file,100,0,1)
#endif
		move a to jukebox
		if success_play
		{
			current_song = a
			jukebox is switchedon
			break
		}
		else
			retry++
	}
}

routine StopJukebox
{
	current_song = 0
	music 0
	audio.current_music = 0
	jukebox is not switchedon
}

routine PlaySong(songfile,loop)
{
	local success_play
	if not loop
	{
		current_song_length.tm_minute = songfile.length
		current_song_length.tm_second = songfile.length #2
	}
#if defined MUSIC_RESOURCE_FILE
	success_play = PlayMusic(MUSIC_RESOURCE_FILE, songfile.file,100,loop,true)
#endif
#if undefined MUSIC_RESOURCE_FILE
	success_play = PlayMusic(0, songfile.file,100,loop,true)
#endif
	if not success_play
	{
		current_song = 0
		music 0
		audio.current_music = 0
	}
	else
		current_song = songfile
}

routine CheckSongEnd
{
	if hugor is switchedon
	{
		if not ExecOpCode(is_music_playing)
			audio.current_music = 0
		return
	}
	GetCurrentTime(current_time)
	CalculateTimeDifference(current_time, song_start, time_difference)
	if IsTimeLonger(time_difference, current_song_length)
		audio.current_music = 0
}

routine NowPlaying
{
	CheckSongEnd
	if not audio.current_music
	{
		if jukebox is switchedon
			PlayJukebox
	}
	if not audio.current_music
		"There is no song currently playing."
	else
	{
		if current_song.artist
			print "\"";
		print current_song.name;
		if current_song.artist
		{
			print "\" by ";
			print current_song.artist;
		}
		print " is currently playing."
	}
}

object jukebox_player
{
	in main_instructions
	execute
	{
		if audio.current_music
			CheckSongEnd
		if not audio.current_music and jukebox is switchedon
			PlayJukebox
	}
}
#endif ! USE_JUKEBOX
!----------------------------------------------------------------------------
!* CONFIGURATION FILE HANDLER
!----------------------------------------------------------------------------

! Roody's note:
! This configuration file manager exists so that a game that uses several
! different systems saving information to a configuration file (like, say,
! colorlib.h, music settings, and whatever else a game may need to save) can
! easily write all of these things without a big headache to the author.
!
! Just declare a DATA_FILE constant before roodylib is included. This is the
! data file that will be created:
!
!constant DATA_FILE "nddata" ! creates a "nddata file"

! Example configuration saving thingie:
!\

property play_music ! just a property for us to save/load today

object jukebox_config "Next Day Jukebox v1"
{
	in config_instructions
	name_sum 0  ! we don't need a value here but we need the slot
	play_music 1
	first_time 1 ! set true if you want something to check for on the first play
	load_info
	{
		self.play_music = readval
!		self.first_time = readval  ! uncomment if you want the setup to run only
                                 ! the first time ever
	}
	save_info
	{
		writeval self.play_music
!		writeval self.first_time   ! uncomment if you want the setup to run only
                                 ! the first time ever
	}
	setup
	{
		if self.first_time
		{
			self.first_time = 0
			print "Do you want to play this game with music? ";
			self.play_music = YesOrNo
			return true ! returning true will cause InitScreen to be called
		}
	}
}
\!
!----------------------------------------------------------------------------
#ifset USE_CONFIG_SYSTEM

!\ FILE_CHECK_START - a "file start" version of FILE_CHECK. Is the first thing
written to a data file, so LoadSettings can check for it and cut out early if
it's not there.
\!
constant FILE_CHECK_START     4659

!\ configlib is an init_instructions object that calls each configuration file's
setup property. This allows pre-game questions like, "Do you want color?" and so
on. Have the setup property return true if InitScreen should be called
afterward.

	It also goes through all of the configuration objects and determines their
	names' StringSum values, saving them in their name_sum properties.
\!

property name_sum alias size
property load_info alias ne_to
! A save_info property already exists, thanks to RoodyLib
!property save_info alias e_to
property setup alias se_to
property first_time alias s_to

time_object before_save
{}

time_object after_save
{}

time_object time_diff
{}

object configlib
{
	in init_instructions
	execute
	{
		local i, l

		if not child(config_instructions)
			return
		if not CheckWordSetting("undo") and not CheckWordSetting("restore")
		{
			for i in config_instructions
			{
				l = string(_temp_string, i.name)
				i.name_sum = StringSum(_temp_string)
			}
		}
		if not CheckWordSetting("undo")
		{
			LoadConfigSettings
			if not CheckWordSetting("restore")
			{
				for i in config_instructions
				{
					if i.setup
						InitScreen
				}
				if not CheckTerpConfigAbility
					Config_Error
			}
		}
	}
}

routine CheckTerpConfigAbility
{
	local x
	GetCurrentTime(before_save)
	SaveConfigSettings
	GetCurrentTime(after_save)
	readfile DATA_FILE
	{
		x = readval
		x = readval
		before_save.tm_year = readval
		before_save.tm_month = readval
		before_save.tm_day = readval
		before_save.tm_hour = readval
		before_save.tm_minute = readval
		before_save.tm_second = readval
	}
	CalculateTimeDifference(after_save, before_save, time_diff)
	x = SecondsSince(time_diff)
	if x < 0 or x > 5
		return false
	return true
}

routine Config_Error
{}

!\ config_instructions is an object for holding all of the configuration file
objects \!

object config_instructions
{
	type settings
}

object config_config "Config File Terp Check v1"
{
	in config_instructions
	name_sum 0  ! we don't need a value here but we need the slot
	load_info
	{
		before_save.tm_year = readval
		before_save.tm_month = readval
		before_save.tm_day = readval
		before_save.tm_hour = readval
		before_save.tm_minute = readval
		before_save.tm_second = readval
	}
	save_info
	{
		writeval before_save.tm_year
		writeval before_save.tm_month
		writeval before_save.tm_day
		writeval before_save.tm_hour
		writeval before_save.tm_minute
		writeval before_save.tm_second
	}
}

routine LoadConfigSettings
{
	readfile DATA_FILE
	{
		local a, i
		while true
		{
			a = readval
			if a++ ~= FILE_CHECK_START
			{
				break
			}
			for i in config_instructions
			{
				a = readval
				if a ~= i.name_sum
					break
				else
					run i.load_info
			}
			a = readval
			break
		}
		if a ~= FILE_CHECK
			return false
		else
			return true
	}
}

routine SaveConfigSettings
{
	if not child(config_instructions)
		return

	writefile DATA_FILE
	{
		local i
		writeval FILE_CHECK_START
		for i in config_instructions
		{
			writeval i.name_sum
			run i.save_info
		}
		writeval FILE_CHECK
	}
	return (not system_status) ! I don't think this actually works but oh well
}

!\ StringSum adds up the ASCII value of a string. This extension uses
it to differentiate one configuration file object from another. \!

routine StringSum(arr)
{
	local i, sum

	while (array arr[i] and i < array arr[])
	{
		sum += array arr[i]
		i++
	}

	return sum
}
#endif

!----------------------------------------------------------------------------
!* DoVersion system
!----------------------------------------------------------------------------
! Roody's note: I figured it'd be nice if Roodylib provided a DoVersion
! routine by default. It's designed to be configurable and use whatever parts
! that you like, but you can always just straight up replace it with whatever
! you'd like.
!
! Just the same, you can #set NO_VERSION to disallow it.
!----------------------------------------------------------------------------

#ifclear NO_VERSION
#if defined GAME_TITLE ! skip this whole section if GAME_TITLE is not define

! Roody's note: I thought it'd be convenient to hold the compilation year
! in a global.

global build_year

object versionlib "versionlib"
{
	in init_instructions
	type settings
	execute
	{
!\  This automatically generates the current year at build time, stored in the
    global build_year    \!
		if word[LAST_TURN] ~= "undo","restore"
		{
			string(_temp_string, serial$, 8)
			local i,n, factor = 1
			for (i=7; i>5; i--)
			{
				n = _temp_string[i]
				if n >= '0' and n <= '9'
				{
					n -= '0'
					build_year += (n*factor)
				}
				elseif i = 0 and n = '-'
					build_year = -build_year
				else
					build_year = 0

				factor *= 10
			}
			build_year = build_year + 2000
		}
	}
}

! Roody's note: Replace this if you'd like to change the order or further tweak
! the DoVersion response.
routine DoVersion
{
	print GameTitle
#if defined BLURB
	 print BLURB ! "An Interactive Blahblahblah"
#endif
#ifclear NO_COPYRIGHT
	Copyright
#endif
	PrintBanner
	ReleaseAndSerialNumber
#if defined IFID
	print "IFID: "; IFID
#endif
#ifset BETA
	BetaNotes
#endif
#ifset DEMO_VERSION
	DemoNotes
#endif
	OtherNotes
}

routine HugorInfo
{
	if hugor is not switchedon
		return
!	select hugor.os
!		case 1 : print "Windows ";
!		case 2 : print "MacOS ";
!		case 3 : print "Linux ";
	print "Hugor v"; number hugor.version ;"."; \
		number hugor.version #2 ; "."; number hugor.version #3
}
! Roody's note: I changed TITLECOLOR to a global. Set it to something else in
! SetGlobalsAndFillArrays if you'd like to provide a special title color.

global TITLECOLOR = DEF_FOREGROUND

routine GameTitle
{
	color TITLECOLOR
	Font(BOLD_ON | ITALIC_OFF)
	print GAME_TITLE;
	Font(BOLD_OFF | ITALIC_OFF)
	color TEXTCOLOR
#ifset DEMO_VERSION
	print "\B (demo version)\b";
#endif
#ifset HUGOFIX
	print "\I (HugoFix Debugging Suite Enabled)\i";
#endif
}

routine Copyright
{
#if defined AUTHOR
	print "Copyright \#169 ";
#if defined FIRST_PUBLICATION
	print FIRST_PUBLICATION;
#endif
#if undefined FIRST_PUBLICATION
	print number build_year;
#endif
	" by ";
	print AUTHOR
#endif
}

routine PrintBanner
{
	print BANNER; " / ";
	print "Roodylib "; ROODYVERSION ;
	if hugor is switchedon
	{
		print " / Hugor v"; number hugor.version ;"."; \
			number hugor.version #2 ; "."; number hugor.version #3
	}
	print newline
}

routine ReleaseAndSerialNumber
{
	string(_temp_string, serial$, 8)
#if defined RELEASE
	print "Release "; RELEASE; " / ";
#endif
#if clear BETA
	print "Serial Number ";
	StringPrint(_temp_string, 0, 2)
	StringPrint(_temp_string, 3, 5)
	StringPrint(_temp_string, 6, 8)
#else
	" \BBETA BUILD # 20";
	StringPrint(_temp_string, 6, 8)
	StringPrint(_temp_string, 0, 2)
	StringPrint(_temp_string, 3, 5)
	Font(BOLD_OFF)
#endif
	print newline
}

#ifset BETA
routine BetaNotes
{
	color TITLECOLOR
	"\n\BDO NOT DISTRIBUTE!\b";
	color TEXTCOLOR
	" This beta release is intended for testing only, not for
	distribution to the general public.";
#if defined EMAIL
   " Please report any errors,
	bugs, etc. to \I<";
	print EMAIL;">\i."
#endif
	print newline
	if betalib is not special
		"HI, TESTERS:  Please type \B>SCRIPT ON\b and send in your transcripts."
}
#endif ! ifset BETA

#ifset DEMO_VERSION
routine DemoNotes
{
#if defined WEBSITE
	"\n(Like this demo?  Visit \B";
	print WEBSITE; "\b for the full version.)"
#endif
}
#endif ! DEMO_VERSION

! Roody's note: This routine exists solely to be replaced, if you'd like other
! trailing text to the DoVersion response.
routine OtherNotes
{
	local a
#ifclear NO_ACCESSIBILITY
	a++
#endif
	if a
		"(New players type \"INFO\")"
}
#endif ! if defined GAME_TITLE
#endif ! ifclear NO_VERSION

!----------------------------------------------------------------------------
!* REPLACED LAST LIBRARY OBJECT
!----------------------------------------------------------------------------

! Roody's note: We replace the "last_library_object" last so it is
! gets a higher object number than any previous object replacements.
!----------------------------------------------------------------------------
#ifset DEBUG
replace last_library_object
{}
#endif

!----------------------------------------------------------------------------
!* ROODYLIB MESSAGES
!----------------------------------------------------------------------------

! Roody's note:  RLibMessage handles both hugolib and verb routine messages
! as it's not really necessary to split them up into multiple routines.
! Still, objlib-esque messages are handled by RLibOMessage, which can be
! found further below.

routine RLibMessage(r, num, a, b)
{
	! Check first to see if the NewRLibMessages routine provides a
	! replacement message:
	if NewRLibMessages(r, num, a, b):  return

	select r
#ifset BETA
		case &BetaInit
		{
			select num
				case 1 : "This is a beta release! If you'd like to start a
				transcript right away, press \BB\b. Otherwise, push any other key
				to begin without starting a transcript."
				case 2
				{
					print "Starting beta transcript";
#if defined GAME_TITLE
					print " for "; GAME_TITLE ;
#endif
					if (TERP & NORMAL_TERP) and hugor is not switchedon
					{
						"."
						return
					}
					print " (";
					if hugor is switchedon
					{
						print "Hugor v"; number hugor.version ;"."; \
							number hugor.version #2 ; "."; \
							number hugor.version #3 ; " ";
						select hugor.version
							case 0: print "Unknown";
							case 1: print "Windows";
							case 2: print "MacOS";
							case 3: print "Linux";
					}
					else
					{
						select TERP
							case SIMPLE_TERP: print "minimal Hugo port";
							case GLK_TERP : print "glk Hugo port";
					}
					print ")"
				}
				case 3 : "No transcript started."
		}
#endif ! ifset BETA
		case &CheckReach
		{
			select num
				case 1
				{
					print capital The(a) ; IsorAre(a,true); \
					" inside "; The(parent(a)); "."
				}
		}
		case &CoolPause
		{
			select num
				case 1  ! default cheapglk "press a key"
				{
					Indent
					if a
						print a! ;
					else
						print "[PRESS A KEY TO CONTINUE]" !;
				}
				case 2  ! default normal "press a key"
				{
					Indent
					Font(ITALIC_ON)
					if a
						print a ! was ;
					else
						"press a key to continue" ! was ;
					Font(ITALIC_OFF)
				}
		}
		case &TopPause
		{
			select num
				case 1: print "[PRESS A KEY TO CONTINUE]";
		}
		case &DescribePlace
		{
			select num
				case 1
				{
					if not (FORMAT & DESCFORM_I)
						print ""
					else
					{
!						local col
!						col = display.cursor_column
!						if system(61) or not display.hasgraphics
!							--col
!						if col
!							"\n";
						print newline
					}
					Font(BOLD_ON)
					print capital a.name;

					! Print ", in <something>" if necessary
					if location = a and player not in a
					{
#ifset USE_SUPERCONTAINER
						if parent(player).type = SuperContainer
						{
							if InList(parent(player), contents_in, player)
								print ", ";  parent(player).prep #1; " ";
							else
								print ", "; parent(player).prep #2; " ";
						}
						else
#endif
						if parent(player).prep
							print ", "; parent(player).prep; " ";
						elseif parent(player) is platform
							print ", "; ON_WORD; " ";
						else
							print ", "; IN_WORD; " ";
						print Art(parent(player))
					}
					print newline
					Font(BOLD_OFF)
				}
#ifset USE_ATTACHABLES
				case 2
				{
					print CThe(a) ; " that "; The(player) ; IsorAre(player);
					print " holding is "; a.attached_desc ; " ";
					ListAttachments(a)
					print ".";
				}
#endif
		}
!		case &DoClose
!		{
!			select num
!			case 1:  print CThe(player); " can't close "; player.pronoun #4; "."
!		}
#ifclear NO_ACCESSIBILITY
		case &DoAccessibility
	{
		select num
			case 1
			{
				print "Normal screen-clearing";
#ifset USE_NEWMENU
				print " and menus";
#endif
				" restored."
			}
			case 2
			{
				print "Normal screen-clearing";
#ifset USE_NEWMENU
				print " and menus";
#endif
				" already set."
			}
			case 3
			{
				print "Screen-clearing disabled";
#ifset USE_NEWMENU
				print " and simple menus enabled";
#endif
				"."
			}
			case 4
			{
				print "Screen-clearing already disabled."
			}
	}
		case &DoCheapHelp
		{
			select num
				case 1
				{
					"To turn on \"cheap mode\", type \BCHEAP ON\b or
					\BCHEAPMODE ON\b. This is the default behavior. To turn cheap
					mode off, type \BCHEAP OFF\b or \BCHEAPMODE OFF\b."
				}
		}
		case &DoCheapOnOff
		{
			select num
				case 1
					"\"Cheap\" mode on."
				case 2
					"\"Cheap\" mode already on."
				case 3
				{
					print "Normal screen-clearing";
#ifset USE_NEWMENU
					print " and menus";
#endif
					" enabled."
				}
				case 4
				{
					print "Normal screen-clearing";
#ifset USE_NEWMENU
					print " and menus";
#endif
					print " already enabled."
				}
#ifset USE_NEWMENU
				case 5
					"\"Simple menus\" mode on."
				case 6
					"\"Simple Menus\" mode already on."
#endif
		}
		case &DoCheapToggle
		{
			select num
				case 1: "\"Cheap\" mode toggled off."
				case 2: "\"Cheap\" mode toggled on."
		}
#endif
		case &DoDrink
		{
			print CThe(player); " can't ";
			print word[1]; " ";
			print player.pronoun #4; "."
		}
		case &DoEat
		{
			print CThe(player); " can't ";
			print word[1]; " ";
			print player.pronoun #4; "."
		}
		case &DoEnter
		{
			select num
			case 1:
			{
				local x = 1
				print CThe(player); " can't ";
!				if object is platform
!					print "get on ";
!				else
!					print "enter ";
				while VerbCheck(word[x])
				{
					print word[x]; " ";
					x++
				}
				if object = player
				{
					print player.pronoun #4;
				}
				else
					print The(object);
				"."
			}
		}
		case &DoExit
		{
			select num
				case 1
				{
					print "Trying to get out of "; player.pronoun #3; " head";
					if player is plural and player_person ~= 2
					{
						print "s";
					}
					print "? Futile."
				}
				case 2
				{
					print CThe(player); " get"; MatchSubject(player); " ";
#ifset USE_SUPERCONTAINER
					if object.type = SuperContainer
					{
						if InList(object, contents_in, player)
							print "out";
						else
							print "off";
					}
					else
#endif
					if object.prep #2
						print object.prep #2;
					elseif object is platform
						print "off";
					else
						print "out";
					print " of "; The(object); "."
				}
		}
		case &EnterNumber
		{
			select num
				case 1: print "Enter a number >";
				case 2
				{
					print "Enter a valid number";
					if b
					{
						print " between "; number a ;
						print " and "; number b ;
					}
					print " >";
				}
		}
#ifclear NO_FANCY_STUFF
#ifclear NO_STRING_ARRAYS
		case &FindStatusHeight
		{
			select num
				case 1
				{
					if not light_source
						print "In the dark";
					else
					{
						print capital a.name;
						print "\_";
					}
				}
		}
#endif
#endif
#ifset USE_FOOTNOTES
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
#endif
		case &DoGet
		{
			select num
				case 1
				{
					print CThe(a); IsorAre(a,1); " wearing"; \
					MatchPlural(object,"that","those");"."
				}
				case 2 ! called with the AUTOMATIC_EXAMINE switch
				{
					print capital player.name; " pick";
					MatchSubject(player)
					print " up "; The(object);".";
				}
		}
		case &DoGo
		{
			select num
				case 1
				{
				! thought I might put some elevated platform code here
				!  but haven't yet
					print CThe(player); " will have to get ";
#ifset USE_SUPERCONTAINER
					if parent(player).type = SuperContainer
					{
						if InList(parent(player), contents_in, player)
							print "out ";
						else
							print "off ";
					}
					else
#endif
					if parent(player).prep #2
						print parent(player).prep #2; " ";
					elseif parent(player) is platform
						print "off ";
					else
						print "out ";
					print "of "; The(parent(player)); " first."
				}
				case 2	!  going to a non-enterable object in the same room
					print Cthe(object) ; IsorAre(object,true) ;" right here."
				case 3
				{
					print CThe(player); " can't go that way."
				}
		}
		case &DoHit
		{
			print "Venting "; player.pronoun #3; " frustrations
			on "; player.pronoun #4; " won't accomplish much."
		}
		case &DoListen
		{
			select num
				case 1   ! default location-listening
				{
					print capital player.pronoun #1;
					MatchPlural(player, "hears", "hear")
					" nothing unexpected."
				}
				case 2   ! default object-listening
					"That would serve no purpose."
		}
		case &DoLookUnder
		{
			print CThe(player); \
			MatchPlural(player, "doesn't", "don't"); \
			" find anything "; word[2]; " "; \
			player.pronoun #4; "."
		}
		case &DoMove
		{
			select num
				case 1
				{
					print CThe(player); " can't ";
					print word[1]; " ";
					print player.pronoun #4; "."
				}
		}
		case &DoOpen
		{
			select num
				case 1: print "Opening "; The(object);" reveals";
		}
		case &DoPushDirTo
		{
        ! Let's set default DoPushDirTo messages
			select num
				case 1: "That would not help the present situation."
				case 2
				{
					print "Try "; word[1] ; "ing "; Art(object); " in a direction."
				}
				case 3
				{
					if word[1] = "push","press"
						a = "es "
					else
						a = "s "
					print CThe(player); " "; word[1];
					if player is not plural
					{
						print a;
					}
					print " "; The(object); " over to the..."
				}
				case 4:  print CThe(object); " slows to a stop."
		}
	case &DoPutIn
	{
		select num
		case 1:
		{
			print "You'll have to be a little more specific
				about exactly where you'd like ";
			if player_person ~= 2:  print The(player, true); " ";
			print "to "; word[1] ; " that."
		}
	}
#ifclear NO_XVERBS
		case &DoQuit
		{
			select num
				case 1
					"Continuing on."
		}
		case &DoRestart
		{
			select num
				case 1 : print "\nOk, continuing on."
		}
		case &DoRecordOnOff
		{
			select num
				case 1: "No playback in progress."
				case 2: "Playback completed."
				case 3: "No playback while recording."
				case 4: "Playback already in progress."
				case 5
					"Pressing the \"+\" key at a MORE prompt may skip
					through the rest of the MORE prompts, depending on your
					interpreter."
				case 6: print "Skip in-game pauses and \"Keep waiting?\" prompts
				during playback? ";
				case 7: "No recording during playback."
				case 8: "Recording already in progress."
				case 9: "No recording in progress."

		}
		case &DoScriptOnOff
		{
			select num
				case 1: "Transcription is already on."
				case 2: "Transcription is not currently on."
				case 3: "Comment recorded!"
				case 4: "Comment not recorded! (Transcription is not on.)"
		}
#endif ! NO_XVERBS
#ifset SCORE_NOTIFY
		case &DoScoreNotifyOn
		{
			select num
				case 1:  "[Score notification already on.]"
				case 2: "[Score notification on.]"
		}
		case &DoScoreNotifyOff
		{
			select num
				case 1:  "[Score notification already off.]"
				case 2: "[Score notification off.]"
		}
		case &ScoreNotify
		{
			select num
				case 1
					print "[Your score has gone up by "; number a; " points.]"
				case 2
					print "[Your score has gone down by "; number a; " points.]"
		}
#endif ! SCORE_NOTIFY
		case &DoSmell
		{
			select num
				case 1
				{
					print capital player.pronoun #1;
					MatchPlural(player, "doesn't", "don't")
					" smell anything unusual."
				}
				case 2 : "Why do that?"
		}
		case &DoTakeOff
		{
			select num
				case 1
					print CThe(xobject); " doesn't let "; player.pronoun #2;"."
		}
		case &DoUnlock
		{
			select num
				case 1
				{
					print "Be more specific about what you want to unlock "; The(object);
					print " with."
				}
				case 2
				{
					print capital player.pronoun #1;
					print " can't reach "; The(a); ", which is currently in ";
					print The(parent(a)); "."
				}
				case 3
				{
					if a
						print "(with "; The(xobject); ")"
					print "Unlocked."
				}
		}
		case &DoLock
		{
			select num
				case 1
				{
					print "Be more specific about what you want to lock "; The(object);
					print " with."
				}
				case 2
				{
					if a
						print "(with "; The(xobject); ")"
					print "Locked."
				}
		}
#ifclear NO_XYZZY
		case &DoXYZZY
		{
		! text suggested by Rob O'Hara. Approved by Ben Parrish.
			print CThe(player); " mumble"; MatchSubject(player);
			" an ancient reference to an archaic game. Nothing happens."
		}
#endif
	case &EndGame
	{
		select num
		case 1
		{
			print "\nThe game has ended.  Do you want to (R)ESTART";
#ifclear NO_XVERBS
			print ", R(E)STORE a saved game";
#ifset NO_UNDO
			print ",";
#endif
#endif
#ifclear NO_UNDO
			if not UNDO_OFF         ! if not otherwise overridden
				print ", (U)NDO your last turn,";
#endif
			print " or (Q)UIT? ";
		}
		case 2
		{
			print "Enter (R)ESTART";
#ifclear NO_XVERBS
			print ", R(E)STORE";
#ifset NO_UNDO
			print ",";
#endif
#endif
#ifclear NO_UNDO
			if not UNDO_OFF
				print ", (U)NDO,";
#endif
			print " or (Q)UIT: ";
		}
	}
#ifset LIST_CLOTHES_FIRST
		case &ListClothesFirst
		{
			select num
				case 1
				{  ! "You are wearing";
					if (FORMAT & USECHARNAMES_F)
						print capital a.name;
					else
						print capital a.pronoun;
						IsOrAre(a,1)
						print " wearing";
				}
		}
#endif
		case &ListObjects
		{
			select num
				case 1: print " "; a;
				case 2: print ":"
		}
#ifset USE_NEWMENU
		case &MakeMenu
		{
			select num
				case 1 : "Opening menu..."
				case 2
				{
					if (TERP & SIMPLE_TERP)
					{
						local l,i
						l = string(_temp_string, a)
						print to (40 - l/2-1);
						print "+";
						for (i=1;i<=l ;i++ )
						{
							print "-";
						}
						print "+"
						print to (40 - l/2-1);
						print "|";
						print a;
						print "|"
						print to (40 - l/2 - 1);
						print "+";
						for (i=1;i<=l ;i++ )
						{
							print "-";
						}
						print "+"
					}
					else
					{
						Indent
						print "\_ "; a
						""
					}
				}
				case 3 : "Window resized. Redrawing screen."
		}
		case &Menu
		{
			select num
				case 1
				{
					if system(61)
						locate 2, display.cursor_row
					else
						locate 1, display.cursor_row
					print "[N]ext item"; to (display.linelength - 11); \
		    				"[Q]uit menu"
					if system(61)
						locate 2, display.cursor_row
					print "[P]revious item"; to (display.linelength - 17);
					print "[Enter] to select"
				}
				case 2
				{
					! The CheapGlk version now works off numbers to make
					! navigation as easy as possible in simple terps
					print "Select the number of your choice or \"Q\" to exit: ";
				}
		}
		case &Help_Hints
		{
			select num
				case 1
					"[Press 'H' for another hint, or 'Q' to quit]"; !;
				case 2
					print "[No more hints.  Press any key...]";
				case 3
					print "[No more hints.  Leaving page.]";
		}
		case &ShowPage
		{
			select num
				case 1
				{
!					"[OPENING PAGE]"
					if a.alt_name
						print "(Opening \""; a.alt_name;"\")"
					else
						print "(Opening \""; a.name;"\")"
				}
				case 2
					"[CLOSING PAGE]"
		}
		case &AfterMenu
		{
			select num
				case 1: print "\I...and now returning to the story.\i"
		}
#endif
		case &Parse
		{
			select num
				case 1: "You will need to type that in again."
		}
#ifset NO_FANCY_STUFF
		case &PrintStatusLine
		{
			if not location
				print "\_";
			elseif not light_source
				print "\_ In the dark";
			else
			{
				print "\_";
				print capital location.name;
			}
		}
#endif
		case &QuitGameText
		{
			"\IThanks for playing!\i"
		}
		case &RepaintScreen
		{
			"[Detected screen size change; redrawing screen...]"
		}
		case &UndoResponse
		{
			select num
				case 1:	"Undone."
				case 2
				{
					if UNDO_OFF
						"UNDO is not currently allowed."
					else
						print "Unable to undo."
				}
		}
		case &WhatsIn
		{
			select num
				case 1: print ":"
				case 2
				{
					print "Also ";
					if a.prep
						print a.prep; " ";
					elseif a is platform
						print "sitting on ";
					else
						print "inside ";
					The(a)
				}
				case 3
				{
					if a.prep
						print capital a.prep; " ";
					elseif a is platform
						print "Sitting on ";
					else
						print "Inside ";
					The(a)
				}
		}
#ifclear NO_FANCY_STUFF
#ifclear NO_STRING_ARRAYS
		case &WriteStatus
		{
			if not location
				print "\_";
			elseif not light_source
				print "\_In the dark";
			else
			{
				print "\_";
				print capital location.name;
			}
		}
#endif
#endif
		case &YesorNo
		{
			select num
			case 1
			{
				print "Please answer YES or NO: ";
			}
			case 2
				print "YES or NO: ";
		}
}

routine NewRlibMessages(r, num, a, b)
{
   select r
		case else : return false
   return true ! this line is only reached if we replaced something
}

#ifclear NO_OBJLIB
routine RlibOMessage(obj, num, a, b)
{
	! Check first to see if the New<blank>OMessages routine provides a
	! replacement message:
	!
	if NewRlibOMessages(obj, num, a, b):  return

	select obj

	case character
	{
		select num
			case 1
			{
				print CThe(xobject) ; " allows "; The(player) ; " to take ";
				print The(object); "."
			}
	}
	case door
	{
		select num
			case 1
			{
				print "(and then opening it)"
			}
			case 2
			{
				print "(unlocking "; Art(self);
				if a
					print " with "; The(a);
				" first)"
			}
	}
#ifset USE_VEHICLES
	case vehicle ! added in case I need to add any elevated special text
	{
		select num
		case 1
		{
			print "To walk, "; The(player); " will have to
				get "; self.prep #2; " of "; The(self); \
				".  Otherwise, try \""; self.vehicle_verbs; \
				"\" ";
			if self.#vehicle_verbs > 1
				print "or \""; self.vehicle_verbs #2; "\" ";
			print "and a direction."
		}
		case 2
		{
			print CThe(player); IsorAre(player); \
				" not ";
			if word[1] = "ride", "gallop", "trot", "canter"
				print "on ";
			else
				print "in ";
			"anything at the moment."
		}
		case 3
			"Good luck with that."
	}
#endif
#ifset USE_ATTACHABLES
	case attachable
	{
		select num
		case 1
		{
			local i, td
			print CThe(player); " can't move "; The(b); " while ";
			for (i = 1;i <= a.attached_to ;i++ )
			{
				if a.attached_to #i ~= b and a.attached_to #i is not mobile and
					a.attached_to #i is static
				{
					td = a.attached_to #i
					break
				}
			}
			if td
			{
				print The(a); IsorAre(a, true); " still "; \
				a.attached_desc; " to "; The(td);
			}
			else
			{
				print " still "; \
				a.attached_desc; " to "; The(a);
			}
			print "."
		}
		case 2
		{
			print CThe(player); " can't "; VerbWord; " "; \
			The(object); " "; object.attach_prep; " ";
			if xobject = player
				print player.pronoun #4; "."
			else
			   print The(xobject); "."
		}
	}
#endif
}

routine NewRlibOMessages(obj, num, a, b)
{
	select obj

!	case <object>
!	{
!		select num
!		case 1:
!	}

		case else : return false

	return true ! this line is only reached if we replaced something
}
#endif ! ifclear NO_OBJLIB

#endif ! _ROODYLIB_H