!----------------------------------------------------------------------------
! newconverse.h - conversation management in Hugo
! Version 2.7, updated by Roody Yogurt 2012
! The original is converse.h
! Version 1.0, Copyright 2001 by Christopher Tate <ctate@acm.org>
!
! v 2.7 - Added usage_desc code for newmenu default menu, changed "top" property
! to "top_menu"
! v 2.6 - Added some related_item code
! v 2.5 - updated to use new Roodylib USE_EXTENSION_CREDITING capability
! v 2.4 - Discovered gAvailableAssertion array was being called wrong in a way
! that would have prevented 5 assertions from working properly; added some code
! to elongate the chat window when dialogue choices are wider than the screen
! width
! v 2.3 - added a routine ForceStopTalk for silently ending a conversation, no
! matter what (the other routine, DoStopTalk, makes a converselib.can_quit check
! that can get in the way of things
! v 2.2 - added a noquotes property to assertions for choices that aren't spoken
! v 2.1 - fixed some DESCFORM_I stuff
! v 2.0 - Split special UNDO stuff into undolib.h extension. Now newconverse
!	requires that and roodylib for full functionality. Will work without
!	special undo stuff without undolib and roodylib, though.
! v 1.9 - Cleaned up SpeakTo, made responses player-person and tense generic
! v 1.8 - Updated to work with roodylib's new take on PrintStatusLine
! v 1.5 - got rid of SpeakTo, cleaned up messages, now requires RoodyLib to
! work best
! v 1.3 - fixed quotation marks
! v 1.2 - changed needscomma to work with non-fullstop entries, too
!
! Also needs newconverse.g for grammar.
!
! TO DO -
!   Decide if there are scenarios where DoGoodbye and DoStopTalk
!   should remain distinct or if one should be eliminated
!
! Relies on HUGOLIB and VERBLIB -- #include this file after
! #including "hugolib.h" and "verblib.h"
!----------------------------------------------------------------------------

#ifclear _NEWCONVERSE_H
#set _NEWCONVERSE_H

#ifset VERSIONS
#message "NewConverse.h Version 2.5"
#endif

#ifset USE_EXTENSION_CREDITING
version_obj newconverse_version "NewConverse 2.5"
{
	in included_extensions
	desc_detail
		" by Christopher Tate and Roody Yogurt";
}
#endif

#ifclear _ROODYLIB_H
#message warning "If you are using roodylib, it must be included *before*
newconverse.h"
#endif

#ifclear _UNDOLIB_H
array undocommands[1]
#endif

!----------------------------------------------------------------------------
! Entering conversation mode is done with >GREET character, or with
! >TALK TO character.  Changing topics is done with >TOPIC or >T
!----------------------------------------------------------------------------

!----------------------------------------------------------------------------
! !!! Things not implemented; may or may not be good ideas:
!     (as originally suggested by Christopher Tate)
!	- Non-hierarchical topic backtracking [go back to earlier topics
!    automatically even without tree structure]
!	- detection of 'abrupt' vs 'natural' topic changes, and NPC reaction based
!    on it
!	- allow NPCs to veto topic changes - scope ceiling a la Emily, or some
!    other mechanism?
!	- somehow hook Assertions into mutiple topics w/out duplicating?  a la
!    Emily's 'pars' system.
!	- 'mood alteration' keying; a property in the Assertion that indicates how
!    the NPC's mood should change in response to this assertion being run
!----------------------------------------------------------------------------

! gAvailableAssertions array- holds the currently available conversation options
array gAvailableAssertions[5]

!----------------------------------------------------------------------------
! defaultoptions
!----------------------------------------------------------------------------
! Some properties that are set by newconverse. author can ignore.

! gNumAvailableAssertions - how many elements are valid in GAvailableAssertions
property gNumAvailableAssertions alias u_to

! gDidSpeak - did the player engage in conversation last turn?
property gDidSpeak alias d_to

class defaultoptions
{
#ifset _ROODYLIB_H
	type settings
#endif
	gNumAvailableAssertions 0
	gDidSpeak 0
}

!----------------------------------------------------------------------------
! talkoptions
!----------------------------------------------------------------------------
!\ These property values should be changed by the author for the wanted
conversation behavior. See talkoptions object example below. \!

! talkoptions.top_menu - set to true if dialogue choices should be at top of screen
property top_menu alias w_to

!talkoptions.show_topics - set true to show available topics
property show_topics alias e_to

!talkoptions.topic_change - set true to allow topic change
property topic_change alias n_to

!talkoptions.conv_force - set true to only allow conversation commands
property conv_force alias s_to

!talkoptions.auto - set true to turn on conversation topic auto-switching
property auto alias ne_to

!talkoptions.stop_undo - set true to disallow UNDO while in conversations
property stop_undo alias nw_to

#ifset _UNDOLIB_H
property skip_back alias sw_to
#endif

!talkoptions.can_quit - set true to allow leaving a conversation
property can_quit alias se_to

! talkoptions should be replaced in your game with the settings you want
object talkoptions "(talkoptions)"
{
	inherits defaultoptions
	top_menu 1     ! should conversation menu be in status bar?
	show_topics 1  ! should it show available topics to talk about?
	topic_change 1 ! is topic-changing allowed?
	conv_force 0 ! are only conversation commands allowed?
	auto 0 ! when a topic is depleted, should it auto-switch to next topic?
	stop_undo 0 ! should it disallow UNDO during conversations?
	can_quit 1 ! can the player >END CONVERSATION (or otherwise leave it)?
#ifset _UNDOLIB_H
	skip_back 0 ! should UNDOing skip back to the start of the conversation?
#endif
}

#ifset _ROODYLIB_H
object converselib "converse"
{
	type settings
	in init_instructions
	save_info
	{
		if word[LAST_TURN] = "undo" ! CheckWordSetting("undo")
		{
			if undocommands[0] = 1
			{
				ConverseVMessage(&DoUndo,1) ! "UNDOing to before conversation."
			}
		}
		return false
	}
#ifset _NEWMENU_H
	usage_desc
	{
		Indent
		"\BTALK TO <character>\b- Initiate conversation with character."
		Indent
		"\B1/2/3/4/5\b- While in a conversation, this chooses a dialogue option."
		if talkoptions.topic_change
		{
			Indent
			"\BTOPIC/T <topic>\b- Change current conversation topic."
		}
		if talkoptions.can_quit
		{
			Indent
			"\BEND CONVERSATION / STOP TALKING\b- End current conversation."
		}

	}
#endif
}
#endif  ! _ROODYLIB_H

!----------------------------------------------------------------------------
! an NPC is someone we can talk to.  NPCs use some custom properties:

attribute chatmenu ! to replace "if speaker.type = npc" checks
property next_assertion alias d_to			! go to this Assertion automatically
														!  next turn if the player doesn't
														!  pick one explicitly
property force_assertion alias cant_go	! go to this Assertion automatically as
													!  soon as the player gives us a chance;
													!  makes SURE we get here
property last_assertion alias in_to			! what did we say last?
property current_topic alias out_to			! what's the NPC currently talking
														!  about?
property greeted alias e_to			! called when SpeakTo() has determined that
												! conversation is actually ensuing

character NPC
{
	type npc
	next_assertion 0
	force_assertion 0
	last_assertion 0
	current_topic 0

	! by default, when you greet an NPC there is no set topic, and any stale cued assertions are flushed
	greeted
	{
		if (self.current_topic = 0)
		{
			ConverseOMessage(npc,1) ! "You contemplate introductory remarks."
			self.current_topic = 0
			self.next_assertion = 0
			self.force_assertion = 0
			AnyTopicsLeft(true)
		}
		else
		{
			ConverseOMessage(npc,2) ! "You speak up."
			talkoptions.gNumAvailableAssertions = 0
			self.next_assertion = 0
			self.force_assertion = 0
			SetCurrentTopic(self.current_topic)
		}
	}
	react_before
	{
		if AnyVerb(location) and verbroutine ~= &DoConverse
		{
			if talkoptions.conv_force and (word[1] ~= "1","2","3","4","5","t","topics","topic","subject","stop") and
				speaking is chatmenu
			{
				ConverseMessage(&ConversePreParse,1) ! "Enter a valid conversation choice."
				return true
			}
		}
		return false
	}
	before
	{
		object DoTalk, DoHello, DoGreet
		{
			Perform(&DoConvStart, self)
			return true
		}
		object DoTell
		{
			PrintConverseUsage(self)
			return true
		}
		object Doask
		{
			if word[1] ~= "ask" and word[1] ~= "question" and
			word[1] ~= "consult" and xobject = player
				Perform(&DoConvStart, self)
			else
				PrintConverseUsage(self)
			return true
		}
	}
	! add "is female" and make pronouns "she", "her", "her", "herself" for
	!  female NPCs
	pronouns "he", "him", "his", "himself"
	is living, static, transparent, chatmenu
	order_response
	{
		select verbroutine
			case &DoConvStart, &DoHello , &DoTalk ! : Perform(&DoConvStart, self)
			{
				object = self
				DoConvStart
			}
			case &DoGoodbye
			{
				if self ~= speaking
					return false
				else
					return Perform(&DoGoodbye)
			}
			case else : return false ! return false so we get an ignore_response
		return true
	}
	ignore_response
	{
		if verbroutine = &DoGoodbye
			return false
		else
			PrintConverseUsage(self)
		return true
	}
	parse_rank
	{
		if not verbroutine
			verbroutine = &DoConvStart
		! prefer Topic objects over similarly-named 'tangible' objects when
		!  parsing >TOPIC obj
		if verbroutine = &DoTopic : return -1
		else : return 1		! 0 is the default parse_rank; we defer to all
									!  non-topic objects like this
	}
}

!----------------------------------------------------------------------------
! The global 'Conversation' object serves as the root of the topic tree.
! *ONLY* Topic objects should be placed 'in' the Conversation.

object Conversation
{
	is transparent, hidden, static
	in_scope
	{
	!	if speaking is chatmenu
			return player
	!	else
	!		return false
	}! will be placed in the player's scope at init time
}

!----------------------------------------------------------------------------
! a 'Topic' is a thing that can be discussed; the current subject of
! conversation is set via >TOPIC FOO or >ASK character ABOUT FOO.  Note that
! you must manually attach derived Topic objects in the proper place, either
! to the Conversation object directly, or to another Topic object (useful for
! providing scoping of topics).

property related_object alias misc

class Topic
{
	type topic
	is hidden, container, openable, not open
	is transparent									! so you can 'see' subtopics
	is known											! so you can ask about it
	is living, unfriendly    ! so we have control over ignore_response commands
	exclude_from_all true
	before
	{
		object
		{
			if verbroutine = &DoTopic : return false
			if self.related_object
				ParseError(11,self.related_object)
			else
				ParseError(11,object)
			return true
		}
		xobject
		{
			if self.related_object
				ParseError(11,self.related_object)
			else
				ParseError(11,xobject)
			return true
		}
	}
	misc 0
	ignore_response
	{
		if self.related_object is living
		{
			AssignPronoun(self.related_object)
			ConverseOMessage(topic,1,self) ! "your intended subject is not here."
		}
		else
		{
			ClearPronoun(self)
			ParseError(6) ! "That doesn't make any sense."
		}
	}
	parse_rank
	{
		! prefer Topic objects over similarly-named 'tangible' objects when
		! parsing >TOPIC obj
		if verbroutine = &DoTopic : return 2
		else : return -1		! 0 is the default parse_rank; we defer to all
									! non-topic objects like this
	}
}

!----------------------------------------------------------------------------
! An "Assertion" is a conversational exchange, and encapsulates both a question
! or statement that the player can pose to an NPC and the NPCs response.
!
! long_desc, which displays the NPCs response to a given conversational gambit,
! should set up any topic chains etc. in the NPC when necessary; it is called
! with the global variable 'speaking' set to the current conversation target
! to facilitate this.

! attribute visited					! indicates that the Assertion has been said;
											!  generally this will be set by the engine
! property short_desc				! menu item text for this Assertion; should
											!  NOT end with a newline
! property long_desc					! full text to be displayed when the Assertion
											!  is selected by the player.

property Prerequisite alias out_to	! Usually a function property; returns 0
												!  (false) if the Assertion is currently
												!  unavailable for any reason
property only_npc alias n_to			! unique NPC  to whom this Assertion
												!  applies.  This is a shortcut for putting
												!  npc-specific testing in Prerequisite.
property only_after alias e_to		! shortcut - only available immediately
												!  after the only_after Assertion is said.
property menu_choice alias name
property menu_response alias long_desc
property pretext alias w_to
property posttext alias s_to
property needscomma alias ne_to
property show_assert alias nw_to  ! reprint the PC's assertion after selection?
property noquotes alias sw_to ! if a choice should not be surrounded by quotes

class Assertion
{
	type assertion

	! short_desc		! prints the text of the menu item for this Assertion,
							!  *without* a trailing newline
	! long_desc			! performs the NPC's side of the conversational exchange

	            			! by default all Assertions are available, and
								!  disappear once they've been selected; override
	Prerequisite			!  this for custom behavior
	{
		if self is not visited : return true
		else : return false
	}
	only_npc 0
	only_after 0
	show_assert 1

	is not known		! don't trouble the parser with these objects
}

!----------------------------------------------------------------------------
! SetCurrentTopic(topic) - sets the current topic and builds the list of
!									available conversational gambits.
!
! The 'obj' argument is the topic object to use, and is set to be the
! speaking NPC's current_topic.  If there are no available assertions in the
! given topic, the routine backs up the topic tree until it either finds some
! available assertions or hits a root topic, i.e. one whose parent is the
! global Conversation object.

routine SetCurrentTopic(obj)
{
	local a, i

	speaking.current_topic = obj

	i = 0
	for a in obj
	{
		! 'visited' testing is left to the Prerequisite function
		if a.type = assertion and
			(a.only_npc = 0 or a.only_npc = speaking) and
			(a.only_after = 0 or a.only_after = speaking.last_assertion) and
			a.Prerequisite
		{
			gAvailableAssertions[i++] = a
			if i = gAvailableAssertions[] : break		! stop if we hit our limit
		}
	}

	talkoptions.gNumAvailableAssertions = i

	! if we didn't find any valid assertions, and this was not a
	! top-level topic, move up the topic tree and try again.
	if i = 0 and parent(obj).type = topic
	{
		SetCurrentTopic(parent(obj))
	}
}

!----------------------------------------------------------------------------
! SayAssertion(assertion, showPrompt) - display and record a conversational
! exchange. If showPrompt = true, the player's side of the exchange is
! displayed, otherwise only the NPC's side is shown.  This is useful when
! calling from the event that continues conversations even in the absence of
! prompting from the player.  This can be called even when there is no active
! conversation.
!
! If showPrompt is true, the player's side of the conversation
! (theAssert.short_desc) is displayed before the NPCs response
! (theAssert.long_desc).  This is done by calling the PrintAssertionPrompt()
! routine, which you can 'replace' to get custom formatting.
! By default, PrintAssertionPrompt() displays the player's question/comment
! enclosed in double quotes, followed by a single newline.

routine SayAssertion(theAssert, showPrompt)
{
	! Run the Assertion, mark it 'visited', and rebuild the assertion list. Note that
	! the assertion is marked 'visited' *before* its *_desc functions are called,
	! so those functions can cleanly mark it not visited to override the default
	! behavior, if desired.

	theAssert is visited

	if showPrompt
	{
		PrintAssertionPrompt(theAssert)				! player's half of the exchange,
																!  if designated
		""                                        ! a blank line, for space!
	}
	Indent													! indent properly
	run theAssert.long_desc								! the NPC's response itself
	if speaking
	{
		speaking.last_assertion = theAssert			! remember what was last said
		SetCurrentTopic(speaking.current_topic)	! resetting the current topic
																!  rebuilds the available
																!  assertion list
		AnyTopicsLeft
		if not talkoptions.gNumAvailableAssertions and not talkoptions.auto and talkoptions.conv_force
		{
			local a
			""
			ConverseMessage(&SayAssertion, 1) ! "You can't think of anything else
														 !  to talk about."
			a = speaking
			speaking = 0
			PrintStatusLine
			speaking = a
			DoStopTalk(1)
		}
	}
}

routine PrintAssertionPrompt(theAssert)
{
	local g
	g = system(61)  ! simple port
	Indent
	print theAssert.pretext;
	if not theAssert.noquotes
	{
		if g
			print "\"";
		else
			print "\#147";
	}


	local a
	if theAssert.needscomma
	{
		text to _temp_string
		print theAssert.menu_choice;
		text to 0
		a = StringLength(_temp_string) - 1
	}
	if (a)
	{
		StringPrint(_temp_string, 0, a)
		if (_temp_string[a] = ".")
		{
			print ",";
		}
		else
		{
			printchar _temp_string[a]
			print ",";
		}
	}
	else
		print theAssert.menu_choice;
	if not theAssert.noquotes
	{
		if g
			print "\"";
		else
			print "\#148";
	}
	print theAssert.posttext;
		""
}

!----------------------------------------------------------------------------
! Routines to cue an Assertion for a given NPC to say next time around.
! To clear an NPCs cued Assertion, call CueAssertion(theNPC, 0)

routine CueAssertion(theNPC, theAssertion)
{
	if theNPC ~= nothing : theNPC.next_assertion = theAssertion
}

routine ForceCueAssertion(theNPC, theAssertion)
{
	if theNPC ~= nothing : theNPC.force_assertion = theAssertion
}

!----------------------------------------------------------------------------
! Cueing messages in the future - fuse-based.  Note that because this uses
! a single fuse, you can have only one pending cue at a time.  If you want more
! than one NPC's conversation cued up at once, or multiple cues, just copy &
! extend this fuse concept.

#ifclear NO_FUSES

fuse cue_fuse
{
	misc 0				! refers to the NPC where the Assertion should be queued
	next_assertion 0	! the Assertion to cue
}

event in cue_fuse
{
	event_flag = 2		! force an interruption
	if not self.tick	! when the timer runs out
	{
		CueAssertion(cue_fuse.misc, cue_fuse.next_assertion)
		Deactivate(cue_fuse)
	}
}

fuse conversation_end
{}

event in conversation_end
{
	if not self.tick	! when the timer runs out
	{
		if not speaking or speaking is not chatmenu
		{
				if undocommands[] = 1
				{
					undocommands[0] = 0
				}
#ifset _UNDOLIB_H
				continue_undo = 0
#endif
				UNDO_OFF = false
		}
		Deactivate(self)
	}
}


routine CueAhead(theNPC, theAssertion, theDelay)
{
	cue_fuse.misc = theNPC
	cue_fuse.next_assertion = theAssertion
	Activate(cue_fuse, theDelay)
}

#endif		! ifclear NO_FUSES

!----------------------------------------------------------------------------
! Global event to handle cued assertions.  Non-forced assertions are favored
! over forced ones, since the forced ones will never be discarded automatically.

event
{
	local a

	if speaking is chatmenu and not talkoptions.gDidSpeak
	{

		a = nothing
		if speaking.next_assertion
		{
			a = speaking.next_assertion
			speaking.next_assertion = nothing	! i.e. CueAssertion(speaking,
															! nothing)
		}
		elseif speaking.force_assertion
		{
			a = speaking.force_assertion
			speaking.force_assertion = nothing	! i.e. ForceCueAssertion(speaking,
															! nothing)
		}
		if a
		{
			SayAssertion(a, false)
			PrintStatusline
			event_flag = 2		! force an interruption
		}
	}
	talkoptions.gDidSpeak = false
	if speaking is chatmenu and speaking in location
	{
		event_flag = 2		! force an interruption
		if not talkoptions.top_menu
			ListOptions
		Activate(conversation_end,2)
	}
!	if speaking = 0
!		UNDO_OFF = false
}

!----------------------------------------------------------------------------
! Continued-UNDO messages
! In games where UNDO should repeat until the start of a conversation, these
! routine replacements make the undo response more conversatio-centric.

#ifset _UNDOLIB_H
replace ContinueUndoSuccess
{
	"UNDOing to before conversation."
}

replace ContinueUndoFailure
{
	"Attempted to UNDO to before conversation but ran out of memory."
}

#endif

!----------------------------------------------------------------------------
! VERB ROUTINES FOR CONVERSATION

routine DoGreet
{
		ConverseVMessage(&DoGreet)		! whom do you want to greet?
		return false
}

routine DoGoodbye
{
	if not speaking
	{
		ConverseVMessage(&DoGoodbye) ! "You aren't talking to anybody!"
		return false
	}
	else
		return Perform(&DoStopTalk)
}

! This is sort-of fake verb routine that we funnel other verbs to, like
! DoHello and DoTalk
routine DoConvStart
{
	if object = speaking
	{
		ConverseVMessage(&DoConvStart, 1) ! "You are already speaking."
		return true
	}

	if object is not unfriendly
	{
		speaking = object
		if not CountTopics(true) ! include current topic
		{
			Indent
			ConverseMessage(&SpeakTo, 1) ! "You can't think of anything to
												  !  talk about."
			speaking = 0
			UNDO_OFF = false
			return true
		}
		if talkoptions.stop_undo
			UNDO_OFF = true
#ifset _UNDOLIB_H
		if talkoptions.skip_back
		{
			continue_undo = true
		}
#endif
		run object.greeted
		speaking = object
		return true			! this takes a turn
	}
	else
	{
		if not object.ignore_response
			Message(&Speakto, 4, self)      ! "X ignores you."
		UNDO_OFF = false
		speaking = 0
		return true
	}
}

routine DoTopic
{
!#ifset _UNDOLIB_H
!	if show_commands
!	{
!!		ClearArray(undocommands)
!		SaveCommand
!		last_turn_true = true
!	}
!#endif
	if speaking = nothing
	{
		ConverseVMessage(&DoTopic, 1)	! "you're not talking to anybody!"
		return false
	}

	if not talkoptions.topic_change
	{
		ConverseVMessage(&DoTopic, 5) ! "Now is not an appropriate time to change
												!  the topic."
		return false
	}

	if object = nothing
	{
		ConverseVMessage(&DoTopic, 2)      ! "Ask about something specific..."
		return false
	}

	! If, despite all of our parse_rank adjustments, the parser has come up
	! with a non-Topic object here, we issue a relevant message and fail.
	if object.type ~= topic
	{
		ConverseVMessage(&DoTopic, 3)	! not an available topic
		return false
	}

	if object = speaking.current_topic
	{
		ConverseVMessage(&DoTopic, 6) ! "You are already talking about that."
		return false
	}

	local a c
	for a in object ! check for available topics
	{
		! 'visited' testing is left to the Prerequisite function
		if a.Prerequisite and (a.only_npc = speaking or not a.only_npc)
			c++
		}
	if not c  ! if no available topics
	{
		ConverseVMessage(&DoTopic, 7) ! "You don't have anything to say about
		return false						!  that right now.
	}
	! okay, we have a new topic - flush any cued non-critical conversation under
	! the old topic
	if object ~= speaking.current_topic
	{
		speaking.next_assertion = nothing  ! i.e. CueAssertion(speaking, nothing)
	}

	! Set the new topic
	SetCurrentTopic(object)

	! suitable feedback that the topic change has happened
	ConverseVMessage(&DoTopic, 4)		! "(You contemplate your options.)"

	if talkoptions.top_menu
		PrintStatusline		! rebuild the status bar manually since this action
									! doesn't reenter main
	else
		ListOptions

	return false			! change of topic doesn't cost a turn
}

routine DoConverse
{
	! this routine actually performs a conversational exchange
	if speaking is not chatmenu
	{
		ConverseVMessage(&DoConverse,1)	! "that option is not currently
		return false							!  available"
	}

	local which
	which = StringToNumber(word[1])

	if which <= talkoptions.gNumAvailableAssertions
	{
		! valid option, so clear any cued Assertion and run the newly-selected
		! one.  Doing so in this order is necessary so that the cued assertion
		! doesn't get cleared after the newly-run one has just established it.

		speaking.next_assertion = nothing		! i.e. CueAssertion(speaking,
															! nothing)
		talkoptions.gDidSpeak = true
		--which
		SayAssertion(gAvailableAssertions[which], gAvailableAssertions[which].show_assert)
		return true
	}
	else
	{
		ConverseVMessage(&DoConverse,2)		! "that option is not currently
		return false								!  available"
	}
}

!----------------------------------------------------------------------------
! The Converse library uses its own VMessage-like system.  To override these,
! replace NewConverseVMessages(r, num, a, b), just like the ordinary verblib.h
! VMessage / NewVMessages system.

routine ConverseVMessage(r, num, a, b)
{
	if NewConverseVMessages(r, num, a, b):  return

	select r
	case &DoConvStart
	{
		select num
			case 1
			{
				print capital player.name; " ";
#ifset _PASTTENSE_H
				if pasttense
					WasorWere(player,true)
				else
#endif
				IsorAre(player,true)
				" already speaking."
			}
	}
	case &DoGoodbye
	{
		VMessage(&DoAskQuestion,1) ! "You aren't talking to anybody!"
	}
	case &DoGreet
	{
		print "It's unclear whom ";
		print player.name;
#ifset _PASTTENSE_H
		if pasttense
			print " wished";
		else
#endif
			MatchPlural(player, "wishes", "wish")
		" to greet."
	}

	case &DoTopic
	{
		select num
			case 1: VMessage(&DoAskQuestion,1) ! "You aren't talking to anybody!"
			case 2
			{
				print  "Please be more specific about what topic "; player.name;
#ifset _PASTTENSE_H
				if pasttense
					print " wished";
				else
#endif
					MatchPlural(player, "wishes", "wish")
				" to introduce."
			}
			case 3
			{
				print capital player.name; " ";
#ifset _PASTTENSE_H
				if pasttense
					WasorWere(player,true)
				else
#endif
					IsorAre(player, true)
				" unsure how to broach that subject."
			}
			case 4
			{
				print "("; capital player.name;
#ifset _PASTTENSE_H
				if pasttense
					print " contemplated ";
				else
#endif
					MatchPlural(player, "contemplates ", "contemplate ")
				print player.pronoun #3;" options.)"
			}
			case 5
			{
#ifset _PASTTENSE_H
				if pasttense
					"It was not an appropriate time to change the topic."
				else
#endif
					"Now is not an appropriate time to change the topic."
			}
			case 6
			{
				print capital player.name; " ";
#ifset _PASTTENSE_H
				if pasttense
					WasorWere(player,true)
				else
#endif
					IsOrAre(player,true)
				" already talking about that."
			}
			case 7
			{
				print capital player.name;
#ifset _PASTTENSE_H
				if pasttense
					print " didn't";
				else
#endif
					MatchPlural(player, "doesn't", "don't")
				" have anything to say about that ";
#ifset _PASTTENSE_H
				if pasttense
					"then."
				else
#endif
					"right now."
			}
	}

	case &DoTopics
	{
		select num
			case 1
			{
				print capital player.name; " ";
#ifset _PASTTENSE_H
				if pasttense
					print "couldn't";
				else
#endif
					"can't";
				" think of anything to ask about."
			}
			case 2
			{
#ifset _PASTTENSE_H
				if pasttense
					"Possible conversation topics:"
				else
#endif
					"Current possible conversation topics:"
			}
			case 3
			{
#ifset _PASTTENSE_H
				if pasttense
					"There was nothing to talk about."
				else
#endif
					"There is nothing to talk about."
			}
	}

	case &DoConverse
	{
		select num
			case 1 : "That option is not available."
			case 2: "Enter a valid option."
	}

	case &DoStopTalk
	{
		select num
			case 1
			{
				print capital player.name;
#ifset _PASTTENSE_H
				if pasttense
					print " stopped";
				else
#endif
					MatchPlural(player, "stops", "stop")
				" the conversation."
			}
			case 2
			{
				print capital player.name;
#ifset _PASTTENSE_H
				if pasttense
					print " ended";
				else
#endif
					MatchPlural(player, "ends", "end")
				" the conversation abruptly."
			}
			case 3
			{
#ifset _PASTTENSE_H
				if pasttense
					"That didn't seem possible."
				else
#endif
					"That doesn't seem possible."
			}
			case 4: "Enter a valid choice."
			case 5: VMessage(&DoAskQuestion,1) ! "You aren't talking to anybody!"
	}

	case &DoUndo
	{
		select num
			case 1 : "UNDOing to before conversation."
			case 2 : "You can't UNDO at the moment."
	}
}

routine NewConverseVMessages(r, num, a, b)	! The NewConverseVMessages routine
{															! may be REPLACED, and should
	return false										! return true if a replacement
}															! message <num> exists for
															! routine <r>

routine ConverseOMessage(r, num, a, b)
{
	if NewConverseOMessages(r, num, a, b):  return

	select r
	case npc
	{
		select num
			case 1
			{
				print capital player.name;
#ifset _PASTTENSE_H
				if pasttense
					print " contemplated";
				else
#endif
					MatchPlural(player, "contemplates", "contemplate")
				" introductory remarks."
			}
			case 2
			{
				print capital player.name;
#ifset _PASTTENSE_H
				if pasttense
					print " spoke";
				else
#endif
					MatchPlural(player, "speaks", "speak")
				" up."
			}
	}
	case topic
	{
		select num
			case 1
			{
				print capital player.pronoun #3; " intended";
				MatchPlural(a, "subject", "subjects")
				" ";
#ifset _PASTTENSE_H
				if pasttense
					WasorWere(a,true)
				else
#endif
					IsorAre(a, true)
				" not here."
			}
	}
}

routine NewConverseOMessages(r, num, a, b)	! The NewConverseVMessages routine
{															! may be REPLACED, and should
	return false										! return true if a replacement
}															! message <num> exists for
															! routine <r>

routine ConverseMessage(r, num, a, b)
{
	if NewConverseMessages(r, num, a, b):  return

	select r
	case &DoGreet
	{
		print "It's unclear whom "; player.name;
#ifset _PASTTENSE_H
		if pasttense
			print " wished";
		else
#endif
		MatchPlural(player, "wishes", "wish")
		" to greet."
	}

	case &AnyTopicsLeft
	{
		select num
			case 1
			{
				print capital player.name;
#ifset _PASTTENSE_H
				if pasttense
					" decided";
				else
#endif
				MatchPlural(player, "decides", "decide")
				" to change the topic."
			}
			case 2
			{
				print capital player.name;
#ifset _PASTTENSE_H
				if pasttense
					" couldn't";
				else
#endif
				" can't";
				" think of anything else to say for now."
			}

	}

	case &ConversePreParse
	{
		select num
			case 1:  "Enter a valid conversation choice."
	}

	case &FancyPrompt
	{
		select num
			case 1
			{
				if a  ! talkoptions.can_quit
					print "Select an option or 0 to say nothing ";
				else
					print "Select an option ";
			}
			case 2 : print ">> ";  ! the FancyPrompt promt
	}

	case &ListOptions
	{
		select num
			case 1: "Enter a valid choice."
			case 2
			{
				print capital player.name;
#ifset _PASTTENSE_H
				if pasttense
					" considered";
				else
#endif
				MatchPlural(player, "considers ", "consider ")
				print player.pronoun #3; " other options."
			}
	}

	case &SayAssertion
	{
		select num
			case 1
			{
				print capital player.name;
#ifset _PASTTENSE_H
				if pasttense
					" couldn't"
				else
#endif
				" can't";
				" think of anything else to talk about."
			}
	}

	case &ShowTopics
	{
		select num
			case 1 : "Current topic: ";
			case 2 : print "none";
			case 3 : print "Available topics:";
	}

	case &PrintConverseUsage
	{
		"To enter into conversation with a character, type \BGREET <Character
		Name>\b.  You can then enter the numbers corresponding to the available
		conversational gambits shown in the status bar area.  If there are no such
		gambits available, or if you wish to direct the conversation to a
		particular subject, at any time during a conversation you may enter
		\BTOPIC <subject>\b to change the matter under discussion.  Selecting a
		new topic does not count as a turn."

		"\n\BTALK TO\b and \BHELLO\b are synonyms for \BGREET\b, and \BTOPIC\b
		can be abbreviated \BT\b."
	}

	case &SpeakTo
	{
		select num
			case 1
			{
				print capital player.name;
#ifset _PASTTENSE_H
				if pasttense
					" couldn't";
				else
#endif
				" can't";
				" think of anything to talk about."
			}
	}
}

routine NewConverseMessages(r, num, a, b)	! The NewConverseVMessages routine may
{														! be REPLACED, and should return true
	return false									! if a replacement message <num>
}														! exists
!----------------------------------------------------------------------------
! REPLACEMENTS FOR HUGOLIB ROUTINES
!----------------------------------------------------------------------------

!\ Note from Roody: I deleted the DoWait replacement because
   I *think* proper event_flag-ing should fix that problem (also
	added "event_flag = 2" to a couple events in here) \!

!----------------------------------------------------------------------------
! custom statusline when using the Converse system

#ifclear _ROODYLIB_H
replace PrintStatusline
{
	! because we might be changing the status line's height,
	! clear the old one *before* recalculating its size
#ifset CHEAPGLK
	if cheap
		return
#endif

	window display.statusline_height
	{
		cls
	}

	! We need to reset the windows if the player has left the
	! location the speaker is in
	if display.needs_repaint or (not speaking and
		display.statusline_height > 2)
	{
		display.statusline_height = 1
		window 0
		display.needs_repaint = false
	}

	if speaking is chatmenu and talkoptions.top_menu and
		talkoptions.show_topics
!		display.statusline_height = talkoptions.gNumAvailableAssertions + 4
		display.statusline_height = CountDialogueLines + 4
	elseif speaking is chatmenu and talkoptions.top_menu and not talkoptions.show_topics
!		display.statusline_height = talkoptions.gNumAvailableAssertions + 3
		display.statusline_height = CountDialogueLines + 4
	else
		display.statusline_height = 1
	if display.linelength < 61 and STATUSTYPE ~=0 and talkoptions.show_topics
		display.statusline_height += 1

	Font(BOLD_OFF | ITALIC_OFF | UNDERLINE_OFF | PROP_OFF)
	window display.statusline_height
	{
		color SL_TEXTCOLOR, SL_BGCOLOR
		cls	! make sure we've drawn the entire status bar in the proper colors
		locate 1,1
		if speaking is chatmenu and talkoptions.top_menu
		{
			locate 3, 1
			print "Talking to "; Art(speaking);
		}
		else
		{
			if not location
				print "\_";
			elseif not light_source
				print "In the dark";
			else
			{
				print "\_ ";
				print capital location.name;
			}
		}

		if display.statusline_height ~= 2
		{
			! write any upper-right corner text to string
         text to _temp_string
		   if STATUSTYPE = 1 and (speaking is not chatmenu or not talkoptions.top_menu)
				print number score; " / "; number counter;
		   elseif STATUSTYPE = 2 and (speaking is not chatmenu or not talkoptions.top_menu)
				print HoursMinutes(counter);
			elseif speaking is chatmenu and talkoptions.top_menu
				print capital location.name;
			text to 0

			!if display.linelength >= 60
			print to (display.screenwidth - (StringLength(_temp_string) + 2));

			if (speaking is chatmenu and talkoptions.top_menu) or (STATUSTYPE ~= 0 and (speaking is not chatmenu or not talkoptions.top_menu))
				StringPrint(_temp_string)
		}
		if display.statusline_height = 2
		{
			locate 3, 2
		     if STATUSTYPE = 1
				print number score; " / "; number counter;
		     elseif STATUSTYPE = 2
				print HoursMinutes(counter);
		}
		! now print the available conversational options
		if speaking is chatmenu and talkoptions.top_menu
		{
			local l
			for (l=0; l < talkoptions.gNumAvailableAssertions; l++)
			{
				locate 2, (l+2)		! skip a line
				print number (l+1); ": ";
				print gAvailableAssertions[l].menu_choice;
			}

			if talkoptions.show_topics
			{
				if display.linelength < 61
					locate 3,(display.statusline_height - 1)
				else
					locate 3, display.statusline_height
				ShowTopics
			!	text to 0
				print to (display.screenwidth - (StringLength(_temp_string) + 2));
				if display.linelength < 61
					locate 3,display.statusline_height
				StringPrint(_temp_string)
			}
		}
	}
	color TEXTCOLOR, BGCOLOR
	Font(DEFAULT_FONT)
}
#else
! this object and its properties draw the conversation window
object conversewindow
{
	in printstatuslib
	find_height
	{
		return (call &FindConversationHeight)
	}
	draw_window
	{
		return (call &DrawConversation)
	}
	status_override 0
}

routine FindConversationHeight
{
	local ret
	conversewindow.status_override = false
	if speaking is not chatmenu or not talkoptions.top_menu
	{
		return 0
	}

	if talkoptions.show_topics
!		ret = talkoptions.gNumAvailableAssertions + 4
		ret = CountDialogueLines + 4
	else
!		ret = talkoptions.gNumAvailableAssertions + 3
		ret = CountDialogueLines + 3
	text to _temp_string
	ConverseMessage(&ShowTopics,1) ! "Current topic: ";
	if speaking.current_topic = 0
		ConverseMessage(&ShowTopics,2) ! print "none";
	else
	{
		print speaking.current_topic.name;
		speaking.current_topic is already_listed
	}
	print "\_   ";
	ConverseMessage(&ShowTopics,3) ! print "Available topics:";
	if not CountTopics  ! don't include current topic
	{
		print " none ";
	}
	else
	{
		local a b c
		for a in conversation
		{
			c = 0
			for b in a
			{
				if b.Prerequisite and (b.only_npc = speaking or not b.only_npc)
				{
					c++
					break
				}
			}
			if c>0 and a is not already_listed
			{
				print " ";
				print a.name;
				a is already_listed
			}
		}
		for a in conversation
		{
			a is not already_listed
		}
	}
	text to 0
	if StringLength(_temp_string) + 4 + (TERP = SIMPLE_TERP)*2 >= display.linelength
		ret += 1
	if (TERP & SIMPLE_TERP)
		ret += 1
	conversewindow.status_override = true
	return ret
}

routine DrawConversation
{
	if not (TERP & SIMPLE_TERP)
		locate 3, 1
	else
		print "\_";
	print "Talking to "; Art(speaking);

	! write any upper-right corner text to string
	local x
	x = string(_temp_string, location.name)

	if (x + display.cursor_column) < (display.linelength - 4)
	{
		if (TERP & NORMAL_TERP)
			print to (display.linelength - (x + 2));
		elseif (TERP & GLK_TERP)
			print to (display.linelength - (x + 2));
		elseif (TERP & SIMPLE_TERP)
			print to (display.linelength - (x + 2 + 1));
!		\
!		(TERP & SIMPLE_TERP) - \
!		(printstatuslib.terp_type = GLK_TERP)));
		print capital location.name;
	}

	print "\n"

	! now print the available conversational options
	local l
	for (l=0 ; l < talkoptions.gNumAvailableAssertions; l++)
	{
		print "\_";
		print number (l+1); ": ";
		print gAvailableAssertions[l].menu_choice
	}

	if talkoptions.show_topics
	{
		""
		if not (TERP & SIMPLE_TERP)
			locate 3, display.cursor_row
		else
			print "\_ ";
		ShowTopics

		if ((display.cursor_row < display.windowlines) and
			not (TERP & SIMPLE_TERP))
			locate 3, display.windowlines
		elseif ((display.cursor_row + 1 < display.windowlines) and
			(TERP & SIMPLE_TERP))
			"\n\_ ";
		else
		{
			print to (display.linelength - \
			(StringLength(_temp_string) + 2 - \
			(TERP & GLK_TERP)));
		}
		StringPrint(_temp_string)
	}
}
#endif

