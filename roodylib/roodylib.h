!::
! ROODYLIB - A collection of hugolib updates and additions
!::

#ifclear _ROODYLIB_H
#set _ROODYLIB_H

constant ROODYBANNER "RoodyLib Version 3.6"
constant ROODYVERSION "3.6"

#ifset VERSIONS
#message "roodylib.h version 3.6"
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
#else
	print "This game";
#endif
	" uses \IRoodylib\i. RoodyLib is a collection of improvements written and/or
	suggested by Kent Tessman, Mike Snyder, Jason McWright, Robb Sherwin, Rob
	O'Hara, Paul Lee, and Roody Yogurt."
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
!		if i.type = fuse
!		{
			run i.fake_event
!		}
	}
}
#endif  ! NEW_FUSE

!\
Activate - added a warning for when it is called before the player global has been set.
\!
replace Activate(a, set)                ! <set> is for fuses only
{
	local err
	if not player
		{
		Font(BOLD_ON)
		print "[WARNING:  The player global must be set before
		daemon (object "; number a;") can be activated.]"
		err = true
		}
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
		print "[Activating "; a.name; " "; number a;
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

	remove a
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

! AssignPronoun(object)
! sets the appropriate pronoun global to <object>

!\ Roody's note: This version has added support for animals and things being
! called "it" instead of something gendered, written by Mike Snyder.
\!

replace AssignPronoun(obj)
{
#ifset NEW_STYLE_PRONOUNS
	if obj >= it_object and obj <= them_object
		return
#endif
	if parser_data[PARSER_STATUS] & PRONOUNS_SET:  return
	if obj = player:  return

	! No use if you can't refer to it
	if not obj.#noun and not obj.#adjective
		return

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
}

!\ Roody's note: This has some extra code added to avoid mobile object
confusion (written by Mike Snyder). \!

replace CalculateHolding(obj)
{
	local i

	obj.holding = 0
	for i in obj
	{
		if not (i is worn and i is clothing and obj = player)
			obj.holding = obj.holding + i.size
	}
}

replace CenterTitle(a, lines,force)
{
#ifset CHEAP
	if cheap and not force
		return
#endif !ifset CHEAP
	local l, g, s ! (simple port)
	g = IsGlk
	s = (not g and system(61))
	if not lines:  lines = 1

	if MENU_SELECTCOLOR = 0 and MENU_SELECTBGCOLOR = 0  ! not set yet
	{
		MENU_SELECTCOLOR = SL_TEXTCOLOR
		MENU_SELECTBGCOLOR = SL_BGCOLOR
	}

	Font(BOLD_OFF|ITALIC_OFF|UNDERLINE_OFF|PROP_OFF)
	l = string(_temp_string, a)
	if not system(61)
		window 0                        ! remove previous window

	if g
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
		if not s ! not non-glk simple port
			{
			color MENU_SELECTCOLOR, MENU_SELECTBGCOLOR
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
	if not s and not force ! non-glk simple port
		cls
	if not system(61) and not force
		locate 1, LinesFromTop
}

!

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
	local i,p

	if not obj or obj = parent(player)
		return true

#ifclear NO_VERBS
	if (verbroutine ~= &DoLook, &DoLookIn) and parent(object) and
		parent(object) ~= player
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

	p = obj.part_of
	while p
	{
	if Contains(parent(player), p)
		return true
	p = p.part_of
	}

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

!\ Roody's note : Replaced so a new line is still printed, even if indenting
is turned off. \!

replace Indent(newl)
{
	local i

	if newl
		print ""

	if override_indent or display.cursor_column > 1
		print AFTER_PERIOD;
	elseif not (FORMAT & NOINDENT_F) and INDENT_SIZE
	{
		! See display.cursor_column > 1, above
		!print newline
		print "\_";
		for (i=2; i<=INDENT_SIZE; i++)
			print " ";
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
	local i, obj, count, pluralcount
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
				obj is known
#endif
		}
		else
			obj is known

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

			if FORMAT & ISORARE_F
			{
				if list_count = 1 and id_count <= 1 and
					obj is not plural
				{
					print " "; IS_WORD;
				}
				else
				{
					print " "; ARE_WORD;
				}
				if FORMAT & LIST_F
					RLibMessage(&ListObjects,1) ! ":"
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
				print to (list_nest * 2);	! INDENT_SIZE);
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
					if FORMAT & FIRSTCAPITAL_F
						CArt(obj)
					else
						Art(obj)
				}
				else
				{
					if FORMAT & FIRSTCAPITAL_F
						print NumberWord(id_count, true);
					else
						print NumberWord(id_count);
					print " "; this_class.name;

					if this_class.type = plural_class
					{
						local k

						if FORMAT & LIST_F
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
									print to ((list_nest+1) * 2);	! INDENT_SIZE);
								}
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

				if FORMAT & FIRSTCAPITAL_F
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

		if obj is not hidden and FORMAT & LIST_F
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
			if list_nest = 1 and FORMAT & ISORAREHERE_F
			{
				if count + pluralcount > 1
					print " "; ARE_WORD;
				else:  print " "; IS_WORD;
				print " "; HERE_WORD;
				FORMAT = FORMAT & ~ISORAREHERE_F  ! clear it

				if not (FORMAT&LIST_F or FORMAT&TEMPLIST_F)
					override_indent = true
			}

			if not (FORMAT & NORECURSE_F)
				print ".";
		}

		i = 0

		for obj in thisobj
		{
			if children(obj) and obj is not hidden and
				(obj is not already_listed or
					thisobj ~= location)
			{
				if FORMAT & TEMPLIST_F
				{
					FORMAT = FORMAT | LIST_F & ~TEMPLIST_F
					i = true
					print newline
				}

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

!\ Roody's note: I replaced WhatsIn so printed things like the colon-printing
(":") are sent to RLibMessages for easier message configuring.
  Changed a carriage return to 'print newline' \!

replace WhatsIn(obj)
{
	local i, totallisted
	local initial_list_nest

	if FORMAT & NORECURSE_F
		return

	for i in obj
	{
		i is not already_listed
		if i is not hidden
			totallisted++
	}

	if totallisted = 0
		return

	list_count = totallisted

	if obj is not container or obj is platform or
		(obj is container and (obj is not openable or
			(obj is openable and
				(obj is open or obj is transparent)))) and
		(obj ~= player or FORMAT & INVENTORY_F) and obj is not quiet
	{
		SpecialDesc(obj)

		! If list_count is 0 now, but totallisted was not, something must have been
		! printed by SpecialDesc


		if list_count = 0
		{
			if FORMAT & INVENTORY_F and not (FORMAT & LIST_F) and
				list_nest = 0
			{
				print newline    ! was ' print "" '
			}
			return totallisted
		}

			if obj.list_contents
			return totallisted

		initial_list_nest = list_nest

		if FORMAT & LIST_F
		{
			if list_nest
			{
				! Indent the first time so that it lines up with
				! paragraph indentation:
				i = list_nest
				if list_nest = 1 and not (FORMAT & NOINDENT_F)
				{
					Indent
					i--
				}

				print to (i * 2);	! INDENT_SIZE);
			}
		}
		else
		{
			Indent
		}

		if obj.contains_desc    ! custom heading
		{
			if FORMAT & LIST_F
#ifset _ROODYLIB_H
				RLibMessage(&WhatsIn, 1 )  !  ":"
#else
				":"
#endif
		}
		else
		{
			if obj = player
			{
				if FORMAT & LIST_F and list_count < totallisted
					print "\n";

				! "You are carrying..."
				Message(&WhatsIn, 1, totallisted)

				if FORMAT & LIST_F
#ifset _ROODYLIB_H
					RLibMessage(&WhatsIn, 1 )  !  ":"
#else
					":"
#endif
			}
			elseif obj is living and not obj.prep
			{
				! "X has..."
				Message(&WhatsIn, 2, obj, totallisted)
				if FORMAT & LIST_F
#ifset _ROODYLIB_H
					RLibMessage(&WhatsIn, 1 )  !  ":"
#else
					":"
#endif
			}
			else
			{
				if list_count < totallisted
					! "Also sitting on/in..."
					Message(&Whatsin, 3, obj)
				else
					! "Sitting on/in..."
					Message(&Whatsin, 4, obj)
				The(obj)
				FORMAT = FORMAT | ISORARE_F
			}
		}

		ListObjects(obj)

		list_nest = initial_list_nest
	}
	return totallisted
}

!\ Roody's note: Added some code to SpecialDesc so that if the obj is the only
child of a parent, it still checks (and lists) any children of that obj. \!
replace SpecialDesc(obj)
{
	local a, c, flag, printed_blankline

	if FORMAT & LIST_F
		return

	list_count = 0
	for a in obj
	{
		if a is not hidden
		{
			c++
			flag = true
		}
		else
			flag = false

		if FORMAT & INVENTORY_F and obj = player and flag
		{
			if &a.inv_desc
				Indent
			if a.inv_desc
			{
				if FORMAT & LIST_F:  print newline
				AddSpecialDesc(a)
			}
		}

		elseif a is not moved and flag
		{
			if &a.initial_desc
			{
				print newline
				override_indent = false
				if FORMAT & INVENTORY_F and FORMAT & NOINDENT_F and not printed_blankline
					print ""
				printed_blankline = true
				Indent
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
			not a.list_contents
			{
				WhatsIn(a)
				list_count = 0
			}
		}
	}
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
				if ++count < 10
					Message(&YesorNo, 1)	! ask "yes" or "no"
				else
					Message(&YesorNo, 2)
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

! Roody's note: Changed the parse$-printing code.
#ifset HUGOFIX
replace DoHugoFix
{
	local i, n

	Font(PROP_OFF)		! proportional printing off

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
			object is known
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
				i is known
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
		object is known
	}
	case "$mp"
	{
		if parent(object) ~= 0 and object is not enterable
		{
			print "[Obj. "; number object; " (";
			print object.name; ") is not a room object]"
		}
		elseif parent(object) = 0 and object.type ~= room
		{
			print "[Obj. "; number object; " (";
			print object.name; ") is not a room object]"
		}
		else
		{
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
			'$ot 0' to draw the entire object tree.]"
			Font(DEFAULT_FONT)
			return
		}

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
			local a
#ifset _WINDOW_H
			if i.type = window_class
				a++
#endif
			if i ~= object and parent(i) = object
				DrawBranch(i,a)
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
				if not parse_rank_only or i.parse_rank
				{
					if ListParserConflicts(i, parse_rank_only)
						count++
				}
			}
			if not count
				"No parser conflicts found."
		}
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
		object is not known
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

! Roody's note - added some code so windows and replaced objects
! get listed when printing the object tree
replace DrawBranch(obj,win_count)
{
	local i, nextobj

	for (i=1; i<=list_nest; i++)
		print ". . ";
	print "["; number obj; "] ";
		if obj = 1
			print "(display)"
		elseif obj.name
			print obj.name
#ifset _WINDOW_H
		elseif obj.type = window_class
			{
			if obj = window_class
				print "(window_class)"
			else
				print "(untitled window #"; number (win_count - 1) ; ")"
			}
#endif
		else
			print "<replaced object>"

	for nextobj in obj
	{
		list_nest++
		DrawBranch(nextobj)
		list_nest--
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
		if a.type ~= room
			{
			if FindObject(a,location)
                              {
                              print "\_  ";
			      print a.name
                              }
			}
		}
}

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
#endif  !ifset USE_VERBSTUB
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

	PrintStatusLine                 ! update one last time
	PrintEndGame(end_type)          ! print appropriate ending message

#ifclear NO_VERBS
	local r

	Message(&EndGame, 1,end_type)    ! ask to RESTART, RESTORE, (UNDO), or QUIT

	r = -1
	while r = -1
	{
	GetInput
	r = ProcessKey(word[1], end_type)
	if r = -1
			Message(&EndGame, 2,end_type)    ! ask again (more succinctly)
	}

	return r

#else   ! i.e., #elseif set NO_VERBS

	return 0;
#endif
}

