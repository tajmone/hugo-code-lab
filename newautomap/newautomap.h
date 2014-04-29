! NewAutomap.h Version 1.7
! By Cardinal Teulbachs, updated by Jon Blask

#ifclear _NEWAUTOMAP_H
#set _NEWAUTOMAP_H

#ifset VERSIONS
#message "NewAutomap.h Version 1.7"
#endif

#ifset USE_EXTENSION_CREDITING
#ifclear _ROODYLIB_H
#message error "Extension crediting requires \"roodylib.h\". Be sure to include it first!"
#endif
version_obj automap_version "NewAutomap Version 1.7"
{
	in included_extensions
	desc_detail
		" by Cardinal Teulbachs and Roody Yogurt";
}
#endif

!----------------------------------------------------------------------------
!                           VARIABLES AND STUFF

property map_list
array from_array[8]


#ifclear _WINDOW_H
#message "Include window.h first if you want to use newautomap.h's
send-map-to-window functionality."
#endif ! _WINDOW_H
global map_window ! if there is a special window you'd like to show the map in,
                  ! set this global to that (requires "windows.h")

property CENTER_MAP_HORZ alias u_to
property CENTER_MAP_VERT alias d_to
property iterating alias e_to
property mapping_door alias s_to
property AUTOMAP_ON alias n_to
property MAP_ON alias in_to
property FULL_ON alias se_to
property map_object alias nw_to
property map_fore alias w_to
property map_back alias ne_to
property field_color alias out_to

object automaplib
{
	CENTER_MAP_HORZ 0
	CENTER_MAP_VERT 0
	iterating 0
	mapping_door 0
	AUTOMAP_ON 0
	MAP_ON 0
	FULL_ON 0
	map_fore
		return BGCOLOR
	map_back
		return TEXTCOLOR
	field_color
		return BGCOLOR
	map_object 0
! if roodylib.h has been included before newautodraw.h, nothing needs to be
! added to the main routine
#ifset _ROODYLIB_H
type settings
	in main_instructions
	execute
	{
		if not BlockMap
			MapSet
	}
#endif  ! _ROODYLIB_H
}

property player_char alias noun
property room_char alias adjective

object map_chars
{
	u_to "u"
	d_to "d"
	in_to "i"
	out_to "o"
	room_char
	{
		if system(61) ! glk in this instance
			return "\#164"
		else
			return " "
	}
	player_char
	{
		if system(61) ! glk in this instance
			return "O"
		else
			return "*"
	}
#ifset _ROODYLIB_H
type settings
#endif  ! _ROODYLIB_H
}

replace room
{
	type room
	is static, light, open
	map_list #13
}

!---------------------------------------------------------------------------
!                               THE ROUTINES