routine CountDialogueLines
{
	local sum, n, l, mult

	for (n=0 ; n < talkoptions.gNumAvailableAssertions; n++)
	{
		l = string(_temp_string, gAvailableAssertions[n].menu_choice)
		mult = 0
		do
		{
			sum++
			mult++
		}
		while l > ((display.screenwidth * mult)-3)
	}
	return sum
}

#ifclear _ROODYLIB_H
replace SpeakTo(char)
{
	local TryOrder, IgnoreResponse, retval, stay, same, different
#ifset USE_CHECKHELD
	if verbroutine = &DoDrop_CheckHeld
		verbroutine = &DoDrop
	elseif verbroutine = &DoPutIn_CheckHeld
		verbroutine = &DoPutIn
#endif

#ifset VERBSTUBS
	if verbroutine = &DoHelpChar and object = player
	{
		verbroutine = &DoHelp
		object = nothing
	}
#endif

#ifset USE_CHECKHELD
	ResetCheckHeld
#endif

#ifset DEBUG
	if debug_flags & D_PARSE
	{
		print "\B[Speakto("; char.name;
		if (debug_flags & D_OBJNUM)
			print " ["; number char; "]";
		print ") verbroutine="; number verbroutine;
		print ", object="; object.name;
		if (debug_flags & D_OBJNUM)
			print " ["; number object; "]";
		print ", xobject="; xobject.name;
		if (debug_flags & D_OBJNUM)
			print " ["; number xobject; "]";
		print "]\b"
	}
#endif

	if char is not living
	{
		ParseError(6)  ! "That doesn't make any sense."
		return
	}

	AssignPronoun(char)

	! Handle player/typist-related ParseError messages:
	if char = player
		Message(&Speakto, 1)    ! "Stop talking to yourself..."
	elseif not ObjectisKnown(object) and not FindObject(object, location)
		ParseError(10, object)
	else
	    stay = true

	if not stay
	{
	   speaking = 0
	   return
	}

	if char is unfriendly
		IgnoreResponse = true
	else
	{
		! In the event of:  >CHARACTER, GO NORTH.  GET THE THING.  GO WEST., etc.
		if not FindObject(char, location)
		{
			speaking = char
			run char.order_response
			return true
		}

		same = (char = speaking)

		select verbroutine
			case 0                  ! Just the character name is given,
						! so just "X is listening."
			{
				if not char.order_response
					Message(&Speakto, 2, char)
				retval = true
			}

	#ifclear NO_VERBS
			case &DoHello           ! Note the ampersands ('&')--or else
			{                       ! the routines themselves would run
				if not char.order_response
				{
					if char is not unfriendly
					{
						! "X nods hello."
						Message(&Speakto, 3, char)
						retval = true
					}
					else
					{
						IgnoreResponse = true
					}
				}
				else
					retval = true
			}

			case &DoAskQuestion
				return Perform(&DoAsk, char, object)

			case &DoTalk
			{
				if xobject
					ParseError(6)
				else
					return Perform(&DoAsk, char, object)
			}

			case &DoTell
			{
				if object = player
					return Perform(&DoAsk, char, xobject)
				else
					TryOrder = true
			}
	#endif  ! ifclear NO_VERBS

			case else
			{

	! If the character can respond to a request, this should be dealt with by
	! an order_response property routine; order_response--if it exists--should
	! return false if there is no response for the given verbroutine
				TryOrder = true
			}
	}

	if TryOrder
	{
		if (not char.order_response)
			IgnoreResponse = true
		else
		{
			retval = true
		}
	}

	different = (speaking ~= char)

!	This same/different local variable stuff allows for certain
!	orders to end conversations. If your order_response code clears
!	the speaking global, this code prevents it being reset.

	if retval and not (same and different)
		speaking = char

	if IgnoreResponse
	{
		if not char.ignore_response
			Message(&Speakto, 4, char)      ! "X ignores you."
		speaking = 0  ! clear the speaking global
	}
	return retval
}

