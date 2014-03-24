!\ Changes from Original Teulbachs version-
	prep properties no longer require a space
	tweaked it so certain code is not run as many times
			                  - Roody
\!

! Following is a handy little routine for Hugo 2.5+ that provides exit
! descriptions for rooms. All you need to do to use it is make sure you
! use the dir.to properties to code your exits, and *don't put any other
! code in them*. For example,
!
!       e_to DinkyRoom
!
! is acceptable, while something like
!
!       e_to { move thingie to player : return DinkyRoom }
!
! is not. Also, each location that needs to be referred to with a preposition
! needs to be supplied with one. For example,
!
!       room street "street running in front of the park"
!       { ...
!         prep "a"
!         ...
!       }
!
! Now, all you need to do to make this thing work is to call
! YouCanGo() after each room's long description:
!
!       room street "street running in front of the park"
!       { ...
!         prep " a"
!         long_desc { "You are on a narrow street fronting the park."
!                     YouCanGo()
!                   }
!       }
!
! That's it! Everything else is automatic, including door-handling.
! Enjoy! This document copyright (c) 2000 by Cardinal Teulbachs
!                                    and 2013 by Roody Yogurt
!::
! Can Go Lite
!::


#ifset VERSIONS
#message "can_go_lite.h Version 1.0"
#endif
#ifclear _CAN_GO_LITE
#set _CAN_GO_LITE

#ifset USE_EXTENSION_CREDITING
#ifclear _ROODYLIB_H
#message error "Extension crediting requires \"roodylib.h\". Be sure to include it first!"
#endif
version_obj can_go_version "Can Go Lite 1.0"
{
	in included_extensions
	desc_detail
		" by Cardinal Teulbachs and Roody Yogurt";
}
#endif


array go_dirs[13]
constant SKIP 2
property cango_name

routine YouCanGo
{
	local n, p, cnt, the_dir

	go_dirs[1] = location.n_to
	go_dirs[2] = location.s_to
	go_dirs[3] = location.e_to
	go_dirs[4] = location.w_to
	go_dirs[5] = location.ne_to
	go_dirs[6] = location.nw_to
	go_dirs[7] = location.se_to
	go_dirs[8] = location.sw_to
	go_dirs[9] = location.in_to
	go_dirs[10] = location.out_to
	go_dirs[11] = location.u_to
	go_dirs[12] = location.d_to

   for(n = 1; n <= 12; n++)
   {
		if go_dirs[n] and go_dirs[n].cango_name ~= SKIP
		{
			go_dirs[0] = 1
			break
		}
		else
			go_dirs[0] = 0
   }

   if go_dirs[0] = 0                          ! if nowhere to go
   {
		Indent(true)
		"There are no obvious exits from here."
		return true                                ! bail out
   }

	Indent(true)
   "You can go ";
	local total
	for (p=1; p<=12; p++)         ! determine whether list end
	{
	if go_dirs[p] ~= 0 and go_dirs[p].cango_name ~= SKIP
		cnt++
	}

   for (n=1; n<=12; n++)                ! begin Exits loop
   {
		if go_dirs[n] ~= 0                 ! skip empty array elements
		{
			if go_dirs[n].type = door
			{
				local x
				for (x=1; x<=2; x++)
				{
					if go_dirs[n].between#x ~= location
						the_dir = go_dirs[n].between#x
				}
			}
			else
				the_dir = go_dirs[n]

			total++
			select go_dirs[n]
				case location.n_to: print "north to";
				case location.s_to: print "south to";
				case location.e_to: print "east to";
				case location.w_to: print "west to";
				case location.ne_to: print "northeast to";
				case location.nw_to: print "northwest to";
				case location.se_to: print "southeast to";
				case location.sw_to: print "southwest to";
				case location.in_to: print "in to";
				case location.out_to: print "out to";
				case location.u_to: print "up to";
				case location.d_to: print "down to";
				case else: total--

			if total
			{
				RoomName(the_dir)
			}

			cnt--
			if cnt = 1 and total > 1: ", or ";      ! choose appropriate punct.
			elseif cnt = 1 : " or ";
			elseif cnt >= 2 : ", ";
			elseif cnt = 0 : { "." : break }        ! and break if end

			go_dirs[n] = 0
		}
   }
	return true                                  ! return true to advance turn
}

routine RoomName(obj)
{
	if obj.prep
	{
		print " ";
		print obj.prep;
	}
	print " ";
	if obj.cango_name
	{
		print obj.cango_name;
	}
	else
		print obj.name;
}

#endif ! _CAN_GO_LITE