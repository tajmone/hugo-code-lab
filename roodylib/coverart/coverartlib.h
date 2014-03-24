!::
! Cover.h by Roody Yogurt   Version 2.2
!::
!\ Changelog:
	V2.2 - Got rid of "covers once" option since I decided it was kind of dumb.
\!

!\ The "Cover Art" Extension-

   This contribution is an attempt to fake the cover art capabilities
	of z-code and Glulx games. Granted, not the most important of features,
	but I figured some Hugo authors might want to do this to make their games
	seem that much more fancy.

	WHAT IT DOES-

	If the detected screen is wide enough, the cover art and cover art text
	will be drawn in a window side-by-side. If it is not wide enough, it'll
	first show the cover art, wait for a keypress, then show the cover text.

	HOW TO USE-
	1. Include "cover.h"
	2. Call "CoverArt" from your init routine. (not necessary if you are using
	"roodylib" and have an init routine that calls Init_Calls)

	routine init
	{
		...
		CoverArt
		...
	}

	3. Replace the CoverText routine with one that has your cover text (wow!).

	replace CoverText
	{
		"<text that will go to the right of the cover art image>"
	}

	REQUIREMENTS AND OPTIONS-

	1. Before "cover.h" is included, you'll need to define a constant called
	COVERFILE that points to the graphic used for the cover.
	2. You can also define a constant called RESOURCEFILE that points
	to the resource file the graphic is in.

resource "resource"
{
   "cover.jpg"
}

constant RESOURCEFILE "resource"
constant COVERFILE "cover"

	(If there is no resourcefile, the game will still compile, but there
	will be a compiler warning message)

	4. If you want the cover art box to be particular colors (which you probably
	will), set the coverforeground and coverbackground globals before calling
	CoverArt.

		coverforeground = WHITE
		coverbackground = BLUE

	(At this point, I'm not including the option to change the "press a key"
	text color but may do so in the future)

	5. If you want to change the default "press a key" text, have the following
	line in init before CoverArt is called:

	pressakey_window.win_title = "PRESS A KEY"

	6. Define the MINIMUMWIDTH constant before "cover.h" is included
	to change the minimum screen width for which the game will try to show
	the cover art side-by-side with the cover art text.

	6. Define a MINIMUMHEIGHT constant if you want a minimum screen height
	to show the side-by-side.

	7. NEW IN VERSION 1.4-
		Cover Art options are now available!
		Include "cover.g" with your grammar.
		(This allows "COVERS ON" and "COVERS OFF" for turning cover art on and
		off.)
		If you define a constant called GAMECOVER, "COVERS ONCE" is also
		supported, allowing the player to show cover art only once.
		like:

		constant GAMECOVER "partyart"

	8. Set COVER_INSTRUCT if you want players to be notified about the
	   cover art commands after the first turn.

		#set COVER_INSTRUCT

	9. Replace NewCoverArtMessages messages to your heart's content!

That's it! If you have any other suggestions, let me know at
roody.yogurt@gmail.com!

\!

#ifset VERSIONS
#message "CoverArt.h Version 2.2"
#endif

#ifset USE_EXTENSION_CREDITING
#ifclear _ROODYLIB_H
#message error "Extension crediting requires \"roodylib.h\". Be sure to
include it first!"
#endif
version_obj cover_version "CoverArt Version 2.2"
{
	in included_extensions
	desc_detail
		" by Roody Yogurt";
}
#endif

#ifclear _COVER_H
#set _COVER_H

#ifclear _SYSTEM_H
#include "system.h"
#endif

#ifclear _WINDOW_H
#include "window.h"
#endif

#ifclear _RESOURCE_H
#include "resource.h"
#endif

#if undefined MINIMUMWIDTH
constant MINIMUMWIDTH 60
#endif

#if undefined MINIMUMHEIGHT
constant MINIMUMHEIGHT 20
#endif

#if undefined RESOURCEFILE
#message warning "RESOURCEFILE constant not defined before cover.h inclusion!"
#endif