routine FullMap(from)
{
	local home_loc, z, tot, connex, neighbor, sw1, glktest, i
	from = Map(from, true)
	home_loc = from
	from.map_list#13 = true
	glktest = system(61)

	for (z = 5; z <= 12; z++)
	{
		if from.map_list#z and from.map_list#z ~= from
		{
			from_array[tot] = from.map_list#z
			from_array[tot].map_list#3 = from
			tot++
		}
	}

	automaplib.iterating = true
	for (z = 5; z <= 12; z++)
	{
		if from.map_list#z ~= 0 and from.map_list#z ~= from
		{
			connex = from.map_list#z
			automaplib.CENTER_MAP_HORZ = connex.map_list#1
			automaplib.CENTER_MAP_VERT = connex.map_list#2
			automaplib.iterating = true
			Map(connex, true)
			connex.map_list#13 = true
		}
	}

:IterateHome

	tot--
	from = from_array[tot]

:IterateNeighbor

	if from = home_loc
		jump IterateHome
	sw1 = false

	for (z = 5; z <= 12; z++)
	{
		if from.map_list#z and from.map_list#z ~= from
		{
			neighbor = from.map_list#z

			if neighbor ~= from.map_list#3 and
				neighbor.map_list#4 = 0 and
				neighbor.map_list#13 = 0
			{
				neighbor.map_list#3 = from
				automaplib.CENTER_MAP_HORZ = neighbor.map_list#1
				automaplib.CENTER_MAP_VERT = neighbor.map_list#2
				automaplib.iterating = true
				Map(neighbor, true)
				neighbor.map_list#13 = true
				sw1 = true
				from = neighbor
				break
			}
		}
	}

	if sw1
		jump IterateNeighbor

	from.map_list#4 = true
	from = from.map_list#3

	if from = home_loc, 0
	{
		if tot <= 0
		{
#ifset _WINDOW_H
			if map_window
			{
				if map_window.win_textcolor ~= map_window.win_backcolor
					color map_window.win_backcolor, map_window.win_textcolor
				else
					color automaplib.map_fore, automaplib.map_back
			}
			else
#endif ! _WINDOW_H
				color automaplib.map_fore, automaplib.map_back
			locate (home_loc.map_list#1), (home_loc.map_list#2)
			for (i=u_to;i<=out_to ;i++ )
			{
				if home_loc.i
					print map_chars.i;
				else
				{
					Font(BOLD_ON)
					print map_chars.player_char;
	Font(BOLD_OFF)
				}
			}

			locate 1, 1
#ifset _WINDOW_H
			if map_window
			{
				if map_window.win_textcolor ~= map_window.win_backcolor
					color map_window.win_textcolor, map_window.win_backcolor
				else
					color automaplib.map_back, automaplib.map_fore
			}
			else
#endif ! _WINDOW_H
				color automaplib.map_back, automaplib.map_fore
			automaplib.iterating = false

			for (z = 1; z <= objects; z++)
			{
				if z.type = room
				{
					z.map_list#4 = 0
					z.map_list#13 = 0
				}
				if z <= 8
					from_array[z] = 0
			}
			locate 11, 1
			if not map_window
			{
				AutomapMessage(&FullMap,1) ! "done"
				Font(BOLD_ON)
				AutomapMessage(&FullMap,2) ! "PRESS A KEY TO CONTINUE"
				Font(BOLD_OFF)
			}

			if not glktest and not map_window
			{
				locate 1, display.windowlines
				local key
				key = system(11) ! READ_KEY
				if system_status
				{
					pause
				}
				else
				{
					while true
					{
						key = system(11) ! READ_KEY
						if key
						{
							word[0] = key
							break
						}
						system(32) ! PAUSE_100TH_SECOND
					}
				}
				color TEXTCOLOR, BGCOLOR
				cls
				locate 1,10
				DescribePlace
			}
			return from
		}
		else
		{
			jump IterateNeighbor
		}
	}
	else
	{
		if from = home_loc
			jump IterateHome
		else
			jump IterateNeighbor
	}
}


routine Map(from, full)
{
	local dir, a, g, startpos, startrow, neighbor,glktest
!Font(PROP_OFF)
	glktest = system(61)
	if not automaplib.iterating
	{
		if not from or from.type ~= room
			from = location
		if from is not visited or not FindLight(from)
		{
#ifset _WINDOW_H
			if map_window
			{
				if map_window.win_textcolor ~= map_window.win_backcolor
					color map_window.win_textcolor, map_window.win_backcolor
				else
					color automaplib.map_fore, automaplib.map_back
				cls
				color automaplib.map_back, automaplib.map_fore
				return
			}
#endif ! _WINDOW_H
			AutomapMessage(&Map,1) ! "You wish."
			return false
		}
		if verbroutine ~= &DoWait, &DoWaitUntil
			""
	}

	automaplib.mapping_door = from                  ! door-handling switch

	if not automaplib.iterating
	{
		if not full
		{
			automaplib.CENTER_MAP_HORZ = display.linelength - 10
			if glktest
				automaplib.CENTER_MAP_VERT = 5
			else
				automaplib.CENTER_MAP_VERT = 4

			startpos = automaplib.CENTER_MAP_HORZ - 7
			startrow = automaplib.CENTER_MAP_VERT - 3

			color automaplib.map_fore, automaplib.map_back

			for (a=1; a<=7; a++)
			{
				locate startpos, startrow
				print to (automaplib.CENTER_MAP_HORZ + 10)
				startrow += 1
			}
		}
		else
		{
			automaplib.CENTER_MAP_HORZ = display.linelength / 2
			automaplib.CENTER_MAP_VERT = display.windowlines / 2

			from.map_list#1 = automaplib.CENTER_MAP_HORZ
			from.map_list#2 = automaplib.CENTER_MAP_VERT
			cls
			locate 1, 1
			if not map_window
				AutomapMessage(&Map,2) !  ! "mapping..."
		}
	}
! tried
	local m
	for (dir=0; dir<=11; dir++)
	{
		m = 0
		select dir
			case 0
			{
				if from.nw_to
				{
					startpos = automaplib.CENTER_MAP_HORZ - 5
					startrow = automaplib.CENTER_MAP_VERT - 2
					neighbor = from.nw_to
					from.map_list#5 = neighbor
				}
			}
			case 1
			{
				if from.n_to
				{
					startpos = automaplib.CENTER_MAP_HORZ
					startrow = automaplib.CENTER_MAP_VERT - 2
					neighbor = from.n_to
					from.map_list#6 = neighbor
				}
			}
			case 2
			{
				if from.ne_to
				{
					startpos = automaplib.CENTER_MAP_HORZ + 5
					startrow = automaplib.CENTER_MAP_VERT - 2
					neighbor = from.ne_to
					from.map_list#7 = neighbor
				}
			}
			case 3
			{
				if from.w_to
				{
					startpos = automaplib.CENTER_MAP_HORZ - 5
					startrow = automaplib.CENTER_MAP_VERT
					neighbor = from.w_to
					from.map_list#8 = neighbor
				}
			}
			case 4
			{
				m = u_to
#ifset _WINDOW_H
				if map_window
				{
					if map_window.win_textcolor ~= map_window.win_backcolor
					{
						color map_window.win_backcolor, map_window.win_textcolor
					}
					else
						color automaplib.map_fore, automaplib.map_back
				}
				else
#endif ! _WINDOW_H
					if not full
						color automaplib.map_back, automaplib.map_fore
					else
						color automaplib.map_fore, automaplib.map_back
					neighbor = -1
			}
			case 5
			{
				m = d_to
				neighbor = -1
			}
			case 6
			{
				m = in_to
				neighbor = -1
			}
			case 7
			{
				m = out_to
				neighbor = -1
			}
			case 8
			{
#ifset _WINDOW_H
				if map_window
				{
					if map_window.win_textcolor ~= map_window.win_backcolor
					{
						color map_window.win_textcolor, map_window.win_backcolor
					}
					else
						color automaplib.map_back, automaplib.map_fore
				}
				else
#endif ! _WINDOW_H
					if not full
						color automaplib.map_fore, automaplib.map_back
					else
						color automaplib.map_back, automaplib.map_fore
					if from.e_to
					{
						startpos = automaplib.CENTER_MAP_HORZ + 5
						startrow = automaplib.CENTER_MAP_VERT
						neighbor = from.e_to
						from.map_list#9 = neighbor
					}
			}
			case 9
			{
				if from.sw_to
				{
					startpos = automaplib.CENTER_MAP_HORZ - 5
					startrow = automaplib.CENTER_MAP_VERT + 2
					neighbor = from.sw_to
					from.map_list#10 = neighbor
				}
			}
			case 10
			{
				if from.s_to
				{
					startpos = automaplib.CENTER_MAP_HORZ
					startrow = automaplib.CENTER_MAP_VERT + 2
					neighbor = from.s_to
					from.map_list#11 = neighbor
				}
			}
			case 11
			{
				if from.se_to
				{
					startpos = automaplib.CENTER_MAP_HORZ + 5
				startrow = automaplib.CENTER_MAP_VERT + 2
				neighbor = from.se_to
				from.map_list#12 = neighbor
				}
			}

		if neighbor ~= 0, -1                     ! and not iterating
		{
			if neighbor ~= from
			{
				neighbor.map_list#1 = startpos
				neighbor.map_list#2 = startrow
				DrawMapLoc(neighbor, startpos, startrow, full)
			}
		}
		if m
		{
			locate (automaplib.CENTER_MAP_HORZ + dir - 4), \
			automaplib.CENTER_MAP_VERT

			if from is visited and
				automaplib.CENTER_MAP_HORZ >= 1 and
				automaplib.CENTER_MAP_HORZ <=		display.linelength and
				automaplib.CENTER_MAP_VERT >= 1 and
				automaplib.CENTER_MAP_VERT <= (display.windowlines )
			{
				if from.m
					print map_chars.m;
				else
				{
					Font(BOLD_ON)
					if not automaplib.iterating
						print map_chars.player_char;
					Font(BOLD_OFF)
				}
			}
		}
		locate 1,1
	}

	if from.nw_to and from is visited
	{
		locate (automaplib.CENTER_MAP_HORZ - 1), \
		(automaplib.CENTER_MAP_VERT - 1)
		if (automaplib.CENTER_MAP_HORZ - 1) >= 1 and
			(automaplib.CENTER_MAP_HORZ - 1) <= display.linelength and
			(automaplib.CENTER_MAP_VERT - 1) >= 1 and
			(automaplib.CENTER_MAP_VERT - 1) <= (display.windowlines) ! - 2
		{
			if from.w_to
			{
				g = from.w_to
				if g.ne_to and g is visited and g ~= from
					print "X";
				else
					print "\\\_";
			}
			elseif from.n_to
			{
				g = from.n_to
				if g.sw_to and g is visited and g ~= from
					print "X";
				else
					print "\\\_";
			}
			else
				print "\\\_";
		}
		locate 1,1
	}
	if from.n_to and from is visited
	{
		locate (automaplib.CENTER_MAP_HORZ + 2), \
		(automaplib.CENTER_MAP_VERT - 1)
		if (automaplib.CENTER_MAP_HORZ + 2) >= 1 and
		(automaplib.CENTER_MAP_HORZ + 2) <= display.linelength and
		(automaplib.CENTER_MAP_VERT - 1) >= 1 and
		(automaplib.CENTER_MAP_VERT - 1) <= (display.windowlines) ! - 2
		{
			print "|";
		}
		locate 1,1
	}
	if from.ne_to and from is visited
	{
		locate (automaplib.CENTER_MAP_HORZ + 4), \
		(automaplib.CENTER_MAP_VERT - 1)
		if (automaplib.CENTER_MAP_HORZ + 4) >= 1 and
			(automaplib.CENTER_MAP_HORZ + 4) <= display.linelength and
			(automaplib.CENTER_MAP_VERT - 1) >= 1 and
			(automaplib.CENTER_MAP_VERT - 1) <= (display.windowlines) ! - 2
		{
			if from.e_to
			{
				g = from.e_to
				if g.nw_to and g is visited and g ~= from
					print "X";
				else
					print "/";
			}
			elseif from.n_to
			{
				g = from.n_to
				if g.se_to and g is visited and g ~= from
					print "X";
				else
					print "/";
			}
			else
				print "/";
		}
		locate 1,1
	}
	if from.w_to and from is visited
	{
		locate (automaplib.CENTER_MAP_HORZ - 1), automaplib.CENTER_MAP_VERT
		if (automaplib.CENTER_MAP_HORZ - 1) >= 1 and
			(automaplib.CENTER_MAP_HORZ - 1) <= display.linelength and
			(automaplib.CENTER_MAP_VERT) >= 1 and
			(automaplib.CENTER_MAP_VERT) <= (display.windowlines) ! - 2
		{
			print "-";
		}
		locate 1,1
	}
	if from.e_to and from is visited
	{
		locate (automaplib.CENTER_MAP_HORZ + 4), automaplib.CENTER_MAP_VERT
		if (automaplib.CENTER_MAP_HORZ + 4) >= 1 and
			(automaplib.CENTER_MAP_HORZ + 4) <= display.linelength and
		(automaplib.CENTER_MAP_VERT) >= 1 and
		(automaplib.CENTER_MAP_VERT) <= (display.windowlines) ! - 2
		{
			print "-";
		}
		locate 1,1
	}

	if from.sw_to and from is visited
	{
		locate (automaplib.CENTER_MAP_HORZ - 1), \
		(automaplib.CENTER_MAP_VERT + 1)
		if (automaplib.CENTER_MAP_HORZ - 1) >= 1 and
			(automaplib.CENTER_MAP_HORZ - 1) <= display.linelength and
			(automaplib.CENTER_MAP_VERT + 1) >= 1 and
			(automaplib.CENTER_MAP_VERT + 1) <= (display.windowlines)  ! changed south to -1 from -2
		{
			if from.w_to
			{
				g = from.w_to
				if g.se_to and g is visited and g ~= from
					print "X";
				else
					print "/";
			}
			elseif from.s_to
			{
				g = from.s_to
				if g.nw_to and g is visited and g ~= from
					print "X";
				else
					print "/";
			}
			else
				print "/";
		}
		locate 1,1
	}

	if from.s_to and from is visited
	{
		locate (automaplib.CENTER_MAP_HORZ + 2), \
		(automaplib.CENTER_MAP_VERT + 1)
		if (automaplib.CENTER_MAP_HORZ + 2) >= 1 and
			(automaplib.CENTER_MAP_HORZ + 2) <= display.linelength and
			(automaplib.CENTER_MAP_VERT + 1) >= 1 and
			(automaplib.CENTER_MAP_VERT + 1) <= (display.windowlines) ! - 2
		{
			print "|";
		}
		locate 1,1
	}
	if from.se_to and from is visited
	{
		locate (automaplib.CENTER_MAP_HORZ + 4), \
		(automaplib.CENTER_MAP_VERT + 1)
		if (automaplib.CENTER_MAP_HORZ + 4) >= 1 and
			(automaplib.CENTER_MAP_HORZ + 4) <= display.linelength and
			(automaplib.CENTER_MAP_VERT + 1) >= 1 and
			(automaplib.CENTER_MAP_VERT + 1) <= (display.windowlines) ! - 2
		{
			if from.e_to
			{
				g = from.e_to
				if g.sw_to and g is visited and g ~= from
					print "X";
				else
					print "\\\_";
			}
			elseif from.s_to
			{
				g = from.s_to
				if g.ne_to and g is visited and g ~= from
					print "X";
				else
					print "\\\_";
			}
			else
				print "\\\_";
		}
		locate 1,1
	}

	automaplib.mapping_door = false
	if not full
		color TEXTCOLOR, BGCOLOR
! Font(DEFAULT_FONT)
	return from
}


routine DrawMapLoc(loc, startpos, startrow, full)
{
	local dir, glktest, m
	glktest = system(61)
	if loc is not visited
		return false

	for (dir=0; dir<=11; dir++)
	{
		m = 0
		select dir
			case 0
			{
				if loc.nw_to
				{
					locate (startpos - 1), (startrow -1)
					if (startpos - 1) >= 1 and
					(startpos - 1) <= display.linelength and
					(startrow - 1) >= 1 and
					(startrow - 1) <= (display.windowlines) ! - 2
					{
						print "\\\_";
					}
				}
			}
			case 1
			{
				if loc.n_to
				{
					locate (startpos + 2), (startrow - 1)
					if (startpos + 2) >= 1 and
						(startpos + 2) <= display.linelength and
						(startrow - 1) >= 1 and
						(startrow - 1) <= (display.windowlines) ! -2
					{
						print "|";
					}
				}
			}
			case 2
			{
				if loc.ne_to
				{
					locate (startpos + 4), (startrow - 1)
					if (startpos + 4) >= 1 and
						(startpos + 4) <= display.linelength and
						(startrow - 1) >= 1 and
						(startrow - 1) <= (display.windowlines) ! -2
					{
						print "/";
					}
				}
			}
			case 3
			{
				if loc.w_to
				{
					locate (startpos - 1), startrow
					if (startpos - 1) >= 1 and
						(startpos - 1) <= display.linelength and
						(startrow) >= 1 and
						(startrow) <= (display.windowlines) ! - 2
					{
						print "-";
					}
				}
			}
			case 4
			{
				locate startpos, startrow
				if not full
					color TEXTCOLOR, BGCOLOR
#ifset _WINDOW_H
				elseif map_window
				{
					if map_window.win_textcolor and map_window.win_backcolor
					{
						color map_window.win_backcolor, map_window.win_textcolor
					}
					else
						color BGCOLOR, TEXTCOLOR
				}
#endif ! _WINDOW_H
				else
					color BGCOLOR, TEXTCOLOR
				if (startpos) >= 1 and (startpos) <= display.linelength and
					(startrow) >= 1 and (startrow) <= (display.windowlines) ! -2
				{
					m = u_to
				}
			}
			case 5
			{
				locate (startpos + 1), startrow
				if (startpos + 1) >= 1 and (startpos + 1) <= display.linelength and
					(startrow) >= 1 and (startrow) <= (display.windowlines) ! -2
				{
					m = d_to
				}
			}
			case 6
			{
				locate (startpos + 2), startrow
				if (startpos + 2) >= 1 and (startpos + 2) <= display.linelength and
					(startrow) >= 1 and (startrow) <= (display.windowlines) ! -2
				{
					m = in_to
				}
			}
			case 7
			{
				locate (startpos + 3), startrow
				if (startpos + 3) >= 1 and (startpos + 3) <= display.linelength and
					(startrow) >= 1 and (startrow) <= (display.windowlines) ! -2
				{
					m = out_to
				}
			}
			case 8
			{
				if not full
					color BGCOLOR, TEXTCOLOR
#ifset _WINDOW_H
				elseif map_window
				{
					if map_window.win_textcolor ~= map_window.win_backcolor
					{
						color map_window.win_textcolor, map_window.win_backcolor
					}
					else
						color TEXTCOLOR, BGCOLOR
				}
#endif
				else
					color TEXTCOLOR, BGCOLOR
				if loc.e_to
				{
					locate (startpos + 4), startrow
					if (startpos + 4) >= 1 and
						(startpos + 4) <= display.linelength and
						(startrow) >= 1 and (startrow) <= (display.windowlines) ! -2
					{
						print "-";
					}
				}
			}
			case 9
			{
				if loc.sw_to
				{
					locate (startpos - 1), (startrow + 1)
					if (startpos - 1) >= 1 and
						(startpos - 1) <= display.linelength and
						(startrow + 1) >= 1 and
						(startrow + 1) <= (display.windowlines)
					{
						print "/";
					}
				}
			}
			case 10
			{
				if loc.s_to
				{
					locate (startpos + 2), (startrow + 1)
					if (startpos + 2) >= 1 and
						(startpos + 2) <= display.linelength and
						(startrow + 1) >= 1 and
						(startrow + 1) <= (display.windowlines)
					{
						print "|";
					}
				}
			}
			case 11
			{
				if loc.se_to
				{
					locate (startpos + 4), (startrow + 1)
					if (startpos + 4) >= 1 and
						(startpos + 4) <= display.linelength and
						(startrow + 1) >= 1 and
						(startrow + 1) <= (display.windowlines)
					{
						print "\\\_";
					}
				}
			}
		if m
		{
			if loc.m :  print map_chars.m;
			else: print map_chars.room_char;
		}
		locate 1,1
  }
  return loc
}

routine MapSet ! draws the map in a window in regular interpreters
{
	if system(61)
	{
		return false ! we need locate to work, which this kind of simple
		             ! interpreter can't do
	}
#ifset _WINDOW_H
	Font(PROP_OFF|BOLD_OFF|ITALIC_OFF|UNDERLINE_OFF)
	if map_window
	{
		run map_window.win_init
		window
		{
			run map_window.win_clear
			FullMap()
		}
		run map_window.win_end
		FONT(DEFAULT_FONT)
		return
	}
#endif ! _WINDOW_H
	if automaplib.MAP_ON ! or automaplib.AUTOMAP_ON)
	{
		Font(PROP_OFF|BOLD_OFF|ITALIC_OFF|UNDERLINE_OFF)
		window (display.statusline_height + 9)! 9
		{
			cls
		}
		Font(DEFAULT_FONT)
		window 0
		automaplib.MAP_ON = false
		PrintStatusLine
		return
	}
	if not system(61) ! not glk in this case
	{
		if automaplib.AUTOMAP_ON       ! The call to Map() using the switch
		{
			Font(PROP_OFF|BOLD_OFF|ITALIC_OFF|UNDERLINE_OFF)
			color automaplib.field_color, automaplib.field_color
			window 1, (display.statusline_height + 1), display.screenwidth, \
				(display.statusline_height + 9)
			{
				cls
			}
			color TEXTCOLOR, BGCOLOR
			window (display.screenwidth - 20) , (display.statusline_height + 2), \
				display.screenwidth , (display.statusline_height + 9)
			{
			   Map()
			}
			display.statusline_height += 9
		}
		Font(DEFAULT_FONT)
	}
}

