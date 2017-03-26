!\ Roody's note: ROTATE_DESC and NEW_ROTATE. Both allow a nicer way to quickly
add multiple descriptions to an object. ROTATE_DESC is quicker to set up but is
limited to 5 descriptions per object. NEW_ROTATE uses as many as you'd like. \!

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