!\-----------------------------------------------------------------------
pasttense.h version 1.5 by Roody Yogurt
set the pasttense global to true to make various verb and parser messages
past tense. you may prefer fewer or more past tense responses. go wild
with it.
Changelog:
1.5 - added more Roodylib compatibility
1.4 - fixed DarkWarning
1.3 - added Use_EXTENSION_CREDITING stuff
1.1- decided ListObjects needed replacing
-----------------------------------------------------------------------\!

#ifclear _PASTTENSE_H
#set _PASTTENSE_H

#ifset VERSIONS
#message "PastTense.h Version 1.5"
#endif

#ifset USE_EXTENSION_CREDITING
#ifclear _ROODYLIB_H
#message error "Extension crediting requires \"roodylib.h\". Be sure to include it first!"
#endif
version_obj pasttense_version "PastTense Version 1.5"
{
	in included_extensions
	desc_detail
		" by Roody Yogurt";
}
#endif

#ifset _NEWCONVERSE_H
#message error "pasttense.h must be included BEFORE newconverse.h!"
#endif

constant WAS_WORD        "was"
constant WERE_WORD        "were"
constant THERE_WORD	"there"

global pasttense

replace DarkWarning
{
	print CThe(player); " stumble";
	if not pasttense
		MatchSubject(player)
	else
		print "d";
	" around in the dark."
}

routine WasorWere(obj, a)
{
	a = true
	if obj is plural
	{
		if obj = player and player_person = 1
		{
			if a:  print " was";
			else:  print "'m";
		}
		else
		{
			if player_person = 3:  a = true
			if a:  print " were";
			else:  print "'re";
		}
	}
	else
	{
		if a:  print " was";
		else:  print "'s";
	}
}