! Roody's note: Companion routine to EndGame
routine ProcessKey(entry,end_type)
{
   local r, i
   r = -1
	if not entry
		return -1
   select entry
	! we do SpecialKey first since options like RESTART can clear word[1]
	! and trigger SpecialKey (which sometimes equals false)
		    case SpecialKey(end_type)
	    {
	    SpecialRoutine
	    }
            case "restart", "r"
            {
					ClearWordArray
					word[1] = "r"
					SaveWordSetting("restart")
					for i in init_instructions
						{
						if i.save_info
							SaveWordSetting(i.name)
						}
               if restart
               {
                  r = 1
                  PrintStatusline
               }
   #ifclear NO_XVERBS
               else
                  Message(&DoRestart, 2)   ! failed
   #endif

            }
            case "restore", "e"
            {
				ClearWordArray
				word[1] = "e"
				SaveWordSetting("restore")
				for i in init_instructions
					{
					if i.save_info
						SaveWordSetting(i.name)
					}
   #ifclear NO_XVERBS
               if Perform(&DoRestore)
                  r = 1
   #else
               if restore
               {
                  r = 1
						for i in init_instructions
							{
							if CheckWordSetting(i.name)
								run i.execute
							}
						VMessage(&DoRestore, 1)         ! "Restored."
                  PrintStatusline
                  DescribePlace(location,true)
#ifset NEW_FUSE
						fake_runevents
#endif
               }
					else:  VMessage(&DoRestore, 2)           ! "Unable to restore."
   #endif
            }
   #ifclear NO_UNDO
            case "undo", "u"
            {
               if not UNDO_OFF
               {
						SaveWordSetting("undo")
						for i in init_instructions
							{
							if i.save_info
								SaveWordSetting(i.name)
							}

                  if undo
                  {
                     r = true
							for i in init_instructions
								{
								if CheckWordSetting(i.name)
									run i.execute
								}
                     PrintStatusLine
                     DescribePlace(location)
#ifset NEW_FUSE
							fake_runevents
#endif
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
#endif
							last_turn_true = true
                  }
   #ifclear NO_XVERBS
                  else
                     Message(&DoUndo, 1)  ! failed
               }
               else
                  Message(&DoUndo, 1)
   #else
               }
		else
			VMessage(&DoUndo)
   #endif
            }
   #endif
            case "quit", "q"
            {
				""
				RLibMessage(&DoQuit,1) ! "Thanks for playing!"
				""
				display.title_caption = PRESS_ANY_KEY
					if not system(61) ! if not simple port simple port
						{
						print PRESS_ANY_KEY;
						HiddenPause
						}
               r = 0
            }
   return r
}

routine SpecialKey(end_type)
{
}

routine SpecialRoutine
{}

