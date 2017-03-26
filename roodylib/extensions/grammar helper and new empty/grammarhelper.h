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

global TOKEN

property grammar_rules

enumerate step *2
{
	TOKEN_RESET,		! 0, for reset
!	ANYTHING_T = 1,   ! accept anything objects
	HELD_T = 1,	     	! force held
	NOTHELD_T
!	,	! force unheld
!	ATTRIBUTE_T,	! force attribute
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
#ifset NEW_STYLE_PRONOUNS
	if obj.type = it_object
	{
		obj = obj.misc
!		obj.grammar_rules = (obj.misc).grammar_rules
	}
#endif
	if obj.grammar_rules
		TOKEN = obj.grammar_rules
	else
		TOKEN = TOKEN_RESET

!	if not (TOKEN & ANYTHING_T)
!	{
!		if not FindObject(obj,location)
!		{
!			ParseError(11,obj)  ! You don't see that.
!			return false
!		}
!	}

	if not Contains(player, obj) and (TOKEN & HELD_T)
		ParseError(15, obj) ! You're not holding that.
	elseif Contains(player, obj) and (TOKEN & NOTHELD_T)
		ParseError(11, obj)  ! You don't see that.
!#ifset NEW_STYLE_PRONOUNS
!	elseif ((obj = it_object and it_obj = goal_obj) or obj = goal_obj)
!		ret = true
!#else
	elseif (obj = goal_obj)
		ret = true
!#endif
	elseif goal_obj
		ParseError(12,obj)  ! You can't do that with that.
!	elseif (TOKEN & ATTRIBUTE_T)
!	{
		elseif (att1 and obj is not att1) or (att2 and obj is not att2) or
		(att3 and obj is not att3)
			ParseError(12, obj) ! "you can't do that with that."
		else
			ret = true
!	}
!	else
!		ret = true
	TOKEN = TOKEN_RESET
	return ret
}

! Roody's note: When using things like the Routine Grammar Helper, if
! your game has multiple items with similar names, disambiguation messages
! can pop up for items not even in the room (since we're using the anything
! grammar token). AnythingTokenCheck exists to make sure those objects aren't
! even considered. You may need to replace and adapt this if you write any
! code that uses the grammar helper.

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