! a nice little utility routine for clearing pronouns. feed it an object
! and it will clear any pronoun variable set to that value.
routine ClearPronoun(obj)
{
	select obj
		case it_obj: it_obj = 0
		case her_obj : her_obj = 0
		case him_obj : him_obj = 0
		case them_obj : them_obj = 0
}
#endif  ! ifclear _ROODYLIB_H

!----------------------------------------------------------------------------
! The ASK/TELL verbroutines originally got replaced by converse.h. Now, there's
! a PrintConverseUsage used as a grammar token, so ASK/TELL still works with
! characters not in the npc class (allowing for conversation-hybrid games)

routine PrintConverseUsage(thing)
{

	if not FindObject(thing, location)
	{
		ParseError(11, thing)
		return false
	}
	elseif (thing is not chatmenu) and (speaking is not chatmenu)
		return true

	ConverseMessage(&PrintConverseUsage)

	return false
}

replace DoAskQuestion
{
	if speaking = 0
	{
		VMessage(&DoAskQuestion)        ! "Not talking to anybody..."
		return false
	}
	elseif xobject
	{
		ParseError(6)
		return false
	}
	elseif speaking is chatmenu
	{
		object = speaking
		PrintConverseUsage
		return false
	}
	return Perform(&DoAsk, speaking, object)
}

