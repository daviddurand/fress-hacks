/* fress.h:	 What you need to call the package.
*
*  Entries included:
*	 FRnew			-- Create a whole new instance.
*	 FRdestroy		-- Destroy an instance.
*	 FRadd			-- 
*	 FRdel			-- 
*	 FRset			-- 
*	 FRget			--
*
*  History:
*  04/--/90:  Written by Steven J. DeRose.
*  03/27/91:  Add a bunch of stuff (illuminating comment, eh?)
*
*/
#ifndef FRpublic_HEADER
#define FRpublic_HEADER


/* ************************* PREREQUISITE INCLUDES ********************* */

#include <stdio.h>						/* For FILE *					 */
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* ******************************* CONSTANTS *************************** */

#define FRCHECK		"FR"				/* Main struct integrity check	 */
#define NSPACES		5					/* Number of spaces				 */

#define STRUCTURE_FLAG		0x6c

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

/******************************typedefs ******************************** */

typedef unsigned char Byte; 

//#define Byte char

/* **************************** MACRO FUNCTIONS ************************ */

/* Unix toupper() is screwed up, so fix here.
*/
#ifdef THINK_C
	#define TOUPPER(c) toupper(c)
#else
#define TOUPPER(c) (islower(c) ? toupper(c) : c)
#endif

#define LABEL(l)  ((Label)(-(((Int16)(l))/2))) 
#define ASCII(c) ascii_conv(c)
#define PRINTABLE(c) ((ASCII_mapped[(c)>>4][(c)&0x0f])== 'x')

#define BELIEVE_ME __attribute__((packed)) __attribute__((aligned(1)))


/* ******************************* TYPEDEFS **************************** */

typedef short	Int16;
typedef unsigned short	UInt16;
typedef void	*Up;

typedef UInt16	PageNum;

typedef UInt16	Label;

/* Names for all the FRESS spaces (see space dictionary on page 0)
*/
typedef enum {
   SP_text				= 1,
   SP_annotation		= 2,
   SP_work				= 3,
   SP_macro				= 4, /* Unimplemented */
   SP_toc				= 5, /* Unimplemented */
   SP_index				= 6, /* Unimplemented */
   SP_structure			= 7,
   SP_label_display		= 8,
   SP_keyword			= 9,
   SP_internal_label	= 10,
   SP_dslink			= 11,
   SP_picture			= 12
} spaceNumbers;

/* Bit masks for the 'headflag' field in the HCB.
*/
typedef enum {
   HD_modify			= 0x80,
   HD_holdflag			= 0x40,
   HD_onlinflg			= 0x01
} headFlags;

/* Structure orders: 0x6C + opCode + modifier + label + dataFields
*/
typedef enum {
   OP_point			= 0x80,
   OP_tag			= 0x81,
   OP_block_start	= 0x82,
   OP_block_end		= 0x83,
   OP_table			= 0x84,
   OP_jump			= 0x85,
   OP_pmuj			= 0x86,
   OP_end_of_page	= 0x07
} opcodes;

/* The bits in the modifier enum differ in interpretation according to what
*	 kind of structure the opcode indicates it is.	This enum declares masks
*	 to and with the modifier to get the bits (hi order = bit 0).
*/ 
typedef enum modMasks_ {
	MOD_bit0		= 128,
	MOD_bit1		= 64,
	MOD_bit2		= 32,
	MOD_bit3		= 16,
	MOD_bit4		= 8,
	MOD_bit5		= 4,
	MOD_bit6		= 2,
	MOD_bit7		= 1
} modMasks;

typedef struct order_ {		/* Not used, since C alingment would choke */
	Byte		op;			/* An opCode		*/
	Byte		mod;		/* A modifier byte	*/
	UInt16		lab;		/* Internal label	*/
	Byte		data[512];	/* Varying length	*/
} order;

/*
CA = soft hyphen
*/
char *CP37[16] = {
   /*		   0123456789ABCDEF  */
   /* 0- */   "                ",
   /* 1- */   "                ",
   /* 2- */   "                ",
   /* 3- */   "                ",
   /* 4- */   "           .<(+|",
   /* 5- */   "&         !$*);~",
   /* 6- */   "-/        |,%_>?",
   /* 7- */   "         `:#@'=\"",
   /* 8- */   " abcdefghi      ",
   /* 9- */   " jklmnopqr      ",
   /* A- */   " ~stuvwxyz      ",
   /* B- */   "^         []  ' ",
   /* C- */   "{ABCDEFGHI-     ",
   /* D- */   "}JKLMNOPQR      ",
   /* E- */   "\\ STUVWXYZ      ",
   /* F- */   "0123456789      "
};
char *ASCII_mapped[16] = {
   /*		   0123456789ABCDEF  */
   /* 0- */   "                ",
   /* 1- */   "                ",
   /* 2- */   "                ",
   /* 3- */   "                ",
   /* 4- */   "x          xxxxx",
   /* 5- */   "x         xxxxx ",
   /* 6- */   "xx         xxxxx",
   /* 7- */   "         xxxxxxx",
   /* 8- */   " xxxxxxxxx      ",
   /* 9- */   " xxxxxxxxx      ",
   /* A- */   " xxxxxxxxx      ",
   /* B- */   "x         xx  x ",
   /* C- */   "xxxxxxxxxx      ",
   /* D- */   "xxxxxxxxxx      ",
   /* E- */   "x xxxxxxxx      ",
   /* F- */   "xxxxxxxxxx      "
};