! Roody's note: Future Boy! replacement by Kent Tessman that omits objects
! held by NPCs unless specically mentioned ("GET ALL FROM FRED")
replace ExcludeFromAll(obj)
{
	if obj is hidden
		return true

! in most cases, disallow >WEAR ALL / REMOVE ALL
#ifclear USE_CHECKHELD
	if verbroutine = &DoTakeOff, &DoWear
#else
	if verbroutine = &DoTakeOff_Checkheld, &DoWear_Checkheld
#endif
	{
#ifset AIF
! Make >WEAR ALL and >REMOVE ALL only acknowledge clothes
		if obj is not clothing
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

! Roody's note: FindObject tweaked to get rid of jumps, just because
! Added some new checkheld code
replace FindObject(obj, objloc, recurse)
{
	local a, p
	local this_parse_rank
	local found_result = true
	local FindObjectIsFound

	if obj = nothing or obj = player
	{
		if obj = nothing and not recurse
		{
#ifset DEBUG
			if debug_flags & D_FINDOBJECT
			{
				print "[Resetting FindObject]"
			}
			if debug_flags & D_PARSE_RANK
			{
				print "[parser_data[BEST_PARSE_RANK] = 0]"
			}
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
		obj is known
		return true
	}

#ifclear NO_FUSES
	if obj.type = fuse or obj.type = daemon
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

	! Do some parse_rank tweaking to rank lower objects which (probably) don't
	! apply to a particular command, like trying to open something that's
	! already open:
	this_parse_rank = obj.parse_rank

	! And be ready to prefer the last specifically referred to object in
	! the event of disambiguation
	if obj = parser_data[LAST_SINGLE_OBJECT]
		this_parse_rank++

#ifclear NO_VERBS
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
#else
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
#endif
#endif	! #ifclear NO_VERBS

#ifset USE_CHECKHELD
	if checkheld is active
	{
		if checkheld is plural		! >DROP ALL, etc. ...
		{
			if CheckHeld_Verb_Check
			{
				if obj is not checkheld_flag
				{
#ifset DEBUG
					if debug_flags & D_FINDOBJECT
					{
						print "[FindObject("; obj.name; " ["; number obj; "], "; \
							objloc.name; " ["; number objloc; "]):  "; \
							"false (not checkheld_flag)]"
					}
#endif
					return false
				}
			}
		}
		elseif obj is checkheld_flag	! ...or >DROP OBJECT, etc.
		{
			if CheckHeld_Verb_Check
			{
				this_parse_rank += 100
			}
		}
	}
#endif	! USE_CHECKHELD

	! The objloc argument is equal to 0 if a) the grammar token is
	! "anything", or b) the engine is re-testing object availability
	! given the setting of parser_data[BEST_PARSE_RANK] during the first
	! pass of object disambiguation.

	if objloc = 0
	{
#ifclear NO_VERBS
		if verbroutine = &DoGet and not recurse
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
				return false
			}
		}
#endif
		if location
		{
			if FindObject(obj, location, true)
				object is known
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
			if parser_data[PARSE_RANK_TESTS]++ = 0
			{
#ifset DEBUG
				if debug_flags & D_FINDOBJECT and not recurse
				{
					print "[FindObject("; obj.name; " ["; number obj; "], "; \
						objloc.name; " ["; number objloc; "]):  "; \
						"true]"
				}
				if debug_flags & D_PARSE_RANK and this_parse_rank > parser_data[BEST_PARSE_RANK]
				{
					print "[parser_data[BEST_PARSE_RANK] = "; number this_parse_rank; "]"
				}
#endif
				parser_data[BEST_PARSE_RANK] = this_parse_rank
				return true
			}

			if this_parse_rank < parser_data[BEST_PARSE_RANK]
			{
#ifset DEBUG
				if debug_flags & D_FINDOBJECT and not recurse
				{
					print "[FindObject("; obj.name; " ["; number obj; "], "; \
						objloc.name; " ["; number objloc; "]):  "; \
						"false (this_parse_rank = "; number this_parse_rank; \
						" < parser_data[BEST_PARSE_RANK] = "; number parser_data[BEST_PARSE_RANK]; ")]"
				}
#endif
				return false
			}
			else
			{
#ifset DEBUG
				if debug_flags & D_PARSE_RANK and this_parse_rank > parser_data[BEST_PARSE_RANK]
				{
					print "[parser_data[BEST_PARSE_RANK] = "; number this_parse_rank; "]"
				}
#endif
				parser_data[BEST_PARSE_RANK] = this_parse_rank
			}
		}

#ifset DEBUG
		if debug_flags & D_FINDOBJECT and not recurse
		{
			print "[FindObject("; obj.name; " ["; number obj; "], "; \
				objloc.name; " ["; number objloc; "]):  "; \
				"true]"
		}
#endif
		return true
	}

	if word[2] = "~all" or word[3] = "~all" and not recurse
	{
		if not IsPossibleXobject(obj) or (xobject and xobject ~= obj)
		{
			a = obj.exclude_from_all
			if a < 2
			{
				if a or ExcludeFromAll(obj) = true
				{
#ifset DEBUG
					if debug_flags & D_FINDOBJECT and not recurse
					{
						print "[FindObject("; obj.name; " ["; number obj; "], "; \
							objloc.name; " ["; number objloc; "]):  "; \
							"false (excluded from \"all\")]"
					}
#endif
					return false
				}
			}
		}
	}

	if obj is not living
		parser_data[PARSER_STATUS] |= FINDOBJECT_NONLIVING

	p = parent(obj)

	! If any object FindObject is attempting to disambiguate
	! is known, make a note of it (if during the parsing phase)
	if parser_data[PARSER_STATUS] & PARSER_ACTIVE
	{
		if ObjectisKnown(obj)
			parser_data[PARSER_STATUS] |= FINDOBJECT_KNOWN
	}

	if obj = objloc or
		(player in obj and obj ~= objloc and (obj~=location or not recurse)) or
		p = obj or p = objloc or p = player
	{
		obj is known
		FindObjectIsFound = true
	}
	elseif (p is not openable or p is platform) and p is not quiet and
		p ~= nothing
	{
		if FindObject(p, objloc, true) and ObjectisKnown(p)
		{
			obj is known
			FindObjectIsFound = true
		}
	}
	elseif p is openable and p is open and p is not quiet and
		p ~= nothing
	{
		if FindObject(p, objloc, true) and ObjectisKnown(p)
		{
			obj is known
			FindObjectIsFound = true
		}
	}
	elseif (p is openable, not open, transparent, not quiet) and
		p ~= nothing
	{
		if FindObject(p, objloc, true) and ObjectisKnown(p)
		{
			obj is known
			found_result = 2
			FindObjectIsFound = true
		}
	}

	if obj.#found_in and not FindObjectIsFound
	{
		for (a=1; a<=obj.#found_in; a++)
		{
			if obj.found_in #a and (obj.found_in #a = objloc or
				FindObject(obj.found_in #a, objloc, true))
			{
				obj is known
				FindObjectIsFound = true
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
				{
					obj is known
					FindObjectIsFound = true
				}
				if FindObject(obj.in_scope #a, objloc, true)
				{
					obj is known
					FindObjectIsFound = true
				}
			}
		}
	}

	if not FindObjectIsFound
	{
#ifset DEBUG
		if debug_flags & D_FINDOBJECT and not recurse
		{
			if obj.type ~= fuse and obj.type ~= daemon
			{
				print "[FindObject("; obj.name; " ["; number obj; "], "; \
					objloc.name; " ["; number objloc; "]):  "; \
					"false]"
			}
		}
#endif
		return false
	}

! FindObjectIsFound area
	if not recurse
	{
		if parser_data[PARSE_RANK_TESTS]++ = 0
		{
#ifset DEBUG
			if debug_flags & D_PARSE_RANK and this_parse_rank > parser_data[BEST_PARSE_RANK]
			{
				print "[parser_data[BEST_PARSE_RANK] = "; number this_parse_rank; "]"
			}
#endif
			parser_data[BEST_PARSE_RANK] = this_parse_rank
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
		}
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

#ifset USE_CHECKHELD
! The new code in this routine makes it so items must be held for >WEAR ALL or
! >REMOVE ALL to work
routine CheckHeld_Verb_Check
{
	if (verbroutine = &DoDrop_CheckHeld, &DoPutonGround_CheckHeld) or
				(verbroutine = &DoPutIn_CheckHeld and xobject)				or
				(verbroutine = &DoTakeOff_CheckHeld, &DoWear_Checkheld)
	return true
}
#endif

!\ Roody's note: This version of HoursMinutes should work for several
"days" (up to 22) \!
replace HoursMinutes(val, military)
{
	local hours, minutes

	hours = val / 60
	minutes = val - hours * 60

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
   local v, obj, xobj, act, ret, LeavingMovePlayer

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

#ifset USE_ATTACHABLES
   if ObjectisAttached(player, location, loc)
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
   object = loc
   xobject = location
   actor = player
   if not ignore
   {
      while (true)
         {
         ret = player.before
         if ret
            {
            LeavingMovePlayer = true
            break
            }
         ret = location.before
         if ret
            {
            LeavingMovePlayer = true
            break
            }
         ret = loc.before
         if ret
            {
            LeavingMovePlayer = true
            break
            }
         break
         }
   }
	if (not LeavingMovePlayer)
	{
		move player to loc
!		local simpleport
!		if system(61) and ! minimal port and..
!		not IsGlk! not glk
!			simpleport = true
!		if not simpleport and old_location
!			PrintStatusline  ! will be printed again by main, anyway

      old_location = location
      if parent(loc) = 0              ! if it's likely a room object
         location = loc
      else                            ! if it's an enterable object
         location = parent(loc)  ! (noting that the object must be
                  ! in a room, not inside another
                  ! non-room object)

#ifset USE_ATTACHABLES
      MoveAllAttachables(player, old_location, location)
#endif
      }

! LeavingMovePlayer area

   if not ret
   {
      if not FindLight(location)
         DarkWarning
      elseif not silent
      {
         DescribePlace(location)
         if not event_flag
            event_flag = true
         location is visited
      }

   ! Check if there's an after routine for MovePlayer in the new
   ! location:
   !
      if not ignore
      {
         ret = player.after
         if not ret
            ret = location.after
      }
   }
   verbroutine = v
   object = obj
   xobject = xobj
   actor = act

   return ret
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

! Roody's note: Took out Parse's HugoFix code, as it tries to print parse$
! before parse$ is set also got rid of jump
replace Parse
{
	local a
	local reparse
	local temp_verb, temp_obj, temp_xobj
	local JumpToEnd

	ResetParse

	list_nest = 0
	event_flag = 0

	if nothing ~= 0                   ! best place to check this
		print "[WARNING:  Objects/classes defined before library]"

#ifset DEBUG
	if debug_flags & D_PARSE_RANK
	{
		print "[parser_data[BEST_PARSE_RANK] = 0]"
	}
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

	if word[1] = "~oops"
		return

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

! The following, devised by Jim Newland, checks to see if the player
! input refers to an unnecessary item of scenery (for example) which is
! nonetheless mentioned in the location description.

	for (a=2; a<=words and word[a]~="" and word[a]~="then"; a++)
	{
		if Inlist(player, extra_scenery, word[a])
		{
			Message(&Parse, 1)
			word[1] = ""            ! force ParseError(0)
			words = 0
			customerror_flag = true
			return true
		}
		elseif Inlist(location, extra_scenery, word[a])
		{
			Message(&Parse, 1)
			word[1] = ""            ! force ParseError(0)
			words = 0
			customerror_flag = true
			return true
		}
	}

	if PreParse:  reparse = true            ! easily replaceable routine

	! Last player-specified object
	if object > 0 and object < objects and last_object ~= -1
		AssignPronoun(object)
	parser_data[LAST_PARSER_STATUS] = 0

	! Must do this after AssignPronoun, and reset it to 0
	! after both of Perform and ParseError:
	parser_data[PARSER_STATUS] = PARSER_ACTIVE

#ifset NEW_STYLE_PRONOUNS
	move it_object to parent(it_obj)
	move him_object to parent(him_obj)
	move her_object to parent(her_obj)
	move them_object to parent(them_obj)
#endif

	! To repeat last command
	if (word[1] = "again" or word[1] = "g") and word[2] = ""
	{
		for (a=1; a<=oldword[0]; a++)
			word[a] = oldword[a]
		words = oldword[0]
		reparse = true
		JumpToEnd = true
	}
	if  not JumpToEnd
		{

		local count
#ifclear NEW_STYLE_PRONOUNS
		local n, number_pronouns
#endif

		for (count=2; count<=words and word[count]~=""; count++)
		{
			select word[count]

#ifclear NEW_STYLE_PRONOUNS
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
					! Avoid, e.g., "put it it something"
					elseif word[count] ~= word[count+1]
					{
						! For her_obj, we can run into trouble, since
						! it's both possessive and a pronoun, so only
						! replace "her" where it's the last word:
						if a ~= her_obj or
							(a = her_obj and (count = 2 or count = words))
						{
							n = SetObjWord(count, a)
							replace_pronoun[number_pronouns] = a
							number_pronouns++
							if n > 1
								count = count + (n - 1)
							parser_data[PARSER_STATUS] |= PRONOUNS_SET
							reparse = true
						}
					}
				}
#endif

				! Allow for "then..." and "and then..."
				case "then"
				{
					! end of this command
					word[count] = "."
					if word[count-1] = "~and"
						DeleteWord(count-1)
					reparse = true
					break
				}
		}

#ifclear NEW_STYLE_PRONOUNS
		if number_pronouns = 2 and replace_pronoun[0] = replace_pronoun[1]
			number_pronouns--

		if number_pronouns > 0
		{
			PrintReplacedPronouns(number_pronouns)
		}
#endif

	! Reword imperatives given as "Tell Bob to do something" as "Bob, do
	! something" so that the engine automatically reroutes them to SpeakTo

		if word[1] = "tell", "order", "instruct", "ask", "command"
		{
			for (a=1; a<=words and word[a]~=""; a++)
			{
				if word[a] = "to"
				{
					!DeleteWord(a)   ! "to"
					word[a] = "~and"
					DeleteWord(1)   ! "tell", "order", etc.
					reparse = true
					break
				}
			}
		}

#ifset USE_PLURAL_OBJECTS
		if ParsePluralObjects:  reparse = true
#endif

#ifset USE_CHECKHELD
		ParseCheckHeld
#endif

		! Store current command for future reference
		local ow
		for (a=1; a<=words and a<MAX_WORDS; a++)
		{
			if word[a] = "g", "again"
			{
				if word[a+1] = ""
					a++
			}
			else
				oldword[++ow] = word[a]
		}
		oldword[a] = ""
		oldword[0] = ow

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
				{
				"(* Not an \Iactual\i referable token.)"
				}
			}
		! this section so we print parse$ only once
		if string(_temp_string,parse$) and
			not (parser_data[PARSER_STATUS] & 32)
			{
			print "[parse$ = \""; parse$; "\"]"
			parser_data[PARSER_STATUS] |= 32
			}
		print "[ParseError errornumber = "; number errornumber; "]"
		Font(BOLD_OFF)
	}
#endif

	local r
	r = PreParseError(errornumber,obj)
	if r
	{
		parser_data[PARSER_STATUS] = PARSER_RESET
		return r
	}

	r = NewParseError(errornumber, obj)
	if r
	{
		parser_data[PARSER_STATUS] = PARSER_RESET
		return r
	}

	if errornumber >= 100
	{
		CustomError(errornumber, obj)
		word[1] = ""            ! force ParseError(0)
		words = 0
		customerror_flag = true
		parser_data[PARSER_STATUS] = PARSER_RESET
		return true
	}
	if customerror_flag
	{
		customerror_flag = 0    ! CustomError already printed error
		parser_data[PARSER_STATUS] = PARSER_RESET
		return true
	}

	select errornumber

		case 1
		{
			print CThe(player); \
				! " can't use the word \""; \
				MatchPlural(player, "doesn't", "don't"); \
				" need to use the word \""; \
				parse$;
#ifset AMERICAN_ENGLISH
				".\""
#else
				"\"."
#endif
		}
		case 3
			{
			local a, b
			 select word[1]
				case "x" : {
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
				case "get", "step"
								{
								a = word[1]
								if word[2] ~= "~all"
									{
									if (word[2] = "outof")
										{
										b = "out of"
										}
									elseif word[2] = "offof"
										{
										b = "off of"
										}
									else
										b = word[2]
									}
								}
				case else : a = parse$
			print CThe(actor); " can't "; a;
			if b
				print " "; b ;
				" multiple
				objects like that."
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
				if parser_data[PARSER_STATUS] & PRONOUNS_SET
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

		case 9
		{
#ifclear AIF
#ifclear USE_CHECKHELD
			if verbroutine = &DoWear, &DoTakeOff
#else
			if verbroutine = &DoWear_Checkheld, &DoTakeOff_Checkheld
#endif
			{
				print "Be specific about what you'd like ";
							if player_person ~= 2:  print The(player, true); " ";
				print "to ";
				if word[1] = "take"
				{
					print "take off";
				}
				else
					print parse$;
				"."
				return true
			}
#endif
			print "Nothing to ";
			if verbroutine
				print parse$; "."
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
			if obj is living and not (parser_data[PARSER_STATUS] & FINDOBJECT_NONLIVING) and obj.pronoun #2 ~= "it"
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
			elseif obj is living
			{
!\
				print CThe(obj); \
					MatchPlural(obj, "isn't", "aren't"); " here."
\!
				print CThe(actor); \
					MatchPlural(actor, "doesn't", "don't"); \
					" see";
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
					" can't do that with "; TheOrThat(obj) ; "."
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
			}
		}

		case else
		{
			parser_data[PARSER_STATUS] = PARSER_RESET
			return false            ! print the default message
		}

	parser_data[PARSER_STATUS] = PARSER_RESET
	return true                             ! message already printed
}

!\ Roody's note - PreParseError is a routine solely for being replaced.
Use it for any code you want to run before parser error messages are printed.
\!
routine PreParseError(errornumber,obj)
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
		parser_data[PARSER_STATUS] |= PRONOUNS_SET
		PrintReplacedPronouns(number_pronouns)
	}
#endif

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
			local rank
			rank = (score*MAX_RANK)/MAX_SCORE
			if rank > MAX_RANK
				rank = MAX_RANK
			print ranking[rank];
!\ Changed so that if the score goes over the MAX_SCORE, it continues
to give the highest ranking. \!
		}
		print "."
	}
}

!\ Roody's note: This version of RunScripts allows scripts to be run with a
true/false argument. \!
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
			if actor in location:  actor is known
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
	obj is known

	if list_nest = 1
		print newline

	AssignPronoun(obj)

	if obj is not moved and &obj.initial_desc
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

		local a
		a = obj.short_desc
		if a > 1
		{
			print a ;
		}
		elseif not a
!		if &obj.short_desc > 1
!		{
!			print obj.short_desc ;
!		}
!		elseif not obj.short_desc
			return
		}