routine ListOptions
{
	if not talkoptions.gNumAvailableAssertions
		return

	local wordsave
	wordsave = word[1]
	local l
	for (l=0 ; l < talkoptions.gNumAvailableAssertions; l++)
	{
		print "\n";
		Indent
		print "[";
		print number (l+1); "] ";
		print "\"";
		print gAvailableAssertions[l].menu_choice;
		print "\"";
	}

	if talkoptions.show_topics
	{
		"\n"
		ShowTopics
	}
	else
		""
	if talkoptions.conv_force and not talkoptions.top_menu and
	not talkoptions.show_topics
	{
		local a
		a = FancyPrompt
		if (a = 0 and not talkoptions.can_quit) or
		a > talkoptions.gNumAvailableAssertions
		{
			ConverseMessage(&ListOptions,1) ! "Enter a valid choice."
			ForceConvRoutine
		}
		elseif a = 0 and talkoptions.can_quit
		{
			if not DoStopTalk
				ForceConvRoutine
		}
		else
		{
#if defined DESCFORM_I
			if not (FORMAT & DESCFORM_I)
#endif
				""
			if Perform(&DoConverse) and talkoptions.gNumAvailableAssertions and
			speaking
			{
				""
				ConverseMessage(&ListOptions,2) ! "You consider your other
														  !  options."
			}
				ForceConvRoutine
		}
		if undocommands[] = 1 or
#ifset _UNDOLIB_H
			not undolib.enough_elements or
#endif
			wordsave = ""
			undocommands[0] = 1
#endif
		Activate(conversation_end,2)
	}
}

