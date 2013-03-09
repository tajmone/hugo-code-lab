!::
! COLORLIB Library Extension V. 1.4
! (For games with changing colors)
! Updated to work with config_sys.h
!::

#ifclear _COLORLIB_H
#set _COLORLIB_H

#ifset VERSIONS
#message "ColorLib.h Version 1.4"
#endif

#ifset USE_EXTENSION_CREDITING
version_obj colorlib_version "ColorLib Version 1.4"
{
	in included_extensions
	desc_detail
		" by Roody Yogurt";
}
#endif

#ifclear _ROODYLIB_H
#message error "ColorLib requires \"roodylib.h\". Be sure to include it first!"
#endif

#if undefined COLOR_FILE
constant COLOR_FILE "colordat"
#endif

global ask_color = true

property default_colors alias u_to
property TEXTCOLOR_save alias d_to
property BGCOLOR_save alias e_to
property SL_TEXTCOLOR_save alias w_to
property SL_BGCOLOR_save alias se_to
property INPUTCOLOR_save alias in_to
property IDEAL_TEXTCOLOR alias sw_to
property IDEAL_BGCOLOR alias s_to
property IDEAL_SL_TEXTCOLOR alias nw_to
property IDEAL_SL_BGCOLOR alias ne_to
property IDEAL_INPUTCOLOR alias out_to

object colorlib "color"
{
	default_colors	0
	TEXTCOLOR_save DEF_FOREGROUND
	BGCOLOR_save DEF_BACKGROUND
	SL_TEXTCOLOR_save DEF_SL_FOREGROUND
	SL_BGCOLOR_save DEF_SL_BACKGROUND
	INPUTCOLOR_save MATCH_FOREGROUND
	IDEAL_TEXTCOLOR DEF_FOREGROUND
	IDEAL_BGCOLOR DEF_BACKGROUND
	IDEAL_SL_TEXTCOLOR DEF_SL_FOREGROUND
	IDEAL_SL_BGCOLOR	DEF_SL_BACKGROUND
	IDEAL_INPUTCOLOR MATCH_FOREGROUND
! if roodylib.h has been included before color.h, nothing needs to be
! added to the init routine
#ifclear USE_CONFIG_SYSTEM
	did_print 0
	in init_instructions
	save_info
		{
		SaveColorSettings
		return true
		}
	execute
		{
		if not CheckWordSetting("restore") and not CheckWordSetting("undo")
			{
			if ColorSettings
				self.did_print = true
			}
		else
			ResetColorSettings
		}
#endif ! ifclear USE_CONFIG_SYSTEM
#ifset _NEWMENU_H
	usage_desc
		{
		if system(61) ! MINIMAL_PORT
			return false
		Indent
		"\BCOLORS ON\b- Use author-defined game colors."
		Indent
		"\BCOLORS OFF\b- Force interpreter-defined color settings."
		}
#endif
	type settings
}

