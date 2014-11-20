
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <tcl.h>

#include "tmpi.h"



int TMPI_InitObjCmd (ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj*CONST objv[]);

int TMPI_FinalizeObjCmd (ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj*CONST objv[]);

int TMPI_SizeObjCmd (ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj* objv[]);

int TMPI_RankObjCmd (ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj*CONST objv[]);

int TMPI_BarrierObjCmd (ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj*CONST objv[]);

int TMPI_SendObjCmd (ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj*CONST objv[]);

int TMPI_RecvObjCmd (ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj*CONST objv[]);



/*
 *----------------------------------------------------------------------
 *
 * TMPI_InitObjCmd --
 *
 *	 Implements a new Tcl command.
 *
 * Results:
 *	A standard Tcl result
 *
 * Side effects:
 *	...
 * tmpi::init ?-mpiqueue -mpiversion -mpinice nn?
 *----------------------------------------------------------------------
 */

int
TMPI_InitObjCmd(clientData, interp, objc, objv)
    ClientData clientData;	/* Not used. */
    Tcl_Interp *interp;		/* Current interpreter */
    int objc;			/* Number of arguments */
    Tcl_Obj*CONST objv[];		/* Argument strings */
{
	int argc = 0;
	char** argv;
	char* itostr;
	int itostrlength;
	int i,j,resMPI, index;
	int length;
	int doqueue = 0;
	int doversion = 0;
	int nn = 0;

	static CONST char *switches[] = {
		"-mpiqueue", "-mpiversion", "-mpinice", NULL
    };

    enum Init_Switches {
		INITMPIQUEUE, INITMPIVERSION, INITMPINICE
    };

	if(objc > 5)
	{
		Tcl_WrongNumArgs(interp, 1, objv, "?arg ...?");
		return TCL_ERROR;
	}

	for (i = 1; i < objc; ++i) {
		if (Tcl_GetIndexFromObj(interp, objv[i], switches, "option", 0,	&index) != TCL_OK)
			return TCL_ERROR;
		switch ((enum Init_Switches) index) {
			case INITMPIQUEUE:
				doqueue = 1;
				break;
			case INITMPIVERSION:
				doversion = 1;
				break;
			case INITMPINICE:
				if(Tcl_GetIntFromObj(interp, objv[++i], &nn) != TCL_OK || nn <= 0) {
					Tcl_WrongNumArgs(interp, i, objv, "expected positive integer");
					return TCL_ERROR;
				}
				break;
		}
	}

	argc += doqueue;
	argc += doversion;
	if(nn > 0)
		argc += 2;

	if(argc > 0)
	{
		argv = (char**)calloc(argc, sizeof(char*));
		i = 0;
		if(doqueue) {
			/* "-mpiqueue" */
			argv[i] = (char*)ckalloc(sizeof(char)*(10));
				argv[i][0] = '-';
				argv[i][1] = 'm';
				argv[i][2] = 'p';
				argv[i][3] = 'i';
				argv[i][4] = 'q';
				argv[i][5] = 'u';
				argv[i][6] = 'e';
				argv[i][7] = 'u';
				argv[i][8] = 'e';
				argv[i][9] = '\0';
				++i;
		}
		if(doversion) {
			/* "-mpiversion" */
			argv[i] = (char*)ckalloc(sizeof(char)*(12));
				argv[i][0] = '-';
				argv[i][1] = 'm';
				argv[i][2] = 'p';
				argv[i][3] = 'i';
				argv[i][4] = 'v';
				argv[i][5] = 'e';
				argv[i][6] = 'r';
				argv[i][7] = 's';
				argv[i][8] = 'i';
				argv[i][9] = 'o';
				argv[i][10] = 'n';
				argv[i][11] = '\0';
				++i;
		}
		if(doversion) {
			/* "-mpinice" */
			argv[i] = (char*)ckalloc(sizeof(char)*(9));
				argv[i][0] = '-';
				argv[i][1] = 'm';
				argv[i][2] = 'p';
				argv[i][3] = 'i';
				argv[i][4] = 'n';
				argv[i][5] = 'i';
				argv[i][6] = 'c';
				argv[i][7] = 'e';
				argv[i][8] = '\0';
				++i;

			itostr = (char*) ckalloc(sizeof(char)*256);
			itostrlength = sprintf(itostr,"%i",nn);
			argv[i] = (char*)ckalloc(sizeof(char)*(itostrlength+1));
			for(j = 0; j < itostrlength; ++j)
				argv[i][j] = itostr[j];
			argv[i][itostrlength] = '\0';
			
			ckfree((char*)itostr);
		}
			
		//printf("%d = %s", i-1, argv[i-1]);
	}

	resMPI = MPI_Init(&argc, &argv);

	if(argc > 0)
	{
		for(i = 0; i < argc; ++i)
		{
			ckfree((char*)argv[i]);
		}
		free(argv);
	}
	
	if(resMPI != MPI_SUCCESS)
		return TCL_ERROR;

	Tcl_SetObjResult(interp, Tcl_NewIntObj(1));	
	return TCL_OK;
}


