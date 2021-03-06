!::
! TIMESASKED Version 1.3 by Roody Yogurt
!::

!\
This library contribution tries to make keeping track of how many times you ask
or tell about an object easy, for use with games with elaborate ASK/TELL system.
It also supports listing currently-available topics.

This is the first iteration of this extension!
                               - Roody, January, 2012
\!

!\
Besides including this in your game, you'll need to to declare two constants
BEFORE this is included. The two constants are:

constant TOTALCHARACTERS # ! should hold total number of characters in the game
constant MAXTOPICS # ! total number of topics any character can be asked about

Next, any object that will have multiple responses (so we need to keep track
of times asked/told) should "inherit"  the topics class. Example:

player_character you "you"
{
	inherits topics ! we can ask characters about ourselves
}

Now, every character should inherit the "chartopics" class. Example:

character randomshmoe "Random Schmoe"
{
	inherits chartopics
}

Now, witihn a character's normal DoAsk/DoTell xobject code, you can get the
current number of times asked/told by calling Asked and Told respectively.
Add some code to increase the counter. Take a look at the included example
for assistance.

If you want to make use of the DoAskTellTopics routine which allows players to
see what objects can be currently asked and told about, add the following to
your verb grammar:
xverb			"topics"
*										DoAskTellTopics

If you want the topic listing to *work properly*, you'll want to make your own
CharRules routine. Filling it out with your characters and objects, you can
define rules under which circumstances objects are available. You can even set
it so one topic won't show up unless another object has been asked/told about
by checking Asked(<other object>) or Told(<other object>)

\!

#ifclear _TIMESASKED_H
#set _TIMESASKED_H

#ifset VERSIONS
#message "timesasked.h Version 1.3"
#endif

#ifset USE_EXTENSION_CREDITING
#ifclear _ROODYLIB_H
#message error "Extension crediting requires \"roodylib.h\". Be sure to include it first!"
#endif
version_obj timesasked_version "TimesAsked Version 1.4"
{
	in included_extensions
	desc_detail
		" by Roody Yogurt";
}
#endif

#if undefined TOTALCHARACTERS
constant TOTALCHARACTERS 10
#endif

#if undefined MAXTOPICS
constant MAXTOPICS 10
#endif

array asktopicsleft[MAXTOPICS]
array telltopicsleft[MAXTOPICS]

! some properties to hold these values
property timesasked
!property asktopicsleft
property timestold
!property telltopicsleft

attribute is_topic

! each NPC should get his or her own charnumber, starting at 1, ending at
! the TOTALCHARACTERS value
property charnumber

class topics
{
	timesasked #TOTALCHARACTERS
	timestold #TOTALCHARACTERS
	is is_topic
}

class chartopics
{
	charnumber 0
	inherits topics
!	asktopicsleft #MAXTOPICS
!	telltopicsleft #MAXTOPICS
}

! just a class object for use with non-physical
! conversation topics and such
class topic_object
{
	found_in
		return location
	before
	{
		object
		{
			"Don't be silly."
		}
		xobject
		{
			if verbroutine = &DoAsk,&DoAskQuestion,&DoTell,&DoTalk
				return false
			"Don't be silly."
		}
	}
	is known
}

object timesaskedlib "timesasked"
{
#ifset _ROODYLIB_H
	type settings
	in init_instructions
	did_print 0
	execute
	{
		if not CheckWordSetting("restore") and not CheckWordSetting("undo")
			SetCharNumbers
	}
#endif  ! _ROODYLIB_H
}

routine SetCharNumbers
{
	local i = 1
	local n
	while i <= objects
	{
		if i.#charnumber and i ~= chartopics
		{
			i.charnumber = ++n
		}
		i++
	}
}

