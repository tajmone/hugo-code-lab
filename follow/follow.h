!\---------------------------------------------------------------------------

	follow.h version 2.0

	Mechanisms for enabling NPCs to follow the player

	by Kent Tessman

	Updated for general usage by Jon Blask
	(See original at http://roody.gerynarsabode.org/follow.hug)

	[Original code also has a routine for counting followers,
	support for a follower-of-a-follower, and multimedia stuff- as
	they pertain to Future Boy!]

---------------------------------------------------------------------------\!
#ifclear _FOLLOW_H
#set _FOLLOW_H

#ifset VERSIONS
#message "Follow.h Version 2.0"
#endif

#ifset USE_EXTENSION_CREDITING
#ifclear _ROODYLIB_H
#message error "Extension crediting requires \"roodylib.h\". Be sure to include it first!"
#endif
version_obj follow_version "Follow Version 2.0"
{
	in included_extensions
	desc_detail
		" by Kent Tessman";
}
#endif

constant MAX_FOLLOWERS 8

array follower_list[MAX_FOLLOWERS]
array moved_follower[MAX_FOLLOWERS]
attribute following
global followers
global following_paused

property follow_message		! for locations, a suffix such as
									! " you down the stairs."

property disallow_follow 	! return true if circumstances disallow
									! following (for characters or locations)

property pause_following	! for NPCs

routine AddFollower(char)
{
	if followers = MAX_FOLLOWERS or char is following
		return false
	follower_list[followers++] = char
	char is following
	return true
}

routine RemoveFollower(char)
{
	local i
	for (i=0; i<MAX_FOLLOWERS; i++)
	{
		if follower_list[i] = char
		{
			while (i < MAX_FOLLOWERS-2)
			{
				follower_list[i] = follower_list[i+1]
				i++
			}
			follower_list[MAX_FOLLOWERS-1] = 0
			followers--
			char is not following
			return
		}
	}
}

!\
	StartFollowing and StopFollowing are just two "helper" routines so authors
	don't need to remember that the following mechanism is handled by a
	daemon (and therefore needs to be activated or deactivated)

	If used with roodylib, you can check if starting the following was
	successful with code like:
		if StartFollowing
			"Follow code was successful!"

	If your game starts with a character following the PC,
	call StartFollowing *after* the player global is set.
\!

routine StartFollowing
{
	return Activate(follow_daemon)
}

routine StopFollowing
{
	return Deactivate(follow_daemon)
}

daemon follow_daemon
{
}

event in follow_daemon
{
	! Had to add DoCrawl for >CRAWL beneath the trees
!	if (verbroutine = &DoGo, &DoExit, &DoCrawl) and location ~= old_location
! ^ Future Boy! has a DoCrawl verb
	if (verbroutine = &DoGo, &DoExit, &DoEnter) and location ~= old_location
		MoveFollowers
}

routine MoveFollowers
{
	local i, mf

	if followers = 0
		return

	if location.disallow_follow
		return

	! Don't allow following through or during a teleport sequence
	! Roody's note: I don't really understand what the next two lines do, but
	! I tried to change them to be less Hugo-shell-centric (since not everybody
	! names their player object "you"
!	if player ~= you
!		return

#ifclear _ROODYLIB_H
	if player.type ~= character
		return
#else
	if player.type ~= player_character
		return
#endif

	for (i=0; i<followers; i++)
	{
		if follower_list[i] in old_location and
			follower_list[i] not in location and
			not follower_list[i].pause_following and
			not follower_list[i].disallow_follow
		{

			move follower_list[i] to location
			moved_follower[mf] = follower_list[i]

			mf++

		}
	}

	if (mf)
	{
		print ""
		FollowMessage(&MoveFollowers,1,mf) ! "So-and-so follows you."
	}
}


#if undefined PrintArrayList
! taken from misc.h ... called from MoveFollowers:

! PrintArrayList(list, count)
! Prints count elements of the array list.

routine PrintArrayList(list, count)
{
	local i
	for (i=1; i<=count; i++)
	{
		if i > 1:  " ";
		The(array list[i-1])
		if i < count and count > 2:  ",";
		if i = count-1:  print " "; AND_WORD;
	}
	return false
}
#endif ! if undefined PrintArrayList

routine FollowMessage(r, num, a, b)
{
	if NewFollowMessages(r, num, a, b):  return

	select r
	case &MoveFollowers
		{
		select num
			case 1:
				{
				print capital PrintArrayList(moved_follower, a);
				" follow";
				if a = 1 and moved_follower[0] is not plural:  "s";
				if not location.follow_message
					{
					print " ";
					if player_person = 1,2
						{
						print player.pronoun #2;
						}
					else
						print player.name;
					"."
					}
				event_flag = true
				}
		}

}

routine NewFollowMessages(r, num, a, b)
{
	return false
}
#endif ! #ifset _FOLLOW_H