#if undefined COVERFILE
#message error "COVERFILE constant not definied before cover.h inclusion!"
#endif

property two_box alias u_to
global coverforeground = DEF_SL_FOREGROUND
global coverbackground = DEF_SL_BACKGROUND

property nocovers alias sw_to
property alwayscovers alias w_to
property coveronce alias in_to

object coverlib "coverlib v1"
{
	two_box 0
	nocovers 0
	alwayscovers 1
	coveronce 0
! if roodylib.h has been included before cover.h, nothing needs to be
! added to the init routine
#ifset _ROODYLIB_H
	type settings
	did_print 0
#ifclear USE_CONFIG_SYSTEM
	in init_instructions
	execute
		{
		if not CheckWordSetting("undo") and not CheckWordSetting("restore")
			{
			if CoverArt
				self.did_print = true
			}
		}
#else
	in config_instructions
	name_sum 0
	load_info
	{
		coverlib.nocovers = readval
		coverlib.alwayscovers = readval
		coverlib.coveronce = readval
		coverlib.first_time = readval
	}
	save_info
	{
		writeval coverlib.nocovers
		writeval coverlib.alwayscovers
		writeval coverlib.coveronce
		writeval coverlib.first_time
	}
	setup
	{
		if CheckWordSetting("restart") or coverlib.nocovers
		{
			cover_fuse.in_scope = 0
			return true
		}
		if (self.first_time or coverlib.alwayscovers)
		{
			self.first_time = 0
			return CoverArt
		}
	}
	first_time 1
#endif
#ifset _NEWMENU_H
	usage_desc
	{
		if system(61) ! MINIMAL_PORT
			return false
		Indent
		"\BCOVERS ON\b- Turns cover art on."
		Indent
		"\BCOVERS OFF\b- Turns cover art off."
!#if defined GAMECOVER
!		Indent
!		"\BCOVERS ONCE\b- Displays cover art once."
!#endif
	}
#endif  ! NEWMENU
#endif  ! _ROODYLIB_H
}


routine CoverArt
{
	local a, not_stay
	not_stay = system(MINIMAL_INTERFACE) or (not display.hasgraphics)
#ifclear USE_CONFIG_SYSTEM
	not_stay = not_stay or (not CoverSettings)
#endif
	if not_stay
	{
#ifset COVER_INSTRUCT
		cover_fuse.in_scope = 0
#endif
		return false! returning false / cover art not shown
	}
	if not coverforeground and not coverbackground
	{
		coverforeground = DEF_SL_FOREGROUND
		coverbackground = DEF_SL_BACKGROUND
	}

	cls ! clear screen, just in case the author forgot

:STARTOVER
	Font(PROP_OFF)
	if display.needs_repaint
	{
		window 0
		color TEXTCOLOR, BGCOLOR, INPUTCOLOR
		cls
		display.needs_repaint = false
	}
	if display.screenwidth = lower(display.screenwidth,(display.screenheight*2))
	{
		coverlib.two_box = false
	}
	else
	{
		if (display.screenheight > MINIMUMHEIGHT) and
		display.screenwidth > MINIMUMWIDTH
		{
			coverlib.two_box = true
		}
		else
		{
			coverlib.two_box = false
		}
	}
	if coverlib.two_box
	{
		a++
	}
	run pressakey_window.win_init
	window
		{}
	run pressakey_window.win_end
	if not coverlib.two_box and not a
	{
		run coverpicture_window.win_init
		window
		{
			run coverpicture_window.win_clear
#if undefined RESOURCEFILE
			picture COVERFILE
#endif
#if defined RESOURCEFILE
			picture RESOURCEFILE , COVERFILE
#endif
		}
		run coverpicture_window.win_end
		CoverPause
		if not display.needs_repaint
		{
			a++
		}
		jump STARTOVER
	}
	elseif not coverlib.two_box and a
	{
		run coverpicture_window.win_init
		run coverpicture_window.win_end
		run cover_text_border_window.win_init
		window
		{
			run cover_text_border_window.win_clear
		}
		run cover_text_border_window.win_end
		run cover_text_window.win_init
		window
		{
			run cover_text_window.win_clear
			locate 1,1
			Font(PROP_ON)
			CoverText
		}
		run cover_text_window.win_end
		CoverPause
		if display.needs_repaint
		{
			jump STARTOVER
		}
	}
	else ! elseif two_box
	{
		run coverpicture_window.win_init
		window
		{
			run coverpicture_window.win_clear
#if undefined RESOURCEFILE
			picture COVERFILE
#endif
#if defined RESOURCEFILE
			picture RESOURCEFILE, COVERFILE
#endif
		}
		run coverpicture_window.win_end
		run cover_text_border_window.win_init
		window
		{
			run cover_text_border_window .win_clear
		}
		run cover_text_border_window.win_end
		run cover_text_window.win_init
		window
		{
			run cover_text_window.win_clear
			locate 1,1
			Font(PROP_ON)
			CoverText
		}
		run cover_text_window.win_end
		CoverPause
		if display.needs_repaint
		{
			jump STARTOVER
		}
	}
	window 0
	cls
	locate 1,5
	return true
}