routine ShowTopics
{
	ConverseMessage(&ShowTopics,1) ! "Current topic: ";
	if speaking.current_topic = 0
		ConverseMessage(&ShowTopics,2) ! print "none";
	else
	{
		print speaking.current_topic.name;
		speaking.current_topic is already_listed
	}
	if not talkoptions.top_menu
	{
		local column
		column = display.cursor_column
		if column < (display.screenwidth / 3 - 2)
			print to (display.screenwidth / 3);
		else
			print "\_          ";
	}
	else
		text to _temp_string
	ConverseMessage(&ShowTopics,3) ! print "Available topics:";
	if not CountTopics  ! don't include current topic
	{
		print " none ";
	}
	else
	{
		local a b c
		for a in conversation
		{
			c = 0
			for b in a
			{
				if b.Prerequisite and (b.only_npc = speaking or not b.only_npc)
				{
					c++
					break
				}
			}
			if c>0 and a is not already_listed ! and a is available
			{
				print " ";
				print a.name;
				a is already_listed
			}
		}
		for a in conversation
		{
			a is not already_listed
		}
	}
	if talkoptions.top_menu
		text to 0
	else
		print newline
}

routine CountTopics(include_current_topic)
{
	local a b
	for a in conversation
	{
		if (a is known)
		{
			for b in a
			{
					if b.Prerequisite and (not b.only_npc or
					b.only_npc = speaking)
					{
						if not (not include_current_topic and
						 a = speaking.current_topic)
						{
							return true
						}
					}
			}
		}
	}
	return false
}