#ifset USE_CONFIG_SYSTEM
object color_config "Color Configuration Info v1"
{
	in config_instructions
	name_sum 0
	load_info
	{
		colorlib.default_colors = readval
		colorlib.TEXTCOLOR_save = readval
		colorlib.BGCOLOR_save = readval
		colorlib.SL_TEXTCOLOR_save = readval
		colorlib.SL_BGCOLOR_save = readval
		colorlib.INPUTCOLOR_save = readval
		self.first_time = readval
	}
	save_info
	{
		writeval colorlib.default_colors
		writeval colorlib.TEXTCOLOR_save
		writeval colorlib.BGCOLOR_save
		writeval colorlib.SL_TEXTCOLOR_save
		writeval colorlib.SL_BGCOLOR_save
		writeval colorlib.INPUTCOLOR_save
		writeval self.first_time
	}
	setup
	{
		local tc, bg, sl_tc, sl_bg, ic, different, ret
		if system(61) ! simple_port
			return false
		else ! save the current colors, just in case
			{
			tc = TEXTCOLOR
			bg = BGCOLOR
			sl_tc = SL_TEXTCOLOR
			sl_bg = SL_BGCOLOR
			ic = INPUTCOLOR
			if (tc and tc ~= DEF_FOREGROUND) or (bg and bg ~= DEF_BACKGROUND) or
			(sl_tc and sl_tc ~= DEF_SL_FOREGROUND) or
			(sl_bg and sl_bg ~= DEF_SL_BACKGROUND) or ic ~= INPUTCOLOR
				different = true
			}

		if self.first_time and ask_color
		{
			self.first_time = 0
			if different
			{
				TEXTCOLOR = DEF_FOREGROUND
				BGCOLOR = DEF_BACKGROUND
				SL_TEXTCOLOR = DEF_SL_FOREGROUND
				SL_BGCOLOR = DEF_SL_BACKGROUND
				INPUTCOLOR = MATCH_FOREGROUND
				color TEXTCOLOR, BGCOLOR, INPUTCOLOR
				cls
				locate 1, LinesFromTop
			}
			ColorLibMessage(&ColorSettings,1) ! This game uses non-default colors
	!: fancy pause stuff below
			local key
			if system_status or system(61) ! if simple port
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
			""
			if word[0] = 'd','D'
			{
				colorlib.default_colors = true
				ColorLibMessage(&ColorSettings, 2) ! Default colors will be used.
			}
			else
			{
				colorlib.default_colors = false
				ColorLibMessage(&ColorSettings, 3) ! Author-defined colors are a go.
			}
			""
			ColorLibMessage(&ColorSettings,4) ! "[press a key]"
			HiddenPause
			""
			ret = true
		}
		! let's save settings
		if different
		{
			colorlib.IDEAL_TEXTCOLOR = tc
			colorlib.IDEAL_BGCOLOR = bg
			colorlib.IDEAL_SL_TEXTCOLOR = sl_tc
			colorlib.IDEAL_SL_BGCOLOR = sl_bg
			colorlib.IDEAL_INPUTCOLOR = ic
		}
		if not colorlib.default_colors and different
		{
			TEXTCOLOR = tc
			BGCOLOR = bg
			SL_TEXTCOLOR = sl_tc
			SL_BGCOLOR = sl_bg
			INPUTCOLOR = ic
			ret = true
		}
		else ! if colorlib.default_colors
		{
			TEXTCOLOR = DEF_FOREGROUND
			BGCOLOR = DEF_BACKGROUND
			SL_TEXTCOLOR = DEF_SL_FOREGROUND
			SL_BGCOLOR = DEF_SL_BACKGROUND
			INPUTCOLOR = MATCH_FOREGROUND
		}
		return ret ! only InitScreen if we've printed something or there are new
		! colors
	}
	first_time 1
}
#endif
routine ColorSettings
{
	local tc, bg, sl_tc, sl_bg, ic, different
	if system(61) ! simple_port
		return false
	else ! save the current colors, just in case
		{
		tc = TEXTCOLOR
		bg = BGCOLOR
		sl_tc = SL_TEXTCOLOR
		sl_bg = SL_BGCOLOR
		ic = INPUTCOLOR
		if (tc and tc ~= DEF_FOREGROUND) or (bg and bg ~= DEF_BACKGROUND) or
		(sl_tc and sl_tc ~= DEF_SL_FOREGROUND) or
		(sl_bg and sl_bg ~= DEF_SL_BACKGROUND) or ic ~= INPUTCOLOR
			different = true
		}

	if not LoadColorSettings and ask_color
		{
		if different
			{
			TEXTCOLOR = DEF_FOREGROUND
			BGCOLOR = DEF_BACKGROUND
			SL_TEXTCOLOR = DEF_SL_FOREGROUND
			SL_BGCOLOR = DEF_SL_BACKGROUND
			INPUTCOLOR = MATCH_FOREGROUND
			color TEXTCOLOR, BGCOLOR, INPUTCOLOR
			cls
			locate 1, LinesFromTop
			}
		ColorLibMessage(&ColorSettings,1) ! This game uses non-default colors
!: fancy pause stuff below
		local key
		if system_status or system(61) ! if simple port
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
			""
		if word[0] = 'd','D'
			{
			colorlib.default_colors = true
			ColorLibMessage(&ColorSettings, 2) ! Default colors will be used.
			}
		else
			{
			colorlib.default_colors = false
			ColorLibMessage(&ColorSettings, 3) ! Author-defined colors are a go.
			}
		""
		ColorLibMessage(&ColorSettings,4) ! "[press a key]"
		HiddenPause
		""
#ifclear USE_CONFIG_SYSTEM
		colorlib.did_print = true
#endif
	}
! let's save settings
	if different
		{
		colorlib.IDEAL_TEXTCOLOR = tc
		colorlib.IDEAL_BGCOLOR = bg
		colorlib.IDEAL_SL_TEXTCOLOR = sl_tc
		colorlib.IDEAL_SL_BGCOLOR = sl_bg
		colorlib.IDEAL_INPUTCOLOR = ic
		}
	if not colorlib.default_colors and different
		{
		TEXTCOLOR = tc
		BGCOLOR = bg
		SL_TEXTCOLOR = sl_tc
		SL_BGCOLOR = sl_bg
		INPUTCOLOR = ic
		}
	else ! if colorlib.default_colors
		{
		TEXTCOLOR = DEF_FOREGROUND
		BGCOLOR = DEF_BACKGROUND
		SL_TEXTCOLOR = DEF_SL_FOREGROUND
		SL_BGCOLOR = DEF_SL_BACKGROUND
		INPUTCOLOR = MATCH_FOREGROUND
		}
	color TEXTCOLOR, BGCOLOR, INPUTCOLOR
	cls
	locate 1, LinesFromTop
	SaveColorSettings
}

