!::
! New Can Go
!::
!\ Changelog
	1.1 - Added door support, although non-door items with door_to properties may
	still need some help (both here and in Roodylib's DoGo)
	1.2 - Added no-exit support. Made exit-listing order consistent.
	      Added "inconsistent" global. When set to true, new_can_go uses the
			less-consistent (but not fully random) exit-ordering system.
\!
#ifclear _NEW_CAN_GO
#set _NEW_CAN_GO

#ifset VERSIONS
#message "new_can_go.h Version 1.2"
#endif

#ifset USE_EXTENSION_CREDITING
#ifclear _ROODYLIB_H
#message error "Extension crediting requires \"roodylib.h\". Be sure to include it first!"
#endif
version_obj can_go_version "Can Go Version 1.2"
{
	in included_extensions
	desc_detail
		" by Roody Yogurt";
}
#endif


!\ We replace the room class so we can call GetExits before moving into
any room. I used the before property since the after properties aren't called
until after DescribePlace is executed.\!
replace room
{
	type room
	is static, light, open
	before
	{
		object MovePlayer
		{
			! This old_location thing exists so GetExits is run only once before
			! the game starts (since during the init routine, the starting location
			! is both the MovePlayer object and the location global variable value.
			if not old_location
			{
				old_location = location
				return false
			}
			GetExits
			return false
		}
	}
	cant_go
	{
		print capital player.name ; " can't go that way."; AFTER_PERIOD ;
		PrintExits
	}
}

constant SKIP 2  ! this is a constant we can set cango_name to when we want
                 ! room exits to ignored completely
!\Example usage:
room SouthRoom "South Room"
{
	long_desc
	{
		PrintExits
	}
	cango_name
	{
		if <some flag>
			return "a southerly room"
		else
			return SKIP
	}
	n_to EastRoom
}
\!
property cango_name  ! how you want the room to be described, if not by the room
                     ! name
!	cango_name "the top of the hill" ! "you can go east to the top of the hill"
property no_clump ! set to true if each destination gets its own sentance
! "You can go east to the river. You can go south to the field."
! keep false if you want everything in one sentence

property direct ! you don't need to worry about this

global inconsistent_exits ! set to true if exit order should not be held to a
                          ! strict order (otherwise, it goes with the order of
								  ! the object hierarchy code it uses)
routine GetExits
{
! reset all of our destination "jars" and direction "pebbles"
	local i

	if not inconsistent_exits
	{
		for (i = north_pebble;i <= out_pebble ;i++ )
		{
			move i to direction_pile
		}
		OtherExits ! (see below)
	}
	local a
	while child(exit_shelf)
	{
		a = child(exit_shelf)
		if inconsistent_exits
		{
			local r
			a = child(exit_shelf)
			r = random(2)
			if r = 2
			{
				a = youngest(exit_shelf)
				while child(a)
				{
					move youngest(a) to direction_pile
				}
			}
			else
			{

				while child(a)
				{
					move youngest(a) to direction_pile
				}
			}
		}
!		if not a
!		{
!			a = child(exit_shield)
!		}
		move a to storage_shelf
	}

	local b, c, d
	a = youngest(direction_pile)  ! pick a direction pebble
	while a
	{
		d = elder(a)   ! choose next pebble, too
		if object.(a.direct) ! see if the direction exists from the intended room
		{
			b = object.(a.direct)
			if b.type = door and b is not locked
			{
				b = b.between #((object = \
				b.between #1) + 1)
			}
			elseif (&b.door_to and b is not locked and
			b.type ~= door and b.type ~= room)
				b = b.door_to
			elseif (b.door_to and b is not locked and
			b.type ~= door and b.type ~= room)
				b = b.door_to

			if b.type = room and b.cango_name ~= SKIP ! make sure the direction
			{                     ! goes to a room and shouldn't be skipped
				for c in exit_shelf
				{
					if c.name = b.cango_name, b.name
					{                    ! if a jar on the "exit shelf" has the same
						move a to c       ! name as the current destination room, move
						break             ! this pebble to that jar
					}
				}
				if a in direction_pile        ! if not, start a new destination
				{                             ! room jar on the exit shelf
					c = child(storage_shelf)
					move c to exit_shelf
					if b.cango_name
					{
						c.name = b.cango_name
					}
					else
						c.name = b.name
					move a to c
				}
			}
		}
		a = d
	}
}

routine OtherExits
{
!\ If your game adds new direction "pebbles" to the "storage jar", you can write
some code to clear them here. Assuming your new direction pebbles are all
declared together, you can do something like this: \!
!		for (i = fore_pebble;i <= port_pebble ;i++ )
!		{
!			move i to direction_pile
!		}
}
! a bunch of objects to represent everything we're tracking
object direction_pile
{}

object north_pebble "north"
{
	direct n_to
	in direction_pile
}

object northeast_pebble "northeast"
{
	direct ne_to
	in direction_pile
}

object east_pebble "east"
{
	direct e_to
	in direction_pile
}

object southeast_pebble "southeast"
{
	direct se_to
	in direction_pile
}

object south_pebble "south"
{
	direct s_to
	in direction_pile
}

object southwest_pebble "southwest"
{
	direct sw_to
	in direction_pile
}

object west_pebble "west"
{
	direct w_to
	in direction_pile
}

object northwest_pebble "northwest"
{
	direct nw_to
	in direction_pile
}

object up_pebble "up"
{
	direct u_to
	in direction_pile
}

object down_pebble "down"
{
	direct d_to
	in direction_pile
}

object out_pebble "out"
{
	direct out_to
	in direction_pile
}

object in_pebble "in"
{
	direct in_to
	in direction_pile
}

object exit_shelf
{}

object storage_shelf
{}

! I only made 11 "jars", so if your game has rooms with more than 11
! different destination exits, you'll need to add some more jars

object jar1
{
	in storage_shelf
}

object jar2
{
	in storage_shelf
}

object jar3
{
	in storage_shelf
}

object jar4
{
	in storage_shelf
}

object jar5
{
	in storage_shelf
}

object jar6
{
	in storage_shelf
}

object jar7
{
	in storage_shelf
}

object jar8
{
	in storage_shelf
}

object jar9
{
	in storage_shelf
}

object jar10
{
	in storage_shelf
}

object jar11
{
	in storage_shelf
}

! the routine responsible for the "You can go" messages. Replace if you need
! to tweak.
routine PrintExits
{
	local a, b, c, d, e,f,g,h

	f = children(exit_shelf)
	g = f > 2
	a = child(exit_shelf)
	while a
!	for a in exit_shelf
	{
		h = younger(a)
		if e++
			print AFTER_PERIOD;
		if location.no_clump or e=1
			CanGoMessage(&PrintExits, 1,1) ! "You can go ";
		b = children(a)
		d = b > 2
		c = child(a)
		while b
		{
			print c.name;
			if b > 1 and d
			{
				print ", ";
			}
			if b = 2
			{
				if not d
				{
					" ";
				}
				print "or ";
			}
			c = younger(c)
			b--
		}
		CanGoMessage(&PrintExits, 1,2) ! " to ";
		print a.name;
		if location.no_clump
			print ".";
		else
		{
			--f
			if f and g
			{
				print ",";
			}
			if (f = 1 and g) or (f and not g)
			{
				print " or";
			}
		}
		a = h
	}
	if e
	{
		if not location.no_clump
		{
			print ".";
		}
		""
	}
	else
		CanGoMessage(&PrintExits,2 ) ! "You can't go anywhere."
}

routine CanGoMessage(r, num, a, b)
{
	! Check first to see if the new messages routine provides a
	! replacement message:
	if NewCanGoMessages(r, num, a, b):  return

	select r

	case &PrintExits
	{
		select num
		case 1
		{
			select a
				case 1
				{
					print capital player.name ; " can go ";
				}
				case 2
					" to ";
		}
		case 2
			print capital player.name ; " can't go anywhere."
	}
}

routine NewCanGoMessages(r, num, a, b)
{
	select r

!	case <first routine>
!	{
!		select num
!		case 1:
!	}

	case else : return false

	return true ! this line is only reached if we replaced something
}
#endif ! _NEW_CAN_GO