#ifclear _ROODYLIB_H
!----------------------------------------------------------------------------
! REPLACEMENT PRINTSTATUSLINE

replace PrintStatusline
{
	local newstatusheight, simpleport, new_total, glktest

	glktest = (display.screenheight + 100) < display.windowlines
	if system(61) and not glktest ! non-glk minimal port
		simpleport = true

	if glktest and not (FORMAT & DESCFORM_F)
		FORMAT = FORMAT | DESCFORM_F
! figure out how many lines we need
	newstatusheight = FindStatusHeight

	new_total = newstatusheight + CalculateMapSpace(newstatusheight)

! remove the windows if the status window height has changed
	Font(BOLD_OFF | ITALIC_OFF | UNDERLINE_OFF | PROP_OFF)

	if (new_total < display.statusline_height)
	{
		window display.statusline_height
		{cls}
		window 0
	}

	display.statusline_height = new_total

	window display.statusline_height
	{
		color SL_TEXTCOLOR, SL_BGCOLOR
		if not simpleport
		{
			cls
			locate 1,1
		}
		if not location
			print "\_";
		elseif not light_source
			print "In the dark";
		else
		{
			if FORMAT & DESCFORM_F:  print "\_";
				print capital location.name;
		}
		local mapping
		mapping = (automaplib.MAP_ON or automaplib.AUTOMAP_ON or
		automaplib.FULL_ON )
		if newstatusheight = 1 and STATUSTYPE and not (mapping and glktest)
		{
			print to (display.screenwidth - \
			(StringLength(_temp_string) + (simpleport*2)));
			StringPrint(_temp_string)
		}
		elseif STATUSTYPE and (newstatusheight = 2 or mapping)
		{
			if simpleport
				""
			else
				locate 1, display.windowlines
			if FORMAT & DESCFORM_F:  print "\_";
				StringPrint(_temp_string)
		}
		if glktest and (automaplib.MAP_ON or automaplib.AUTOMAP_ON or
			automaplib.FULL_ON )
		{
			""
			if automaplib.MAP_ON
			{
				automaplib.MAP_ON = false
				Map(automaplib.map_object)
				automaplib.map_object = 0
			}
			elseif automaplib.AUTOMAP_ON
			{
				Map()
			}
			elseif automaplib.FULL_ON
			{
				FullMap(automaplib.map_object)
				locate 15, 1
				pause
				automaplib.map_object = 0
				automaplib.FULL_ON = false
			}
		}
	}
	color TEXTCOLOR, BGCOLOR
	Font(DEFAULT_FONT)
}