routine SaveColorSettings
{
	colorlib.TEXTCOLOR_save = TEXTCOLOR
	colorlib.BGCOLOR_save = BGCOLOR
	colorlib.SL_TEXTCOLOR_save = SL_TEXTCOLOR
	colorlib.SL_BGCOLOR_save = SL_BGCOLOR
	colorlib.INPUTCOLOR_save = INPUTCOLOR

#ifclear USE_CONFIG_SYSTEM
	local test2
	writefile COLOR_FILE
		{
		writeval colorlib.default_colors
		writeval colorlib.TEXTCOLOR_save
		writeval colorlib.BGCOLOR_save
		writeval colorlib.SL_TEXTCOLOR_save
		writeval colorlib.SL_BGCOLOR_save
		writeval colorlib.INPUTCOLOR_save
		test2 = FILE_CHECK
		writeval test2
      }
   if test2 ~= FILE_CHECK
		{
          print "\n[Error saving file.]"
      }
#else
	return SaveSettings
#endif
}

routine LoadColorSettings
{
	if system(61) ! MINIMUM_INTERFACE.. will affect glk ports, too
		return
#ifclear USE_CONFIG_SYSTEM
	local test2
	readfile COLOR_FILE
		{
		colorlib.default_colors = readval
		colorlib.TEXTCOLOR_save = readval
		colorlib.BGCOLOR_save = readval
		colorlib.SL_TEXTCOLOR_save = readval
		colorlib.SL_BGCOLOR_save = readval
		colorlib.INPUTCOLOR_save = readval
		test2 = readval
      }
   if test2 ~= FILE_CHECK
		{
		return false
      }
	return true
#else
	return LoadSettings
#endif
}

