#ifndef _TMPI
#define _TMPI

#include <tcl.h>


#ifdef BUILD_tmpi
#undef TCL_STORAGE_CLASS
#define TCL_STORAGE_CLASS DLLEXPORT
#endif


EXTERN int	Tmpi_Init _ANSI_ARGS_((Tcl_Interp * interp));

#endif