/*
 *----------------------------------------------------------------------
 *
 * TMPI_FinalizeObjCmd --
 *
 *	 Implements a new Tcl command.
 *
 * Results:
 *	A standard Tcl result
 *
 * Side effects:
 *	Terminates MPI execution environment
 *  All processes must call this routine before exiting. 
 *  The number of processes running after this routine is called is undefined,
 *  it is best not to perform much more than a return rc after calling MPI_Finalize.
 *
 *----------------------------------------------------------------------
 */

int
TMPI_FinalizeObjCmd(clientData, interp, objc, objv)
    ClientData clientData;	/* Not used. */
    Tcl_Interp *interp;		/* Current interpreter */
    int objc;			/* Number of arguments */
    Tcl_Obj*CONST objv[];		/* Argument strings */
{

	if(objc > 1)
	{
		Tcl_WrongNumArgs(interp, 1, objv, NULL);
		return TCL_ERROR;
	}
	
	if(MPI_SUCCESS == MPI_Finalize()) {
		Tcl_SetObjResult(interp, Tcl_NewIntObj(1));	
		return TCL_OK;
	}

	Tcl_AppendResult(interp, Tcl_GetString(objv[0]) , " failed ", NULL);
	return TCL_ERROR;
}

/*
 *----------------------------------------------------------------------
 *
 * TMPI_SizeObjCmd --
 *
 *	 Implements a new Tcl command.
 *
 * Results:
 *	A standard Tcl result
 *
 * Side effects:
 *	...
 *
 *----------------------------------------------------------------------
 */