routine ResetColorSettings
{
	local change, tc,bg, sl_tc, sl_bg, ic
	if system(61) ! MINIMUM_INTERFACE.. will affect glk ports, too
		return
#ifclear USE_CONFIG_SYSTEM
	local test2
	readfile COLOR_FILE
		{
		colorlib.default_colors = readval
		tc = readval
		bg = readval
		sl_tc = readval
		sl_bg = readval
		ic = readval
		test2 = readval
		}
#else
	readfile DATA_FILE
	{
		local i
		while true
		{
			i = readval
			select i
				case color_config.name_sum
				{
					colorlib.default_colors = readval
					tc = readval
					bg = readval
					sl_tc = readval
					sl_bg = readval
					ic = readval
					break
				}
				case FILE_CHECK : break
			if system_status
				break
		}
	}
#endif
	if CheckWordSetting("undo")
	{
		if SL_TEXTCOLOR ~= sl_tc or
		SL_BGCOLOR ~= sl_bg
		{
			change = true
			PrintStatusLine
		}
		if ((tc and tc ~= colorlib.IDEAL_TEXTCOLOR) or
		(bg and bg ~= colorlib.IDEAL_BGCOLOR) or
		(sl_tc and sl_tc ~= colorlib.IDEAL_SL_TEXTCOLOR) or
		(sl_bg and sl_bg ~= colorlib.IDEAL_SL_BGCOLOR)) and
		not colorlib.default_colors
		{
			TEXTCOLOR = colorlib.IDEAL_TEXTCOLOR
			BGCOLOR = colorlib.IDEAL_BGCOLOR
			SL_TEXTCOLOR = colorlib.IDEAL_SL_TEXTCOLOR
			SL_BGCOLOR = colorlib.IDEAL_SL_BGCOLOR
			change = true
		}
		elseif ((TEXTCOLOR and TEXTCOLOR ~= DEF_FOREGROUND) or
		(BGCOLOR and BGCOLOR ~= DEF_BACKGROUND) or
		(sl_TEXTCOLOR and sl_textcolor ~= DEF_SL_FOREGROUND) or
		(sl_bgcolor and sl_bgcolor ~= DEF_SL_BACKGROUND) or
		inputcolor ~= MATCH_FOREGROUND) and
		colorlib.default_colors
		{
			TEXTCOLOR = DEF_FOREGROUND
			BGCOLOR = DEF_BACKGROUND
			SL_TEXTCOLOR = DEF_SL_FOREGROUND
			SL_BGCOLOR = DEF_SL_BACKGROUND
			INPUTCOLOR = MATCH_FOREGROUND
			change = true
		}

		if (ic ~= colorlib.IDEAL_INPUTCOLOR) and not colorlib.default_colors
		{
			INPUTCOLOR = colorlib.IDEAL_INPUTCOLOR
			color TEXTCOLOR, BGCOLOR, INPUTCOLOR
			change = true
		}

		if TEXTCOLOR ~= tc or BGCOLOR ~= bg
		{
			color TEXTCOLOR, BGCOLOR, INPUTCOLOR
			cls
			locate 1, LinesFromTop
			ColorLibMessage(&ResetColorSettings,1)
			change = true
		}
		if INPUTCOLOR ~= ic and change ~= true
			color TEXTCOLOR, BGCOLOR, INPUTCOLOR
	}
	else  ! restoring then
	{
		if colorlib.default_colors
		{
			if (tc and tc ~= DEF_FOREGROUND) or (bg and bg ~= DEF_BACKGROUND) or
			(sl_tc and sl_tc ~= DEF_SL_FOREGROUND) or
			(sl_bg and sl_bg ~= DEF_SL_BACKGROUND) or ic ~= MATCH_FOREGROUND
			{
				TEXTCOLOR = DEF_FOREGROUND
				BGCOLOR = DEF_BACKGROUND
				SL_TEXTCOLOR = DEF_SL_FOREGROUND
				SL_BGCOLOR = DEF_SL_BACKGROUND
				INPUTCOLOR = MATCH_FOREGROUND
				change = true
				color TEXTCOLOR, BGCOLOR, INPUTCOLOR
				cls
				locate 1, LinesFromTop
			}
		}
		else
		{
			if (tc and tc ~= colorlib.IDEAL_TEXTCOLOR) or
			(bg and bg ~= colorlib.IDEAL_BGCOLOR) or
			(sl_tc and sl_tc ~= colorlib.IDEAL_SL_TEXTCOLOR) or
			(sl_bg and sl_bg ~= colorlib.IDEAL_SL_BGCOLOR)
			{
				TEXTCOLOR = colorlib.IDEAL_TEXTCOLOR
				BGCOLOR = colorlib.IDEAL_BGCOLOR
				SL_TEXTCOLOR = colorlib.IDEAL_SL_TEXTCOLOR
				SL_BGCOLOR = colorlib.IDEAL_SL_BGCOLOR
				change = true
				color TEXTCOLOR, BGCOLOR, INPUTCOLOR
				cls
				locate 1, LinesFromTop
			}

			if ic ~= colorlib.IDEAL_INPUTCOLOR
			{
				INPUTCOLOR = colorlib.IDEAL_INPUTCOLOR
				color TEXTCOLOR, BGCOLOR, INPUTCOLOR
				change = true
			}
		}
	}
	if change
		SaveColorSettings
}