!\ ChangeValueAsked - utility routine for manually changing the
'timeasasked' property for a specific character (not actually called
by anything \!
routine ChangeValueAsked(obj,char,value)
{
	obj.timesasked #(char) = obj.timesasked #(char) + value
}

routine Asked(obj)  ! only meant to be called by character's DoAsk code
{                   ! returns the 'times asked' number of the current object
                    ! being asked about
	if not obj
		obj = xobject
	return obj.timesasked #(speaking.charnumber)
}

routine Told(obj) ! only mean to be called by character's DoTell code
{                 ! returns the 'times told' number of the current object
                  ! being asked about
	if not obj
		obj = xobject
	return obj.timestold #(speaking.charnumber)
}


! ArrayCheck checks an object's property elements
! and returns true when it finds a value

routine ArrayCheck(arr)
{
	if array arr[0]
		return true
	else
		return false
}

!\ TopicsList is a modified version of PropertyList. "you" as a topic gets
changed to "yourself" and the speaker is listed as "himself" or "herself" \!

routine TopicsList(arr, artic)
{
	local a, b, n, total

	for (a=0; a< MAXTOPICS; a++)
	{
		if array arr[a]:  total++
	}

	for (a=0; a< MAXTOPICS; a++)
	{
		b = array arr[a]
		if b
		{
			if artic
				The(b)
			else
			{
				if b = speaking or b = player
					print b.pronoun #4;
				else
					Art(b)
			}

			if (a+1) < total and total > 2
				print ", ";
			elseif (a+1) = total-1:  print " ";

			if a+1 = total - 1
				print AND_WORD; " ";
			n++
		}
	}
	return n
}


routine DoAskTellTopics
{
	if not speaking
	{
		TimesAskedVMessage(&DoAskTellTopics,1) !	"You are not talking to anyone
															!   at the moment."
		return false
	}

	local i,j

! time to clear the character's asktopicsleft and telltopicsleft properties
!   so we can refill them

	for (i=0; i<MAXTOPICS; i++)
	{
		asktopicsleft[i] = 0
		telltopicsleft[i] = 0
	}

	verbroutine = &DoAsk
	for (i=29; i<=objects; i++)
	{
		if i is is_topic and CharRules(i)
			asktopicsleft[j++] = i
	}

	j = 0

	verbroutine = &DoTell
	for (i=29; i<=objects; i++)
	{
		if i is is_topic and CharRules(i)
			telltopicsleft[j++] = i
	}

	local m,n
	TimesAskedVMessage(&DoAskTellTopics,5) ! "("
	if ArrayCheck(asktopicsleft)
	{
		TimesAskedVMessage(&DoAskTellTopics,2) ! "You can ask <char> about..."
		m = 1
	}
	if ArrayCheck(telltopicsleft)
	{
		if m
			print " ";
		TimesAskedVMessage(&DoAskTellTopics, 3 , m) ! "You can tell <char>
																  ! about..."
		n = 1
	}
	if not n and not m
		TimesAskedVMessage(&DoAskTellTopics,4) ! "You have nothing to talk about
												         !  at this time."
	TimesAskedVMessage(&DoAskTellTopics,5,1)  ! ")"
}

routine TimesAskedVMessage(r, num, a, b)
{
	if NewTimesAskedVMessages(r, num, a, b):  return

	select r
		case &DoAskTellTopics
		{
			select num
				case 1: "You are not talking to anyone at the moment."
				case 2
				{
					print "You can ask "; speaking.name ; " about ";
					TopicsList(asktopicsleft)
					print ".";
				}
				case 3
				{
					print "You can tell ";
					if a
						print speaking.pronoun #2 ;
					else
						print speaking.name ;
					print " about ";
					TopicsList(telltopicsleft)
					print ".";
				}
				case 4
				{
					print "You have nothing to talk about at this time.";
				}
				case 5
				{
					if not a
						print "(";
					else
						print ")"
				}
		}
}

!\ The NewTimesAskedVMessages routine may be REPLACED and should return
true if a replacement message exists for routine <r> \!

routine NewTimesAskedVMessages(r, num, a, b)
{
	return false
}

routine CharRules(obj)
{
	return true
}

#endif
