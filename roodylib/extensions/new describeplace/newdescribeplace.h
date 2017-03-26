

attribute already_printed
property describe_loc alias misc

property first_pass alias long_desc
property second_pass alias short_desc
property list_order alias n_to

object describeplace_settings
{
	in settings
	type settings
	describe_loc 0
	list_order ListParentofPlayerScenery \
	           ListCharsWithDescs \
	           ListCharsWithoutDescs \
	           ListObjsWithDescs \
	           ListObjsWithoutDescs \
	           ListAttachablesChildren \
	           HeldAttachables
}

replace Describeplace(place, long)
{
	local obj,  didprint, i
	local need_carriage

	if not place
		place = location
	describeplace_settings.describe_loc = place

	if AssignPronounsToRoom
		parser_data[LAST_PARSER_STATUS] &= ~PRONOUNS_SET

	! Since, for example, a room description following entering via
	! DoGo does not trigger before/after properties tied to looking
	! around:
	!
#ifclear NO_VERBS
	if verbroutine = &MovePlayer ! was "if verbroutine ~= &DoLookAround"
	{
#ifset MULTI_PCS
		if not ObjectIsMovedVisited(place) and verbosity ~= 1
#else
		if place is not visited and verbosity ~= 1
#endif
			return Perform(&DoLookAround)
		elseif long = true or verbosity = 2
			return Perform(&DoLookAround)
	}
#endif

	exit_type = 0  ! clear the exit_type global

	if not light_source
	{
		Message(&DescribePlace, 1)     ! "It's too dark to see..."
		return
	}

#ifset MULTI_PCS
	MakeKnown(place)
#else
	place is known
#endif

   ! Print the name of the location as a heading
	RLibMessage(&DescribePlace,1,place)

	override_indent = false

#ifset MULTI_PCS
	if not ObjectIsMovedVisited(place) and verbosity ~= 1
#else
	if place is not visited and verbosity ~= 1
#endif
	{
		if &place.initial_desc or &place.long_desc
		{
			didprint = true
			Indent
		}
		if not place.initial_desc
			run place.long_desc
	}
	elseif long = true or verbosity = 2
	{
		if &place.long_desc
		{
			Indent
			didprint = true
		}
#ifset NEW_ROOMS
		if &place.initial_desc and place.first_visit = counter and
			(parser_data[VERB_IS_XVERB] or word[1] = "undo")
			run place.initial_desc
		else
#endif
		run place.long_desc
	}
#ifset MULTI_PCS
	elseif not ObjectIsMovedVisited(place) and verbosity = 1
#else
	elseif place is not visited and verbosity = 1
#endif
	{
		if &place.initial_desc
		{
			Indent
			didprint = true
		}
		run place.initial_desc
	}

	if &place.list_contents and (FORMAT & DESCFORM_F) and didprint
		print ""        ! for double-space-after-heading formatting

	! A location may contain an overriding listing routine, as may any
	! parent, in the list_contents property
	!
	for obj in place
		obj is not already_printed

	for obj in describeplace_settings
		obj is not already_printed

	if not place.list_contents
	{
		list_nest = 0
		local a
		for (i = 1 ;i <= describeplace_settings.#list_order ;i++)
		{
			a = describeplace_settings.list_order #i
			need_carriage = a.first_pass
			if ((FORMAT & DESCFORM_F) and i=1 and
				(not (FORMAT & DESCFORM_D) or (FORMAT & LIST_F)))
			{
				for obj in place
				{
					if obj is not hidden and
						(player not in obj or children(obj) > 1)
					{
						print ""
						break
					}
				}
			}
			elseif (FORMAT & DESCFORM_D) and not (FORMAT & LIST_F)
			{
				if need_carriage and didprint
					""
			}

			if need_carriage
			{
				need_carriage = false
				didprint = a.second_pass
				a is already_printed
			}
		}
		print newline
		need_newline = false
	}
}

! routine for listing the player's siblings when he or she is in or on an
! object in the room; also lists children of scenery (hopefully already
! mentioned in the room's long_desc

object ListParentofPlayerScenery
{
	in describeplace_settings
	first_pass
	{
		local obj,ret, place
		place = describeplace_settings.describe_loc
		if player not in place and place = location
		{
			for obj in (Parent(player))
			{
				if obj is not hidden and obj ~= player
					return true
			}
		}
		for obj in place
		{
			if obj.type = scenery
			{
				if player not in obj and
				(obj is open or obj is not openable or obj is platform or
				obj is transparent) and obj is not quiet
				{
					local a
					obj is not already_listed
					for a in obj
					{
						if a is not hidden
							return true
					}
				}
			}
		}
		return false
	}
	second_pass
	{
		local obj, ret, place
		place = describeplace_settings.describe_loc
#ifset USE_RELATIVE_DESCRIPTIONS
		if RelativeParent(parent(player)) and children(parent(player)) > 1 and
			player not in place and place = location and
			not (FORMAT & LIST_F)
		{
			local tempformat
			tempformat = FORMAT
			FORMAT = FORMAT | FIRSTCAPITAL_F | ISORAREHERE_F
			DESCRIBEPLACEFORMAT = DESCRIBEPLACEFORMAT | OVERRIDEHERE_F
			list_nest = 2
			! called WhatsIn and not ListObjects so the parent's listcontents
			! property will be checked
			ret = WhatsIn(parent(player))
			FORMAT = tempformat
		}
		else
		{
#endif
			list_nest = 1
			if player not in place and place = location
				ret = WhatsIn(Parent(player))
#ifset USE_RELATIVE_DESCRIPTIONS
		}
#endif
		local ret2
		for obj in place
		{
			if obj.type = scenery
			{
				obj is known
				if player not in obj and
					((obj is container and ((obj is openable and obj is open)  or
					obj is not openable or obj is transparent))  or
					obj is platform) and obj is not quiet
				{
					if not (FORMAT & DESCFORM_D) or (FORMAT & LIST_F)
						print newline
					list_nest = 1
					if WhatsIn(obj)
						ret2 = true
				}
			}
			! For scenery-derived objects that may change the type
			elseif obj is static, hidden
#ifset MULTI_PCS
				MakeKnown(obj)
#else
				obj is known
#endif
		}
		print newline
		list_nest = 0
		return (ret or ret2)
	}
}

! routine for listing characters with short_desc descriptions
object ListCharsWithDescs
{
	in describeplace_settings
	first_pass
	{
		local obj,place
		place = describeplace_settings.describe_loc
		for obj in place
		{
			if (obj is not hidden and obj is living and
				 obj ~= player and ((&obj.short_desc and verbosity ~= 1) or
				(obj is not moved and &obj.initial_desc)) and
				obj is not already_printed )
			{
				return true
			}
		}
		return false
	}
	second_pass
	{
		local obj, place
		place = describeplace_settings.describe_loc
		! List all characters, if any
!		count = 0
		for obj in place
		{
			if obj is hidden or obj is not living or
				player in obj
			{
				obj is already_listed
			}
			else
			{
				obj is not already_listed
			}
		}
		local ret, count
		for obj in place
		{
			if obj is not already_listed
			{
				if count++ and (FORMAT & DESCFORM_D) and not (FORMAT & LIST_F)
					""
				if verbosity ~= 1 or (obj is not moved and &obj.initial_desc)
					ShortDescribe(obj)
				if obj is already_listed  ! did we print something?
				{
					ret = true
					obj is already_printed
				}
			}
		}
		return ret
	}
}

! routine for listing characters without short_desc descriptions
object ListCharsWithoutDescs
{
	in describeplace_settings
	first_pass
	{
		local obj,place
		place = describeplace_settings.describe_loc
		for obj in place
		{
			if (obj is not hidden and obj is living and
			obj ~= player and
			(not &obj.short_desc or
			(&obj.short_desc  and verbosity = 1)) and
			obj is not already_printed)
				return true
		}
		return false
	}
	second_pass
	{
		local tempformat, count, obj, place
		place = describeplace_settings.describe_loc
#ifset USE_RELATIVE_DESCRIPTIONS
		local also_check
		if not (FORMAT & LIST_F)
		{
			if ListObjsWithoutDescs is already_printed
				also_check = true
!			if IsYoungerThan(self, ListObjsWithoutDescs)
!				also_check = ListObjsWithoutDescs.first_pass
		}
#endif
		for obj in place
		{
			if (obj is not hidden and obj is living and
			obj ~= player and
			(not &obj.short_desc or
			(&obj.short_desc  and verbosity = 1)) and
			obj is not already_printed)
			{
				obj is not already_listed
				count++
			}
			else
				obj is already_listed
		}
		if count ! list_count      ! if characters are to be listed
		{
			list_count = count
			tempformat = FORMAT
			FORMAT = FORMAT | FIRSTCAPITAL_F | ISORAREHERE_F
#ifset USE_RELATIVE_DESCRIPTIONS
			if RelativeParent and not (FORMAT & LIST_F)
			{
				DESCRIBEPLACEFORMAT = DESCRIBEPLACEFORMAT | OVERRIDEHERE_F
				if also_check
					DESCRIBEPLACEFORMAT = DESCRIBEPLACEFORMAT | ALSO_F
			}
#endif
			Indent
			if list_count > 1
				FORMAT = FORMAT | USECHARNAMES_F
			if (FORMAT & LIST_F)
			{
				FORMAT = FORMAT & ~LIST_F       ! clear it
				FORMAT = FORMAT | TEMPLIST_F
			}
			list_nest = 0
			ListObjects(place)
			FORMAT = tempformat
			return true
		}
		return false
	}
}

! routine for listing objects with short_desc descriptions
object ListObjsWithDescs
{
	in describeplace_settings
	first_pass
	{
		local obj, ret, place
		place = describeplace_settings.describe_loc
		for obj in place
		{
	#ifset USE_ATTACHABLES
		! Exclude all attachables for now (and characters)
			if obj is not living and not (obj.type = attachable) and
				player not in obj and obj is not hidden and
				((verbosity ~= 1 and &obj.short_desc) or
	!			(&obj.initial_desc and verbosity = 1)) and
				(&obj.initial_desc and obj is not moved)) and
				obj is not already_printed
	#else
			if obj is not living and player not in obj and
			obj is not hidden and
			((verbosity ~= 1 and &obj.short_desc) or
	!			(&obj.initial_desc and verbosity = 1)) and
				(&obj.initial_desc and obj is not moved)) and
				obj is not already_printed
	#endif
			{
#ifset USE_PLURAL_OBJECTS
         ! ...And don't list identical objects yet, either

				if (obj.identical_to).type = identical_class
					obj is already_listed
				else
				{
#endif
					ret = true
					obj is not already_listed
#ifset USE_PLURAL_OBJECTS
				}
#endif
			}
			else
				obj is already_listed
		}
		return ret
	}
	second_pass
	{
		local obj, ret
		local count, place
		place = describeplace_settings.describe_loc
		for obj in place
		{
			if player not in obj
			{
				if obj is not already_listed and
					obj is not hidden and obj is not already_printed
				{
					if count++ and (FORMAT & DESCFORM_D) and not (FORMAT & LIST_F)
						""
					if verbosity ~= 1 or (verbosity = 1 and
						(obj is not moved and &obj.initial_desc))
						ShortDescribe(obj)
					if obj is already_listed ! did we print something?
					{
						ret = true
						obj is already_printed
					}
				}
			}
		}
		return ret
	}
}

