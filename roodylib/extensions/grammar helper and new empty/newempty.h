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


property empty_type alias grammar_rules
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
!	if obj.empty_type
!	{
!		TOKEN = obj.empty_type
!		if not CheckObject(obj)
!			return false
!		elseif obj.empty_type = NO_EMPTY_T
!		{
!			ParseError(12, obj)
!			return false
!		}
!		return true
!	}
!	elseif not CheckObject(obj)
!		return false
	if not CheckObject(obj)
		return false
	elseif obj.empty_type = NO_EMPTY_T
	{
		ParseError(12, obj)
		return false
	}
	return true
}

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

replace AnythingTokenCheck(obj,objloc)
{
	local failed_check
	select verbroutine
#ifclear NO_VERBS
		case &DoEmpty ! should only be called if NEW_EMPTY is being used
		{
			if not parent(obj) or not FindObject(parent(obj),location,true)
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
