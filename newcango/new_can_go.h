!::
! New Can Go
!::

#ifset VERSIONS
#message "new_can_go.h Version 1.0"
#endif
#ifclear _NEW_CAN_GO
#set _NEW_CAN_GO

#ifset USE_EXTENSION_CREDITING
#ifclear _ROODYLIB_H
#message error "Extension crediting requires \"roodylib.h\". Be sure to include it first!"
#endif
version_obj can_go_version "Can Go Version 1.0"
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
			Perform(&GetExits,object)
			return false
		}
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

routine GetExits
{
! reset all of our destination "jars" and direction "pebbles"
	while child(exit_shelf)
	{
		while child(child(exit_shelf))
		{
			move child(child(exit_shelf)) to direction_pile
		}
		move child(exit_shelf) to storage_shelf
	}

	local a, b, c, d
	a = child(direction_pile)  ! pick a direction pebble
	while a
	{
		d = younger(a)   ! choose next pebble, too
		if object.(a.direct) ! see if the direction exists from the intended room
		{
			b = object.(a.direct)
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
	local a, b, c, d, e,f,g

	f = children(exit_shelf)
	g = f > 2
	for a in exit_shelf
	{
		if e++
			print " ";
		if location.no_clump or e=1
			print capital player.name ; " can go ";
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
		" to ";
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
	}
	if not location.no_clump
	{
		print ".";
	}
	""
}

#endif ! _NEW_CAN_GO