!::
!   Roody Shell Switches and Grammar inclusions
!::

!:: switches              ! example how one can add compiler switches to source
!#switches -ls            ! print compilation stats to .LST file

#ifset HDX
#switches -d              ! if the -d switch is not set in the compiler itself,
                          ! #set HDX will produce a Hugo Debuggable Executable
                          ! (.HDX) with extension .HEX
#endif                    ! Sneaky!


#ifset USE_ROODYLIB
#include "roodylib.g"
#endif

#ifset CHEAP
#include "cheaplib.g"
#endif

#ifset USE_COVERART
#include "coverartlib.g"
#endif

#ifset USE_COLORLIB
#include "colorlib.g"
#endif

#include "verblib.g" ! will include verbstub.g and hugofix.g, if applicable