routine ChangeColors(foreground,background,sl_fore,sl_back, dont_change)
{
	local change
	if not dont_change
		{
		if foreground or background
			{
			colorlib.IDEAL_TEXTCOLOR = foreground
			colorlib.IDEAL_BGCOLOR = background
			}
		if sl_fore or sl_back
			{
			colorlib.IDEAL_SL_TEXTCOLOR = sl_fore
			colorlib.IDEAL_SL_BGCOLOR = sl_back
			}
		}
	if system(61)
		return false
	if colorlib.default_colors
		{
		if TEXTCOLOR ~= DEF_FOREGROUND or BGCOLOR ~= DEF_BACKGROUND or
		SL_TEXTCOLOR ~= DEF_SL_FOREGROUND or SL_BGCOLOR ~= DEF_SL_BACKGROUND
			{
			TEXTCOLOR = DEF_FOREGROUND
			BGCOLOR = DEF_BACKGROUND
			SL_TEXTCOLOR = DEF_SL_FOREGROUND
			SL_BGCOLOR = DEF_SL_BACKGROUND
			change = true
			}
		else
			return false
		}
	else
		{
		if (foreground or background) and
		((TEXTCOLOR ~= foreground) or (BGCOLOR ~= background))
			{
			TEXTCOLOR = foreground
			colorlib.TEXTCOLOR_save = TEXTCOLOR
			BGCOLOR = background
			colorlib.BGCOLOR_save = BGCOLOR
			change = true
			}
		if (sl_fore or sl_back) and
		((SL_TEXTCOLOR ~= sl_fore) or (SL_BGCOLOR ~= sl_back))
			{
			SL_TEXTCOLOR = sl_fore
			colorlib.SL_TEXTCOLOR_save = sl_fore
			SL_BGCOLOR = sl_back
			colorlib.SL_BGCOLOR_save = SL_back
			change = true
			}
		}
	if (sl_fore or sl_back) and change
		PrintStatusLine
	if change
		{
		color TEXTCOLOR, BGCOLOR, INPUTCOLOR
		SaveColorSettings
		}
	return change
}

routine InputColorIs(new_color)
{
	local change
	if not colorlib.default_colors
		{
		INPUTCOLOR = new_color
		color TEXTCOLOR, BGCOLOR, INPUTCOLOR
		change = true
		}
	colorlib.IDEAL_INPUTCOLOR = new_color
	if change
		SaveColorSettings
}