int
TMPI_SizeObjCmd(clientData, interp, objc, objv)
    ClientData clientData;	/* Not used. */
    Tcl_Interp *interp;		/* Current interpreter */
    int objc;			/* Number of arguments */
    Tcl_Obj*CONST objv[];		/* Argument strings */
{

	Tcl_Obj* resultPtr;
	int size;

	if(objc > 1)
	{
		Tcl_WrongNumArgs(interp, 1, objv, NULL);
		return TCL_ERROR;
	}

	if(MPI_SUCCESS != MPI_Comm_size(MPI_COMM_WORLD, &size))
	{
		Tcl_AppendResult(interp, Tcl_GetString(objv[0]) , " failed ", NULL);
		return TCL_ERROR;
	}
	
	resultPtr = Tcl_GetObjResult(interp);
	Tcl_SetIntObj(resultPtr, size);

	return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * TMPI_RankObjCmd --
 *
 *	 Implements a new Tcl command.
 *
 * Results:
 *	A standard Tcl result
 *
 * Side effects:
 *	...
 *
 *----------------------------------------------------------------------
 */

int
TMPI_RankObjCmd(clientData, interp, objc, objv)
    ClientData clientData;	/* Not used. */
    Tcl_Interp *interp;		/* Current interpreter */
    int objc;			/* Number of arguments */
    Tcl_Obj*CONST objv[];		/* Argument strings */
{

	Tcl_Obj* resultPtr;
	int rank;

	if(objc > 1)
	{
		Tcl_WrongNumArgs(interp, 1, objv, NULL);
		return TCL_ERROR;
	}

	if(MPI_SUCCESS != MPI_Comm_rank(MPI_COMM_WORLD, &rank))
	{
		Tcl_AppendResult(interp, Tcl_GetString(objv[0]) , " failed ", NULL);
		return TCL_ERROR;
	}

	resultPtr = Tcl_GetObjResult(interp);
	Tcl_SetIntObj(resultPtr, rank);

	return TCL_OK;
}


/*
 *----------------------------------------------------------------------
 *
 * TMPI_BarrierObjCmd --
 *
 *	 Implements a new Tcl command.
 *
 * Results:
 *	A standard Tcl result
 *
 * Side effects:
 *	Blocks until all process have reached this routine.
 *
 *----------------------------------------------------------------------
 */

int
TMPI_BarrierObjCmd(clientData, interp, objc, objv)
    ClientData clientData;	/* Not used. */
    Tcl_Interp *interp;		/* Current interpreter */
    int objc;			/* Number of arguments */
    Tcl_Obj*CONST objv[];		/* Argument strings */
{
	if(objc > 1)
	{
		Tcl_WrongNumArgs(interp, 1, objv, NULL);
		return TCL_ERROR;
	}

	if(MPI_SUCCESS == MPI_Barrier(MPI_COMM_WORLD)) {
		Tcl_SetObjResult(interp, Tcl_NewIntObj(1));	
		return TCL_OK;
	}
	
	Tcl_AppendResult(interp, Tcl_GetString(objv[0]) , " failed ", NULL);

	return TCL_ERROR;
}

/*
 *----------------------------------------------------------------------
 *
 * TMPI_SendObjCmd --
 *
 *	 Implements a new Tcl command.
 *
 * Results:
 *	A standard Tcl result
 *
 * Side effects:
 *	This routine may block until the message is received.
 *
 *----------------------------------------------------------------------
 */

//tmpi::send -type integer|double ?-list? val dest
    
int
TMPI_SendObjCmd(clientData, interp, objc, objv)
    ClientData clientData;	/* Not used. */
    Tcl_Interp *interp;		/* Current interpreter */
    int objc;			/* Number of arguments */
    Tcl_Obj*CONST objv[];		/* Argument strings */
{

	int dest; /* destination process */
	int resMPI;  /* return result from MPI_Send */

	/* sending a list or value */
	int listlength = 1;
	Tcl_Obj** sendlistPtr;
	int* intarrayforsend;
	double* doublearrayforsend;
	
	/* helpers */
	int i, index;
	CONST char* sendtype;
	int listflag = 0;
	
	static CONST char *switches[] = {
		"-type", "-list", NULL
    };

    enum Send_Switches {
		SENDTYPE, SENDLIST
    };

	if(objc > 6 || objc < 5)
	{
		Tcl_WrongNumArgs(interp, 1, objv, "?arg ...?");
		return TCL_ERROR;
	}

	for (i = 1; i < objc-2; ++i) {
		if (Tcl_GetIndexFromObj(interp, objv[i], switches, "option", 0,	&index) != TCL_OK)
			return TCL_ERROR;
		switch ((enum Send_Switches) index) {
			case SENDTYPE:
				sendtype = Tcl_GetString(objv[++i]);
				if(strcmp(sendtype,"integer")!=0 && strcmp(sendtype,"double")!=0) {
					Tcl_WrongNumArgs(interp, i, objv, "expected double|integer");
					return TCL_ERROR;
				}
				break;
			case SENDLIST:
				listflag = 1;
				break;
		}
	}


	if(listflag && Tcl_ListObjGetElements(interp, objv[objc-2], &listlength, &sendlistPtr) != TCL_OK)
		return TCL_ERROR;

	if(Tcl_GetIntFromObj(interp, objv[objc-1], &dest) != TCL_OK)
		return TCL_ERROR;

	if(strcmp(sendtype,"integer")==0) {
		intarrayforsend = (int*)ckalloc(sizeof(int)*listlength);

		if(listflag) {
			for(i = 0; i < listlength; ++i) {
				if(Tcl_GetIntFromObj(interp, sendlistPtr[i], &intarrayforsend[i]) != TCL_OK) {
					ckfree((char*)intarrayforsend);
					return TCL_ERROR;
				}	
			}
		} else {
			if(Tcl_GetIntFromObj(interp, objv[objc-2], &intarrayforsend[0])!= TCL_OK) {
				ckfree((char*)intarrayforsend);
				return TCL_ERROR;
			}	
		}

		resMPI = MPI_Send( (void*)intarrayforsend, listlength, MPI_INT, dest, 0, MPI_COMM_WORLD);

		ckfree((char*)intarrayforsend);

	} else if(strcmp(sendtype,"double")==0) {
		doublearrayforsend = (double*)ckalloc(sizeof(double)*listlength);

		if(listflag) {
			for(i = 0; i < listlength; ++i) {
				if(Tcl_GetDoubleFromObj(interp, sendlistPtr[i], &doublearrayforsend[i]) != TCL_OK) {
					ckfree((char*)doublearrayforsend);
					return TCL_ERROR;
				}	
			}
		} else {
			if(Tcl_GetDoubleFromObj(interp, objv[objc-2], &doublearrayforsend[0])!= TCL_OK) {
				ckfree((char*)doublearrayforsend);
				return TCL_ERROR;
			}	
		}

		resMPI = MPI_Send( (void*)doublearrayforsend, listlength, MPI_DOUBLE, dest, 0, MPI_COMM_WORLD);

		ckfree((char*)doublearrayforsend);
	} else {
		Tcl_WrongNumArgs(interp, 1, objv, "missing type");
		return TCL_ERROR;
	}

	if(MPI_SUCCESS == resMPI) {
		Tcl_SetObjResult(interp, Tcl_NewIntObj(1));	
		return TCL_OK;
	}

	Tcl_AppendResult(interp, Tcl_GetString(objv[0]) , " failed ", NULL);
	return TCL_ERROR;

}

/*
 *----------------------------------------------------------------------
 *
 * TMPI_RecvObjCmd --
 *
 *	 Implements a new Tcl command.
 *
 * Results:
 *	A standard Tcl result
 *
 * Side effects:
 *	This routine may block until the message is sended.
 *
 *----------------------------------------------------------------------
 */
//val [tmpi::recv -type integer|double ?-length l? source]
int
TMPI_RecvObjCmd(clientData, interp, objc, objv)
    ClientData clientData;	/* Not used. */
    Tcl_Interp *interp;		/* Current interpreter */
    int objc;			/* Number of arguments */
    Tcl_Obj*CONST objv[];		/* Argument strings */
{

	/* recieved result either a simple value or a list of values */
	Tcl_Obj* resultPtr;

	int source; /* source process */
	int resMPI; /* return result from MPI_Recv */
	MPI_Status status;

	CONST char* recvtype;
	int listlength = 1;
	int listflag = 0;
	int* intarrayrecv;
	double* doublearrayrecv;
	
	int i, index;
	
	static CONST char *switches[] = {
		"-type", "-length", NULL
    };

    enum Recv_Switches {
		RECVTYPE, RECVLENGTH
    };

	
	if(objc > 6 || objc < 4)
	{
		Tcl_WrongNumArgs(interp, 1, objv, "?arg ...?");
		return TCL_ERROR;
	}

	for (i = 1; i < objc-1; ++i) {
		if (Tcl_GetIndexFromObj(interp, objv[i], switches, "option", 0,	&index) != TCL_OK)
			return TCL_ERROR;
		switch ((enum Recv_Switches) index) {
			case RECVTYPE:
				recvtype = Tcl_GetString(objv[++i]);
				if(strcmp(recvtype,"integer")!=0 && strcmp(recvtype,"double")!=0) {
					Tcl_WrongNumArgs(interp, i, objv, "expected double|integer");
					return TCL_ERROR;
				}
				break;
			case RECVLENGTH:
				if(Tcl_GetIntFromObj(interp,objv[++i],&listlength) != TCL_OK)
					return TCL_ERROR;
				listflag = 1;
				break;
		}
	}

	if(Tcl_GetIntFromObj(interp, objv[objc-1], &source) != TCL_OK)
		return TCL_ERROR;

	if(strcmp(recvtype,"integer")==0) {
		
		intarrayrecv = (int*)ckalloc(sizeof(int)*listlength);

		resMPI = MPI_Recv( (void*)intarrayrecv, listlength, MPI_INT, source, 0, MPI_COMM_WORLD, &status);

		if(listflag) {
			resultPtr = Tcl_NewListObj(0, NULL);
			for(i = 0; i < listlength; ++i)
				Tcl_ListObjAppendElement(interp, resultPtr, Tcl_NewIntObj(intarrayrecv[i]));
		} else {
			resultPtr = Tcl_NewIntObj(intarrayrecv[0]);
		}

		ckfree((char*)intarrayrecv);

	} else if(strcmp(recvtype,"double")==0) {

		doublearrayrecv = (double*)ckalloc(sizeof(double)*listlength);

		resMPI = MPI_Recv( (void*)doublearrayrecv, listlength, MPI_DOUBLE, source, 0, MPI_COMM_WORLD, &status);

		if(listflag) {
			resultPtr = Tcl_NewListObj(0, NULL);
			for(i = 0; i < listlength; ++i)
				Tcl_ListObjAppendElement(interp, resultPtr, Tcl_NewDoubleObj(doublearrayrecv[i]));

		} else {
			resultPtr = Tcl_NewDoubleObj(doublearrayrecv[0]);
		}
		
		ckfree((void*)doublearrayrecv);


	} else {
		Tcl_WrongNumArgs(interp, 1, objv, "missing type");
		return TCL_ERROR;
	}

	if(resMPI != MPI_SUCCESS)
	{
		Tcl_AppendResult(interp, Tcl_GetString(objv[0]) , " failed ", NULL);
		return TCL_ERROR;
	}

	Tcl_SetObjResult(interp, resultPtr);

	return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * TMPI_Init --
 *
 *	Initialize the new package.
 *
 * Results:
 *	A standard Tcl result
 *
 * Side effects:
 *	The tmpi package is created.
 *	One new command "tmpi" is added to the Tcl interpreter.
 *
 *----------------------------------------------------------------------
 */

int
Tmpi_Init(Tcl_Interp *interp)
{
	if (Tcl_InitStubs(interp, "8.0", 0) == NULL) {
		return TCL_ERROR;
	}
	if (Tcl_PkgRequire(interp, "Tcl", TCL_VERSION, 0) == NULL) {
		if (TCL_VERSION[0] == '7') {
			if (Tcl_PkgRequire(interp, "Tcl", "8.0", 0) == NULL) {
				return TCL_ERROR;
			}
		}
	}
	if (Tcl_PkgProvide(interp, "tmpi", "0.1") != TCL_OK) {
		return TCL_ERROR;
	}
	Tcl_CreateObjCommand(interp, "::tmpi::init", TMPI_InitObjCmd,
		NULL, (Tcl_CmdDeleteProc*)NULL);

	Tcl_CreateCommand(interp, "::tmpi::finalize", TMPI_FinalizeObjCmd,
	    NULL, (Tcl_CmdDeleteProc*)NULL);

	Tcl_CreateCommand(interp, "::tmpi::size", TMPI_SizeObjCmd,
	    NULL, (Tcl_CmdDeleteProc*)NULL);

	Tcl_CreateCommand(interp, "::tmpi::rank", TMPI_RankObjCmd,
	    NULL, (Tcl_CmdDeleteProc*)NULL);

	Tcl_CreateCommand(interp, "::tmpi::barrier", TMPI_BarrierObjCmd,
	    NULL, (Tcl_CmdDeleteProc*)NULL);

	Tcl_CreateObjCommand(interp, "::tmpi::send", TMPI_SendObjCmd,
	    NULL, (Tcl_CmdDeleteProc*)NULL);

	Tcl_CreateObjCommand(interp, "::tmpi::recv", TMPI_RecvObjCmd,
	    NULL, (Tcl_CmdDeleteProc*)NULL);


    return TCL_OK;
}
