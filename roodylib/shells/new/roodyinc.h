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

#ifset BETA
#include "beta.h"
#endif

#ifset PASTTENSE
#include "pasttense.h"
#endif