!::
! FINDPATH
! Roody Yogurt's attempt at pathfinding!
!::

#ifclear _FINDPATH_H
#set _FINDPATH_H

#ifset VERSIONS
#message "findpath.h Version 1.8"
#endif

#ifset USE_EXTENSION_CREDITING
#ifclear _ROODYLIB_H
#message error "Extension crediting requires \"roodylib.h\". Be sure to include it first!"
#endif
version_obj findpath_version "FindPath Version 1.8"
{
	in included_extensions
	desc_detail
		" by Roody Yogurt";
}
#endif

! Since rooms already use most existing hugolib properties, we'll make up some
! new ones

property forward
property backward

replace room
{
	forward 0
	backward 0
	type room
	is static, light, open
}

! MAXDISTANCE is the maximum rooms between rooms that will be
!   used
#if undefined MAXDISTANCE
constant MAXDISTANCE 20
#endif

! new property for characters
property dirsteps alias forward

replace character
{
	type character
	pronouns "he", "him", "his", "himself"
	capacity 50
	holding 0
	exclude_from_all true
	dirsteps #MAXDISTANCE
	is living, transparent, static
}

replace female_character
{
	inherits character
	pronouns "she", "her", "her", "herself"
	is female
}

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
#ifset _ROODYLIB_H
		actor
			{
			if verbroutine = &PreParse
				return false
			if AnyVerb(location)
				last_turn_true = false
			return false
			}
#ifset HUGOFIX
		actor DoVerbTest
			{
			DoVerbTest
			return true
			}
#endif  ! HUGOFIX
#endif ! ROODYLIB_H
		}
	capacity 100
	holding 0
	is hidden, living, transparent, static
	is plural       ! because player_person defaults to 2
}

routine MakePath(start,end)
{
	ClearRooms
	DistanceFrom(start,forward)
	DistanceFrom(end,backward)
}

routine SetPath(char)
{
	local a
	local b = 1
	local c,d
	a = parent(char)
	! let's clear the character steps
	! before filling it up again
	for (c = 1 ; c <= MAXDISTANCE ; c++)
		{
		char.dirsteps #c = 0
		}
	while a
	{
	d = FindPath(a)
	if not d
		return false

	for (c = n_obj ; c <= out_obj ; c++)
		{
		if d = c.dir_to
			break
		}
	char.dirsteps #(b++) = c


	a.forward = 0

	if (a.d).type = room
		a = a.d
	else
		{
		if InList(a.d, between, a ) = 1
			a = (a.d).between #2
		else
			a = (a.d).between
		}
	if a.backward = MAXDISTANCE
		a = 0
	}

	b = 1

	if FindScript(char)
		CancelScript(char)

	a = Script(char, PropertyCount(char,dirsteps))

	while b <= PropertyCount(char,dirsteps)
		{
		setscript[a++] = &CharMove
		setscript[a++] = char.dirsteps #(b++)
		}
	return true
}

!----------------------------------------------------------------------------
! BEHIND THE SCENES ROUTINES
!----------------------------------------------------------------------------

routine ClearRooms
{
	local i
	for (i = 29 ; i <= objects ; i++)
	{
	if i.type = room
		{
		i.forward = 0
		i.backward = 0
		}
	}
}

routine DistanceFrom( room_obj , prop)
{
	local max = MAXDISTANCE
	local i,j,c,d
	room_obj.prop = max
while max
{
	for (i = 29 ; i <= objects ; i++)
		{
		if i.type = room and i.prop = max
			{
			 for  (j=n_to; j<=out_to; j++)
			  {
			  if i.j.type = room and not i.j.prop
					{
					i.j.prop = max - 1
					c=1
					}
			elseif  i.j.type = door and not (i.j is not open and i.j is locked)
					{
					if InList(i.j, between, i) = 1
						d = i.j.between #2
					else
						d = i.j.between
					if not d.prop
						{
						d.prop = max - 1
						c = 1
						}
					}
			  }
			}
		}
	if c=0
		break
	c=0
	max--
}
}

routine FindPath(rm)
{

	local j,a,d
  for (j=n_to; j<=out_to; j++)
	  {
	  if rm.j.type = room
			{
			if (rm.j.forward + rm.j.backward) > a
				a = (rm.j.forward + rm.j.backward)
			}
	  elseif rm.j.type = door and not (rm.j is not open and rm.j is locked)
		{
		if InList(rm.j, between, rm)= 1
			d = rm.j.between #2
		else
			d = rm.j.between
		if (d.forward + d.backward) > a
			a = (d.forward + d.backward)
		}
	}

  for (j=n_to; j<=out_to; j++)
  {
	  if rm.j.type = room
			{
			if (rm.j.forward + rm.j.backward) = a
				break
			}
	  elseif rm.j.type = door and not (rm.j is not open and rm.j is locked)
		{
		if InList(rm.j, between, rm) = 1
			d = (rm.j).between #2
		else
			d = (rm.j).between
		if (d.forward + d.backward) = a
			{
		!\	if d.forward = 0
				return 0
			else
				return j \!
			break
			}
		}
	}
	if (rm.j.type = room and not rm.j.forward) or (rm.j.type = door and not \
			d.forward)
		return 0
	else
		return j
}

! This counts how many elements in a property array have
! values
routine PropertyCount(obj, prop)
{
	local a, total

	for (a=1; a<=obj.#prop; a++)
	{
		if obj.prop #a:  total++
	}

	return total
}

#ifclear _ROODYLIB_H
replace CharMove(char, dir)
{
#ifclear NO_OBJLIB

	local newroom, a

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
#endif  ! ifclear _ROODYLIB_H

#endif ! ifclear _FINDPATH_H