routine DoColorOnOff
{
if system(61) ! simple port
	{
	ColorLibMessage(&DoColorHelp,2) ! not supported
	return
	}
local setting ,change
select word[2]
	case "on"
		{
		if not colorlib.default_colors
			{
			ColorLibMessage(&DoColorOnOff,1)
			return
			}
		change = true
		colorlib.default_colors = 0
		if ChangeColors(colorlib.IDEAL_TEXTCOLOR , colorlib.IDEAL_BGCOLOR , \
		colorlib.IDEAL_SL_TEXTCOLOR , colorlib.IDEAL_SL_BGCOLOR , 1)
			{
			setting = true
			cls
			locate 1, LinesFromTop
			}
		if colorlib.IDEAL_INPUTCOLOR ~= INPUTCOLOR
			{
			INPUTCOLOR = colorlib.IDEAL_INPUTCOLOR
			color TEXTCOLOR, BGCOLOR, INPUTCOLOR
			change = true
			}
		ColorLibMessage(&DoColorOnOff,2)
		if setting
			DescribePlace(location)
		}
	case "off","default"
		{
		if colorlib.default_colors
			{
			ColorLibMessage(&DoColorOnOff,3) ! "Default colors already on."
			return
			}
		change = true
		colorlib.default_colors = true
		if ChangeColors(DEF_FOREGROUND, DEF_BACKGROUND, \
		DEF_SL_FOREGROUND,DEF_SL_BACKGROUND,true)
			{
			setting = true
			cls
			locate 1, LinesFromTop
			}
		if INPUTCOLOR ~= MATCH_FOREGROUND
			{
			INPUTCOLOR = MATCH_FOREGROUND
			color TEXTCOLOR, BGCOLOR, INPUTCOLOR
			change = true
			}
		ColorLibMessage(&DoColorOnOff,4)
			if setting
				DescribePlace(location)
		}
!	colorlib.default_colors = setting
!	if change
!		{
!		""
!		ColorLibMessage(&DoColorOnOff,5)
!		if setting
!			ChangeColors(DEF_FOREGROUND, DEF_BACKGROUND, \
!		DEF_SL_FOREGROUND,DEF_SL_BACKGROUND)
!		HiddenPause
!		cls
!		locate 1, LinesFromtop
!		ColorLibMessage(&DoColorOnOff,5) ! Default colors on.
!
!		}
	if change
		SaveColorSettings
}

routine DoColorHelp
{
	if not system(61) ! simple port
		{
		ColorLibMessage(&DoColorHelp,1) ! "COLORS ON / COLORS OFF"
		}
	else
		ColorLibMessage(&DoColorHelp,2) ! "Sorry, color management not supported."
}

routine ColorLibMessage(r, num, a, b)
{
	if NewColorLibMessages(r, num, a, b):  return

	select r
		case &DoColorHelp
			{
			select num
				case 1
					{
					"To turn on \"game-defined colors\", type \BCOLORS ON\b. This is
					the default behavior. To force the game to use
					interpreter-defined colors, type \BCOLORS OFF\b or \BCOLORS
					DEFAULT\b."
					}
				case 2
					{
					"Sorry, color management is not supported with this interpreter."
					}
			}
	case &ColorSettings
		{
		select num
			case 1 : "This game uses non-default colors! If you hate author-defined
			colors or are on an interpreter where certain combinations are unreadable, push the \"D\" key now to keep the game using the colors set by the interpreter (you can also type \"COLORS OFF\" at any point
			in the game)."
			case 2 : "Default colors will be used."
			case 3 : "Color flag set."
			case 4 : print "[ press a key ]";
		}
	case &DoColorOnOff
		{
		select num
			case 1: print "Normal colors already on."
			case 2:  print "Normal coloring on."
			case 3: print "Default interpreter colors already on."
			case 4: print "Default interpreter colors."
			case 5 : print "[ press a key ]";
		}
	case &ResetColorSettings
		{
		select num
			case 1: print "\I[Color change detected. Clearing screen...]\i"
		}
}

!\ The NewColorLibMessages routine may be REPLACED and should return
true if a replacement message exists for routine <r> \!

routine NewColorLibMessages(r, num, a, b)
{
	return false
}

! Personally, I think color-changing looks better with bottom-justified games
replace LinesFromTop
{
	return display.windowlines
}

#endif ! #ifclear _COLORLIB_H