window_class pressakey_window "press a key to continue"
{
	win_left     1  ! leftmost column
	win_top      1  ! topmost row
	win_right
		return display.screenwidth
	win_bottom
		return (display.screenheight - 4) ! * 4 / 5)

	win_textcolor
		return cover_text_window.win_textcolor
	win_backcolor
		return cover_text_window.win_backcolor
	win_titlecolor
		return cover_text_window.win_textcolor
	win_titleback
		return cover_text_window.win_backcolor
	win_init
	{
		local x, y, titlelen, width, height

		titlelen = string(_win_title_string, self.win_title, 64)

		height = self.win_bottom - self.win_top
		if height++ = 0:  height = 10
		if self.win_title:  height++

		width =  self.win_right - self.win_left
		if width++ = 0:  width = 20
		if titlelen > width
			width = titlelen+2

		! The default position is centered in the middle of
		! the screen
		!
		x = self.win_left
		if x = 0:  x = display.screenwidth/2 - width/2 + 1
		y = self.win_top
		if y = 0:  y = display.screenheight/2 - height/2 + 1

		! Check to make sure the window will actually fit on the
		! screen--and if it doesn't, make it
		!
		if x + width > display.screenwidth + 1
		{
			if width > display.screenwidth
				width = display.screenwidth
			x = display.screenwidth - width + 1
		}
		if y + height > display.screenheight + 1
		{
			if height > display.screenheight
				height = display.screenheight
			y = display.screenheight - height + 1
		}

		! Draw the window, in the supplied color(s), with or
		! without a static title:
		!
		window x, y, (x+width-1), (y+height-1)
		{
			color self.win_textcolor, self.win_backcolor
			cls

			if self.win_title ~= ""
			{
				Font(PROP_OFF)
				locate 1, height
				color self.win_titlecolor,
					self.win_titleback
				print to ((width-titlelen)/2);
!			Font(BOLD_ON)
			print self.win_title;
!			Font(BOLD_OFF)
			print to width;
			}

		}

		! If necessary, redefine the window below the static
		! title:
		!
!		if self.win_title ~= ""
!		{
!			window x, (y+1), (x+width-1), (y+height-1)
!			{}			! empty window body
!		}

		color self.win_textcolor, self.win_backcolor
		self.win_active = true
	}
}