!----------------------------------------------------------------------------
! VERB LIBRARY MESSAGES
!
! VMessage and NewVMessages mirror HUGOLIB.H's Message and NewMessages,
! respectively.
!
! VMessage(&routine, number)
! prints message <number> for <routine>
!
! VMessage (&routine, number, var1, var2)
! where one or two variables--which may be objects or any other value--
! are used by message <number> for <routine>
replace NewVMessages(r, num, a, b)      ! The NewVMessages routine may be
{                                       ! replaced, and should return true

if not pasttense
	return false                    ! if a replacement message <num>
                                        ! exists for routine <r>
	select r
	case &DoVague:
	{
		print "Be a little more specific about what you wanted ";
		if player_person ~= 2
			print The(player, true); " ";
		print "to "; VerbWord; "."
	}

	case &DoLook
	{
		select num
		case 1:  {"It was too dark to see anything."}
		case 2
		{
			print CThe(object);
#ifclear NO_OBJLIB
			if object = u_obj, d_obj
				print " "; The(player);
#endif
		print " looked just like "; The(player);
		" expected."
		}
	}

	case &DoLookIn
	{
		select num
		case 1:  {print CThe(object); WasorWere(object, true); " closed."}
		case 2:  {print CThe(object); WasorWere(object, true); " empty."}
	}

	case &DoLookThrough
	{
		select num
		case 1:  {print "Through "; Art(object); " "; The(player); " could see ";}
		case 2:  {print CThe(player); " could't see through that."}
	}

	case &DoLookUnder: {  print CThe(player);  " didn't";
				print " find anything "; word[2]; " "; \
				The(object); "."
				}

	case &DoWait:	{"Time passed..."}

	case &DoWaitforChar
	{
		select num
		case 1:
		{
			print CThe(object); WasorWere(object, true);
				" right there.";
			if player_person = 2:  print "  Good detective work."
			print newline
		}
		case 2
		{
			print "\n("; CThe(object);
			if object  is plural
				" had";
				" arrived.";
			if STATUSTYPE = 2
				{
				print "  It was now ";
				print HoursMinutes(counter);
				}
			print ")"
		}
		case 3
		{
			print "\n("; CThe(player); " waited ";
			select STATUSTYPE
				case 2
					print "an hour";
				case else
					print "for twenty turns";
			print ", and "; The(object); " still hadn't arrived.)"
		}
	}
case &DoWaitUntil
	{
		select num
		case 1:  {print "Wow.  Time flew."}
		case 2:
		{
			if player_person = 2
				print CThe(player); WasorWere(player); \
					" ahead of your time."
			else
				print "You were getting ahead of "; \
					The(player, true); "."
		}
		case 3
		{
			print ""
			select STATUSTYPE
			case 2: print "It was now "; HoursMinutes(counter)
			case else: print "It was now turn "; number object; "."
		}
	}

	case &KeepWaiting:
	{
		print "\nDid you want ";
		if player_person ~= 2:  print The(player, true); " ";
		print "to keep waiting (YES or NO)? ";
	}

! no real good way to past-tense-ize this next one
!\	case &DoTalk:
	{
		print "Try ";
		if player_person ~= 2
			print "having "; The(player, true); " ask or tell";
		else:  print "asking or telling";
		print " someone about something in particular."
	} \!

	case &DoAsk
	{
		select num
		case 1:
		{
			print "Maybe "; The(player) ; " should have asked
			about something in particular."
		}
		case 2:  {print "Talking to "; player.pronoun #4; " wasn't a
			big step forward."}
		case 3:  {print "Hopefully "; The(player); " knew"; \
			" as much as anyone."}
		case 4
		{
			print CThe(object); " wouldn't talk about "; object.pronoun #4; "."

		}
		case 5
		{
			print CThe(object); " glanced";  \
				" toward "; The(xobject); ".  "; \
				capital object.pronoun; " wouldn't
				say anything with "; \
				The(xobject); " right there."

		}
		case 6
		{
			print CThe(object); \
				" didn't";
				" seem to know anything about ";
			if xobject is living
				The(xobject)
			else
				print "that";
			print "."

		}
	}
	case &DoAskQuestion
	{
		if player_person = 1 or player is not plural
			print CThe(player); " wasn't";
		else
			print CThe(player); " weren't";
		" talking to anybody."

	}

	case &DoTell
	{
		select num
		case 1:
		{
			print "Maybe "; The(player) ; " should have told ";
			print The(object); " about something in particular."

		}
		case 2
		{
			 print CThe(object); \
				" didn't"; " seem
				particularly interested in what "; The(player);
				print " had"; " to say about ";
			if xobject is living
				The(xobject)
			else
				print "that";
			print "."

		}
	}

	case &DoListen
	{
		select num
		case 1:
		{
			print "Be a little more specific about exactly what
				you wanted ";
			if player_person ~= 2:  print The(player, true); " ";
			print "to listen to."

		}
		case 2:  {print CThe(object); WasorWere(object, true); " not
				making a sound."}
	}

	case &DoGo
	{
		select num
		case 1:  {print CThe(obstacle); " stopped"; \
			" you from going anywhere."}
		case 2:  {print CThe(player); " couldn't go that way."}
		case 3
		{
			print CThe(player); " would have to get ";
			if parent(player).prep #2
				print parent(player).prep #2; " ";
			elseif parent(player) is platform
				print "off ";
			else
				print "out ";
			print "of "; The(parent(player)); " first."

		}
		case 4
		{
			print "Which way did you want ";
			if player_person ~= 2:  print The(player, true); " ";
			"to go?"

		}
	}

	case &DoEnter
	{
		select num
		case 1:
		{
			print "Be a little more specific about where you'd
				like ";
			if player_person ~= 2:  print The(player, true); " ";
			print "to "; VerbWord; "."

		}
		case 2:
		{
			print CThe(player); " couldn't ";
			if object is platform
				print "get on ";
			else
				print "enter ";
			print The(object); "."

		}
		case 3: { print "A bit redundant, don't you think?"}
		case 4
		{
			print CThe(player); " got ";
			if object.prep
				print object.prep;
			elseif object is platform
				print "onto";
			else
				print "into";
			print " "; The(object); "."

		}
	}

	case &DoExit
	{
		select num
		case 1
		{
			print CThe(player); WasorWere(player); " not ";
			if object.prep
				print object.prep;
			elseif object is platform
				print "on";
			else
				print "in";
			print " "; The(object); "."

		}
		case 2
		{
			print CThe(player); " got ";
			if object.prep #2
				print object.prep #2;
			elseif object is platform
				print "off";
			else
				print "out";
			print " of "; The(object); "."

		}
	}

	case &DoInventory : {print CThe(player); WasorWere(player); " not carrying anything." }

	case &DoClose
	{
		select num
		case 1:  {print CThe(player); " couldn't close "; The(object); "."}
		case 2:  {print CThe(object); WasorWere(object, true); " already closed." }
		case 3:  {print "Closed." }
		case 4:  {print "\nEverything went dark." }

	}

	case &DoOpen
	{
		select num
		case 1:  {print CThe(player); " couldn't open "; The(object); "." }
		case 2:  {print CThe(object); WasorWere(object, true); " already open." }
		case 3:  {print CThe(object); WasorWere(object, true); " locked." }
		case 4:  {print "Opened." }
	}

	case &DoMove:  {print CThe(player); " couldn't move "; The(object); "." }

	case &DoGet
	{
		select num
		case 1:  {print CThe(player); " already"; \
			" had"; " that." }
		case 2:  {print "This was not progress "; The(player); \
			WasorWere(player, true); " making." }
		case 3:  {print CThe(object); " wasn't too big
				on that idea." }
		case 4
		{
			print CThe(parent(object)); \
				" didn't"; \
				" want to give "; The(object); " to "; \
				The(player, true); "."

		}
		case 5:  {print CThe(parent(object)); \
			WasOrWere(parent(object), true); " closed." }
		case 6
		{
			print CThe(player); " couldn't take "; The(object); \
				" while "; player.pronoun; WasorWere(player); \
				" ";
			if object.prep:  print object.prep;
			elseif object is platform:  print "in";
			else:  print "on";
			if object is plural:  print " them."
			else:  print " it."

		}
		case 7
		{
			print CThe(player); " couldn't take";
			MatchPlural(object, "that", "those")
			print "."

		}
		case 8:  {
			print CThe(player); "took "; The(object);"."

			}
		case 9:  {
			print CThe(player); WasorWere(player); " carrying
			too much to take that."

			}
	}

	case &ImplicitTake
	{
		print "(having taken "; The(object); " first)"

	}

	case &DoDrop
	{
		select num
		case 1:  print CThe(player); " would have to take "; \
			The(object); " off first."
		case 2:  print "Dropped."

	}

	case &DoPutonGround
	{
		print CThe(player); WasorWere(player); " not ";
		if xobject.prep
			print xobject.prep;
		else
			print "in";
		print " "; The(xobject); "."

	}

	case &DoPutIn
	{
		select num
		case 1:
		{
			print "You'll have to be a little more specific
				about exactly with what you wanted ";
			if player_person ~= 2:  print The(player, true); " ";
			print "to do that."
		}
		case 2:  print CThe(xobject); WasorWere(xobject, true); \
				" closed."
		case 3:  print CThe(player); " couldn't do that."
		case 4:  print "Trying to bend the laws of physics, were we?"
		case 5
		{
			print CThe(player); " put";\
				" "; The(object); " ";
			if xobject.prep
				print xobject.prep;
			elseif xobject is platform
				print "on";
			else
				print "in";
			print " "; The(xobject); "."
		}
		case 6
		{
			print "There was no room ";
			if xobject.prep
				print xobject.prep; " ";
			elseif xobject is platform
				print "on ";
			else
				print "in ";
			print The(xobject); "."
		}
		case 7:  print CThe(player); " could't do that with "; \
			The(xobject); "."
		case 8
		{
			print CThe(object); WasorWere(object, true); " already ";
			if xobject.prep
				print xobject.prep;
			elseif xobject is platform
				"on";
			else
				"in";
			print " "; The(xobject); "."
		}

	}

	case &DoEmpty
	{
		select num
		case 1
		{
			print CThe(object); WasorWere(object, true); \
				" closed."
		}
		case 2:  print CThe(object); WasorWere(object, true); \
				" already empty."
		case 3:  print CThe(player); " can't move that."

	}

	case &DoGive
	{
		select num
		case 1:
		{
			print "Maybe "; The(player) ; " should have given
			something in particular."
		}
		case 2:
		{
			print "Not quite sure what you hoped ";
			if player_person ~= 2:  print The(player); " would";
			else:  print "to";
			print " accomplish with that."
		}
		case 3:
		{
			print "The kind gesture went unnoticed by "; \
				The(xobject); "."
		}
		case 4
		{
			print CThe(xobject);
			if xobject is not unfriendly
				print " politely";
			print " refused";  "."
		}
		case 5:  print "(having taken "; The(object); " first)"

	}

	case &DoShow
	{
		select num
		case 1:
		{
			print "Maybe "; The(player) ; " should have shown ";
			print The(object); " something in particular."
		}
		case 2:  print "Showing "; Art(object); " to "; The(xobject); \
				" didn't accomplish a whole lot."
		case 3:  print CThe(xobject); \
				" didn't"; \
				" seem too interested in "; \
				The(object); "."

	}

	case &DoWear
	{
		select num
		case 1:  print CThe(player); " couldn't wear "; The(object); "."
		case 2:  print CThe(player); WasorWere(player); " already
			wearing that."
		case 3:  print CThe(player); " put";  \
			" on "; The(object); "."

	}

	case &DoTakeOff
	{
		select num
		case 1:  print CThe(player); " couldn't do that with "; \
			The(object); "."
		case 2:  print CThe(player); WasorWere(player); " not
			wearing that."
		case 3:  print CThe(player); " took"; \
			" off "; The(object); "."

	}

	case &DoSwitchOn
	{
		select num
		case 1:  print CThe(object); WasorWere(object); " already turned on."
		case 2:  print CThe(player); " switched "; The(object); " on."

	}

	case &DoSwitchOff
	{
		select num
		case 1:  print CThe(object); WasorWere(object); " already turned off."
		case 2:  print CThe(player); " switched "; The(object); " off."

	}

	case &DoUnlock
	{
		select num
		case 1:  print "That didn't seem to do the trick."
		case 2:  print CThe(player); WasorWere(player); " not holding
			any key that fit."
		case 3:  print CThe(object); WasorWere(object); " already unlocked."
		case 4
		{
			if not xobject and object.key_object
				print "(with "; The(object.key_object); ")"
			print "Unlocked."
		}

	}

	case &DoLock
	{
		select num
		case 1:  print CThe(object); WasorWere(object); " already locked."
		case 2:  print CThe(player); " would have to close "; \
			object.pronoun; " first."
		case 3
		{
			if not xobject and object.key_object
				print "(with "; The(object.key_object); ")"
			print "Locked."
		}

	}

	case &DoEat:  {print CThe(player); " couldn't eat "; The(object); "." }

	case &DoDrink: {print CThe(player); " couldn't drink "; The(object); "." }

	case &DoHit:  {print "Venting "; player.pronoun #3; " frustrations
			on "; The(object); " wouldn't have accomplished much." }

	case &DoScore:   {"Nobody kept score back then." }

	case else : return false

	return true
}

!----------------------------------------------------------------------------
! Message(&routine, number)
! prints message <number> for <routine>
!
! Message (&routine, number, var1, var2)
! where one or two variables--which may be objects or any other value--
! are used by message <number> for <routine>

replace NewMessages(r, num, a, b)
{


	if not pasttense
		return false

	select r

	case &Parse
	{
		select num
		case 1:  print CThe(player); \
				" didn't"; \
				" need to refer to that."
		case 2:  print "(assuming you meant";
		case 3:  print ", respectively";


	}

	case &Speakto
	{
		if a = 0:  a = object   ! Speakto messages are sometimes
					! co-opted by verb routines
		select num
		case 1:  print CThe(player); " decided against spending
				time talking to "; player.pronoun #4; "."
		case 2:  print CThe(a); WasorWere(a, true); " listening."
		case 3:  print CThe(a); " nodded"; \
				" hello to "; The(player, true); "."
		case 4:  print CThe(a); " ignored"; " "; \
				The(player, true); "."

	}

	case &DescribePlace
	{
		select num
		case 1:  print "It was too dark to see anything."
		case 2:  print "There"; WasorWere(a, true); " "; Art(a); \
				" there."

	}

	case &WhatsIn
	{
		select num
		case 1
		{
			print CThe(player); WasorWere(player, true); " ";
			if list_count < a
				print "also ";
			print "carrying";
		}
		case 2
		{
			if FORMAT & USECHARNAMES_F
				CThe(a)
			else
				print capital a.pronoun;
			if list_count < b
				print " also";
			" had";
		}
		case 3
		{
			print "Also ";
			if a.prep
				print a.prep; " ";
			elseif a is platform
				print "sitting on ";
			else
				print "inside ";
		}
		case 4
		{
			if a.prep
				print capital a.prep; " ";
			elseif a is platform
				print "Sitting on ";
			else
				print "Inside ";
		}

	}

	case &CheckReach
	{
		select num
		case 1
		{
			print "Except that "; The(parent(a)); \
				" had"; \
				" "; The(a); "."
		}
		case 2
		{
			print "Except that "; The(parent(a)); \
				" didn't"; \
				" want to give "; The(player, true); \
				" "; The(a); "."
		}
		case 3:  print CThe(player); " couldn't reach "; The(a); \
				" from "; The(parent(player)); "."

	}

#ifclear NO_SCRIPTS
#ifclear NO_OBJLIB
	case &CharMove
	{
		select num
		case 1
		{
			print "\n"; CThe(a); " headed ";
			if b = u_obj or b = d_obj
				print b.name; "ward."
			else
			{
				print "off to the ";
				print b.name; "."
			}
		}
		case 2
		{
			print "\n"; CThe(a); " arrived"; \
				" from ";
			if b ~= u_obj and b ~= d_obj
				print "the "; (b.dir_from).name; "."
			elseif b = u_obj
				print "below."
			else
				print "above."
		}

	}
#endif  ! ifclear NO_OBJLIB

	case &CharGet:  {print "\n"; CThe(a); " picked"; \
			" up "; The(b); "."}

	case &CharDrop:  {print "\n"; CThe(a); " put"; \
			" down "; The(b); "."}
#endif  ! ifclear NO_SCRIPTS
	case else : return false
	return true
}

replace NewOMessages(obj, num, a, b)
{
	select obj

	case self_class
	{
		select num
		! seems to be a parser response and should not be affected by tense
		case 1
		{
			print "\""; capital self.name;
			print "\" isn't quite appropriate for ";
			print The(object); "."
		}
	}
	case direction
	{
		select num
		case 1
		{
			print CThe(player);
			if pasttense
			{
				print " saw ";
			}
			else
			{
				print " see"; MatchPlural(player);
			}
			"nothing special ";
			if self.article
				"to ";
			print The(self);
			if not self.article
				print " "; player.pronoun #2;
			print "."
		}
		case 2
		{
			"You'll have to be a little more specific about
			what you'd like";
			if player_person ~= 2
				print " "; The(player);
			" to look ";
			print self.name; "."
		}
	}
	case door
	{
		select num
		case 1
		{
			print CThe(player);
			if pasttense
			{
				print " would ";
			}
			else
				print " will ";
			print "have to get ";
			if parent(player).prep #2
				print parent(player).prep #2; " ";
			else
				print "out ";
			print "of "; The(parent(player)); " first."
		}
		case 2:  print "(opening "; The(self); " first)"
		case 3:  print CThe(self); IsorAre(self, true); " locked."
		case 4:  print CThe(self); IsorAre(self, true); " closed."
		case 5
		{
			CThe(actor)
			print " open"; MatchSubject(actor); " "; \
			The(self); " and ";

			if actor in location
			{
				if not pasttense
				{
					print "leave"; MatchSubject(actor);
				}
				else
					print "left";
			}
			elseif location = self.between #((parent(actor) = \
				self.between #1) + 1)
			{
				if not pasttense
				{
					print "come"; MatchSubject(actor); " in";
				}
				else
					print "came in";
			}
			print ", closing it behind ";
			if actor is not plural or actor = player
				print actor.pronoun #4; "."
			else
				print "themselves."
		}
		case 6
		{
			print CThe(self);
			if not pasttense
			{
				IsorAre(self, true)
			}
			else
				WasorWere(self, true)
			" open."
		}
		case 7
		{
			print CThe(self);
			if not pasttense
			{
				IsorAre(self, true)
			}
			else
				WasorWere(self, true)
			" closed."
		}
	}

	case component
	{
		print CThe(player);
		if not pasttense
			print " can't ";
		else
			print " couldn't ";
		print "separate "; \
				The(self); " from "; The(self.part_of); "."
	}

#ifset USE_VEHICLES
	case vehicle
	{
		select num
		case 1
		{
			 print "To walk, "; The(player);
			 if not pasttense
				" will ";
			else
				" would ";
			print "have to
				get "; self.prep #2; " of "; The(self); \
				".  Otherwise, try \""; self.vehicle_verbs; \
				"\" ";
			 if self.#vehicle_verbs > 1
				print "or \""; self.vehicle_verbs #2; "\" ";
			 print "and a direction."
		}
		case 2
		{
			print CThe(player);
			if not pasttense
				IsorAre(player)
			else
				WasorWere(player)
			" not in anything at the moment."
		}
		case 3
		{
			print CThe(player);
			if not pasttense
				IsorAre(player)
			else
				WasorWere(player)
			print " "; \
			a.prep; " "; The(a);"--try \""; a.vehicle_verbs; "\"."
		}
		case 4:  print "You'll have to specify a direction as well."
		case 5
		{
			print CArt(obstacle); " stop";
			if pasttense
			{
				print "ped";
			}
			else
				MatchSubject(obstacle)
			print " "; The(player, true); " from going anywhere."
		}
		case 6
		{
			print CThe(player);
			if not pasttense
				print " can't ";
			else
				print " couldn't ";
			print a.vehicle_verbs; " ";
			if object.prep
				print object.prep;
			else
				print "in";
			print " "; The(object); "."
		}
		case 7
		{
			print CThe(player);
			if not pasttense
				print " can't ";
			else
				print " couldn't ";
			print a.vehicle_verbs; \
			" that way."
		}
	}
#endif  ! ifset USE_VEHICLES

#ifset USE_PLURAL_OBJECTS
	case plural_class
	{
		select num
		case 1
		{
			if pluralobj_heldmode = 1
			{
				print CThe(player);
				if not pasttense
					MatchPlural(player, "doesn't", "don't")
				else
					print " didn't";
				print " have any ";
			}
			else
			{
				if not pasttense
					print "There are no ";
				else
					print "There were no ";
			}
			print self.name; " to "; VerbWord; "."
		}
		case 2
		{
			if pluralobj_heldmode = 1
			{
				print CThe(player); " only";
				if not pasttense
					MatchPlural(player, "has", "have")
				else
					print " had";
				print " ";
			}
			else
			{
				print "There ";
				if a = 1
				{
					if not pasttense
						print "is";
					else
						print "was";
				}
				else
				{
					if not pasttense
						print "are";
					else
						print "were";
				}
				print " only ";
			}
			print NumberWord(a); " ";
			if a > 1
				print self.noun;
			else
				print self.single_noun;
			print " to "; VerbWord; "."
		}
		case 3
		{
			print CThe(player);
			if not pasttense
				print " will";
			else
				 print " would";
			print "have to do that one "; \
				self.single_noun; " at a time."
		}
	}
#endif  ! ifset USE_PLURAL_OBJECTS

#ifset USE_ATTACHABLES
	case attachable
	{
		select num
		case 1
		{
			print CArt(self);
			if not pasttense
				IsorAre(self, true)
			else
				WasorWere(self,true)
			print " "; \
				self.attached_desc; " ";
			ListAttachments(self)
			print "."
		}
		case 2
		{
			print CThe(player);
			if not pasttense
				print " see";
			else
				print " saw";
			MatchSubject(player)
			print " nothing special about "; The(self); "."
		}
		case 3
		{
			print CThe(self);
			if not pasttense
				IsorAre(self, true)
			else
				WasorWere(self,true)
			print " "; \
				self.attached_desc; " ";
			ListAttachments(self)
			print "."
		}
		case 4
		{
			print CThe(player);
			if not pasttense
				print " will";
			else
				print " would";
			print " have to "; \
			self.detach_verb; " before you can take it."
		}
		case 5:  print "(having to "; self.detach_verb; " "; \
			The(self); " "; self.detach_prep; " "; \
			The(self.attached_to #a); " first)"
		case 6
		{
			print CThe(player);
			if not pasttense
				print " can't ";
			else
				print " couldn't ";
			VerbWord
			print " "; \
			The(object); "."
		}
		case 7:  print "You might want to try \""; a; "\" with "; \
			The(object); " instead of "; "\""; VerbWord; "\"."
		case 8:
		{
			print "You'll have to be a little more specific
				about what you'd like ";
			if player_person ~= 2:  print The(player, true); " ";
			print "to "; object.attach_verb; " "; \
			The(object); " "; object.attach_prep; "."
		}
		case 9:  print "(taking "; The(object); " first)"
		case 10
		{
			print CThe(player);
			if not pasttense
				print " can't ";
			else
				print " couldn't ";
			VerbWord
			print " "; \
			The(object); " "; object.attach_prep; " "; \
			The(xobject); "."
		}
		case 11
		{
			print "Except that "; The(object);
			if not pasttense
				IsorAre(object, true)
			else
				WasorWere(object, true)
			print " already "; \
			object.attached_desc; " "; The(xobject); "."
		}
		case 12
		{
			print "Except that "; The(object);
			if not pasttense
				print " is ";
			else
				print " was ";
			print "already "; \
			object.attached_desc; " ";
			ListAttachments(object)
			print "."
		}
		case 13:  print CThe(player); " "; object.attach_verb; \
			MatchSubject(player); " "; The(object); " "; \
			object.attach_prep; " "; The(xobject); "."
		case 14
		{
			print "Except that "; The(object);
			if not pasttense
				IsorAre(object, true)
			else
				WasorWere(object, true)
			print " not "; \
			object.attached_desc; " "; The(xobject); "."
		}
		case 15
		{
			print "Except that "; The(object);
			if not pasttense
				IsorAre(object, true)
			else
				WasorWere(object, true)
			print " not "; \
			object.attached_desc; " anything."
		}
		case 16:
		{
			print "You'll have to be a little more specific
				about what you'd like ";
			if player_person ~= 2:  print The(player, true); " ";
			print "to "; VerbWord; " "; The(object); " "; \
				object.detach_prep; "."
		}
		case 17:  print CThe(player); " "; object.detach_verb; \
			MatchSubject(player); " "; The(object); " "; \
			object.detach_prep; " "; The(xobject); "."
		case 18
		{
			print CThe(player);
			if not pasttense
				print " can't ";
			else
				print " couldn't ";
			print "go anywhere while "; \
				The(a); IsorAre(a, true); " still "; \
				a.attached_desc; " ";
			ListAttachments(a)
			print "."
		}
		case 19
		{
			print CThe(player);
			if not pasttense
				print " can't";
			else
				print " couldn't";
			print "move whatever is "; \
			a.attached_desc; " "; The(a); ", namely ";
			ListAttachments(a)
			print "."
		}
		case 20
		{
			print CThe(player);
			if not pasttense
				print " can't";
			else
				print " couldn't";
			print " pull "; The(a); \
			" that way."
		}
		case 21
		{
			print "(with "; The(a); " "; a.attached_desc; " ";
			ListAttachments(a)
			print ")"
		}
		case 22
		{
			print " "; The(player);
			if not pasttense
				IsorAre(player)
			else
				WasorWere(player)
			print " holding";
		}
	}
#endif  ! ifset USE_ATTACHABLES
	case else : return false

	return true ! this line is only reached if we replaced something
}

replace NewParseError(errornumber, obj)
{

	if not pasttense
		return false
	if errornumber >= 100
	{
		CustomError(errornumber, obj)
		word[1] = ""            ! force ParseError(0)
		words = 0
		customerror_flag = true
		parser_data[PARSER_STATUS] = PARSER_RESET
		return true
	}
	if customerror_flag
	{
		customerror_flag = 0    ! CustomError already printed error
		parser_data[PARSER_STATUS] = PARSER_RESET
		return true
	}

#ifset DEBUG
	if debug_flags & D_PARSE
	{
		Font(BOLD_ON)
		print "[ParseError("; number errornumber; ", "; obj.name; \
			" ("; number obj; ")]"
		Font(BOLD_OFF)
	}
#endif

	select errornumber

		case 1
			print "You"; \
				! " can't use the word \""; \
				MatchPlural(player, "doesn't", "don't"); \
				" need to use the word \""; \
				parse$; "\"."

		case 3
			{print CThe(actor); " couldn't "; parse$; " multiple
				objects like that."}

		case 5
		{
			if VerbWord = "give", "offer", "hand", "show"
			{
				print "Not sure what you're referring to--try \""; \
					VerbWord; " (something) \Ito\i (someone)\"."
			}
			else
			{
!\
			  	print CThe(player); \
					MatchPlural(player, "hasn't", "haven't");
					print " seen any";
#ifset OLD_STYLE_PRONOUNS
					! If we built an object phrase using
					! OLD_STYLE_PRONOUNS, avoid printing a
					! potential nonsense name:
					if parser_data[PARSER_STATUS] & PRONOUNS_SET
						print "thing like that";
					else
#endif
						print " \""; parse$; "\"";
					print ", nor"; IsorAre(player, true); " "; The(player); \
					" likely to, even if such a thing exists."
\!
			  	print CThe(player); \
					" hadn't";
					print " encountered any";
#ifset OLD_STYLE_PRONOUNS
				if parser_data[PARSER_STATUS] & PRONOUNS_SET
					print "thing like that";
				else
#endif
					print " \""; parse$; "\"";
				print ".";
				print "  (If you're sure you need to refer to that, try
					putting it another way.)"
			}

		}

		case 10
		{
			print CThe(player); \
			  " hadn't";
			if &obj.long_desc
				" seen ";
			else
				" encountered ";
			if obj is living and not (parser_data[PARSER_STATUS] & FINDOBJECT_NONLIVING)
				print "anybody";
			else:  print "anything";
			print " like that";
#ifset DEBUG
			if (debug_flags & D_OBJNUM)
				print " ["; number obj; "]";
#endif
			print "."

		}

		case 11
		{
			if not ParseError_ObjectIsKnown(obj)
			{
				NewParseError(10, obj)

			}
			elseif obj is living
			{
!\
				print CThe(obj); \
					MatchPlural(obj, "isn't", "aren't"); " here."
\!
				print CThe(actor); \
					" didn't"; \
					" see";
			  	if obj is plural
			  		" them";
			  	elseif obj is female
			  		" her";
			  	else
			  		" him";
#ifset DEBUG
				if (debug_flags & D_OBJNUM)
					print " ["; number obj; "]";
#endif
				print "."
			}
			else
			{
#ifset USE_CHECKHELD
				if verbroutine = &DoDrop_CheckHeld, &DoPutIn_CheckHeld,
					&DoPutonGround_CheckHeld
				{
					NewParseError(15, obj)
				}
				else
				{
#endif
					print CThe(actor); \
						" didn't"; \
						" see that"; ! MatchPlural(obj, "that", "those");
#ifset DEBUG
					if (debug_flags & D_OBJNUM)
						print " ["; number obj; "]";
#endif
					print "."
#ifset USE_CHECKHELD
				}
#endif
			}

		}

		case 12
		{
			if not ParseError_ObjectIsKnown(obj)
			{
				NewParseError(10, obj)
			}
			else
			{
				print CThe(actor); \
					" couldn't do that with "; TheOrThat(obj) ; "."
			}

		}


		case 14
		{
			if not ParseError_ObjectIsKnown(obj)
			{
				NewParseError(10, obj)
			}
			elseif xobject is living
			{
				print CThe(xobject); \
					" didn't"; \
					" have "; TheOrThat(obj); "."
			}
			else
			{
				print CThe(actor); \
					" didn't"; \
					" see "; TheOrThat(obj); " there."
			}

		}

		case 15
		{
			if not ParseError_ObjectIsKnown(obj)
			{
				NewParseError(10, obj)
			}
			elseif obj = player
			{
				NewParseError(12, obj)
			}
			else
			{
				print CThe(actor); WasorWere(actor); " not
					holding"; MatchPlural(obj, "that", "those"); "."
			}

		}

		case else
		{
			parser_data[PARSER_STATUS] = PARSER_RESET
			return false            ! print the default message
		}

	parser_data[PARSER_STATUS] = PARSER_RESET
	return true                             ! message already printed
}
#ifset _ROODYLIB_H
replace NewRlibMessages(r, num, a, b)
{
   select r
		case &CheckReach
		{
			select num
				case 1
				{
					print capital The(a) ;
					if pasttense
						WasorWere(a,true)
					else
						IsorAre(a,true)
					print " inside "; The(parent(a)); "."
				}
		}
		case &DoGo
		{
			select num
				case 1	!  going to a non-enterable object in the same room
					print Cthe(object) ;
					if pasttense
						WasorWere(object,true)
					else
						IsorAre(object,true)
					print " right here."
		}
		case &DoListen
		{
			select num
				case 1   ! default location-listening
				{
					print capital player.pronoun #1;
					if pasttense
						print " heard";
					else
						MatchPlural(player, "hears", "hear")
					" nothing unexpected."
				}
				case 2   ! default object-listening
				{
					if pasttense
						"That would have served no purpose."
					else
						"That would serve no purpose."
				}
		}
		case &DoPushDirTo
		{
        ! Let's set default DoPushDirTo messages
		select num
			case 1
			{
				if pasttense
					"That would not have helped."
				else
					"That would not help the present situation."
			}
			case 2:  print "Try pushing "; Art(object); " in a direction."
			case 3
			{
				print CThe(player);
				if pasttense
					print " pushed ";
				else
					MatchPlural(player, " pushes ", " push ")
				print Art(object); " over to the..."
			}
			case 4
			{
				print CArt(object);
				if pasttense
				{
					print " slowed ";
				}
				else
					 print " slows ";
				"to a stop."
			}
		}
		case &DoSmell
		{
			select num
				case 1
				{
				print capital player.pronoun #1;
				if pasttense
					print " didn't";
				else
					MatchPlural(player, "doesn't", "don't")
				" smell anything unusual."
				}
				case 2 : "Why do that?"
		}
		case &DoUnlock
		{
			select num
			case 1
			{
				if a
					print "(with "; The(xobject); ")"
				print "Unlocked."
			}
			case 2
			{
				print capital player.pronoun #1;
				if pasttense
					print " couldn't ";
				else
					print " can't ";
				print "reach "; The(a); ", which ";
				if pasttense
				{
					print "was";
				}
				else
					print "is currently";
				print "in "; The(parent(a)); "."
			}
		}
		case &DoLock
		{
			select num
			case 1
			{
				if a
					print "(with "; The(xobject); ")"
				print "Locked."
			}
		}
#ifclear NO_XYZZY
		case &DoXYZZY
		{
		! text suggested by Rob O'Hara. Approved by Ben Parrish.
		print capital player.name; " mumble";
		if pasttense
			print "d";
		else
			MatchSubject(player)
		" an ancient reference to an archaic game. Nothing happens."
		}
#endif  ! NO_XYZZY
   case else : return false
   return true ! this line is only reached if we replaced something
}

replace NewRlibOMessages(obj, num, a, b)
{
	select obj

	case character
	{
		select num
			case 1
			{
				print CThe(xobject) ;
				if pasttense
					print " allowed ";
				else
					print " allows ";
				print player.name ; " to take ";
				print The(object); "."
			}
	}
	case door
	{
		select num
		case 3
		{
		print "(unlocking "; Art(self); " first)"
		}
	}

	case else : return false

	return true ! this line is only reached if we replaced something
}
#endif !  _ROODYLIB_H

replace MatchSubject(obj)
{
	if pasttense
		print "ed";
	elseif obj is not plural
		print "s";
}

#if defined DoSearch
replace DoSearch
{
	if object = player
	{
		"Search ";
		The(player)
		" indeed."
	}
	elseif object is container and child(object)
		Perform(&DoLookIn, object)
	elseif object is living
	{
		print CThe(object);
		if not pasttense
			print MatchPlural(object, "doesn't", "don't");
		else
			" didn't";
		" let ";
		The(player)
		if object.pronouns #2
		{
			" search ";
			if object.pronouns #2
				print object.pronouns #2;
			else
				print object.pronoun;
		}
		print "."
	}
	else
	{
		CThe(player)
		if not pasttense
			MatchPlural(player, "doesn't", "don't")
		else
			" didn't";
		" find anything new."
	}
	return true
}
#endif ! if defined DoSearch

#ifset _VERBSTUB_G

replace DoYes
{
	if not pasttense
		"That was a rhetorical question.  (Although you sound rather positive.)"
	else
		"That was a rhetorical question.  (Although you sounded rather positive.)"
}

replace DoNo
{
	if not pasttense
		"That was a rhetorical question.  (Although you sound rather negative.)"
	else
		"That was a rhetorical question.  (Although you sounded rather negative.)"
}


replace DoUse
{
	if not pasttense
		"Be a little more specific about what you'd like ";
	else
		"Be a little more specific about what you wanted ";
	if player_person ~= 2
		print player.pronoun #2; " ";
	"to do with ";
	print The(object, true); "."
}

#ifclear _ROODYLIB_H
replace DoSmell
{
	print capital player.pronoun #1;
	if not pasttense
		MatchPlural(player, "doesn't", "don't")
	else
		" didn't";
	" smell anything unusual."
	return true
}
#endif ! ifclear _ROODYLIB_H

replace DoJump
{
	print capital player.pronoun #1;
	" jump";
	MatchSubject(player)
	" up and down, accomplishing little."
	return true
}

replace DoWaveHands
{
	print capital player.pronoun #1;
	print " wave";
	MatchSubject(player)
	".  Nothing happens."
	return true
}

replace DoWave
{
	print capital player.pronoun #1;
	" wave";
	MatchSubject(player)
	print " "; The(object, true);
	".  Nothing happens."
	return true
}

replace DoThrowAt
{
	if not xobject
	{
		if not pasttense
		"You'll have to be a little more specific about what
		you'd like ";
		else
		"You'll have to be a little more specific about what you wanted ";
		if player_person ~= 2
			print player.pronoun #2; " ";
		"to throw ";
		print The(object, true); " at."
	}
	elseif xobject is not living
	{
		if not pasttense
			{
			"What exactly ";
			if player_person = 2
				"are you";
			else
				print "should "; player.pronoun #1; " be";
			" hoping for?"
			}
		else
			{
			"What exactly ";
			if player_person = 2
				"were you";
			else
				print "should "; player.pronoun #1; " have been";
			" hoping for?"
			}
	}
	else
	{
		print capital player.pronoun #1;
		print " ultimately decide";
		MatchSubject(player)
		" that throwing ";
		print The(object, true); " at "; The(xobject, true);
		if not pasttense
			" isn't";
		else
			" wasn't";
		"such a great idea."
	}
	return true
}

replace DoClimb
{
	if object = u_obj
	{
		Perform(&DoGo, u_obj)
	}
	elseif object
	{
		print capital player.pronoun #1;
		if not pasttense : " can't climb "; : else : " couldn't climb ";
		print The(object, true); "."
	}
	elseif not pasttense:  "Not here; not now." : else : "Not there; not then."
	return true
}

replace DoClimbOut
{
	if player not in location
		return Perform(&DoExit)
	else
	{
		CThe(player)
		if not pasttense
			MatchPlural(player, "isn't", "aren't")
		else
			MatchPlural(player, "wasn't", "weren't")
		" in anything at the moment."
	}
}

replace DoSleep
{
	if not pasttense
		"Not now--there's work to be done."
	else
		"Not then--there was work to be done."
}

replace DoPush
{
	if not CheckReach(object):  return false

	"Pushing ";
	The(object, true)
	if not pasttense
		" doesn't get ";
	else
		" didn't get ";
	print player.pronoun #2;
	" anywhere."
	return true
}

replace DoPull
{
	if not CheckReach(object):  return false

	"Pulling ";
	The(object, true)
	if not pasttense
		" doesn't get ";
	else
		" didn't get ";
	print player.pronoun #2;
	" anywhere."
	return true
}

replace DoKiss
{
	if not CheckReach(object):  return false

	if object is not living
		{
		if not pasttense
			"You need to get out more."
		else
			"You needed to get out more."
		}
	else
	{
		CThe(object)
		if not pasttense
			MatchPlural(object, "doesn't", "don't")
		else
			" didn't"
		" seem to share ";
		print player.pronoun #3;
		" affection."
		return true
	}
}

replace DoSwim
{
	if not pasttense
		"Not here, you won't."
	else
		"Not there, you didn't."
}

replace DoWake
{
	print capital player.pronoun #1;
	if not pasttense
		{
		if player_person = 1
			" am";
		elseif player is plural
			" are";
		else
			" is";
		" already awake, like it or not."
		}
	else
		{
		if player_person = 1
			" was";
		elseif player is plural
			" were";
		else
			" was";
		" already awake, like it or not."
		}
}

replace DoWakeCharacter
{
	if not pasttense
		"That's probably not necessary."
	else
		"That wasn't necessary."
}

replace DoTouch
{
	if not CheckReach(object):  return false

	"Fiddling with ";
	The(object, true)
	if not pasttense
		" like that probably isn't the best use of ";
	else
		" like that probably wasn't the best use of ";
	print player.pronoun #3;
	" time at the moment."
}

replace DoTie
{
	if not CheckReach(object):  return false

	"Trying to tie ";
	The(object, true)
	if not pasttense
		" doesn't get ";
	else
		" didn't get ";
	print player.pronoun #2;
	" anywhere."
}

replace DoTieR
{
	return Perform(&DoTie, xobject, object)
}

replace DoUntie
{
	if not CheckReach(object):  return false

	"Trying to untie ";
	The(object, true)
	if not pasttense
		" doesn't get ";
	else
		" didn't get ";
	print player.pronoun #2;
	" anywhere."
}

replace DoBurn
{
	if not pasttense
		"You didn't learn anything as a child, did you?  Don't play
		with fire.  And stay away from my house."
	else
		"That wouldn't have accomplished anything."
}

replace DoCut
{
	if not pasttense
		"A destructive and somewhat less than completely useful
		course of action."
	else
		"That wouldn't have accomplished anything."
}

replace DoDig
{
	if not pasttense
		"That won't accomplish anything."
	else
		"That wouldn't have accomplished anything."
}

replace DoYell
{
	print capital player.pronoun #1;
	" let";
	if not pasttense
		MatchPlural(player)
	else
		" ";
	"out a hearty bellow.  Nothing else happen";
	if not pasttense
		"s."
	else
		"ed."
	return true
}

replace DoFollow
{
	if object = player
	{
		if speaking
			SpeakTo(speaking)
		else
		{
			if not pasttense
				{
				if player_person = 2
					"Who are you talking to?"
				else
					{
					"It's not obvious who you want ";
					print player.pronoun #2;
					" to talk to."
					}
				}
			else
				{
				if player_person = 2
					"Who were you talking to?"
				else
					{
					"It's not obvious who you wanted ";
					print player.pronoun #2;
					" to talk to."
					}
				}
		}
	}
	elseif object in location
		{
		if not pasttense
			print CThe(object); IsorAre(object, true); " right here."
		else
			print CThe(object); WasorWere(object, true); " right here."
		}
	else
		{
		if not pasttense
			print "Which way did "; object.pronoun; " go?"
		else
			print "Which way had "; object.pronoun; " gone?"
		}
}

replace DoHelp
{
	"Unfortunately, 'twould seem ";
	if not pasttense
		{
		if player_person = 1
			"we're on our own."
		else
			"you're on your own."
		}
	else
		{
		if player_person = 1
			"we were on our own."
		else
			"you were on your own."
		}
}

replace DoHelpChar
{
	if object = player
	{
		Perform(&DoHelp)
		return
	}
	elseif object is living
	{
		if not pasttense
			"How exactly would you like ";
		else
			"How exactly did you want ";
		if player_person ~= 2
			print player.pronoun #2;
		" to try to help ";
		print The(object, true); "?"
	}
	else
	{
		if not pasttense
			print CThe(object); IsorAre(object, true);
		else
			print CThe(object); WasorWere(object, true);
		" not in need of ";
		print player.pronoun #3;
		" help at the moment."
	}
}
#endif  ! VerbStub replacements

! we need to replace ListObjects for "thing is here" messages
replace ListObjects(thisobj)
{
	local i, obj, count, pluralcount
	local templist_count            ! temporary total of unlisted objs.
	local id_count                  ! for identical (or plural) objects
#ifset USE_PLURAL_OBJECTS
	local j, this_class
#endif

	list_nest++
	for obj in thisobj
	{
		if obj is hidden
		{
			obj is already_listed
#ifclear NO_OBJLIB
			if obj.type = scenery
				obj is known
#endif
		}
		else
			obj is known

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

			if FORMAT & ISORARE_F
			{
				if list_count = 1 and id_count <= 1 and
					obj is not plural
				{
				if not pasttense
					print " "; IS_WORD;
				else
					print " "; WAS_WORD;
				}
				else
				{
				if not pasttense
					print " "; ARE_WORD;
				else
					print " "; WERE_WORD;
				}
				if FORMAT & LIST_F
					print ":"
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
					print " "; AND_WORD;
				if not (FORMAT & FIRSTCAPITAL_F)
					print " ";
			}
			else
			{
				print to (list_nest * 2);	! INDENT_SIZE);
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
					if FORMAT & FIRSTCAPITAL_F
						CArt(obj)
					else
						Art(obj)
				}
				else
				{
					if FORMAT & FIRSTCAPITAL_F
						print NumberWord(id_count, true);
					else
						print NumberWord(id_count);
					print " "; this_class.name;

					if this_class.type = plural_class
					{
						local k

						if FORMAT & LIST_F
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
									print to ((list_nest+1) * 2);	! INDENT_SIZE);
								}
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

				if FORMAT & FIRSTCAPITAL_F
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

		if obj is not hidden and FORMAT & LIST_F
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
			if list_nest = 1 and FORMAT & ISORAREHERE_F
			{
				if count + pluralcount > 1
					{
					if not pasttense
						print " "; ARE_WORD;
					else
						print " "; WERE_WORD;
					}
				else
					{
					if not pasttense
						print " "; IS_WORD;
					else
						print " "; WAS_WORD;
					}
				print " ";
				if not pasttense
					print HERE_WORD;
				else
					print THERE_WORD;
				FORMAT = FORMAT & ~ISORAREHERE_F  ! clear it

				if not (FORMAT&LIST_F or FORMAT&TEMPLIST_F)
					override_indent = true
			}

			if not (FORMAT & NORECURSE_F)
				print ".";
		}

		i = 0

		for obj in thisobj
		{
			if children(obj) and obj is not hidden and
				(obj is not already_listed or
					thisobj ~= location)
			{
				if FORMAT & TEMPLIST_F
				{
					FORMAT = FORMAT | LIST_F & ~TEMPLIST_F
					i = true
					print newline
				}

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

#endif	! _PASTTENSE_H
