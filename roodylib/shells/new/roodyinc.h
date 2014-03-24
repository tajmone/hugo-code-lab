!::
! Roody Inclusions
!::

!\#ifset VERSIONS
#message "[NAME].h Version #"
#endif \!


#include "hugolib.h"

#ifset RESOURCES
#include "resource.h"
#endif

#ifset SYSTEM
#include "system.h"
#endif

#ifset WINDOWS
#include "window.h"
#endif

#ifset USE_ROODYLIB
#include "roodylib.h"
#endif

#ifset NEW_MENU
#include "newmenu.h"
#endif

#ifset CHEAP
#include "cheaplib.h"
#endif

#ifset PASTTENSE
#include "pasttense.h"
#endif

#ifset USE_CONFIG_SYSTEM
#include "config_sys.h"
#endif

#ifset USE_COVERART
#include "coverartlib.h"
#endif

#ifset USE_COLORLIB
#include "colorlib.h"
#endif

#ifset BETA
#include "beta.h"
#endif

#ifset DEBUG
replace last_library_object
{}
#endif