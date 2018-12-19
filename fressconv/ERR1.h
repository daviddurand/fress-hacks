/* EWRR1.h:  Public includes for twit error handler.
*
*  History:
*  04/19/90:  By sjd.
*/

#ifndef ERR1_HEADER
#define ERR1_HEADER
#include <stdlib.h>
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

typedef char Boolean;

/* ************************* PREREQUISITE INCLUDES ********************* */


/* ******************************* CONSTANTS *************************** */


/* ******************************* TYPEDEFS **************************** */

typedef struct ERR1_ *ERR1Id;

/* ************************ EXTERNAL PROTOTYPES ************************ */

extern	ERR1Id 		ERR1new(void);


extern	long			ERR1destroy(
			struct 		ERR1_ *toDestroy
);


extern	void			ERR1setThreshold(
			int			l
);

extern	void			ERR1setTranscript(
			char			*fileName,
			Boolean		appendFlag
);

#ifndef NO_ERR_HACK
extern	Boolean		ERR1msg(
	char	*proc,		/* Name of issuing routine	*/
	int	level,			/* Severity level				*/
	char	*fmt,		/* Text of error message	*/
	...					/* Optionals */
);
#endif
#endif
/* End of ERR1.h */