! Roody's note: Companion routine to above PrintStatusLine
routine FindStatusHeight
{
	local a, b
	text to _temp_string
	if not location
		print "\_";
	elseif not light_source
		print "In the dark";
	else
	{
		if FORMAT & DESCFORM_F:  print "\_";
		print capital location.name;
		print "\_";
	}
	text to 0
	a = StringLength(_temp_string)

	if STATUSTYPE = 1
	{
		text to _temp_string
!	if (FORMAT & DESCFORM_F)
!		print "\_";
		print number score; " / "; number counter;
		if (FORMAT & DESCFORM_F)
			print "\_";
		text to 0
	}
	elseif STATUSTYPE = 3
	{
		text to _temp_string
!	if (FORMAT & DESCFORM_F) : print "\_";
		print "Score: "; number score; "\_ "; "Moves: "; number counter;
		if (FORMAT & DESCFORM_F) : print "\_";
		text to 0
		b = StringLength(_temp_string)
	}
	elseif STATUSTYPE = 2
	{
		text to _temp_string
!	if (FORMAT & DESCFORM_F) : print "\_";
		print HoursMinutes(counter);
		if (FORMAT & DESCFORM_F) : print "\_";
		text to 0
	}

	elseif STATUSTYPE = 4
	{
		text to _temp_string
!	if (FORMAT & DESCFORM_F) : print "\_";
		STATUSTYPE4 ! routine for configurable statusline
		if (FORMAT & DESCFORM_F) : print "\_";
			text to 0
	}

	if (b + a + 4)<display.screenwidth ! let's force a 4 character gap between
	{                               ! the two fields
		return 1
	}
	elseif (b + a - 4 ) < display.screenwidth and STATUSTYPE = 3
	{
		text to _temp_string
		if (FORMAT & DESCFORM_F) : print "\_";
		print "S: "; number score; "\_ "; "M: "; number counter;
		if (FORMAT & DESCFORM_F) : print "\_";
		text to 0
		return 1
	}
	else
		return 2
}

