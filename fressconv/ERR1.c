/* Twit error handler.
*
*  Entries included:
*		ERR1new				-- Open the package
*		ERR1destroy			-- Close the package
*		ERR1setThreshold	-- Set the error reporting threshold
*		ERR1setTranscript	-- Open/close the transcript file.
*		ERR1msg				-- Issue a message if level >= threshold.
*		ERRpause				-- Stop and talk to user.
*
*  General meaning of error levels (take with grains of salt):
*	  <400	-- Trace-level information
*		400	-- Recoverable limit or other error
*		700	-- Interface error (routines not talking as they should,
*					unsupported feature invoked, etc.
*		800	-- Outright program bug detected
*		900	-- Fatal, ERR1msg issues message then aborts whole program.
*
*  History:
*  04/19/90:  moved out of ET.c by sjd.  Add test to force newline.
*/

#include "stdio.h"

/* following line to kill twit ansi errors. don't ask */
#define NO_ERR_HACK
#include "ERR1.h"

typedef struct ERR1_ {
	int 				thresh;
	FILE				*transcript;
} ERR1;

ERR1Id		ERR1gl = NULL;

/* ***************************************************************** ERR1new
*/
ERR1Id ERR1new() {
	if (ERR1gl==NULL) {
		ERR1gl = (ERR1Id) malloc(sizeof(ERR1));
		if (ERR1gl!=NULL) {
			ERR1gl->thresh	= 200;
			ERR1gl->transcript	= NULL;
		}
	}
	return(ERR1gl);
}


/* ***************************************************************** ERR1destroy
*/
long ERR1destroy(toDestroy)
	struct 		ERR1_ *toDestroy;
{
	long			saved = 0L;
	
	if (ERR1gl==NULL)
		return(0L);
	if (ERR1gl->transcript!=NULL) {
		fclose(ERR1gl->transcript);
		saved += sizeof(FILE);
	}
	free(ERR1gl);
	return((long) saved + sizeof(ERR1));
}


/* ***************************************************************** ERR1setThreshold
*/
void		ERR1setThreshold(l)
	int			l;
{
	if (ERR1gl==NULL)
		fprintf(stderr,("ERR1setThreshold(800):  Error package not initialized.\n"));
	else ERR1gl->thresh = l;
}


/* ***************************************************************** ERR1setTranscript
*  Pass empty filename to turn off.
*/
void		ERR1setTranscript(
	char			*fileName,
	Boolean		appendFlag)
{
	if (ERR1gl==NULL)
		fprintf(stderr,("ERR1setTranscript(800):  Error package not initialized.\n"));
		
	else if (fileName==NULL || !*fileName) {
		if (ERR1gl->transcript==NULL)
			ERR1msg("ERR1setTranscript",300,"Transcript was not running.");
		else {
			ERR1msg("ERR1setTranscript",300,"Closing transcript file.");
			fclose(ERR1gl->transcript);
			ERR1gl->transcript = NULL;
		}
	} /* close */
	
	else { /* wants to open */
		if (ERR1gl->transcript!=NULL) {
			ERR1msg("ERR1setTranscript",302,"Transcript was running, old file closed.");
			fclose(ERR1gl->transcript);
		}
		if ((ERR1gl->transcript=fopen(fileName, appendFlag ? "a":"w"))==NULL)
			ERR1msg("ERR1setTranscript",402,"Unable to open transcript.");
#ifdef THINK_C
		else ERR1msg("ERR1setTranscript",402,"\n*** Transcript started at %s",
			ctime(NULL));
#else
		else ERR1msg("ERR1setTranscript",402,"\n*** Transcript started.");
#endif
	} /* open */
}
/* ***************************************************************** ERR1pause
*/
void		ERR1pause(
	char			*proc,		/* Name of issuing routine	*/
	int			level,		/* Severity level				*/
	char			*fmt,			/* Text of prompt				*/
	long			a,
	long			b,
	long			c,
	long			d,
	long			e,
	long			f,
	long			g /* Optionals */
)
{
	char			mung[80];
	
	if (ERR1gl==NULL)
		fprintf(stderr,("ERR1pause(800):  Error package not initialized.\n"));
	else {
		if (ERR1msg(proc,level,fmt,a,b,c,d,e,f,g))
			ERR1msg("ERR1pause",899,"Continue or Abort?\n");
			gets(mung);
			if (*mung=='a' || *mung=='A') abort();
	}
} /* End ERR1pause */




