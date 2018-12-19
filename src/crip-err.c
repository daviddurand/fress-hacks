#include "stdio.h"
#include <string.h>
/* following line to kill twit ansi errors. don't ask */
#define NO_ERR_HACK
#include "ERR1.h"

typedef struct ERR1_ {
	int					thresh;
	FILE				*transcript;
} ERR1;

extern ERR1Id		ERR1gl;


/* ***************************************************************** ERR1msg
*/
Boolean ERR1msg(
	char			*proc,		/* Name of issuing routine	*/
	int				level,		/* Severity level				*/
	char			*fmt,		/* Text of error message	*/
	long			a,
	long			b,
	long			c,
	long			d,
	long			e,
	long			f,
	long			g /* Optionals */
)
{
register
	Boolean		hasCR;
	Boolean		rc = FALSE;
	
	if (ERR1gl==NULL) /* No real package, but try to display msg anyway.... */
		fprintf(stderr,("ERR1msg(800):	Error package not initialized.\n"));
		
	if (proc==NULL || fmt==NULL) {
		fprintf(stderr,"ERR1msg(700):  NULL proc or message string received.\n");
	}
		
	else if (ERR1gl==NULL || level>=ERR1gl->thresh) {
		hasCR = (fmt[strlen(fmt)-1]=='\n');
		fprintf(stderr,"%s(%03d):  ",proc,level);
		fprintf(stderr,fmt,a,b,c,d,e,f,g);
		if (*fmt && !hasCR) fprintf(stderr,"\n");
		
		if (ERR1gl!=NULL && ERR1gl->transcript!=NULL) {
			fprintf(ERR1gl->transcript,"%s(%03d):  ",proc,level);
			fprintf(ERR1gl->transcript,fmt,a,b,c,d,e,f,g);
			if (*fmt && !hasCR) fprintf(ERR1gl->transcript,"\n");
		}
		rc = TRUE;
	} /* level sufficient */
	
	if (level>=900) abort();					/*** BOOM ***/
	else return(rc);
} /* End ERR1msg */