window_class coverpicture_window
{
	win_left 2
	win_top    !  1  ! topmost row
	{
		if not coverlib.two_box
		{
			return (higher((pressakey_window.win_bottom/2 - \
				(((display.screenwidth/2)-3)/2)),3))
		}
		else
			return 3
	}
	win_right
	{
		if not coverlib.two_box
		{
			return (display.screenwidth-2)
		}
		else
			return (display.screenwidth/2)
	}
	win_bottom
	{
!		return (self.win_right/2)
		return (lower((pressakey_window.win_bottom/2 + \
			(display.screenwidth/4)),(pressakey_window.win_bottom-1))-1)
	}

	win_textcolor
		return cover_text_window.win_textcolor
	win_backcolor
		return cover_text_window.win_backcolor
}

window_class cover_text_border_window
{
	win_left
	{
		if not coverlib.two_box
			return 1
		else
			return (display.screenwidth/2 + 1)
	}
	win_top      1  ! topmost row
!		return higher((pressakey_window.win_bottom/2 - \
!		(display.screenwidth/4)),2)
	win_right
		return display.screenwidth
	win_bottom
	{
!		return (self.win_right/2)
		return lower((pressakey_window.win_bottom/2 + \
			(display.screenwidth/4)),(pressakey_window.win_bottom-1))
	}
	win_textcolor
		return cover_text_window.win_textcolor
	win_backcolor
		return cover_text_window.win_backcolor
}

window_class cover_text_window
{
!  win_left
!	return (cover_text_border_window.win_left + 1)
	win_left
	{
		return  (cover_text_border_window.win_left + coverpicture_window.win_left)
!	return (self.win_top/2 + cover_text_border_window.win_left)
	}
	win_top
	{
		return ( coverpicture_window.win_top + ((coverpicture_window.win_bottom - \
			coverpicture_window.win_top)/2) - \
			(lower(( (coverpicture_window.win_right - \
			coverpicture_window.win_left)/4) , ( (coverpicture_window.win_bottom - \
			coverpicture_window.win_top)/2) ) ) )
	}
	win_right
	{
		if not coverlib.two_box
			return (cover_text_border_window.win_right - 1)
		else
		{
			return lower(((cover_text_border_window.win_right + \
				cover_text_border_window.win_left)/2 + \
				(self.win_bottom - self.win_top)), \
				(cover_text_border_window.win_right - 1) )
		}
	}

	win_bottom
		return (cover_text_border_window.win_bottom - 1)

	win_textcolor
	{
#ifset _COLORLIB_H
		if colorlib.default_colors
			return DEF_SL_FOREGROUND
		else
#endif
			return coverforeground
	}
  win_backcolor
  {
 #ifset _COLORLIB_H
		if colorlib.default_colors
			return DEF_SL_BACKGROUND
		else
#endif
			return coverbackground ! background color
	}
}

#if undefined CoverText
routine CoverText
{
	"Your game description will go here."
}
#endif

routine CoverPause
{
	local key
	key = system(READ_KEY)
	if system_status
	{
		pause
		return
	}
	else
	{
		while true
		{
			key = system(READ_KEY)
			if key: break
			system(PAUSE_100TH_SECOND)
		}
	}
}

routine CoverSettings
{
	local test2
	if system(61) ! MINIMUM_INTERFACE.. will affect glk ports, too
		return
	readfile "coverdat"
	{
		coverlib.nocovers = readval
		coverlib.alwayscovers = readval
		coverlib.coveronce = readval
		test2 = readval
	}
   if test2 ~= FILE_CHECK
	{
  !        print "Error reading file."
	}

	if coverlib.alwayscovers
		return true
	elseif coverlib.nocovers
		return

#if defined GAMECOVER
	if coverlib.coveronce or test2 ~= FILE_CHECK
	{
		local a
		a = coverlib.coveronce
		test2 = 0
		readfile GAMECOVER
		{
			coverlib.coveronce = readval
			test2 = readval
		}
		if test2 ~= FILE_CHECK and a
			coverlib.coveronce = 1
		if coverlib.coveronce
		{
			writefile GAMECOVER
			{
				writeval ++coverlib.coveronce
				writeval FILE_CHECK
			}
		}
	}
	writefile "coverdat"
	{
		writeval coverlib.nocovers
		writeval coverlib.alwayscovers
		writeval coverlib.coveronce
		test2 = FILE_CHECK
		writeval test2
	}
#endif
	if coverlib.coveronce < 3
		return true
}