!  "ListContents" section
	obj is already_listed
	AssignPronoun(obj)

	list_count = 0
!	if children(obj) > 0 and (obj is not container or
!		(obj is container and obj is not openable) or
!		(obj is container and obj is openable and
!			(obj is open or obj is transparent))) and
!		obj is not quiet

	if children(obj) > 0 and (obj is platform or obj is transparent or
	(obj is container and
	(obj is not openable or (obj is openable and obj is open)))) and
	obj is not quiet
	{
		list_nest = 1
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
	a = actor
	actor = player
	verbroutine = &SpeakTo
	retval = player.before
	actor = a

	if retval
	{
#ifset DEBUG
		if debug_flags & D_PARSE
		{
			print "\B["; player.name;
			if debug_flags & D_OBJNUM
				print " ["; number player; "]";
			print ".before returned "; number retval; "]\b"
		}
#endif
		return retval
	}

	retval = location.before
	if retval
	{
#ifset DEBUG
		if debug_flags & D_PARSE
		{
			print "\B["; location.name;
			if debug_flags & D_OBJNUM
				print " ["; number location; "]";
			print "before returned "; number retval; "]\b"
		}
#endif
		return retval
	}

	verbroutine = v

	if char is not living
	{
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

	if char is unfriendly
		IgnoreResponse = true
	else
	{
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
					{
						IgnoreResponse = true
					}
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
		"give", "offer"  , "remove", "wear"
	{
		return 1			! explicitly held
	}
	return 0
}
#endif

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

This is actually useful when the relevant object is *no longer* there,
as the player will just get "You don't see that." instead of

"(Assuming you mean the so-and-so)
 You don't see that."

Which can sometimes show the seams in your programming.

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
	if CheckObject(obj, box1) ! (see next Roodylib entry)
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
	misc { return it_obj }
	parse_rank 1000
	found_in
	{
		local obj
		obj = self.misc
		if FindObject(obj, location)
			return parent(obj)
		else
			return false
	}
}

it_object him_object "him"
{
	nouns "him"
	misc { return him_obj }
}

it_object her_object "her"
{
	nouns "her"
	misc { return her_obj }
}

it_object them_object "them"
{
	nouns "them"
	misc { return them_obj }
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

					if Perform(&DoGet, b)
						player.holding = a + b.size
					else
						player.holding = a
					object = obj
					xobject = xobj

					if b not in object
						object.holding = object.holding - b.size
				}

				thisobj = nextobj
			}

			run object.after
			return true
		}
	}
}

