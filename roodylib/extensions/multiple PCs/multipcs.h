!::
! EXTENSION NAME
!::

!\#ifset VERSIONS
#message "[NAME].h Version #"
#endif \!


replace Acquire(newparent, newchild)
{
	local p,h

	CalculateHolding(newparent)

	if newparent.#holding
		h = newparent.holding
	else
		h = holding_global

	if h + newchild.size > newparent.capacity
		return false
	else
	{
		p = parent(newchild)
		move newchild to newparent
		CalculateHolding(p)
#ifset MULTI_PCS
		MakeMovedVisited(newchild)
#else
		newchild is moved
#endif
		newchild is not hidden
		if newparent.#holding
			newparent.holding = newparent.holding + newchild.size
		return true
	}
}

replace ListObjects(thisobj, conjunction)
{
	local i, obj, count, pluralcount,d
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
#ifset MULTI_PCS
				MakeKnown(obj)
#else
				obj is known
#endif  ! MULTI_PCS
#endif  ! NO_OBJLIB
		}
		else
#ifset MULTI_PCS
			MakeKnown(obj)
#else
			obj is known
#endif

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

			if (FORMAT & ISORARE_F)
			{
				if list_count = 1 and id_count <= 1 and
					obj is not plural
					RLibMessage(&ListObjects,1, IS_WORD) ! "is"
				else
					RLibMessage(&ListObjects,1, ARE_WORD) ! "are"
				if (FORMAT & LIST_F)
					RLibMessage(&ListObjects,2) ! ":"
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
				d = list_nest
				if (verbroutine ~= &DoInventory, &DoLookIn,&DoLook)  and
				FORMAT & NOINDENT_F
					d--
				print to ((d ) * 2);	! INDENT_SIZE);
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
					if (FORMAT & FIRSTCAPITAL_F) or (FORMAT & LIST_F)
						CArt(obj)
					else
						Art(obj)
				}
				else
				{
					if (FORMAT & FIRSTCAPITAL_F) or (FORMAT & LIST_F)
						print NumberWord(id_count, true);
					else
						print NumberWord(id_count);
					print " "; this_class.name;

					if this_class.type = plural_class
					{
						local k

						if (FORMAT & LIST_F)
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
									d = list_nest
									if (verbroutine ~= &DoInventory, &DoLookIn,&DoLook) and  (FORMAT & NOINDENT_F)
										d--
									print to ((d ) * 2);	! INDENT_SIZE);
								}
								if (FORMAT & LIST_F) and verbroutine ~= &DoInventory
									CArt(j)
								else
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

				if ((FORMAT & FIRSTCAPITAL_F) or (FORMAT & LIST_F)) and
					verbroutine ~= &DoInventory
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

		if obj is not hidden and (FORMAT & LIST_F)
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
			if list_nest = 1 and (FORMAT & ISORAREHERE_F)
			{
				if count + pluralcount > 1
					print " "; ARE_WORD;
				else:  print " "; IS_WORD;
#ifset USE_RELATIVE_DESCRIPTIONS
				if not (DESCRIBEPLACEFORMAT & OVERRIDEHERE_F)  or
				((DESCRIBEPLACEFORMAT & OVERRIDEHERE_F) and
				thisobj = parent(player))
#endif
					print " "; HERE_WORD;
#ifset USE_RELATIVE_DESCRIPTIONS
				if DESCRIBEPLACEFORMAT & OVERRIDEHERE_F
				{
					if DESCRIBEPLACEFORMAT & ALSO_F
					{
						print " also";
						DESCRIBEPLACEFORMAT = DESCRIBEPLACEFORMAT & ~ALSO_F
					}
					print " ";
					RelativeText(thisobj)
					if DESCRIBEPLACEFORMAT & OVERRIDEHERE_F
						DESCRIBEPLACEFORMAT = DESCRIBEPLACEFORMAT & ~OVERRIDEHERE_F
						! clear it
				}
#endif
				FORMAT = FORMAT & ~ISORAREHERE_F  ! clear it
				if not ((FORMAT & LIST_F) or (FORMAT & TEMPLIST_F))
					override_indent = true
			}

			if not (FORMAT & NORECURSE_F)
				print ".";
		}

		i = 0
		local char_count
		for obj in thisobj
		{
			if children(obj) and obj is not hidden and
				(obj is not already_listed or
					thisobj ~= location) and not ClothingCheck(obj)
			char_count++
			if char_count = 2
				break
		}
		for obj in thisobj
		{
			if children(obj) and obj is not hidden and
				(obj is not already_listed or
					thisobj ~= location) and not ClothingCheck(obj)
			{
				if (FORMAT & TEMPLIST_F)
				{
					FORMAT = FORMAT | LIST_F & ~TEMPLIST_F
					i = true
					print newline
				}

#ifclear NO_OBJLIB
				if count > 1 and obj.type = character
				{
					FORMAT = FORMAT | USECHARNAMES_F
					if char_count = 2
					{
						print newline
						override_indent = false
					}
				}
#endif
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

replace SpecialDesc(obj)
{
	local a, c, flag, printed_blankline, d

	if (FORMAT & LIST_F)
		return

	list_count = 0
	for a in obj
	{
		if a is not hidden
		{
			flag = true
			c++
		}
		else
			flag = false

		if (FORMAT & INVENTORY_F) and obj = player and flag
		{
			if &a.inv_desc
			{
				print newline
				Indent
			}
			if a.inv_desc
			{
				if (FORMAT & LIST_F):  print newline
				AddSpecialDesc(a)
			}
		}
#ifset MULTI_PCS
		elseif not ObjectIsMovedVisited(a) and flag
#else
		elseif a is not moved and flag
#endif
		{
			if &a.initial_desc
			{
				d++
				print newline
				override_indent = false
				if (FORMAT & INVENTORY_F) and (FORMAT & NOINDENT_F) and not printed_blankline
					print ""
				printed_blankline = true
				Indent
				need_newline = true
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
			not a.list_contents and not ClothingCheck(a)
			{
				WhatsIn(a)
				list_count = 0
			}
		}
	}
}

#ifset DEBUG
replace DoHugoFix
{
	local i, n

	Font(PROP_OFF) ! proportional printing off

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
		if opcodeterp is switchedon
		{
			"\_    $om - Opcode monitor on/off"
			"\_    $oa - Opcode audit"
		}
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
		\_    $pn                - List current pronouns\n\
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
		\_    $rp                - Skip in-game pauses in recording playback\n\
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
#ifset MULTI_PCS
			MakeKnown(object)
#else
			object is known
#endif
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
#ifset MULTI_PCS
				MakeKnown(i)
#else
				i is known
#endif
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
#ifset MULTI_PCS
		MakeKnown(object)
#else
		object is known
#endif
	}
	case "$mp"
	{
		if (parent(object) and object is not enterable)
		{
			print "[Obj. "; number object; " (";
			print object.name; ") is not a room or enterable object]"
		}
#ifclear NO_OBJLIB
		elseif (not parent(object) and object.type ~= room)
		{
			print "[Obj. "; number object; " (";
			print object.name; ") is not a room or enterable object]"
		}
#endif
		else
		{
			Font(DEFAULT_FONT)
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
	case "$oa"
	{
		if opcodeterp is not switchedon
			"[Opcode-enabled interpreter not detected. Opcode monitor not
			turned on.]"
		else
			OpcodeAudit
	}
	case "$om"
	{
		if opcodeterp is not switchedon
			"[Opcode-enabled interpreter not detected. Opcode monitor not
			turned on.]"
		else
		{
			print "[Opcode monitor";
			OnorOff(D_OPCODE_MONITOR)
		}
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
			'$ot 0' to draw the entire object tree, including the children
			of the \"object classes\" and \"settings\" objects.]\n"
	!		Font(DEFAULT_FONT)
	!		return
		}
		elseif word[2] = "0"
			"[Skipped object numbers are replaced objects.]\n"

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
			local a,b,c
#ifset _WINDOW_H
			if i.type = window_class
				a++
#endif
#ifclear NO_FUSES
			if i.type = fuse and i ~= fuse
				b++
			elseif i.type = daemon and i ~= daemon
				c++
#endif
			if i ~= object and parent(i) = object
				DrawBranch(i,a,b,c)
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
				if i.noun and (not parse_rank_only or i.parse_rank)
				{
					if ListParserConflicts(i, parse_rank_only)
						count++
				}
			}
			if not count
				"No parser conflicts found."
		}
	}
	case "$pn"
	{
		n = 1
		while n < 5
		{
			select n
				case 1
				{
					print "it object: ";
					i = it_obj
				}
				case 2
				{
					print "him object: ";
					i = him_obj
				}
				case 3
				{
					print "her object: ";
					i = her_obj
				}
				case 4
				{
					print "them object: ";
					i = them_obj
				}
			if not i
				print "unassigned"
			else
			{
				print i.name; " ("; number i; ")"
			}
			n++
		}
	}
	case "$rp"
	{
		"[In-game pause-skipping in recording playback";
		OnorOff(D_PLAYBACK_HELPER)
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
#ifset MULTI_PCS
		MakeUnknown(object)
#else
		object is not known
#endif
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

routine MakeUnknown(obj)
{
	obj is not known
}
#endif ! ifset DEBUG

replace FindObject(obj, objloc, recurse)
{
	local a, p
	local this_parse_rank
	local found_result = true
	local FindObjectIsFound

	parser_data[PARSER_STATUS] |= FINDOBJECT_CALLED

	if obj = nothing or obj = player
	{
		if obj = nothing and not recurse
		{
#ifset DEBUG
			if debug_flags & D_FINDOBJECT
				print "[Resetting FindObject]"
			if debug_flags & D_PARSE_RANK
				print "[parser_data[BEST_PARSE_RANK] = 0]"
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
#ifset MULTI_PCS
		MakeKnown(obj)
#else
		obj is known
#endif
		return true
	}
#ifclear NO_FUSES
	elseif obj.type = fuse or obj.type = daemon
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

	if not recurse and parser_data[PARSER_STATUS] & PARSER_ACTIVE
	{
		if FailsAllChecks(obj,objloc)
			return false

	! Do some parse_rank tweaking to rank lower objects which (probably) don't
	! apply to a particular command, like trying to open something that's
	! already open:
		this_parse_rank = obj.parse_rank

	! And be ready to prefer the last specifically referred to object in
	! the event of disambiguation
		if obj = parser_data[LAST_SINGLE_OBJECT]
			this_parse_rank++

#ifclear NO_VERBS
		this_parse_rank = CheckParseRank(obj, this_parse_rank)

#ifset USE_CHECKHELD
		if checkheld is active
		{
			if DismissUnheldItems(obj,objloc)
				return false
			elseif PrioritizeHeldItems(obj)
				this_parse_rank += 100
		}
#endif	! USE_CHECKHELD
#endif	! #ifclear NO_VERBS
	}

	p = parent(obj)  ! find the parent of the object

	! The objloc argument is equal to 0 if a) the grammar token is
	! "anything", or b) the engine is re-testing object availability
	! given the setting of parser_data[BEST_PARSE_RANK] during the first
	! pass of object disambiguation.

	if objloc = 0 and parser_data[PARSER_STATUS] & PARSER_ACTIVE
	{
#ifset MULTI_PCS
		if location and not ObjectIsKnown(obj)
#else
		if location and obj is not known
#endif
		{
			if FindObject(obj, location, true)
#ifset MULTI_PCS
				MakeKnown(obj)
#else
				obj is known
#endif
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
			if not AnythingTokenCheck(obj,objloc)
				return false
			elseif this_parse_rank < parser_data[BEST_PARSE_RANK]
			{
#ifset DEBUG
				if debug_flags & D_FINDOBJECT
				{
					print "[FindObject("; obj.name; " ["; number obj; "], "; \
						objloc.name; " ["; number objloc; "]):  "; \
						"false (this_parse_rank = "; number this_parse_rank; \
						" < parser_data[BEST_PARSE_RANK] = "; number parser_data[BEST_PARSE_RANK]; ")]"
				}
#endif
				return false
			}
		}
		FindObjectisFound = true
	}
	elseif obj = objloc or
		(player in obj and obj ~= objloc and (obj~=location or not recurse)) or
		p = obj or p = objloc or p = player
	{
#ifset MULTI_PCS
		MakeKnown(obj)
#else
		obj is known
#endif
		FindObjectIsFound = true
	}
	elseif p  ! does obj have a parent?
	{
		local supercheck
#ifset USE_SUPERCONTAINER
		if p.type = SuperContainer
		{
			if InList(p, contents_in, obj) and p is not open and p is openable
!			and p is not transparent
			{
				if p ~= parent(player) or (p = parent(player) and
					InList(p, contents_on, player))
					supercheck = true
			}
		}
#endif
		if (p is not openable or p is platform) and p is not quiet and
				not supercheck
		{
			if FindObject(p, objloc, true) and ObjectisKnown(p)
			{
#ifset MULTI_PCS
				MakeKnown(obj)
#else
				obj is known
#endif
				FindObjectIsFound = true
			}
		}
		elseif p is openable and p is open and p is not quiet
		{
			if FindObject(p, objloc, true) and ObjectisKnown(p)
			{
#ifset MULTI_PCS
				MakeKnown(obj)
#else
				obj is known
#endif
				FindObjectIsFound = true
			}
		}
		elseif p is transparent, not quiet
		! (p is openable, not open, transparent, not quiet)
		{
			if FindObject(p, objloc, true) and ObjectisKnown(p)
			{
#ifset MULTI_PCS
				MakeKnown(obj)
#else
				obj is known
#endif
				found_result = 2
				FindObjectIsFound = true
			}
		}
	}

	if obj.#found_in and not FindObjectIsFound
	{
		for (a=1; a<=obj.#found_in; a++)
		{
			if obj.found_in #a and (obj.found_in #a = objloc or
				FindObject(obj.found_in #a, objloc, true))
			{
#ifset MULTI_PCS
				MakeKnown(obj)
#else
				obj is known
#endif
				FindObjectIsFound = true
				break
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
					FindObjectIsFound = true
				elseif FindObject(obj.in_scope #a, objloc, true)
					FindObjectIsFound = true
				if FindObjectIsFound
				{
#ifset MULTI_PCS
					MakeKnown(obj)
#else
					obj is known
#endif
					break
				}
			}
		}
	}
	if parser_data[PARSER_STATUS] & PARSER_ACTIVE and not recurse
		MakeParserNotes(obj, FindObjectIsFound)
	if not FindObjectIsFound
	{
#ifset DEBUG
		if debug_flags & D_FINDOBJECT and not recurse
		{
			print "[FindObject("; obj.name; " ["; number obj; "], "; \
				objloc.name; " ["; number objloc; "]):  "; \
				"false]"
		}
#endif
		return false
	}

! FindObjectIsFound area
	if not recurse and parser_data[PARSER_STATUS] & PARSER_ACTIVE
	{
		local new_disamb
		if parser_data[PARSE_RANK_TESTS]++ = 0
		{
#ifset DEBUG
			if debug_flags & D_PARSE_RANK and this_parse_rank > parser_data[BEST_PARSE_RANK]
			{
				print "[parser_data[BEST_PARSE_RANK] = "; number this_parse_rank; "]"
			}
#endif
			parser_data[BEST_PARSE_RANK] = this_parse_rank
			new_disamb = true
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
!			new_disamb = true
		}
#ifclear NO_DISAMB_HELP
	if not objloc or (new_disamb and disamb_holder.disamb_suspects)
		FillDisambHolder(obj,new_disamb,this_parse_rank)
#endif
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
			if actor in location
#ifset MULTI_PCS
	MakeKnown(actor)
#else
	actor  is known
#endif
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
			if actor in location
#ifset MULTI_PCS
	MakeKnown(actor)
#else
	actor  is known
#endif
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

replace ShortDescribe(obj)
{
	local ListContents
#ifset MULTI_PCS
	MakeKnown(obj)
#else
	obj is known
#endif

	if list_nest = 1
		print newline

	AssignPronoun(obj)

#ifset MULTI_PCS
	if not ObjectIsMovedVisited(obj) and &obj.initial_desc
#else
	if obj is not moved and &obj.initial_desc
#endif
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

		if not obj.short_desc
			return
	}

!  "ListContents" section
	obj is already_listed
	AssignPronoun(obj)

	list_count = 0

	if children(obj) > 0 and (obj is platform or obj is transparent or
	(obj is container and
	(obj is not openable or (obj is openable and obj is open)))) and
	obj is not quiet
	{
		list_nest = (LIST_F = (FORMAT & LIST_F))
		WhatsIn(obj)
	}

	! If INDENT_SIZE is 0, formatting may be thrown off when listing
	! the contents of an object:
	if INDENT_SIZE = 0:  need_newline = true

	if need_newline:  print newline
}

#ifset USE_VEHICLES
replace DoMoveinVehicle
{
	local v, moveto

	if player in location
	{
		RlibOMessage(vehicle, 2)    ! "You aren't in anything."
		return false
	}

	v = parent(player)              ! the vehicle
	if v.type ~= vehicle
	{
		RlibOMessage(vehicle,3) ! "Good luck with that."
		return false
	}
	if v.before:  return true

	! Match the verb
	if not InList(v, vehicle_verbs, VerbWord)
	{
		OMessage(vehicle, 3, v)        ! wrong verb for this vehicle
		return false
	}

	if not object or (object = v and player in object)
	{
		OMessage(vehicle, 4)   ! "Specify a direction as well..."
		return false
	}

	if not v.vehicle_move:  return true

	if obstacle
	{
		OMessage(vehicle, 5, v) ! "X stops you from moving."
		return true
	}

	if object.type ~= direction
	{
		moveto = object.door_to
		if not moveto
			! "You can't (drive) in there..."
			OMessage(vehicle, 6, v)

		if moveto <= 1
			return
	}
	else
		moveto = location.(object.dir_to)

	if not InList(moveto, vehicle_path, v) and moveto ~= 1
	{
		OMessage(vehicle, 7, v) ! "You can't (drive) that way."
		return false
	}
	elseif moveto = 0
	{
		if not location.cant_go
			OMessage(vehicle, 7, v)
		return false
	}
	elseif moveto = 1               ! already printed message
		return false            !   (moveto is never 1)

#ifset USE_ATTACHABLES
	if ObjectisAttached(v, location, moveto)
		return false
#endif

	! Finally, the vehicle can move
	move v to moveto
#ifset MULTI_PCS
	MakeMovedVisited(v)
#else
	v is moved
#endif
	old_location = location
	location = moveto

#ifset USE_ATTACHABLES
	MoveAllAttachables(v, old_location, location)
#endif

	if not FindLight(location)
		DarkWarning
	else
	{
		if not event_flag
			event_flag = true
		DescribePlace(location)
#ifset MULTI_PCS
		MakeMovedVisited(location)
#else
		location is visited
#endif
	}

	run parent(player).after

	return true
}
#endif ! #ifset USE_VEHICLES

replace DoOpen
{
	local tempformat, light_check, skip_ahead, force_def

#ifset FORCE_DEFAULT_MESSAGES
	force_def = true
#endif
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
#ifset MULTI_PCS
		MakeMovedVisited(object)
#else
		object is moved
#endif
		local x, i
		if not Contains(object,player) and object is not transparent
		{
			for x in object
			{
				if x is not hidden
				{
					i = true
					x is not already_listed
				}
			}
		}
		if i
		{
			if not light_source
				light_check = FindLight(location)  ! in case the light source
                                               ! has been revealed
		}
		if not object.after
		{
			if not i or object is quiet or force_def
				VMessage(&DoOpen, 4)     ! "Opened."
			else
			{
				list_count = 0
				for x in object
				{
					if x is not hidden
						list_count++
				}
				if list_count = 1
					parser_data[LAST_PARSER_STATUS] &= ~PRONOUNS_SET
				RLibMessage(&DoOpen,1) ! "opening the <blank> reveals"
				ListObjects(object)
				parser_data[LAST_PARSER_STATUS] |= PRONOUNS_SET
				skip_ahead = true
			}
		}
		else
			skip_ahead = true ! object.after has to list the contents
			                  ! if it exists

		if i and object is not quiet and
			not skip_ahead
		{
			print ""
			if children(object) = 1
				parser_data[LAST_PARSER_STATUS] &= ~PRONOUNS_SET
			tempformat = FORMAT
			FORMAT = FORMAT | NOINDENT_F
			list_nest = 0
			WhatsIn(object)
			FORMAT = tempformat
			parser_data[LAST_PARSER_STATUS] |= PRONOUNS_SET
		}
	}
	if light_check
		Perform(&DoLookAround)
	return true
}

replace DoClose
{
	if not CheckReach(object):  return false

	if object is not openable
	{
		VMessage(&DoClose, 1)            ! "You can't close that."
		return
	}
	elseif object is not open
		VMessage(&DoClose, 2)            ! "It's already closed."
	else
	{
		object is not open
		MakeMovedVisited(object)
		if not object.after
			VMessage(&DoClose, 3)    ! "Closed."

		if not FindLight(location)      ! in case the light source
						! has been concealed

			VMessage(&DoClose, 4)    ! "Everything goes dark."
	}
	return true
}

replace DoDrop
{
	if object is clothing and object is worn
		VMessage(&DoDrop, 1)     ! "You'll have to take it off first."

	! For the following, xobject is true only
	! when called by DoPutonGround:
	elseif player not in location and (parent(player) is container or
		parent(player) is platform) and not xobject
	{
		Perform(&DoPutIn, object, parent(player))
	}
	else
	{
		move object to location
		MakeMovedVisited(object)
		player.holding = player.holding - object.size
		if not object.after
			VMessage(&DoDrop, 2)     ! "Dropped."
	}
	xobject = 0
	return true
}

replace TheorThat(obj)
{
	if ObjectIsKnown(obj)
		print The(obj);
	else
		print "that";
}

routine MakeMovedVisited(obj)
{
	obj is moved
}

routine ObjectIsMovedVisited(obj)
{
	if obj is moved
		return true
	return false
}

routine MakeKnown(obj)
{
	obj is known
}

replace AddSpecialDesc(a)
{
	a is already_listed
#ifset MULTI_PCS
	MakeKnown(a)
#else
	a is known
#endif
	list_count++
	AssignPronoun(a)
}

#ifset USE_ATTACHABLES
replace Attachable_MoveAttached(att, obj, oldloc, newloc)
{
	local i, j, first

	if att.#attached_to
	{
		for (i=1; i<=att.#attached_to; i++)
		{
			j = att.attached_to #i

			if j
			{
				if Contains(oldloc, j) and j not in obj
				{
					if obj is not workflag
					{
						first = att
						obj is workflag
					}
					move j to newloc
					MakeMovedVisited(j)
					Attachable_MoveChildren(j, oldloc, newloc)
					Attachable_MoveAttachedTo(j, oldloc, newloc)
				}
			}
		}
	}
	return first
}


! Attachable_MoveAttachedTo scans <oldloc> for anything attached to <obj>, moving
! any valid matches to <newloc>.  Not particularly graceful, since it must
! recurse inward for each object, since attachments are explicit only from
! attachable to object, and not vice-versa.

replace Attachable_MoveAttachedTo(obj, oldloc, newloc)
{
	local i, j, k, first

	for i in oldloc
	{
		for (j=1; j<=i.#attached_to; j++)
		{
			if i.attached_to #j = obj or
				Contains(obj, i.attached_to #j):
			{
				move i to newloc
				MakeMovedVisited(j)
				if not first:  first = i
				k = Attachable_MoveAttached(i, obj, oldloc, newloc)
				if not first:  first = k
				k = Attachable_MoveChildren(i, obj, oldloc, newloc)
				if not first:  first = k
			}
		}

		if child(i)
		{
			k = Attachable_MoveAttachedTo(obj, child(i), newloc)
			if not first:  first = k
		}
	}
	return first
}
#endif ! USE_ATTACHABLES