routine DoCoverHelp
{
	if system(MINIMAL_INTERFACE) or not display.hasgraphics
	{
		CoverArtMessage(&CoverArt,1)
		return
	}
	CoverArtMessage(&DoCoverHelp,1) ! >COVERS ALWAYS, >COVERS NEVER
!#if defined GAMECOVER
!	CoverArtMessage(&DoCoverHelp,2) ! >COVERS ONCE
!#endif
	""
}

routine DoCoverSettings
{
	local a
	if system(MINIMAL_INTERFACE) or not display.hasgraphics
	{
		CoverArtMessage(&CoverArt,1)
		return
	}
	select word[2]
		case "always","on"
		{
			a = 1
			coverlib.alwayscovers = true
			coverlib.nocovers = false
			coverlib.coveronce = false
		}
		case "never","off"
		{
			a = 2
			coverlib.alwayscovers = false
			coverlib.nocovers = true
			coverlib.coveronce = false
		}
#if defined GAMECOVER
		case "once"
		{
			a = 3
			coverlib.alwayscovers = false
			coverlib.nocovers = false
			coverlib.coveronce = true
		}
#endif
#if defined DATA_FILE
		case "once"
		{
			a = 3
			coverlib.alwayscovers = false
			coverlib.nocovers = false
			coverlib.coveronce = true
		}
#endif
	CoverArtMessage(&DoCoverSettings, a)
#ifclear USE_CONFIG_SYSTEM
	SaveCoverSettings
#else
	SaveSettings
#endif
}

routine SaveCoverSettings
{
	local test2
	writefile "coverdat"
	{
		writeval coverlib.nocovers
		writeval coverlib.alwayscovers
		writeval coverlib.coveronce
		test2 = FILE_CHECK
		writeval test2
	}
   if test2 ~= FILE_CHECK
	{
		print "\n[Error saving file.]"
   }

#if defined GAMECOVER
	writefile GAMECOVER
	{
		writeval coverlib.coveronce
		writeval FILE_CHECK
	}
#endif

}

#ifset COVER_INSTRUCT
fuse cover_fuse
{
	in_scope
		return player
}

event in cover_fuse ! the "in" is optional
{
	if not self.tick
		CoverInstructions
}

routine CoverInstructions
{
	CoverArtMessage(&CoverInstructions,1)
}
#endif

routine CoverArtMessage(r, num, a, b)
{
	! Check first to see if the NewMessages routine provides a
	! replacement message:
	if NewCoverArtMessages(r, num, a, b):  return

	select r
		case &CoverArt
		{
			select num
				case 1
					"Cover art is not available for this interpreter. Try Hugor
					 today! Links at: http://ifwiki.org/index.php/Hugor"
		}
		case &DoCoverHelp
		{
			select num
				case 1
				{
					"To turn on cover art, type \BCOVERS ON\b.
					This is the default behavior. To turn cover art
					off permenantly, type \BCOVER NEVER\b or \BCOVERS OFF\b.";
				}
				case 2
				{
					" To display a game's cover art just once, type \BCOVERS ONCE\b
					(this feature is not supported across all games with cover
					art).";
				}
		}
		case &DoCoverSettings
		{
			select num
				case 1
					"Cover art on."
				case 2
					"Cover art off."
				case 3
					"Cover art shows once."
		}
#ifset COVER_INSTRUCT
		case &CoverInstructions
		{
			select num
				case 1 : "\n[Type \B>COVER HELP\b for Cover Art commands.]"
		}
#endif
}

routine NewCoverArtMessages(r, num, a, b)
{
	select r

		case &DoCoverHelp
		{
			select num
			case 1: return false
			case 2: return false
		}

		case else : return false

	return true ! this line is only reached if we replaced something
}

#endif


