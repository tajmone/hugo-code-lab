!----------------------------------------------------------------------------
!* SCOPE_REACT
!----------------------------------------------------------------------------

object scope_objects
{}

object scopelib "scope organizer"
{
	in init_instructions
	type settings
	execute
	{
		if word[LAST_TURN] ~= "undo"
		{
			if word[LAST_TURN] ~= "restore" ! new game
			{
				OrganizeScopeObjects
			}
		}
	}
}

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
		real_loc = parent(loc) ! no need to check deeper as the default Hugo
		                       ! library doesn't really support being inside
		                       ! an object within an object

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
			if ret : break
			ret = ScopeReactBefore
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
			if not ret
				ret = ScopeReactAfter
		}
		if lig and not silent
		{
#ifset NEW_ROOMS
			local firsttime
#ifset MULTI_PCS
			firsttime = not ObjectIsMovedVisited(location)
#else
			firsttime = location is not visited
#endif
			if not location.first_visit or firsttime
				location.first_visit = counter + 1
			location.first_visit #2 = counter + 1
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

!  These temp objects guarantee we go back to whatever the previous
!  settings were before Perform was called
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
		ApplyPronouns(object, xobject)
#endif

!  some stuff we do when Perform is called by the engine
	if (parser_data[PARSER_STATUS] & PARSER_ACTIVE) and not isxverb
	{
		last_actor = actor
		parser_data[LAST_PARSER_STATUS] &= ~PRONOUNS_SET
		SetPronouns
		parser_data[LAST_PARSER_STATUS] |= PRONOUNS_SET
		parser_data[PARSER_STATUS] &= ~PARSER_ACTIVE
		SaveOldWord
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
	if not r
		r = ScopeReactBefore
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
			ScopeReactAfter
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