! Roody's note: Replace this if you want to use the top right area
! for something else ("HUNGRY", "TIRED", or whatever)
routine STATUSTYPE4
{}

routine CalculateMapSpace(add)
{
	local glktest
	glktest = (display.screenheight + 100) < display.windowlines
	if glktest and automaplib.FULL_ON
		return (display.screenheight - (4 + add))
	elseif glktest and (automaplib.MAP_ON or automaplib.AUTOMAP_ON)
		return (9 - add)

	return false
}
#else

! this object and its properties draw the map window
object mapwindow
{
	in printstatuslib
	find_height
	{
		return (call &FindMapHeight)
	}
	draw_window
	{
		return (call &DrawMapWindow)
	}
	status_override 0
}

routine FindMapHeight
{
	local mapping
	mapping = (automaplib.MAP_ON or automaplib.AUTOMAP_ON or automaplib.FULL_ON )
	if (printstatuslib.terp_type ~= GLK_TERP) or not mapping
		return 0
	local a
	a = statuswindow.find_height
	if automaplib.FULL_ON
		return (display.screenheight - 4)
	elseif (automaplib.MAP_ON or automaplib.AUTOMAP_ON)
		return (a + 9)
}

routine DrawMapWindow
{
	if not location
		print "\_";
	elseif not light_source
		print "In the dark";
	else
	{
   !         if FORMAT & DESCFORM_F or (printstatuslib.terp_type = GLK_TERP)
		if FORMAT & DESCFORM_F or (printstatuslib.terp_type = GLK_TERP)
			print "\_";
		print capital location.name;
	}
	if STATUSTYPE
	{
		locate 1, display.windowlines
		if FORMAT & DESCFORM_F or (printstatuslib.terp_type = GLK_TERP)
			print "\_";
		StringPrint(_temp_string)
	}
	if automaplib.MAP_ON
	{
		automaplib.MAP_ON = false
		Map(automaplib.map_object)
		automaplib.map_object = 0
	}
	elseif automaplib.AUTOMAP_ON
	{
		Map()
	}
	elseif automaplib.FULL_ON
	{
		FullMap(automaplib.map_object)
		locate 15, 1
		pause
		automaplib.map_object = 0
		automaplib.FULL_ON = false
	}
}
#endif

