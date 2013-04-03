!::
! FINDPATH
! Roody Yogurt's attempt at pathfinding!
!::

#ifclear _FINDPATH_H
#set _FINDPATH_H

#ifset VERSIONS
#message "findpath.h Version 1.9"
#endif

#ifset USE_EXTENSION_CREDITING
#ifclear _ROODYLIB_H
#message error "Extension crediting requires \"roodylib.h\". Be sure to include it first!"
#endif
version_obj findpath_version "FindPath Version 1.9"
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

array PathArray[MAXDISTANCE]

routine MakePath(start,end)
{
	ClearRooms
	DistanceFrom(start,forward)
	ClearRooms
	DistanceFrom(end,backward)
}

routine SetPath(char)
{
	local a
	local b ! = 1
	local c,d, i
	a = parent(char)
	local total
!	this cleaing code is memory-wasteful
!	! let's clear the character steps
!	! before filling it up again
!	for (c = 1 ; c <= MAXDISTANCE ; c++)
!		{
!		char.dirsteps #c = 0
!		}


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
!		char.dirsteps #(b++) = c
		PathArray[b++] = c
		total = b
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

!	PathArray[b++] = 0

!	char.dirsteps #(b) = 0

	if FindScript(char)
		CancelScript(char)

!	a = Script(char, PropertyCount(char,dirsteps))
	a = Script(char, total)

!	while b <= PropertyCount(char,dirsteps)
	for (i = 0;i < total ; i++ )
	{
		setscript[a++] = &CharMove
		setscript[a++] = PathArray[i]
	}
!	while b < total
!	{
! ! char.dirsteps #(b++)
!		b++
!	}
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
			i is not already_listed
!		i.forward = 0
!		i.backward = 0
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
					if i.j.type = room and i.j is not already_listed ! not i.j.prop
					{
						i.j.prop = max - 1
						c=1
						i is already_listed
					}
					elseif  i.j.type = door and
					not (i.j is not open and i.j is locked)
					{
						if InList(i.j, between, i) = 1
							d = i.j.between #2
						else
							d = i.j.between
						if d is not already_listed ! not d.prop
						{
							d is already_listed
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

	local j,a,d , highest
	for (j=n_to; j<=out_to; j++)
	{
		if rm.j.type = room
		{
			if (rm.j.forward + rm.j.backward) > a
			{
				a = (rm.j.forward + rm.j.backward)
				highest = j
			}
		}
		elseif rm.j.type = door and not (rm.j is not open and rm.j is locked)
		{
			d = rm.j.between #((rm = rm.j.between # 1) + 1)
			if (d.forward + d.backward) > a
			{
				a = (d.forward + d.backward)
				highest = j
			}
		}
	}

	if (rm.highest.type = room and not rm.highest.forward) or
	(rm.highest.type = door and \
		not (rm.highest.between #((rm = rm.highest.between # 1) + 1)).forward)
		return 0
	else
		return highest
}

!#ifclear _ROODYLIB_H
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
			if a is locked
			{
				""
				print CThe(char) ; " can't unlock the door."
				CancelScript(char)
				return
			}
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

!#endif  ! ifclear _ROODYLIB_H

#endif ! ifclear _FINDPATH_H