class held_to_player
{
	empty_type HELD_T
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
		{
			return false
		}
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
		{
			ParseError(12, obj) ! "you can't do that with that."
		}
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
property terp_type alias w_to   ! this property gets set to the current
                                !   interpreter type automatically
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

! "terp_type" values  0, 2, 4
enumerate step * 2
{
    NORMAL_TERP, GLK_TERP = 2, SIMPLE_TERP
}

!\ a PrintStatusLine object in which we will put our instruction objects in (by
inclusion of extensions and what not. PrintStatusLine will call this directly).
\!
object printstatuslib
{
	find_height
	{
		local highest, i, a
		if children(self) ~= 1
		{
			for i in self
			{
				a = i.find_height
				if i.status_override
				{
					self.chosen_window = i
					highest = i.find_height
					break
				}
				if higher(highest,a) = a
				{
					self.chosen_window = i
					highest = i.find_height
				}
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
	terp_type NORMAL_TERP
	bottom_justified 1
	status_top 0
}

replace PrintStatusline
{
local newstatusheight, window_top

! set the "terp_type" value
if IsGlk
    printstatuslib.terp_type = GLK_TERP
elseif system(61) ! minimal port
        printstatuslib.terp_type = SIMPLE_TERP
else
    printstatuslib.terp_type = NORMAL_TERP
#ifclear CHEAP
	local cheap
#endif

		if cheap and printstatuslib.terp_type ~= SIMPLE_TERP
			{
#if defined GAME_TITLE
			CenterTitle(GAME_TITLE,0,1)
#endif
#if undefined GAME_TITLE
			CenterTitle(CheapTitle,0,1)
#endif
			display.needs_repaint = false
			return
			}
		elseif cheap
			return

! figure out the size our window will be
newstatusheight = printstatuslib.find_height
window_top = printstatuslib.status_top

if not window_top
	window_top = 1

! clear/remove the window if the status window height has changed
if (newstatusheight < display.statusline_height) and not system(61)
	{
	window display.statusline_height
	{cls} ! clear whatever's there
	window 0
	}

display.statusline_height = newstatusheight

Font(BOLD_OFF | ITALIC_OFF | UNDERLINE_OFF | PROP_OFF)
window 1, window_top, display.screenwidth,
	(display.statusline_height + window_top - 1)
    {
    if printstatuslib.terp_type ~= SIMPLE_TERP
        {
		  color SL_TEXTCOLOR, SL_BGCOLOR
        cls
        locate 1,1
        }
    run printstatuslib.draw_window
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
local a, b
text to _temp_string
! can't start off a string with a space, it seems
!if not location     ! so we'll save this space-writing code for the
!    print "\_";     ! "status-writing" routine
!else
if not light_source
    print "In the dark";
else
    {
        print capital location.name;
!		 if FORMAT & DESCFORM_F
        print "\_";
    }
text to 0
a = StringLength(_temp_string)
text to _temp_string
select STATUSTYPE
	case 1 : print number score; " / "; number counter;
	case 2 : print HoursMinutes(counter);
	case 3 : print "Score: "; number score; "\_ "; "Moves: "; number counter;
	! STATUSTYPE case 3 is the "Infocom"-style status
	case 4 : StatusType4 ! routine for configurable statusline
!if (FORMAT & DESCFORM_F) and (printstatuslib.terp_type ~= GLK_TERP)
!	print "\_";
text to 0
if STATUSTYPE
{
	b = StringLength(_temp_string)
	statuswindow.counter_length = ++b
}
else
{
	b = 1
	while display.screenwidth < (b*a)
	{
		b++
	}
	return b
}
if (b + a + 4)<display.screenwidth ! let's force a 4 character gap between
    {                              ! the two fields
    return 1
    }
elseif (b + a - 4 ) < display.screenwidth and STATUSTYPE = 3
    {
    text to _temp_string
    print "S: "; number score; "\_ "; "M: "; number counter;
 !   if (FORMAT & DESCFORM_F) and (printstatuslib.terp_type ~= GLK_TERP)
	if (printstatuslib.terp_type ~= GLK_TERP)
		print "\_";
    text to 0
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
        printstatuslib.terp_type ~= SIMPLE_TERP
        {
        if statuswindow.find_height = 2
				row = display.statusline_height - 1
        else
				row = display.statusline_height
         locate 1, row
        }
		else
			row = 1
    if not location
        print "\_";
    elseif not light_source
        print "\_In the dark";
    else
        {
   !         if FORMAT & DESCFORM_F or (printstatuslib.terp_type = GLK_TERP)
					print "\_";
            print capital location.name;
        }

    if statuswindow.find_height = 1 and STATUSTYPE
        {
	!		if display.linelength = 80 and printstatuslib.terp_type ~= GLK_TERP
	!			a = 79
	!		else
				a = display.linelength
            print to (a - \
            ( statuswindow.counter_length + \
            (printstatuslib.terp_type = SIMPLE_TERP)* 2 - \
				 (printstatuslib.terp_type = GLK_TERP)));
            StringPrint(_temp_string)
        }
    elseif STATUSTYPE and statuswindow.find_height = 2
        {
            if printstatuslib.terp_type ~= SIMPLE_TERP
                locate 1, ++row
            else
                ""
!			if (FORMAT & DESCFORM_F) or (printstatuslib.terp_type = GLK_TERP)
	print "\_";
            StringPrint(_temp_string)
        }
}
#endif  !ifset NO_FANCY_STUFF
#ifset NO_FANCY_STUFF
replace PrintStatusline
{
	local simple
	simple = (not isGlk and system(61))
   if display.linelength < 60
      display.statusline_height = 2
   else
      display.statusline_height = 1

   Font(BOLD_OFF | ITALIC_OFF | UNDERLINE_OFF | PROP_OFF)
      color SL_TEXTCOLOR, SL_BGCOLOR
   window display.statusline_height
   {
		if not system(61)
			cls
		if not simple
			locate 1, 1
      if not location
         print "\_";
      elseif not light_source
         print "\_ In the dark";
      else
      {
       !  if FORMAT & DESCFORM_F:
			print "\_";
			print capital location.name;
      }

! (The part we're changing)
! print to 65; ! is 65 characters good for every window size? No!

! Instead, let's begin by writing the entire 'SCORE / MOVES' to array
! _temp_string (_temp_string is an array declared by the library)

      text to _temp_string
		select STATUSTYPE
			case 1 : print number score; " / "; number counter;
			case 2 : print HoursMinutes(counter);
			case 3 : print "Score: "; number score; "\_ "; "Moves: "; number counter;
	! STATUSTYPE case 3 is the "Infocom"-style status
			case 4 : StatusType4 ! routine for configurable statusline
      text to 0
! Ok, we've closed off the string array

! Now, if the screen is wide enough, let's move to the end of the screen
! MINUS the length of the _temp_string array
      if display.statusline_height = 1
         print to (display.screenwidth - (StringLength(_temp_string) + 1 + (simple*2)));
      else
      {
         locate 1, 2
         print "\_ ";
      }

 ! Now let's print it!
      if STATUSTYPE
			StringPrint(_temp_string)
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

routine SaveWordSetting(w)
{
	local i
	for (i=20 ; i<30 ; i++ )
		{
		if word[i] = "" ! found an open slot
			word[i] = w  ! add word to word array
		if word[i] = w ! word is already in word array
			return i
		}
	return false ! didn't find anything
}

routine CheckWordSetting(w)
{
	local i
	for (i=20 ; i<30 ; i++ )
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
}

object init_instructions
{}

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
		if not CheckWordSetting("undo")
			{
			if not CheckWordSetting("restore")
				{
				CalculateHolding(player)
#ifset USE_PLURAL_OBJECTS
				InitPluralObjects
#endif
				}
#ifclear NO_FANCY_STUFF
			local a
			a = CheckWordSetting("roodylib")
			select word[(a-1)]
				case "brief" : verbosity = 0
				case "superbrief" : verbosity = 1
				case "verbose" : verbosity = 2
#endif
			}
		}
}

!\ Roody's note: the last_turn_true global uses the player's before property
and an event to keep track of when turns are 'true' (are successful and take
up a turn and when they aren't) \!

global last_turn_true = true

routine Main_Calls
{
	local i
	for i in main_instructions
		{
		run i.execute
		}
}

object main_instructions
{}

!\ Roody's note: The last_turn_true global used below here is an attempt to let the author know if the previous turn was successful (returned true) or not, as
that can be useful knowledge in some scenarios.

Check the value of last_turn_true during the PreParse stage
\!

object success_detector
{
	in main_instructions
	execute
		{
		last_turn_true = true
		}
}

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
#ifclear CHEAP
	local cheap
#endif

	color TEXTCOLOR, BGCOLOR, INPUTCOLOR
	Font(DEFAULT_FONT)
	simple_port = (not IsGlk and system(61)) ! non-glk simple port
	if not system(61)
		{
		window 0
		}
#ifset CHEAP
	if not cheap
		{
		if CheckWordSetting("cheap")
			cheap = true
		}
	if not simple_port and not cheap
		cls
#else
	if not simple_port ! not non-glk simple port
		cls
#endif
	if not system(61) and not cheap ! CheckWordSetting("cheap")
		CenterTitle("",0,1) ! Draw an empty window
	else
		CenterTitle(CheapTitle,0,1) ! See CheapTitle routine
#ifset CHEAP
	if not system(61) and not cheap
		locate 1, LinesFromTop
	elseif cheap or simple_port ! non-glk simple port
		""
#else
	if not system(61)
		locate 1,LinesFromTop
	elseif simple_port
		""
#endif ! CHEAP
	if display.needs_repaint
	{
		display.needs_repaint = 0
	}
}

routine CheapTitle
{
	if &display.title_caption
		{
		return display.title_caption
		}
	return "Hugo Interactive Fiction"
}

routine LinesFromTop
{
	if not display.hasgraphics
	{
		return display.windowlines
	}
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

!----------------------------------------------------------------------------
!* PREPARSE CODE
!----------------------------------------------------------------------------

#ifclear NO_FANCY_STUFF
replace PreParse
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
{}

object parse_redraw
{
	in preparse_instructions
	type settings
	execute
		{
		RedrawScreen
		return false
		}
}
#endif
!\ Roody's note: RedrawScreen is a generic routine for redrawing the screen
after a screen-size change. Ideally, it should be called in PreParse.
\!

global oldwidth

routine RedrawScreen
{
	! if the screen size has changed, we'll clear the screen,
	! print the player's command, and redraw the status line before
	! proceeding to interpret the command

	if display.needs_repaint
		{
#ifset CHEAP
		if cheap
			{
#if defined GAME_TITLE
			CenterTitle(GAME_TITLE,0,1)
#endif
#if undefined GAME_TITLE
			CenterTitle(CheapTitle,0,1)
#endif
			display.needs_repaint = false
			return
			}
#endif ! CHEAP
		"[Detected screen size change; redrawing screen...]"
		local get_key
		get_key = system(11) ! READ_KEY
		if not (system_status or system(61)) ! 61 = MINIMAL_PORT
			{
			local a
			while a <200
				{
				if system(11)
					break
				system(32) ! wait 1/100th second
				a++
				}
			}
		cls
		PrintStatusLine
		locate 1, LinesFromTop
		print prompt;
		local i = 1, showfullstops
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
				{
				print ".";
				}
			}
		print ""
		display.needs_repaint = false
		}
	if system(61) ! simple port
	{
		if not oldwidth
			oldwidth = display.screenwidth

		if IsGlk and oldwidth ~= display.screenwidth
			{
			PrintStatusLine
			}

		oldwidth = display.screenwidth
	}
}

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

attribute actually_transparent

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
			if PrepWord("in") or PrepWord("into") or PrepWord("inside")
			{
				if self is openable and self is not open
				{
					CThe(self)
					" is closed."
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
			if PrepWord("in") or PrepWord("into") or PrepWord("inside")
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

routine PrepWord(str)
{
	local i
	for (i=1; i<=words; i++)
	{
		if not ObjWord(word[i], object)
		{
			if word[i] = str
				return true
			if word[i] = ""
				break
		}
	}
}
#endif ! USE_SUPERCONTAINER

!----------------------------------------------------------------------------
!* REPLACED OBJLIB.H CODE
!----------------------------------------------------------------------------

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
			OMessage(door, 1)
			return true
		}

		if self is not open and actor = player
		{
#ifclear NO_VERBS
			if verbroutine = &DoGo
			{
				if self is not locked and self is openable:
				{
					OMessage(door, 2)       ! "(opening it first)"
					Perform(&DoOpen, self)
					Main    ! counts as a turn
					if self is open
						Perform(&DoGo, self)
					return true
				}
				elseif self is lockable
				{
#ifclear NO_AUTOMATIC_DOOR_UNLOCK
					RlibOMessage(door, 3)      ! "(unlocking <the blank> first))"
					Perform(&DoUnlock, self)
					if self is not locked
					{
						Main
						Perform(&DoGo, self)
						return true
					}
					else
#else
					OMessage(door, 3)      ! "It is locked."
#endif
						return true
				}
				else
				{
#endif
					OMessage(door, 4)       ! "It is closed."
					return true
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
!		{
!		if Inlist(self,between,location)
!			{
!			return 1
!			}
!		else
!			return -1
!		}
}

!\ Roody's note: replaced the female_character character class so it is of
type "character" (like a regular character), as you can always check for the
female attribute if you are specifically looking for a female character\!

replace female_character
{
	inherits character
	pronouns "she", "her", "her", "herself"
	is female
}


!\ Roody's note: replaced the player_character object so it
can be of type "player_character", as I feel authors will often
want to distinguish playable characters from NPC's \!

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
	{
		print "Looking good."
	}
	before
	{
		actor
		{
			if verbroutine = &PreParse
				return false
			if AnyVerb(location)
				last_turn_true = false
			return false
		}
#ifset USE_SUPERCONTAINER
		actor
		{
			if parent(object).type = SuperContainer
			{
				if Inlist(parent(object), contents_in, object) and
					parent(object) is not open and parent(object) is not transparent
				{
					print capital player.pronoun #1;
					MatchPlural(player, "doesn't", "don't")
					" see that."
					return true
				}
			}
			return false
		}
#endif  ! USE_SUPERCONTAINER
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
		if word[2] = "out" and object = self
			{
			object = out_obj
			return false
			}
		if (object ~= u_obj, out_obj) and object.type = direction
			{
			! "To walk, you'll have to get out..."
			OMessage(vehicle, 1, self)
			return true
			}
		else
			return object
		}
	}
#endif
	is enterable, static
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
					obj = self.plural_of #i
					AssignPronoun(obj)
					n++
				}
			}

			if n > 1:  AssignPronoun(self)

			if n = 0
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
			k = WordisNumber(word[w-1])
			if k >= 1                       ! e.g., "one (or more) things"
			{
				DeleteWord(w-1)
				w--
				pobj_number = k
			}

!			if WordisNumber(word[w-1])>=1   ! "two of the three"
!			{
!				pobj_number = WordisNumber(word[w-1])
!				DeleteWord(w-1)
!				w--
!			}
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
	parser_data[PARSER_STATUS] |= PRONOUNS_SET  ! since we rebuilt object(s)

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
	case else
		{
		local a
		a = StringToNumber(parse$)
		if not a
			return -1
		else
			return a
		}
}
#endif ! #ifset USE_PLURAL_OBJECTS

#ifset USE_ATTACHABLES
! ObjectisAttached returns true if something in <oldloc> is keeping
! <obj> from moving to <newloc>.

! Roody's note- got rid of a couple jumps
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

!----------------------------------------------------------------------------
!* REPLACED RESOURCE.H CODE
!----------------------------------------------------------------------------
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

	if not display.hasgraphics or system(61):  return false

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
			music resfile, song, vol
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
			if b not in object
				object.holding = object.holding - b.size
		}

		thisobj = nextobj
	}

	run object.after
	return true
}

! Roody's note: Mostly not changed. Just commented out the part where word[1]
! is cleared and made the for loop break earlier.
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

	! To prevent endless loops if the player_character class
	! automatically resets the object to in_obj if word[1] = "in"
!	word[1] = ""    ! causing problems

	if &object.door_to
		return Perform(&DoGo, object)   ! routine
	elseif object.door_to
		return Perform(&DoGo, object)   ! object

	if object is not enterable or Contains(player, object)
		VMessage(&DoEnter, 2)    ! "You can't enter that."
	elseif player in object
		VMessage(&DoEnter, 3)    ! already in it
	elseif player not in location
		VMessage(&DoGo, 3)       ! "You'll have to get up..."
	elseif object is openable, not open
		VMessage(&DoLookIn, 1)   ! "X is closed."
	else
	{
		move player to object
		if not object.after
			VMessage(&DoEnter, 4)    ! "You get in..."
		object is not quiet
		DescribePlace(location)
		return true
	}
	return
}

! Roody's note: Fixed a bug Juhana Leinonen found where "X is closed."
! messages weren't properly printing the parent's name.
replace DoExit
{
	local p

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

#ifclear NO_OBJLIB
	if object and player not in object
#else
	if object and player not in object
#endif
		VMessage(&DoExit, 1)             ! "You aren't in that."
	elseif p is openable, not open
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
			VMessage(&DoExit, 2)     ! "You get out.."
	}
	return true
}

! Roody's note: Makes "you'll have to get up" message more container/platform
! specific also has some code to work with new vehicle replacement. Also got
! rid of jump. Also changed the answer for trying to go to a nearby,
! non-enterable object from "You can't enter that" to "<The object> is right
! here."
replace DoGo
{
	local moveto, JumpToEnd
#ifset NO_OBJLIB
	local wordnum, m
#endif

	if player not in location               ! sitting on or in an obj.
	{
#ifclear NO_OBJLIB
		if (object ~= parent(player) and ! make sure player isn't already in object
		not (object in direction and parent(player) = location.(object.dir_to))) \
		and ((object ~= u_obj and parent(player) is platform) or
			(object ~= out_obj and parent(player) is container))
		{
			VMessage(&DoGo, 3)      ! "You'll have to get up..."
			return false
		}
#endif
		local b
		b = parent(player).before	! i.e., a vehicle, etc.
		if b > 1  or ! is b a direction?
		((word[2] = "out","off") and object = parent(player)) ! "go out of <object>"
			return Perform(&DoExit,parent(player))
		elseif b
			return false ! so the error message doesn't take up a turn
	}
	elseif obstacle
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

		if not &object.door_to
		{
			if not object.door_to and object is enterable
				return Perform(&DoEnter, object)
		}

		moveto = object.door_to
		if not moveto
		{
			if object is container and (word[2] ~= "to","toward","towards")
				VMessage(&DoEnter, 2)  ! "You can't enter..."
			else
				RLibMessage(&DoGo, 1) ! "The <object> is right here."
			return
		}
		if moveto = 1
			return true
	}
}  !  if not JumpToEnd bracket