char *CP500[16] = {
   /*          0123456789ABCDEF  */
   /* 0- */   "                "
   /* 1- */   "                "
   /* 2- */   "                "
   /* 3- */   "                "
   /* 4- */   "          [.<(+!"
   /* 5- */   "&         ]$*);^"
   /* 6- */   "-/        |,%_>?"
   /* 7- */   "         `:#@'=\""
   /* 8- */   " abcdefghi      "
   /* 9- */   " jklmnopqr      "
   /* A- */   " ~stuvwxyz      "
   /* B- */   "          ~|  ' "
   /* C- */   "{ABCDEFGHI-     "
   /* D- */   "}JKLMNOPQR      "
   /* E- */   "\\ STUVWXYZ      "
   /* F- */   "0123456789      "
};
/* ********************************************************************* */


typedef struct BELIEVE_ME commonHCB_ {					/* Common to all page headers		*/
	UInt16		headFlag;
	PageNum		pageName;
	PageNum		prevPage;
	PageNum		nextPage;
	Byte		spaceNo;
	Byte		mung01;
	UInt16		freeArea;
} commonHCB;

#define CHSIZE sizeof(commonHCB)

typedef struct BELIEVE_ME _dslink_code {
	Label hereLab;
	Label thereLab;
	char name[8];
} dslink_code, *dslink_code_ptr;
/*
typedef struct _dslink_name {
} dslink_name, *dslink_name_ptr;
*/
typedef struct BELIEVE_ME _mainFields {
	UInt16		dataArea;
	UInt16		lastIntp;					/* label of last struc on prev pp	*/
	UInt16		areaNump;					/* # of cur area at top of page		*/
	Byte		dum;
	Byte		textScop;
	Byte		areaSignal;
} mainFields;

typedef struct BELIEVE_ME mainPage_ {					/* text, annot, and work spaces		*/
	commonHCB	com;
	mainFields m;
	Byte		data[1600 - sizeof(mainFields) - sizeof(commonHCB)];
} mainPage;

typedef struct BELIEVE_MElabelPage_ {					/* internal label space				*/
	commonHCB	com;
	UInt16		mung;						/* padding */
	Label		lastIntp;					/* label of last struc on prev pp	*/
	Byte		data[1600 - sizeof(commonHCB) - 5];
} labelPage;

typedef struct BELIEVE_ME keywordPage_ {				/* keyword space					*/
	commonHCB	com;
	Byte		refFlag;					/* Misc. flags: 0x01 = refs are split */
	Byte		data[1600 - sizeof(commonHCB) - 1];
} keywordPage;


											/* Stuff for the HCB, page 0		*/
__attribute__((aligned(1)))
typedef struct KeyEntry_ {
	Byte		theKey[16];
	PageNum		thePage;
} KeyEntry;

__attribute__((aligned(1)))
typedef struct SpaceDict_ {
   PageNum		firstPage;
   PageNum		lastPage;
} SpaceDict;

#define NKEYS (1600 - 0x8a) / sizeof(KeyEntry)	/* Num keyword Entries that fit */

/* Header Control block for page 0, see handbook p. 6.
*/
__attribute__((aligned(1)))
typedef struct HCB_ {
   Byte			hdrFlags;
   Byte			mung1;
   UInt16		moremung;
   PageNum		numPages;
   PageNum		freePage;
   PageNum		lastPage;
   Label		nextLabel;
   Byte			flag;
   Byte			mung2[5];
   SpaceDict	spaces[SP_picture]; /* FRESS counts from 1, not 0! */

	PageNum		pageList[(0x86 - 0x4A)/2];
	Int16		maxDisp;			/* x86 */
	Int16		freeArea;			/* x88 */
									/* x8A unbounded array: */
	KeyEntry	keyDict[NKEYS];
	Byte		trailer[1600 - 0x8a - NKEYS*sizeof(KeyEntry)]; /* 4 */
} HCB, *HCBptr;

__attribute__((aligned(1)))
typedef union Page_ {
   HCB			H;
   mainPage		m;
   keywordPage	k;
   labelPage	l;
   Byte			generic[1600];
} Page, *PagePtr;

/* ************************ EXTERNAL PROTOTYPES ************************ */

/* ************************ INTERNAL PROTOTYPES ************************ */

void dslink_process(
		 Page	  *thePage
			);
void exportPage(
		 Page	  *thePage
			);
#endif
/* End of fress.h */