!----------------------------------------------------------------------------
!                     VERB ROUTINES


routine DoMapOn
{
	local glktest, p
	if system(61) and ((display.screenheight + 100) > display.windowlines)
	{ ! non-glk simple interpreter
		AutomapMessage(&DoMap,1) ! "This interpreter doesn't support mapping."
		return false
	}
	elseif system(61)
		glktest = true
	if not automaplib.AUTOMAP_ON
	{
		automaplib.AUTOMAP_ON = true
		p = 1
	}
	else
	{
		AutomapMessage(&DoMapOn,2) ! "Mapping already enabled."
		return false
	}
	if BlockMap
	{
		return
	}
!	if display.cursor_row < (display.statusline_height + 9)
!		{
!		cls
!		locate 1,(display.statusline_height + 10)
!		}
	if not glktest
	{
		Font(BOLD_OFF | ITALIC_OFF | UNDERLINE_OFF | PROP_OFF)
		color automaplib.field_color, automaplib.field_color
		window 1, (display.statusline_height + 1), display.screenwidth, \
			(display.statusline_height + 9)
		{cls}
	}
	else
		PrintStatusLine

	if not glktest
	{
		color TEXTCOLOR, BGCOLOR
		window (display.screenwidth - 20) , (display.statusline_height + 2), \
			display.screenwidth , (display.statusline_height + 9)
		{
			Map()
		}
		display.statusline_height += 9
		Font(DEFAULT_FONT)
	}
	if p : AutomapMessage(&DoMapOn,1) ! "On-screen mapping enabled."
	return false
}