#ifclear NO_OBJLIB
	if moveto.type = direction and moveto in direction
	{
		if player not in location and object = out_obj
		{
			if parent(player) is platform
			{
				VMessage(&DoGo, 3)
				return false
			}
			else
				return Perform(&DoExit)
		}
		elseif player not in location and object = u_obj
		{
			if parent(player) is container
			{
				VMessage(&DoGo, 3)
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
		}
		elseif moveto.door_to
			moveto = moveto.door_to
		elseif moveto is enterable ! or
	!	(((moveto is platform) or (moveto is container)) and
	!	player in location)
			return Perform(&DoEnter, moveto)
	}
#else
	if m
	{
		if player not in location and m = out_to
		{
			if parent(player) is platform
			{
				VMessage(&DoGo, 3)
				return false
			}
			else
				return Perform(&DoExit)
		}
		elseif player not in location and m = u_to
		{
			if parent(player) is container
			{
				VMessage(&DoGo, 3)
				return false
			}
			else
				return Perform(&DoExit, parent(player))
		}

		moveto = location.m
		if moveto.door_to
			moveto = moveto.door_to
	}
#endif

	if moveto = false
	{
		if not location.cant_go
			VMessage(&DoGo, 2)      ! "You can't go that way."
		return false
	}

	elseif moveto = true                    ! already printed message
		return true                     ! (moveto is never 1)

	elseif player not in location           ! sitting on or in an obj.
	{
		if parent(player) = moveto  ! does the direction lead to parent(player)?
			VMessage(&DoEnter, 3)    ! already in it message
		else
			VMessage(&DoGo, 3)              ! "You'll have to get up..."
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
		MovePlayer(moveto)

		return true
	}
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
	if xobject and xobject.type = direction
		xobject = 0

   if xobject ~= 0
   {
      a = InList(object,key_object,xobject)
      if not a
      {
         VMessage(&DoUnlock, 1)  ! "Doesn't seem to work..."
         return true
      }
   }
   elseif object.key_object
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
      if not xobject
      {
			if cant_reach
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
      if not object.after
      {
         if not xobject.after
            RLibMessage(&DoUnlock, 1, list_key)   ! "Unlocked."
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
   elseif object.key_object
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
      if not xobject
      {
			if cant_reach
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
      object is locked
      if not object.after
      {
         if not xobject.after
            RLibMessage(&DoLock, 1, list_key)     ! "Locked."
      }
   }
   return true
}

! Replaced LOOK routines\

!\ Roody's note: I added a flag called NO_LOOK_TURNS. #set it if you want your
game to not count LOOK moves (LOOK, EXAMINE <object>, LOOK THROUGH <object)
as turns.

Added children-listing to all transparent objects. \!

replace DoLook
{
	local i

	if not light_source
		VMessage(&DoLook, 1)     ! "It's too dark to see anything."
	else
	{
		if not object.long_desc
			! "Looks just like you'd expect..."
			VMessage(&DoLook, 2)

!		if object is living, transparent, not quiet
		if ( object is transparent or
		    !(object is living, transparent) or
			object is platform or
			(object is container and (object is open or object is not openable))) and
			object is not quiet ! and object is not already_listed
		{
			for i in object
			{
				if i is not hidden
					break
			}
			if i and object ~= player
			{
				local tempformat
				tempformat = FORMAT
				FORMAT = FORMAT | NOINDENT_F
				list_nest = 0
				print ""
				WhatsIn(object)
				FORMAT = tempformat
			}
		}

		run object.after
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

Also, non-container, transparent items are now allowed. \!

replace DoLookIn
{
	local tempformat

	if not CheckReach(object):  return false

	if not light_source
		VMessage(&DoLook, 1)     ! "It's too dark to see anything."
	elseif object is not container and object is not transparent
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
		if object is container and (object is openable and
			object is not open and object is not transparent):
			VMessage(&DoLookIn, 1)           ! "It's closed."
		else
		{
			if not object.after
			{
				object is not quiet

				tempformat = FORMAT
				FORMAT = FORMAT | NOINDENT_F
				list_nest = 0

				if WhatsIn(object) = 0
					VMessage(&DoLookIn, 2)   ! "It's empty."

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
		if not CheckReach(object)
			return false

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

!\ Roody's note: roodylib, by default, supports PUSH and PULL. It redirects them
to DoMove if verbstub hasn't been included.  \!

replace DoMove
{
	if not CheckReach(object):  return false

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
replace DoOpen
{
	local tempformat

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
		object is moved
		if not object.after
		{
			VMessage(&DoOpen, 4)     ! "Opened."

			FindLight(location)     ! in case the light source
						! has been revealed
			if children(object) and object is not quiet and
			not (children(object) = 1 and child(object) = player)
			{
				print ""
				tempformat = FORMAT
				FORMAT = FORMAT | NOINDENT_F
				list_nest = 0
				WhatsIn(object)
				FORMAT = tempformat
			}
		}
	}
	return true
}

!----------------------------------------------------------------------------
! Roody's note: Changed DoQuit to give one final ending message

#if undefined PRESS_ANY_KEY
constant PRESS_ANY_KEY "[ press any key to exit ]"
#endif

replace DoQuit
{
	PrintScore
	VMessage(&DoQuit)                        ! "Are you sure?"
	GetInput
	if YesorNo = true
	{
		""
		RLibMessage(&DoQuit,1) ! "Thanks for playing!"
		""
		display.title_caption = PRESS_ANY_KEY
		if not system(61) ! if not simple port simple port
		{
			print PRESS_ANY_KEY;
			HiddenPause
		}
		quit
	}
}

replace DoRestart
{
	if MAX_SCORE
		PrintScore
	VMessage(&DoRestart, 1)                  ! "Are you sure?"
	GetInput
	if YesorNo = true
	{
#ifclear NO_FANCY_STUFF
		ClearWordArray
		SaveWordSetting("restart")
		local i
		for i in init_instructions
			{
			if i.save_info
				SaveWordSetting(i.name)
			}
#else
		SaveWordSetting("restart")
#endif
		if not restart
			VMessage(&DoRestart, 2)  ! "Unable to restart."
		else
			return true
	}
}

!----------------------------------------------------------------------------
replace DoRestore
{
	SaveWordSetting("restore")
#ifclear NO_FANCY_STUFF
	local i
	for i in init_instructions
		{
		if i.save_info
			SaveWordSetting(i.name)
		}
#endif
	if restore
	{
#ifclear NO_FANCY_STUFF
		for i in init_instructions
			{
			if CheckWordSetting(i.name)
				run i.execute
			}
#endif
		VMessage(&DoRestore, 1)         ! "Restored."
		PrintStatusline
		DescribePlace(location, true)
#ifset NEW_FUSE
		fake_runevents
#endif
		ClearWordArray
		last_turn_true = true
		return true
	}
	else:  VMessage(&DoRestore, 2)           ! "Unable to restore."
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
		object is not worn
		if not object.after
			VMessage(&DoTakeOff, 3)  ! "You take it off."
	}
	return true
}

!\ Roody's note: The after_undo array is a way to execute code after a
successful undo. It must be filled with routine addresses, though.
  "after_undo[0]= &CheckDaemon"
\!
#ifset USE_AFTER_UNDO
array after_undo[5]
#endif

replace DoUndo
{
	if not UNDO_OFF
	{
		local c
		SaveWordSetting("undo")
#ifclear NO_FANCY_STUFF
		local i
		for i in init_instructions
			{
			if i.save_info
				SaveWordSetting(i.name)
			}
		if display.statusline_height > 2
			{
			c = SaveWordSetting("statusheight")
			word[(c+1)] = display.statusline_height
			}
#endif
		if undo
		{
#ifclear NO_FANCY_STUFF
			i = 0
			for i in init_instructions
				{
				if CheckWordSetting(i.name)
					run i.execute
				}
			c = CheckWordSetting("statusheight")
			if c
				display.statusline_height = word[(c+1)]
#endif
			PrintStatusline
			DescribePlace(location)
#ifset NEW_FUSE
			fake_runevents
#endif
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
#ifclear NO_FANCY_STUFF
			ClearWordArray
#endif
			verbroutine = &DoUndo
			return true
		}
		else
			VMessage(&DoUndo)
	}
	else
		VMessage(&DoUndo)
	ClearWordArray
}

!----------------------------------------------------------------------------
!* REPLACED VERBSTUB.H ROUTINES
!----------------------------------------------------------------------------
! Roody's note: Fixes some pronoun stuff. Suggested by Mike Snyder.
#ifset _VERBSTUB_H
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
#else
! Roody's note: SEARCH is a pretty common verb. Roodylib adds it regardless.
routine DoSearch
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
#endif

! Roody's note: Applies "DoListen" logic to DoSmell. Makes it a standard verb.
#ifset _VERBSTUB_H
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
#else
routine DoSmell
{
	if not object
		{
			if not location.after
				{
				RLibMessage(&DoSmell) ! "You smell nothing unusual."
				return true
				}
			verbroutine = ""
			return true
		}
	elseif not object.after
		RLibMessage(&DoSmell) ! "You smell nothing unusual."
	! we have to clear verbroutine or else location.after.DoListen will run again
	verbroutine = ""
	return true
}
#endif

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
#endif
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

	DescribePlaceArray[0] = &ParentofPlayer,
	 &ObjsWithDescs,&ObjsWithoutDescs,
	&CharsWithDescs, &CharsWithoutDescs,
	&AttachablesScenery

\!

constant DESCFORM_I 4096

#ifclear NEW_DESCRIBEPLACE
replace Describeplace(place, long)
{
   local obj, count, notlisted, tempformat

	if not place
		place = location

   parser_data[PARSER_STATUS] &= ~PRONOUNS_SET

   ! Since, for example, a room description following entering via
   ! DoGo does not trigger before/after properties tied to looking
   ! around:
   !
#ifclear NO_VERBS
   if verbroutine = &MovePlayer
   {
      if place is not visited and verbosity ~= 1
         return Perform(&DoLookAround)
      elseif long = true or verbosity = 2
         return Perform(&DoLookAround)
   }
#endif

   if not light_source
   {
      Message(&DescribePlace, 1)     ! "It's too dark to see..."
      return
   }

   place is known

   ! Print the name of the location as a heading
	RLibMessage(&DescribePlace,1,place)

   override_indent = false

	if place is not visited and verbosity ~= 1
	{
		if &place.initial_desc or &place.long_desc
			Indent
		if not place.initial_desc
			run place.long_desc
	}
	elseif long = true or verbosity = 2
	{
		if &place.long_desc
			Indent
		run place.long_desc
	}
	elseif place is not visited and verbosity = 1
	{
		if &place.initial_desc
			Indent
		run place.initial_desc
	}

   if &place.list_contents and FORMAT & DESCFORM_F
      print ""        ! for double-space-after-heading formatting

   ! A location may contain an overriding listing routine, as may any
   ! parent, in the list_contents property
   !
   if not place.list_contents
   {
      list_nest = 0

      ! For double-space-after-heading formatting:
      if FORMAT & DESCFORM_F
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
         list_nest = 1
         WhatsIn(Parent(player))
      }

      ! List all characters, if any
      count = 0
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
         tempformat = FORMAT
         FORMAT = FORMAT | FIRSTCAPITAL_F | ISORAREHERE_F
         FORMAT = FORMAT | USECHARNAMES_F
         if FORMAT & LIST_F
         {
            FORMAT = FORMAT & ~LIST_F       ! clear it
            FORMAT = FORMAT | TEMPLIST_F
         }
         Indent
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
         if FORMAT & LIST_F
         {
            FORMAT = FORMAT & ~LIST_F       ! clear it
            FORMAT = FORMAT | TEMPLIST_F
         }
         Indent
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
            obj is known
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
#endif  ! ifclear NO_OBJLIB

      print newline
      need_newline = false

   }
}
#else
! A DescribePlaceArray-setting object just so most authors don't have to worry
! about setting DescribePlace order themselves
object describe_order "DescribePlace order"
{
	in init_instructions
	type settings
	execute
	{
		if not CheckWordSetting("undo")
		{
			if not CheckWordSetting("restore")
			{
				if not DescribePlaceArray[0]
				{
#ifclear NEW_DESC
					DescribePlaceArray[0] = &ParentofPlayer, &CharsWithDescs,
					&CharsWithoutDescs, &ObjsWithDescs,&ObjsWithoutDescs,
					&AttachablesScenery
#else
					DescribePlaceArray[0] = &ParentofPlayer, &CharsWithDescs,
					&CharsWithNewDescs,
					&CharsWithoutDescs, &ObjsWithDescs,
					&objsWithNewDescs, &ObjsWithoutDescs,
					&AttachablesScenery
#endif
				}
			}
		}
	}
}

! This constant needs to be declared with something higher if you are
! adding additional elements into the mix
#if undefined DESCRIBEPLACE_ELEMENTS
#ifset NEW_DESC
	constant DESCRIBEPLACE_ELEMENTS 8
#else
	constant DESCRIBEPLACE_ELEMENTS 6
#endif
#endif

attribute already_printed
array DescribePlaceArray[DESCRIBEPLACE_ELEMENTS]

replace Describeplace(place, long)
{
	local obj,  didprint
	local need_carriage

	if not place
		place = location

	parser_data[PARSER_STATUS] &= ~PRONOUNS_SET

	! Since, for example, a room description following entering via
	! DoGo does not trigger before/after properties tied to looking
	! around:
	!
#ifclear NO_VERBS
	if verbroutine = &MovePlayer ! was "if verbroutine ~= &DoLookAround"
	{
		if place is not visited and verbosity ~= 1
			return Perform(&DoLookAround)
		elseif long = true or verbosity = 2
			return Perform(&DoLookAround)
	}
#endif

	if not light_source
	{
		Message(&DescribePlace, 1)     ! "It's too dark to see..."
		return
	}

	place is known

   ! Print the name of the location as a heading
	RLibMessage(&DescribePlace,1,place)

	override_indent = false

	if place is not visited and verbosity ~= 1
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
		run place.long_desc
	}
	elseif place is not visited and verbosity = 1
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

	if not place.list_contents
	{
		list_nest = 0
		local a
		while a < DescribePlaceArray[] and DescribePlaceArray[a]
		{
			need_carriage = call DescribePlaceArray[a](place)

			if (FORMAT & DESCFORM_F)
			{
				if need_carriage and didprint
					""
			}

			if need_carriage
			{
				need_carriage = false
				didprint = call DescribePlaceArray[a](place,true)
			}
			a++
		}
		print newline
		need_newline = false
	}
}

! routine for listing the player's siblings when he or she is in or on an
! object in the room
routine ParentofPlayer(place, for_reals)
{
	local obj
	if not for_reals
	{
		if player not in place and place = location
		{
			for obj in (Parent(player))
			{
				if obj is not hidden and obj ~= player
					return true
			}
		}
		return false
	}
	else
	{
		list_nest = 1
		if WhatsIn(Parent(player))
			return true
	}
}

! routine for listing characters with short_desc descriptions
routine CharsWithDescs(place, for_reals)
{
	local obj
	if not for_reals
	{
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
	else
	{
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
		local ret
		for obj in place
		{
			if obj is not already_listed
			{
				print newline
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

#ifset NEW_DESC
! routine for listing characters with short_desc descriptions
routine CharsWithNewDescs(place, for_reals)
{
	local obj
	if not for_reals
	{
		for obj in place
		{
			if (obj is not hidden and obj is living and
				 obj ~= player and (obj.new_desc and verbosity ~= 1) and
				obj is not already_printed )
			{
				return true
			}
		}
		return false
	}
	else
	{
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
		local ret
		for obj in place
		{
			if obj is not already_listed
			{
!				print newline
				if verbosity ~= 1
					NewDescribe(obj)
				if obj is already_listed  ! did we print something?
				{
					ret = true
					obj is already_printed
				}
			}
		}
		print newline
		return ret
	}
}
#endif

! routine for listing characters without short_desc descriptions
routine CharsWithoutDescs(place,for_reals)
{
	local tempformat, count, obj
	if not for_reals
	{
		for obj in place
		{
			if (obj is not hidden and obj is living and
			obj ~= player and
			(not &obj.short_desc or
			(&obj.short_desc  and verbosity = 1)) and
			obj is not already_printed)
			{
				return true
			}
		}
		return false
	}
	else
	{
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
			FORMAT = FORMAT | USECHARNAMES_F
			if (FORMAT & LIST_F)
			{
				FORMAT = FORMAT & ~LIST_F       ! clear it
				FORMAT = FORMAT | TEMPLIST_F
			}
			Indent
			list_nest = 0
			ListObjects(place)
			FORMAT = tempformat
			return true
		}
		return false
	}
}

! routine for listing objects with short_desc descriptions
routine ObjsWithDescs(place, for_reals)
{
	local obj, ret
	if not for_reals
	{
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
				{
					obj is already_listed
				}
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
	else
	{
		for obj in place
		{
			if player not in obj
			{
				if obj is not already_listed and
					obj is not hidden and obj is not already_printed
				{
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

#ifset NEW_DESC
routine ObjsWithNewDescs(place, for_reals)
{
	local obj, ret
	if not for_reals
	{
		for obj in place
		{
#ifset USE_ATTACHABLES
		! Exclude all attachables for now (and characters)
			if obj is not living and not obj.type = attachable and
				player not in obj and obj is not hidden and
				(verbosity ~= 1 and obj.new_desc) and
				obj is not already_printed
#else
			if obj is not living and player not in obj and
			obj is not hidden and
			(verbosity ~= 1 and obj.new_desc) and
				obj is not already_printed
#endif
			{
#ifset USE_PLURAL_OBJECTS
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
	else
	{
		for obj in place
		{
			if player not in obj
			{
				if obj is not already_listed and
					obj is not hidden and obj is not already_printed
				{
					if verbosity ~= 1
						NewDescribe(obj)
					if obj is already_listed ! did we print something?
					{
						ret = true
						obj is already_printed
					}
				}
			}
		}
		print newline
		return ret
	}
}
#endif

! routine for listing objects without short_desc descriptions
routine ObjsWithoutDescs(place, for_reals)
{
	local obj, tempformat, count
	if not for_reals
	{
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
	else
	{
		for obj in place
		{
#ifset USE_ATTACHABLES
			! Exclude all attachables for now (and characters)

			if obj is living or (obj.type = attachable) or
				player in obj or (&obj.short_desc and verbosity ~= 1) or
				obj is already_printed
#else
			if obj is living or player in obj or (&obj.short_desc and verbosity ~= 1) or obj is already_printed
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
			if FORMAT & LIST_F
			{
				FORMAT = FORMAT & ~LIST_F       ! clear it
				FORMAT = FORMAT | TEMPLIST_F
			}
			Indent
			list_nest = 0
			ListObjects(place)
			FORMAT = tempformat
			return true
		}
		return false
	}
}

! routine for listing attachables and contents of scenery objects
routine AttachablesScenery(place, for_reals)
{
	local obj, ret
	if not for_reals
	{
#ifset USE_ATTACHABLES
		for obj in place
		{
			! Print attachables last
			if obj.type = attachable and obj is not hidden
			{
				obj is not already_listed
				ret = true
			}
		}
#endif
		for obj in place
		{
			if obj.type = scenery
			{
            if player not in obj and
           !    (obj is open or obj is not openable)
			!	((obj is container and (obj is open or obj is transparent))  or
			!	obj is platform) and obj is not quiet
				(obj is open or obj is not openable or obj is platform or
				obj is transparent) and obj is not quiet
				{
					local a
					obj is not already_listed
					for a in obj
					{
						if a is not hidden
						{
								ret = true
								break
						}
					}
				}
			}
		}
		return ret
	}
	else
	{
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
				obj = already_printed
				ret = true
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
				obj is known
				if player not in obj and
					((obj is container and (obj is open or obj is transparent))  or
					obj is platform) and obj is not quiet
				{
					list_nest = 1
					if WhatsIn(obj)
						ret = true
				}
			}

			! For scenery-derived objects that may change the type
			elseif obj is static, hidden
				obj is known
		}
		print newline
		override_indent = false
		return ret
#endif  ! ifclear NO_OBJLIB
	}
}
#endif

#ifset NEW_DESC
routine NewDescribe(obj)
{
	local ListContents
	obj is known

	if list_nest = 1
		print newline

	AssignPronoun(obj)

	if (not ListContents)
		{
                if verbosity = 1
	                return

		local a
		a = obj.new_desc

		if a
		{
			Indent
			print a ;
		}
		elseif not a
			return
	}
!  "ListContents" section
	obj is already_listed
	obj is already_printed
	AssignPronoun(obj)

	! If INDENT_SIZE is 0, formatting may be thrown off when listing
	! the contents of an object:
	if INDENT_SIZE = 0:  need_newline = true

}
#endif
!----------------------------------------------------------------------------
!* NEW VERB ROUTINES
!----------------------------------------------------------------------------

!\ Roody's note - If you want your game to not allow verbosity changes, #set
NO_MODE_CHANGE before Roodylib grammar is included. Then the grammar will call
the following routine which fakes a "I don't understand that" message.
\!
#ifset NO_MODE_CHANGE
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
#endif

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
routine DoPushDir
{
	local a,m

	! the for loop finds the last word before the direction
	! which should be the object's noun
	for (a=2; ; a=a+1)
		{
		if ObjWord(word[a],object) = noun
			break
		}
	! Let's add one to find the direction words
	a = a + 1

	! Let's identify what we've got and point to the right direction object
	select word[a]
		case "north", "n":      m = n_obj
		case "south", "s":      m = s_obj
		case "east", "e":       m = e_obj
		case "west", "w":       m = w_obj
		case "northwest", "nw": m = nw_obj
		case "northeast", "ne": m = ne_obj
		case "southeast", "se": m = se_obj
		case "southwest", "sw": m = sw_obj
		case "up", "u":         m = u_obj
		case "down", "d":       m = d_obj
		case "in", "inside":    m = in_obj
		case "out", "outside":  m = out_obj

	! Let's run it
	return Perform(&DoPushDirTo, object, m)
}

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

routine DoPushDirTo
{
	local moveto
	moveto = xobject

 ! if the item is not rollable, we'll just act like the player just tried to
 ! move it. since mobile = worn, we make sure the item is not clothing.
	if not (object is mobile and  object is not clothing)
#if defined DoPush
		return Perform(&DoPush, object)
#else
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

		! Let's print a success message before the move "You start pushing..."
		if not object.pushstart
			RLibMessage(&DoPushDirTo,3) ! "You push the object over to the..."
		}

	! If DoGo works, then move the object to the location and print
	! a followup success message.
	if Perform(&DoGo, xobject)
		{
		print ""
		move object to location
		RLibMessage(&DoPushDirto,4)
		return true
		}
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
!----------------------------------------------------------------------------
!* ARRAY SORTING
!----------------------------------------------------------------------------

#ifclear NO_SORTING
!\ Roody's note - SortArray and the comparison routines below were
written by Christopher Tate \!

!-----------------------------------------------------------------------
! SortArray() arguments:
!
! data = array to sort
! num = number of elements in the array
! comp_routine = the address of the comparison routine to use
! beginn = beginning element to start with (for arrays not starting with 0 )

routine SortArray(data, num, comp_routine,beginn)
{
	local i
	local did_swap

	for (i = beginn; i < (beginn + num); i = i + 1)
	{
		local j
		did_swap = false
		for (j = (num + beginn - 1); j > i; j = j - 1)
		{
			local swap
			swap = call comp_routine(array data[j-1], array data[j])
			if swap
			{
				local tmp
				tmp = array data[j]
				array data[j] = array data[j-1]
				array data[j-1] = tmp
				did_swap = true
			}
		}
		! if we didn't swap any elements, then the array is sorted
		if not did_swap : return
	}
}

! SortProp is a version that works off a property array
!  obj = the object, prp = the property array to be sorted
!  comp_routine = like above
routine SortProp(obj, prp, comp_routine)
{
	local i
	local did_swap

	for (i = 1; i <= obj.#prp; i = i + 1)
	{
		local j
		did_swap = false
		for (j = obj.#prp ; j > i; j = j - 1)
		{
			local swap
			swap = call comp_routine(obj.prp #(j-1), obj.prp #j)
			if swap
			{
				local tmp
				tmp = obj.prp #j
				obj.prp #j = obj.prp #(j-1)
				obj.prp #(j-1) = tmp
				did_swap = true
			}
		}
		! if we didn't swap any elements, then the array is sorted
		if not did_swap : return
	}
}

!-----------------------------------------------------------------------
! SORT_ASCENDING() and SORT_DESCENDING()
!
! These simple comparison routines are provided to handle the
! common cases of sorting a numeric array in ascending or
! descending order.  For example, to sort an entire numeric array
! called "data" in ascending order, just do this:
!
! SortArray(data, data[], &SORT_ASCENDING)

routine SORT_ASCENDING(obj1, obj2)
{
	return (obj1 > obj2)
}

routine SORT_DESCENDING(obj1, obj2)
{
	return (obj1 < obj2)
}

routine SORT_RANDOM(obj1, obj2)
{
	return (random(2) - 1)
}
#endif ! ifclear NO_SORTING

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
\!

#ifset OBJECT_SORTING
object temp_bowl
{}

!\ MovePosition - moves object "obj" in parent "par" to position "pos"
( pos = 1 makes it the oldest child, bigger numbers make it younger) \!
routine MovePosition(obj,par,pos)
{
	local n = 1
	while child(par)
	{
		move child(par) to temp_bowl
	}

	while child(temp_bowl)
	{
		if n++ = pos
			move obj to par
		elseif child(temp_bowl) = obj
			move obj to temp_bowl
		else
			move child(temp_bowl) to par
	}
}

! MixObjects - Randomizes order of children of parent "par"
routine MixObjects(par)
{
	local n, obj

	while child(par)
	{
		move child(par) to temp_bowl
	}

	while child(temp_bowl)
	{
		n = children(temp_bowl)
		n = random(n)
		n--
		obj = child(temp_bowl)
		while n
		{
			obj = sibling(obj)
			n--
		}
		move obj to par
	}
}

! ReverseOrder - Reverses the order of parent's (par's) children
routine ReverseOrder(par)
{
	while child(par)
	{
		move child(par) to temp_bowl
	}

	while child(temp_bowl)
	{
		move youngest(temp_bowl) to par
	}
}

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
		move child(par) to temp_bowl
	}

	obj = youngest(temp_bowl)

	while elder(obj)
	{
		qualify = call arg(obj)
		if qualify
		{
			move obj to par
			obj = youngest(temp_bowl)
		}
		else
			obj = elder(obj)
	}

	while child(temp_bowl)
	{
		move child(temp_bowl) to par
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
			{
				break
			}
		}
}
! Roody's note: The previous array won't let us use it on the word array so
!  here is a word-array-specific one
routine ClearWordArray
{
	local n ! ,t
!	for (n=1;n< word[] ; n++ )
!	{
!		if word[n] = ""
!			t++
!		else
!		{
!			t = 0
!			word[n] = ""
!		}
!		if t = 2
!			break
!	}
	for (n=20;n< 30 ; n++ )
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

	key = system(11) ! READ_KEY
	if system_status or system(61) ! MINIMAL_INTERFACE
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
	list_count = children(obj)
	if list_count
		ListObjects(obj,conjunction)
}
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
		case &CheckReach
		{
			select num
				case 1
				{
					print capital The(a) ; IsorAre(a,true); \
					" inside "; The(parent(a)); "."
				}
		}
		case &DescribePlace
		{
			select num
				case 1
				{
					if not (FORMAT & DESCFORM_I) or (verbroutine ~= &MovePlayer and
					verbroutine ~= &DoLookAround and a = location)
						print "\n";
					else
					{
						local col
						col = display.cursor_column
						if system(61) or not display.hasgraphics
							--col
						if col
							"\n";
					}
					Font(BOLD_ON)
					print capital a.name;

					! Print ", in <something>" if necessary
					if location = a and player not in a
					{
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
		}
		case &DoGo
		{
			select num
				case 1	!  going to a non-enterable object in the same room
					print Cthe(object) ; IsorAre(object,true) ;" right here."
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
		case &DoPushDirTo
		{
        ! Let's set default DoPushDirTo messages
		select num
		case 1: "That would not help the present situation."
		case 2:  print "Try pushing "; Art(object); " in a direction."
		case 3:  print CThe(player); MatchPlural(player, " pushes ", " push "); Art(object); " over to the..."
		case 4:  print CArt(object); " slows to a stop."
		}
		case &DoQuit
		{
		select num
			case 1 : "Thanks for playing!"
		}
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
		case &DoUnlock
		{
			select num
			case 1
			{
				if a
					print "(with "; The(xobject); ")"
				print "Unlocked."
			}
			case 2
			{
				print capital player.pronoun #1;
				print " can't reach "; The(a); ", which is currently in ";
				print The(parent(a)); "."
			}
		}
		case &DoLock
		{
			select num
			case 1
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
		print capital player.name; " mumble"; MatchSubject(player);
		" an ancient reference to an archaic game. Nothing happens."
		}
#endif
		case &ListObjects
		{
			select num
				case 1: print ":"
		}
		case &WhatsIn
		{
			select num
				case 1: print ":"
		}
}

routine NewRlibMessages(r, num, a, b)
{
   select r
   case else : return false
   return true ! this line is only reached if we replaced something
}


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
				print CThe(xobject) ; " allows "; player.name ; " to take ";
				print The(object); "."
			}
	}
	case door
	{
		select num
		case 3
		{
		print "(unlocking "; Art(self); " first)"
		}
	}

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

#endif ! _ROODYLIB_H