! routine for listing objects without short_desc descriptions
object ListObjsWithoutDescs
{
	in describeplace_settings
	first_pass
	{
		local obj, place
		place = describeplace_settings.describe_loc
		for obj in place
		{
#ifset USE_ATTACHABLES
			! Exclude all attachables for now (and characters)

			if obj is not living and not (obj.type = attachable) and
				player not in obj and obj is not hidden and
				obj is not already_printed and
				(not &obj.short_desc or (&obj.short_desc and verbosity = 1))
#else
			if obj is not living and player not in obj and
			obj is not hidden and obj is not already_printed and
			(not &obj.short_desc or (&obj.short_desc and verbosity = 1))
#endif
			{
				return true
			}
		}
		return false
	}
	second_pass
	{
		local obj, tempformat, count, place
		place = describeplace_settings.describe_loc
#ifset USE_RELATIVE_DESCRIPTIONS
		local also_check, a, b, i
		if not (FORMAT & LIST_F)
		{
			if ListCharsWithoutDescs is already_printed
				also_check = true
!			if IsYoungerThan(self,ListCharsWithoutDescs)
!				also_check = ListCharsWithoutDescs.first_pass
		}
#endif
		for obj in place
		{
#ifset USE_ATTACHABLES
			! Exclude all attachables for now (and characters)

			if obj is living or (obj.type = attachable) or
				player in obj or (&obj.short_desc and verbosity ~= 1) or
				obj is already_printed or obj is hidden
#else
			if obj is living or player in obj or (&obj.short_desc and verbosity ~= 1) or obj is already_printed or obj is hidden
#endif
				obj is already_listed
			else
			{
				count++
				obj is not already_listed
			}
		}

		if count
		{
			list_count = count
			tempformat = FORMAT
			FORMAT = FORMAT | FIRSTCAPITAL_F | ISORAREHERE_F
			Indent
			if (FORMAT & LIST_F)
			{
				FORMAT = FORMAT & ~LIST_F       ! clear it
				FORMAT = FORMAT | TEMPLIST_F
			}
#ifset USE_RELATIVE_DESCRIPTIONS
			if RelativeParent and not (FORMAT & TEMPLIST_F)
			{
				DESCRIBEPLACEFORMAT = DESCRIBEPLACEFORMAT | OVERRIDEHERE_F
				if also_check
					DESCRIBEPLACEFORMAT = DESCRIBEPLACEFORMAT | ALSO_F
			}
#endif
			list_nest = 0
			ListObjects(place)
			FORMAT = tempformat
			return true
		}
		return false
	}
}