routine DoMapOff
{
	if system(61) and ((display.screenheight + 100) > display.windowlines)
	{ ! non-glk simple interpreter
		AutomapMessage(&DoMap,1) ! "This interpreter doesn't support mapping."
		return false
	}
	if not automaplib.AUTOMAP_ON
	{
		MapSet
		AutomapMessage(&DoMapOff,2) ! "Automap already off."
		return
	}
	automaplib.AUTOMAP_ON = false
	AutomapMessage(&DoMapOff,1) ! "On-screen mapping disabled."
	Font(BOLD_OFF | ITALIC_OFF | UNDERLINE_OFF | PROP_OFF)
	window (display.statusline_height)! 9
	{
		cls
	}
  	Font(DEFAULT_FONT)
	window 0
	PrintStatusLine
	return false
}

routine DoMap
{
	local glktest
	if system(61) and ((display.screenheight + 100) > display.windowlines)
	{ ! non-glk simple interpreter
		AutomapMessage(&DoMap,1) ! "This interpreter doesn't support mapping."
		return false
	}
	elseif system(61)
		glktest = true
	if BlockMap
	{
		AutomapMessage(&BlockMap) ! "Mapping isn't allowed at the moment."
		return false
	}
  if automaplib.AUTOMAP_ON = TRUE
	{
		AutomapMessage(&DoMap,2) ! "Automap is on. No need to draw another map."
		return false
	}

!	if display.cursor_row < (display.statusline_height + 9) and not glktest
!		{
!		cls
!		locate 1,(display.statusline_height + 10)
!		}
	if not glktest
	{
		Font(BOLD_OFF | ITALIC_OFF | UNDERLINE_OFF | PROP_OFF)
		color automaplib.field_color, automaplib.field_color
		window 1, (display.statusline_height + 1), display.screenwidth, \
			(display.statusline_height + 9)
		{
			cls
		}
		color TEXTCOLOR, BGCOLOR
		automaplib.MAP_ON = true
		if object
		{
			window (display.screenwidth - 20) , (display.statusline_height + 2), \
				display.screenwidth , (display.statusline_height + 9)
			{
				Map(object)
			}
		}
		else
		{
			window (display.screenwidth - 20) , (display.statusline_height + 2), \
				display.screenwidth , (display.statusline_height + 9)
			{
				Map()
			}
		!	display.statusline_height += 9
		}
	}
	else
	{
		if object
		{
			automaplib.MAP_ON = true
			automaplib.map_object = object
		}
		else
		{
			automaplib.MAP_ON = true
		}
		PrintStatusLine
	}
	Font(DEFAULT_FONT)
	return false
}