routine DoStopTalk(silent)
{
	if speaking = 0
	{
		ConverseVMessage(&DoStopTalk,5) ! "You aren't talking to anybody!"
		return false
	}
	elseif speaking is not chatmenu
	{
		ConverseVMessage(&DoStopTalk, 1) !	"You stop the conversation."
		speaking = 0
		return true
	}
	elseif not talkoptions.can_quit and verbroutine = &DoStopTalk
	{
		if not talkoptions.top_menu and talkoptions.conv_force and
		not talkoptions.show_topics
			ConverseVMessage(&DoStopTalk, 4) ! "Enter a valid choice."
		else
			ConverseVMessage(&DoStopTalk, 3) ! "That doesn't seem possible."
		return false
	}
	elseif speaking is chatmenu
	{
		if not silent
			ConverseVMessage(&DoStopTalk, 2) ! "You end the conversation abruptly."
		speaking = 0
		Activate(conversation_end,2)
!		UNDO_OFF = false
#ifclear _ROODYLIB_H
		if talkoptions.top_menu
		{
			if not system(61) ! MINIMAL_PORT
			{
				window display.statusline_height
				{
					cls
				}
				display.statusline_height = 1
				window 0
				PrintStatusLine
			}
		}
#endif
		return true
	}
}

#ifclear _ROODYLIB_H
replace DoUndo
{
	if not UNDO_OFF
	{
		if speaking ~= 0 and speaking is chatmenu and talkoptions.top_menu
		{
			local c
			c = display.statusline_height
			while word[c] ~= ""
			{
				c++
			}
			word[c] = "statusheight"
		}
		if undo
		{
			local a
			if speaking = 0 and talkoptions.top_menu
			{
				a = 1
				while a < 11
				{
					if word[a] = "statusheight"
					{
						window a
						{ cls }
						window 0
						break
					}
					a++
				}
			}
			PrintStatusline
			DescribePlace(location)
			if word[10] = "test" and not talkoptions.top_menu and speaking
				ListOptions
		}
		else
			VMessage(&DoUndo)
	}
	else
		ConverseVMessage(&DoUndo,2) ! "You can't UNDO at the moment."
}
#endif   ! ifclear _ROODYLIB_H

