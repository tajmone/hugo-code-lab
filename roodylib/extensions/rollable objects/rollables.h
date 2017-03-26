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
replace DoPushDirTo
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
	if k.type = door
		k = k.door_to
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
	else
		return true ! we printed a "door is locked" type message

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
#ifset USE_ATTABLES
	elseif player in location
		MoveAllAttachables(object, k, location, true)
#endif
	! Hopefully, it didn't get to Perform(&DoGo) only to have the command
	! fail, but if it does, we silently move the attachables back to the
	! first room
}