! routine for listing attachables and contents of scenery objects
object ListAttachablesChildren
{
	in describeplace_settings
#ifclear NO_OBJLIB
#ifset USE_ATTACHABLES
	first_pass
	{
		local obj, ret, place
		place = describeplace_settings.describe_loc
		for obj in place
		{
			! Print attachables last
			if obj.type = attachable and obj is not hidden and
				obj is not already_printed
			{
				obj is not already_listed
				ret = true
			}
		}
		return ret
	}
	second_pass
	{
		local obj, ret, count, place
		place = describeplace_settings.describe_loc
		for obj in place
		{
			! Print attachables last
			if obj.type = attachable and obj is not hidden and
				obj is not already_printed
			{
				if count++ and (FORMAT & DESCFORM_D) and not (FORMAT & LIST_F)
					""
				ShortDescribe(obj)
				if obj is not already_listed
					Message(&DescribePlace, 2, obj)
				obj is already_printed
				ret = true
			}
		}

		print newline
		override_indent = false
		return ret
	}
#endif  ! ifset USE_ATTACHABLES
#endif  ! ifclear NO_OBJLIB
}

object HeldAttachables
{
	in describeplace_settings
#ifset USE_ATTACHABLES
	first_pass
	{
		local i,a,no_good, good, place
		place = describeplace_settings.describe_loc
		for i in player
		{
			no_good = 0
			good = 0
			if i.type = attachable
			{
				if Inlist(i,attached_to,player)
					no_good = true
				if not no_good
				{
					for (a=1; a<=i.#attached_to; a++)
					{
						if i.attached_to #a
						{
							if not Contains(player,i.attached_to #a)
							{
								if FindObject(i.attached_to #a,place)
								{
									good = true
									break
								}
							}
						}
					}
					if good
						return true
				}
			}
		}
		return false
	}
	second_pass
	{
		local i,a,no_good, good, count, place
		place = describeplace_settings.describe_loc
		for i in player
		{
			no_good = 0
			good = 0
			if i.type = attachable
			{
				if Inlist(i,attached_to,player)
					no_good = true
				if not no_good
				{
					for (a=1; a<=i.#attached_to; a++)
					{
						if i.attached_to #a
						{
							if not Contains(player,i.attached_to #a)
							{
								if FindObject(i.attached_to #a,place)
								{
									good = true
									break
								}
							}
						}
					}
					if good
					{
						if count++
							print AFTER_PERIOD;
						else
							Indent
						RLibMessage(&DescribePlace,2, i) ! "The <blank> you are
																	  !  holding is tied to the
																	  !  <blank>."
					}
				}
			}
		}
		print newline
		return count
	}
#endif
}

replace WhatsIn(obj,dont_flush)
{
	local i, totallisted
	local initial_list_nest

	if (FORMAT & NORECURSE_F)
		return

	for i in obj
	{
		if not dont_flush
			i is not already_listed
		if i is not hidden and i is not already_listed
			totallisted++
	}

	if totallisted = 0
	{
		print newline
		return
	}
	list_count = totallisted

	if obj is not container or obj is platform or
		(obj is container and (obj is not openable or
			(obj is openable and
				(obj is open or obj is transparent)))) and
		(obj ~= player or (FORMAT & INVENTORY_F)) and obj is not quiet
	{
		SpecialDesc(obj)

		! If list_count is 0 now, but totallisted was not, something must have been
		! printed by SpecialDesc

		if dont_flush and list_count
		{
			list_count = 0
			for i in obj
			{
				if i is not hidden and i is not already_listed
					list_count++
			}
		}

		if list_count = 0
		{
			if (FORMAT & INVENTORY_F) and not (FORMAT & LIST_F) and
				list_nest = 0
			{
				print newline    ! was ' print "" '
			}
			return totallisted
		}

		if need_newline and (FORMAT & DESCFORM_D) and
			not (FORMAT & LIST_F)
			""
		if obj.list_contents
			return totallisted

		initial_list_nest = list_nest

		if (FORMAT & LIST_F)
		{
			if list_nest
			{
				! Indent the first time so that it lines up with
				! paragraph indentation:
				i = list_nest
				if (FORMAT & NOINDENT_F) and
				verbroutine ~= &DoInventory, &DoLook, &DoLookIn
				{
					i--
				}
				print to (i * 2);	! INDENT_SIZE);
			}
		}
		else
			Indent

		if obj.contains_desc    ! custom heading
		{
			if (FORMAT & LIST_F)
				RLibMessage(&WhatsIn, 1 )  !  ":"
#ifset USE_RELATIVE_DESCRIPTIONS
			FORMAT = FORMAT & ~FIRSTCAPITAL_F
			FORMAT = FORMAT & ~ISORAREHERE_F
			DESCRIBEPLACEFORMAT = DESCRIBEPLACEFORMAT & ~ OVERRIDEHERE_F
#endif
		}
		else
		{
			if obj = player
			{
				if (FORMAT & LIST_F) and list_count < totallisted
					print "\n";

				! "You are carrying..."
				Message(&WhatsIn, 1, totallisted)

				if (FORMAT & LIST_F)
					RLibMessage(&WhatsIn, 1 )  !  ":"
			}
			elseif obj is living and not obj.prep
			{
				! "X has..."
				Message(&WhatsIn, 2, obj, totallisted)
				if (FORMAT & LIST_F)
					RLibMessage(&WhatsIn, 1 )  !  ":"
			}
#ifset USE_RELATIVE_DESCRIPTIONS
			elseif not (DESCRIBEPLACEFORMAT & OVERRIDEHERE_F)
#else
			else
#endif
			{
				if list_count < totallisted
					! "Also sitting on/in..."
					RLibMessage(&WhatsIn, 2, obj )
				else
					! "Sitting on/in..."
					RLibMessage(&WhatsIn, 3, obj )
				FORMAT = FORMAT | ISORARE_F
			}
		}

		ListObjects(obj)
		for i in obj
		{
			i is already_listed
		}
		list_nest = initial_list_nest
	}
	return totallisted
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
				if (FORMAT & DESCFORM_D) and d > 1
					""
				elseif not (FORMAT & DESCFORM_D)
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
				if (FORMAT & DESCFORM_D)
					""
				WhatsIn(a)
				list_count = 0
			}
		}
	}
}

! Roody's note: Jump command removed. in superbrief mode, objects with
! short_descs treated like objects without them.
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
		local x
		if (FORMAT & DESCFORM_D) and not (FORMAT & LIST_F)
		{
			for x in obj
			{
				if x is not hidden
					break
			}
			if x
				""
		}
		list_nest = (LIST_F = (FORMAT & LIST_F))
		WhatsIn(obj)
	}

	! If INDENT_SIZE is 0, formatting may be thrown off when listing
	! the contents of an object:
	if INDENT_SIZE = 0:  need_newline = true

	if need_newline:  print newline
}