routine DoFullMap
{
	local glktest
	if system(61) and ((display.screenheight + 100) > display.windowlines)
	{ ! non-glk simple interpreter
		AutomapMessage(&DoMap,1) ! "This interpreter doesn't support mapping."
		return false
	}
	elseif system(61)
		glktest = true
	if BlockMap
	{
		AutomapMessage(&BlockMap) ! "Mapping isn't allowed at the moment."
		return false
	}
	if automaplib.AUTOMAP_ON = TRUE
	{
		AutomapMessage(&DoFullMap,1) ! "Turn off automap first."
		return true
	}
	window 0
	cls
	PrintStatusLine
	if object
	{
		if not glktest
		{
			Font(PROP_OFF|BOLD_OFF|ITALIC_OFF|UNDERLINE_OFF)
			window (display.screenheight - 3)
			{
				FullMap(object)               ! ditto
			}
			Font(DEFAULT_FONT)
		}
		else
		{
			automaplib.FULL_ON = true
			automaplib.map_object = object
			PrintStatusLine
		}
	}
	elseif not glktest
	{
		Font(PROP_OFF|BOLD_OFF|ITALIC_OFF|UNDERLINE_OFF)
		window (display.screenheight - 3)
		{
			FullMap()
		}
		Font(DEFAULT_FONT)
	}
	else
	{
		automaplib.FULL_ON = true
		PrintStatusLine
	}
#ifset _ROODYLIB_H
	InitScreen
#else
	window 0
	cls
	local linesfromtop = display.windowlines
#endif
	PrintStatusLine
	locate 1, LinesFromTop
	DescribePlace(location)
	return false                    ! return false when mapping full-screen to
}                                 ! suppress linefeed


routine BlockMap
{
	return false
}

routine AutomapMessage(r, num, a, b)
{
	! Check first to see if the NewRLibMessages routine provides a
	! replacement message:
	if NewAutomapMessages(r, num, a, b):  return

	select r
		case &BlockMap
		{
			"Mapping is not allowed at the moment."
		}
		case &DoMap
		{
			select num
				case 1: "Mapping is not supported at all in this interpreter,
				sorry."
				case 2: "Automap is on. No need to draw another map."
		}
		case &Map
		{
			select num
				case 1:  "You wish."
				case 2: print "mapping...";
		}
		case &DoMapOn
		{
			select num
				case 1: "On-screen mapping enabled."
				case 2: "Mapping already enabled."
		}
		case &DoMapOff
		{
			select num
				case 1: "On-screen mapping disabled."
				case 2: "Automap already off."
		}
		case &DoFullMap
		{
			select num
				case 1: "Turn off automap first."
		}
		case &FullMap
		{
			select num
				case 1: print "done";
				case 2: print to (display.linelength/2 - 11); \
					"PRESS A KEY TO CONTINUE";
		}
}

routine NewAutomapMessages(r, num, a, b)
{
   select r
		case else : return false
   return true ! this line is only reached if we replaced something
}

#endif