routine AnyTopicsLeft(new)
{
	local a b c d
	if talkoptions.gNumAvailableAssertions = 0
	{
		for a in conversation
		{
			for b in a
			{
				if b.Prerequisite and (b.only_npc = speaking or not b.only_npc)
				{
					c++
					break
				}
			}
			if c and (a is not already_listed)
			{
				d = 1
				break
			}
		}
		if d
		{
			if talkoptions.auto or (new and not speaking.current_topic)
			{
				if not new
				"\n";
				if not new and talkoptions.show_topics
				{
					Indent
					ConverseMessage(&AnyTopicsLeft,1) ! You decide to change the
																 !  topic.
				}
				SetCurrentTopic(a)
			}
		}
		else
		{
			"\n";
			ConverseMessage(&AnyTopicsLeft,2) ! You can't think of anything else
														 !  to say for now.
			DoStopTalk(1)
!			UNDO_OFF = false
			Activate(conversation_end,2)
		}
	}
}

! a "fancy prompt color global"
global FPCOLOR = BROWN

routine FancyPrompt
{
   ! This routine emulate's ptalk.h's ability to have cool prompts
   ! it requires the top global to be false and the conv_force global to be true
	! and show_topics to be false

   local temp
	Font(BOLD_ON)
	""
	ConverseMessage(&FancyPrompt,1, talkoptions.can_quit)
	!\ "Select an option or 0 to say nothing " or
		"Select an option ", depending on talkoptions.can_quit
	\!
	Font(BOLD_OFF)
   COLOR FPCOLOR
   ConverseMessage(&FancyPrompt,2) ! print ">> ";
   COLOR TEXTCOLOR
   input

! checking for it should add "0" to dictionary table no matter what
	if word[1] = "0" or words = 0 or (word[1] ~= "1", "2", "3", "4", "5")
		return 0

	temp = StringToNumber(word[1])
	if temp ~= 0
		return temp
	else
		return 20
}

#ifset _ROODYLIB_H
object converse_parse
{
	in preparse_instructions
	type settings
	execute
	{
		return ConversePreParse
	}
}
#endif   ! _ROODYLIB_H

routine ConversePreParse
{
	local a

	if not speaking
		return false

	if speaking is chatmenu and words = 1
	{
		for a in conversation
		{
			if word[1] = a.name
			{
				words = 2
				word[2] = word[1]
				word[1] = "topic"
				object = a
				return true
			}
		}
	}
	if talkoptions.conv_force and AnyVerb(location) and
	not AcceptableConversationWords and speaking is chatmenu
	{
		ConverseMessage(&ConversePreParse,1) ! "Enter a valid conversation choice."
		ParseError(100)
		return true
	}
	return false
}

routine AcceptableConversationWords
{
	if word[1] = "1","2","3","4","5","t","topic","topics","undo","save",
	"restore","restart", "script","transcript","quit"
		return true
	else
		return false
}

routine DoTopics
{
	local a, b, c, d,e,f
	d = 0
	if speaking = 0
	{
		ConverseVMessage(&DoTopic,1) ! "You are not currently conversing with
		return false					  !  anyone."
	}

	if speaking is chatmenu
	{
		for e in speaking
		{
			if e.type = conversation
				f = e
		}
		for a in conversation
		{
			for b in a
			{
				if b.Prerequisite and (b.only_npc = speaking or not b.only_npc)
					d = 1
			}
		}
		if d = 0
		{
			ConverseVMessage(&DoTopics,1) ! "You can't think of anything to ask
			return false						!  about."
		}
		ConverseVMessage(&DoTopics,2) ! "Current possible conversation topics:"
		for a in conversation
		{
			c = 0
			for b in a
			{
				if b.Prerequisite and (b.only_npc = speaking or not b.only_npc)
					c = c + 1
			}
			if c>0 and a is not already_listed ! and a is available
			{
				print Art(a)
				print newline
				a is already_listed
			}
		}
		for a in conversation
		{
			a is not already_listed
		}
	}
	else
		ConverseVMessage(&DoTopics,3)  ! There is nothing to talk about.
}

routine ForceConvRoutine
{
	if speaking not in location
		speaking = 0
	if speaking is chatmenu and not talkoptions.gDidSpeak
	{
		local a

		a = nothing
		if speaking.next_assertion
		{
			a = speaking.next_assertion
			speaking.next_assertion = nothing ! i.e. CueAssertion(speaking,
														 !  nothing)
		}
		elseif speaking.force_assertion
		{
			a = speaking.force_assertion
			speaking.force_assertion = nothing	! i.e. ForceCueAssertion(speaking, nothing)
		}
		if a
		{
			SayAssertion(a, false)
			PrintStatusline
			event_flag = 2		! force an interruption
		}
	}
	talkoptions.gDidSpeak = false
	if speaking is chatmenu and speaking in location
	{
		if not talkoptions.top_menu
			ListOptions
		Activate(conversation_end,2)
	}
!	if speaking = 0
!		UNDO_OFF = false
!	if (verbroutine ~= &DoTalk, &DoGreet, &DoHello) and
!	(undocommands[] = 1)
!		undocommands[0] = 0
}

routine ForceStopTalk
{
	if speaking and speaking is chatmenu
	{
		speaking = 0
		Activate(conversation_end,2)
!		UNDO_OFF = false
#ifclear _ROODYLIB_H
		if talkoptions.top_menu
		{
			if not system(61) ! MINIMAL_PORT
			{
				window display.statusline_height
				{
					cls
				}
				display.statusline_height = 1
				window 0
				PrintStatusLine
			}
		}
#endif
		return true
	}
}
#endif    !  _NEWCONVERSE_H
