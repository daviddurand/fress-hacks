/* fress.c:	 Load a FRESS file and extract structure and content
*	 into an SGML representation.
*
*  History:
*  04/--/90:  Written by sjd.
*  03/27/91:  Specify output DTD; add EBCDIC xlate code.
*
*/

/* The SGML structure to use for output:
*/

/* ************************* PREREQUISITE INCLUDES ********************* */

#include <stdlib.h>

#include "ERR1.h"
#include "fress.h"

	char *getFN(fn,mode)
		char		*fn;
		char		mode;
	{
		gets(fn);
		if (*fn && fn[strlen(fn)-1]=='\n')
			fn[strlen(fn)-1] = '\0';
		return(fn);
	}


	unsigned char ascii_conv(Byte c) {
		int row = c>>4 &0x0f;
		int col = c&0x0f;
		if (ASCII_mapped[row][col] == 'x')
			return CP37[row][col];
		else
			return c;
	}


/* ******************************* GLOBAL VAR **************************** */

Page		 pageZero;					 /* The main header page */
Page			pageBufs[SP_picture];	/* Current page loaded from each space */
#define PAGE_OF(SPACE) (pageBufs[(SPACE-1)])

FILE			*inFile;
FILE			*outFile;

UInt16 swapBytes(UInt16 halfWord) {
   UInt16 res = (halfWord >>8) | ((halfWord&0xff)<< 8);
   // fprintf(stderr, "swapping %d: %1x %1x\nres: %d\n\n", halfWord, halfWord&0xff, halfWord>>8, res);
   return res;
}

#define SWAP(c) ((c) = swapBytes(c))

void dump_key_entry(KeyEntry *ke, char* message) {
#ifdef DEBUG
	if (ke->thePage != 0) {
		fprintf(stderr, "Page: %d\n", ke->thePage);
		fprintf(stderr, "Key %s: ", message);
		for (int j = 0; j < 16; j++) {
			fprintf(stderr, "j = %d: v= %d", j, ke->theKey[j]);
			fputc(ASCII(ke->theKey[j]), stderr);
		}
	}
#endif
}

void fixKeyEntry(KeyEntry  *ke) {
   dump_key_entry(ke, "before ");
   SWAP(ke->thePage);
   dump_key_entry(ke, "after");
}


/**************************************************************** getPage ******
*  Load nth page of file into buffer passed.
*  Returns:	 Success flag.
*/
Boolean load_page(
	FILE			*theFile,
	PageNum		thePageNum,
	PagePtr		buf
	)
{
	if (!theFile || thePageNum<0 || !buf){
		ERR1msg("getPage",400,"Bad file or page num %u (%x).",thePageNum,thePageNum);
	  return(FALSE);
	}
	if (fseek(theFile,(long)thePageNum*1600L, 0)!=0) {
		ERR1msg("getPage",400,"Bad file seek for page num %u (%x).",thePageNum,thePageNum);
	   return(FALSE);
	}
	if (fread((Byte *)buf, 1L, 1600L, theFile)==0) {
		ERR1msg("getPage",400,"Could not read page %u (%x).",thePageNum,thePageNum);
		return(FALSE);
	}
	return(TRUE);
} /* End  */


void fix_header(commonHCB *page0) {
  SWAP(page0->headFlag);
  SWAP(page0->pageName);
  SWAP(page0->prevPage);
  SWAP(page0->nextPage);
  SWAP(page0->freeArea);
}

void dumpHeader(commonHCB header) {
#ifdef DEBUG
	fprintf(stderr, "\n---\nheadFlag: %x\n", header.headFlag);
	fprintf(stderr, "pageName: %d\n", header.pageName);
	fprintf(stderr, "prevPage: %d\n", header.prevPage);
	fprintf(stderr, "nextPage: %d\n", header.nextPage);
	fprintf(stderr, "freeArea: %d\n", header.freeArea);
	fprintf(stderr, "spaceNo: %d\n", header.spaceNo);
#endif
}
Boolean getPage(
	FILE			*theFile,
	PageNum		thePageNum,
	PagePtr		buf
	)
{
   Boolean res = load_page(theFile, thePageNum, buf);
   fix_header(&buf->m.com);
   dumpHeader(buf->m.com);
   switch (buf->m.com.spaceNo) {
		case SP_text:
		case SP_annotation:
		case SP_work:
		SWAP(buf->m.m.areaNump);
		break;
   }
   return res;
}
/**************************************************************** parseParms ******
*  Parse up user's command line into words.
*/
int parseParms(
	char			*cmd,
	int			maxNParms,
	int			maxParmLen,
	char			**parms,
	Boolean		keepCase
	)
{
	char			*foo, *bar;
	int			nParsed;
	int			lim;

	for (nParsed=0; nParsed<maxNParms; nParsed++) /* Clear all parms */
		*(parms[nParsed]) = '\0';
	
	for (foo=cmd, nParsed=0; nParsed<maxNParms; nParsed++) {
		bar = parms[nParsed];
		lim = maxParmLen;
		while(isspace(*foo)) foo++;
		if (*foo=='"') {
			foo++;
			while(*foo && *foo!='"') {
				if (--lim<=0) *bar = '\0';
				else *(bar++) = keepCase ? *(foo++) : toupper(*(foo++));
			}
			if (*foo) foo++;
		}
		else {
			while(*foo && !isspace(*foo)) {
				if (--lim<=0) *bar = '\0';
				else *(bar++) = keepCase ? *(foo++) : toupper(*(foo++));
			}
		}
		*bar = '\0';
	} /* for all tokens */
	return(nParsed);
} /* End parseParms */

Byte		*accept_data(string, save, length)
Byte		*string;
Byte		*save;
int		length;
{
	register int i, j;
	
	for(i = 0, j = 0; string[i] != '\0' && i < length; i++) {
		Byte orig = string[i];
		save[j] = ASCII(orig);
		if (!PRINTABLE(orig)) {
			if (orig==0x6a) {
				sprintf(&save[j], "&#xA6;");
			} else if (orig==0x5f) {
				sprintf(&save[j], "&#xAC;");
			} else if (orig==0x8f) {
				sprintf(&save[j], "&#xB1;");
			} else if (orig==0x4a) {
				sprintf(&save[j], "&#xA2;");
			} else if (orig==0x16) {
				sprintf(&save[j], "&backspace;");
			} else {
				sprintf(&save[j], "&weirdo%02x;", orig);
			}
			j += strlen(save) - 1;
		} else if (save[j] == '"') {
			strcpy(&save[j], "&quot;");
			j += 5;
		} else if (save[j] == '&') {
			strcpy(&save[j], "&amp;");
			j += 4;
		} else if (save[j] == '<') {
			strcpy(&save[j], "&lt;");
			j += 3;
		}
		j++;
	}
	save[j] = '\0';
	return(save);
}
void dumpPageZero()
{
   int i;
	fprintf(stderr, "=========\n\nGot page zero, numpages = %u nextLabel=%x.\n",pageZero.H.numPages,
		pageZero.H.nextLabel);
	fprintf(stderr, "	Free area on page zero = %u.\n",pageZero.H.freeArea);
	for (i=0; i<SP_picture; i++) {
		fprintf(stderr, "Space %u:	first page %6i, last page %6i.\n", i+1,
			pageZero.H.spaces[i].firstPage, pageZero.H.spaces[i].lastPage);
	}
	
	fprintf(stderr, "	hdrFlags = %xb\n",pageZero.H.hdrFlags);
	fprintf(stderr, "	FreePage = %u.\n",pageZero.H.freePage);
	fprintf(stderr, "	LastPage = %u.\n",pageZero.H.lastPage);
	fprintf(stderr, "	maxDisp = %u.\n",pageZero.H.maxDisp);
	fprintf(stderr, "	Free area on page zero = %u.\n",pageZero.H.freeArea);
//	  int j;
//	  for (i = 0; i < NKEYS; i++) {
//		 fprintf(stderr, "Keytable %d: ", i);
//		 dump_key_entry(&pageZero.H.keyDict[i], "dump");
//	  }
}


void fixSpaceDict(SpaceDict *sd) {
   SWAP(sd->firstPage);
   SWAP(sd->lastPage);
   fprintf(stderr, "spacedict: %d, %d.\n", sd->firstPage, sd->lastPage);
}

void fix_page0(HCBptr page0) {
   int i;
  SWAP(page0->numPages);
  SWAP(page0->moremung);
  SWAP(page0->freePage);
  SWAP(page0->lastPage);
  SWAP(page0->nextLabel);
  for (i = 0; i<SP_picture; i++)
	 fixSpaceDict(&page0->spaces[i]);
  SWAP(page0->maxDisp);
  SWAP(page0->freeArea);
//	for(i = 0; i < NKEYS; i++)
//	   fixKeyEntry(&page0->keyDict[i]);
}


Boolean load_page0(
	FILE			*theFile,
	PagePtr		buf
	)
{
   Boolean res = load_page(theFile, 0, buf);
   fix_page0(&buf->H);
   return res;
}

/**************************************************************** FParseCmd ******
*  Handle subcommand of the user's parse command:  Parse a whole FRESS file.
*
*/
void FParseCmd(
	Byte				*subcmd,
	Byte				*subcmd2
	)
{
	Byte				ucmd[80];
	int				i;
   PageNum			cp;
	
	if (!*subcmd) {
		ERR1msg("",300,"Choose FRESS file to parse:");
		getFN(subcmd,'r');
		if (!*subcmd) return;
	}
	fprintf(stderr, "Converting fress file '%s'", subcmd);

	if ((inFile=fopen(subcmd,"r"))==NULL) {
		ERR1msg("",400,"Unable to open input file '%s'.",subcmd);
		return;
	}
	strcat(subcmd,".xml");
	if ((outFile=fopen(subcmd,"w"))==NULL) {
		ERR1msg("",400,"Unable to open output file '%s'.",subcmd);
		fclose(inFile);
		return;
	}
	fprintf(stderr, " into '%s'.\n", subcmd);
	
	if (!load_page0(inFile,&pageZero)) {
		ERR1msg("FParseCmd",300,"Couldn't load page 0 in text space.");
		return;
	}
	dumpPageZero();

	fprintf(outFile, "<?xml version='1.0' ?>\n");
	fprintf(outFile, "<!DOCTYPE FressFile [\n");
	for (int weird=0; weird<256; weird++) {
		fprintf(outFile, "    <!ENTITY weirdo%02x \"<weirdo val='%02x' />\">\n", weird, weird);
	}
	fprintf(outFile, "    <!ENTITY backspace '<bs />'>\n");
	fprintf(outFile, "]>\n");

	fprintf(outFile, "<FressFile>\n");
	fprintf(outFile, "<TextSpace>");
	cp	= pageZero.H.spaces[SP_text-1].firstPage;
	 while (cp!=0) {
		if (!getPage(inFile,cp,&(PAGE_OF(SP_text)))) {
			ERR1msg("FParseCmd",300,"Couldn't load page %4d in text space.", cp);
			break;
		}
//		ERR1msg("FParseCmd",300,"Loaded page %4d in text space, next = %4d.",cp,
//			PAGE_OF(SP_text).m.com.nextPage);
		if (cp<=0 || cp>pageZero.H.numPages) {
			ERR1msg("",601,"Page number higher than total num of pages!");
			break;
		}
		exportPage(&PAGE_OF(SP_text));
		cp = PAGE_OF(SP_text).m.com.nextPage;
	}
	fprintf(outFile, "\n</TextSpace>\n");
	ERR1msg("",300,"At end of text space.");
	
	fprintf(outFile, "<WorkSpace>\n");
	cp	= pageZero.H.spaces[SP_work-1].firstPage;
	while (cp!=0) {
		if (!getPage(inFile,cp,&PAGE_OF(SP_text))) {
			ERR1msg("FParseCmd",300,"Couldn't load page %4d in work space.", cp);
			break;
		}
/*		ERR1msg("FParseCmd",300,"Loaded page %4d in work space, next = %4d.",cp,
			PAGE_OF(SP_text).m.com.nextPage);*/
		if (cp<=0 || cp>pageZero.H.numPages) {
			ERR1msg("",601,"Page number higher than total num of pages!");
			break;
		}
		exportPage(&PAGE_OF(SP_text));
		cp = PAGE_OF(SP_text).m.com.nextPage;
	}
	fprintf(outFile, "\n</WorkSpace>\n");
/*	ERR1msg("",300,"At end of work space.");	*/

	fprintf(outFile, "<AnnotationSpace>\n");
	cp	= pageZero.H.spaces[SP_annotation-1].firstPage;
	while (cp!=0) {
		if (!getPage(inFile,cp,&(PAGE_OF(SP_text)))) {
			ERR1msg("FParseCmd",300,"Couldn't load page %4d in annotation space.", cp);
			break;
		}
/*		ERR1msg("FParseCmd",300,"Loaded page %4d in annotation space, next = %4d.",cp,
			pageBufs[SP_text].m.com.nextPage);*/
		if (cp<=0 || cp>pageZero.H.numPages) {
			ERR1msg("",601,"Page number higher than total num of pages!");
			break;
		}
		exportPage(&PAGE_OF(SP_text));
		cp = PAGE_OF(SP_text).m.com.nextPage;
	}
	fprintf(outFile, "\n</AnnotationSpace>\n");
/*	ERR1msg("",300,"At end of Annotation space.");*/

	fprintf(outFile, "<InterfileSpace>\n");
	cp	= pageZero.H.spaces[SP_dslink-1].firstPage;
	if (cp == 0)
		fprintf(stderr, "No interfile links.\n");
	while (cp != 0) {
		if (!getPage(inFile,cp,(PagePtr)&(PAGE_OF(SP_dslink)))) {
			ERR1msg("FParseCmd",300,"Couldn't load page %4d -- the dslink space.", cp);
		} else
			dslink_process(&PAGE_OF(SP_dslink));
		cp = PAGE_OF(SP_dslink).m.com.nextPage;
	}
	fprintf(outFile, "</InterfileSpace>\n");
	fprintf(outFile, "</FressFile>\n");

	fclose(outFile);
	fclose(inFile);
} /* End FParseCmd */

void dslink_process(
   Page		*thePage
   )
{
	register dslink_code_ptr	the_code;
	Byte						namebuf[9*5];

	the_code = (dslink_code_ptr)(&thePage->m.m.lastIntp);
	while (the_code->hereLab != 0x6c07 && the_code <
					(dslink_code_ptr)((Byte *)thePage + 1600)
					&& the_code->hereLab != 0) {
						fprintf(stderr, "h: %x ", the_code->hereLab);
						fprintf(stderr, " t: %x\n", the_code->thereLab);
						SWAP(the_code->hereLab);
						SWAP(the_code->thereLab);
						fprintf(stderr, "h: %x", the_code->hereLab);
						fprintf(stderr, " t: %x\n", the_code->thereLab);
						fprintf(stderr, "h: %x", LABEL(the_code->hereLab));
						fprintf(stderr, " t: %x\n", LABEL(the_code->thereLab));
		fprintf(outFile, "<InterfileLink hereReference=\"id%d\" thereReference=\"id%d\" File=\"%s\"/>\n",
			LABEL(the_code->hereLab), LABEL(the_code->thereLab),
						accept_data(the_code->name, namebuf, 8));
		the_code = (dslink_code_ptr)((Byte *)the_code + sizeof(dslink_code));
	}
}

void exportPage(
   Page		*thePage
   )
{
   int		l;
   int start = 0;
   Byte		cur;
   Byte		foo;
   Byte		bar;
   int		endloop = 0;
   int line = 70;
   int		adjust;
   char char_buf[100];
   
   // adjust =	(int)thePage->m.com.dataArea - 27;
   // fprintf(stderr, "adjust: %d.\n", adjust);
/*	 fprintf(stderr, "adjust = %d dataArea = %d.\n", adjust, thePage->m.com.dataArea);*/
   //for (l=adjust; l<1600-CHSIZE-5 && !endloop; l++) {
   SWAP(thePage->m.m.lastIntp);
   SWAP(thePage->m.m.areaNump);
#ifdef DEBUG
   fprintf(stderr, "com-header-size: %ld.\n", sizeof(thePage->m.com));
   fprintf(stderr, "m-header-size: %ld.\n", sizeof(thePage->m.m));
   fprintf(stderr, "m-data-size: %ld.\n", sizeof(thePage->m.data));
   fprintf(stderr, "dataArea: %4x.\n", thePage->m.m.dataArea);
   fprintf(stderr, "lastIntp: %4x.\n", thePage->m.m.lastIntp);
   fprintf(stderr, "areaNump: %04x.\n", thePage->m.m.areaNump);
   fprintf(stderr, "areaSignal: %02x.\n", thePage->m.m.areaSignal);
   fprintf(stderr, "textScop: %02x.\n", thePage->m.m.textScop);

#ifdef DEBUG2
   fprintf(stderr, "\n\nraw data\n");
   for (l=0; l<sizeof(thePage->m.data); l++) {
	   if (l%32 == 0) fprintf(stderr, "\n");
	   fprintf(stderr, "%02x ", thePage->m.data[l]);
   }
   
   fprintf(stderr, "\n\nASCIIfied data");
   for (l=0; l<sizeof(thePage->m.data); l++) {
	   if (l%32 == 0) fprintf(stderr, "\n");
	   fprintf(stderr, "%c", PRINTABLE(thePage->m.data[l])? ASCII(thePage->m.data[l]) : '`');
   }
   fprintf(stderr, "\n\nmain loop\n");
#endif
#endif
   for (l=0; l<sizeof(thePage->m.data) && !endloop && l < thePage->m.com.freeArea; l++) {
	   if (l==0 && ASCII(thePage->m.m.textScop) == '%') l -=2;
	  cur = thePage->m.data[l];
//	  fprintf(stderr, "l: %d: Code %02x: %c.\n", l, cur, PRINTABLE(cur)? ASCII(cur) : '`');
	  switch (cur) {
		 case 0x6c:					/* '%':	 Structure order */
			if (!doCmd(thePage,&l))
				endloop = 1;
				line += 70; /* we generally want to break after structure if
							   white space permits */
			break;
		 case 0x40:
			if (line > 65) {
				fputc('\n', outFile);
				line = 0;
			} else {
				fputc(' ', outFile);
			}
			break;
		 case 0x5A:					/* '!':	 Edit/alter code */
			foo = ASCII(thePage->m.data[l+1]);
			bar = tolower(ASCII(thePage->m.data[l+2]));
			if(foo == '-' && (bar == 'p' || bar == 'i' || bar == 'j' || bar == 's')) {
				fputc('\n', outFile);
				line = 0;
			} else if (foo == '+') {	
				fputc('\n', outFile);
				line = 0;
			}
				/** NO BREAK; we continue the case below! **/
		 default:
				line += fprintf(outFile, "%s", accept_data(&thePage->m.data[l], char_buf, 1)) - 1;
				break;
	  } /* switch */
	  line++;
   }
} /* End exportPage */
void showstrings(Byte * data, unsigned char *msg) {
	/*
	fprintf(stderr, "%s\n", msg);
    for (int i=0; i<32; i++) {
 	   fprintf(stderr, "%c", PRINTABLE(data[i])? ASCII(data[i]) : '`');
    }
 	fprintf(stderr, "\n");
	*/
}
int
doCmd(
   Page		*thePage,
   int		*l
   )
{
	
	order 		o;
	Byte		*data;		/* Varying length		*/
	
	/* Create a buffer for each possible data field.  These are all
	*  x00-terminated strings, the first (if any) starting at 'data',
	*  so can just be strcpy'd and then stepped past.  They become
	*  attributes in the SGML ouput.
	* leave room for escaping of entity characters, worst case, every
	* char needs escaping. 
	*/
	#define BIG_LEN 256*10
	#define LEN 256
	Byte		keyword[BIG_LEN];
	Byte		label[BIG_LEN];
	Byte		viewspec[BIG_LEN];
	Byte		explainer[BIG_LEN];
	Byte		pictureRef[BIG_LEN];
	Byte		decimalBlockRef[BIG_LEN];
	memcpy(&o, &thePage->m.data[*l+1], sizeof(o));
	SWAP(o.lab);
	o.lab = LABEL(o.lab);
	fprintf(stderr, "op (mod) label: %02x (%02x) %x\n", o.op, o.mod, o.lab);
	if (o.op == OP_end_of_page) {
		return FALSE;
	}
	*l += 4;
	data = &o.data[0];
	showstrings(data, "start");
/*	fprintf(stderr, "%%%x-%x(%x)\n", o.op, o.mod, o.lab);*/
	if (!(o.mod & MOD_bit0))
		fprintf(stderr, "Modifier bit 1 not set!\n");
	switch (o.op) {
	   case OP_point:
		if (o.mod == (MOD_bit0 | MOD_bit5)) { /* %L(label) */
			showstrings(data, "point-label");
			fprintf(outFile, "<Point id=\"id%d\"><label>%s</label></Point>",
									o.lab, accept_data(data, label, LEN));
			*l += strlen(data)+1;
		}
		else if (o.mod == (MOD_bit0 | MOD_bit5 | MOD_bit1)) { /* %L(label) w/ dkeys */
			fprintf(outFile, "<Point id=\"id%d\"><label>%s</label>", o.lab,
											accept_data(data, label, LEN));
											showstrings(data, "point-label");
			*l += strlen(data)+1;
			data += strlen(data)+1;
			showstrings(data, "point-dkeys");
			fprintf(outFile, "<dkeywords>%s</dkeywords></Point>", accept_data(data, keyword, LEN));
			*l +=strlen(data);
			data += strlen(data)+1;
		}
		else {
			fprintf(stderr, "Bad label modifier %u.\n", o.mod);
			break;
		}
		  break;
	  case OP_tag:
		if (o.mod == (MOD_bit0 | MOD_bit3)) /* *** AREA *** */
			fprintf(outFile, "\n<Area id=\"id%d\"/>\n", o.lab);
		else if (o.mod & MOD_bit2) {		 /* %T"keywords" (annotation) */
			fprintf(outFile, "<AnnotationRef id=\"id%d\" reference=\"id%d\">", o.lab, o.lab+1);
			if (o.mod & MOD_bit1) {
				fprintf(outFile, "<dKeywords>%s</dKeywords>", accept_data(data, keyword, LEN));
				*l +=strlen(data)+1;
				data += strlen(data)+1;
			}
			if (o.mod & MOD_bit6) {
				fprintf(outFile, "<keywords>%s</keywords>", accept_data(data, keyword, LEN));
				*l +=strlen(data)+1; 
				data += strlen(data)+1;
			}
			fprintf(outFile, "</AnnotationRef>");
		}
		else if (o.mod & MOD_bit4) {
			fprintf(outFile, "<PictureRef id=\"id%d\" pict=\"%d\">", o.lab,
					-(data[0]<<8) | data[1]);
			*l += 3;
		}
		else if (o.mod & MOD_bit5) { /* decimal label reference */
			fprintf(outFile, "<DecRef id=\"id%d\" reference=\"id%d\"/>", o.lab,
					LABEL(data[0]<<8 | data[1]));
			*l += 3;
		}
		else if (o.mod & MOD_bit7) {
			fprintf(outFile, "<FileInclusion id=\"id%d\" filename=\"%s\">", o.lab,
					 accept_data(data, keyword, LEN));
			*l +=strlen(data)+1;
			data += strlen(data)+1;
			if (o.mod & MOD_bit1) {
				fprintf(outFile, "<dkeywords>%s</dkeywords>", accept_data(data, keyword, LEN));
				*l +=strlen(data)+1;
				data += strlen(data)+1;
			}
			fprintf(outFile, "</FileInclusion>");
		} else {
			fprintf(stderr, "Unknown Tag modifier '%d'.\n", o.mod);
		}
		  break;
	  case OP_block_start: /* Blocks %< %> */
		fprintf(outFile, "<Block id=\"id%d\"", o.lab);
		if (o.mod & MOD_bit2) {
			fprintf(outFile, " type=\"Annotation\">");
		} else if (o.mod & MOD_bit3) {
			fprintf(outFile, " type=\"Decimal\">");
		} else {
			fprintf(outFile, ">");
		}
		if (o.mod & MOD_bit1) {
			fprintf(outFile, "<dkeywords>%s</dkeywords>", accept_data(data, keyword, LEN));
			*l +=strlen(data)+1;
			data += strlen(data)+1;
		}
		if (o.mod & MOD_bit5) {
			fprintf(outFile, "<label>%s</label>", accept_data(data, label, LEN));
			*l +=strlen(data)+1;
			data += strlen(data)+1;
		}
		if (o.mod & MOD_bit6) {
			fprintf(outFile, "<keywords>%s</keywords>", accept_data(data, keyword, LEN));
			*l +=strlen(data)+1;
			data += strlen(data)+1;
		}
			fprintf(outFile, "<Contents>");
		  break;
	  case OP_block_end:
		fprintf(outFile, "</Contents></Block>");
		  break;
	  case OP_table:
		fprintf(stderr, "Table operation detected --- error\n");
		  break;
	  case OP_jump:
		fprintf(outFile, "<Jump id=\"id%d\" reference=\"id%d\" type=\"", o.lab, o.lab+1);
		if (o.mod & MOD_bit3)
			fprintf(outFile, "splice\">");
		else
			fprintf(outFile, "jump\">");
		if (o.mod & MOD_bit4) {
			fprintf(outFile, "<viewspecs>%s</viewspecs>", accept_data(data, viewspec, LEN));
			*l +=strlen(data)+1;
			data += strlen(data)+1;
		}
		if (o.mod & MOD_bit1) {
			fprintf(outFile, "<dkeywords>%s</dkeywords>", accept_data(data, keyword, LEN));
			*l +=strlen(data)+1;
			data += strlen(data)+1;
		}
		if (o.mod & MOD_bit6) {
			fprintf(outFile, "<keywords>%s</keywords>", accept_data(data, keyword, LEN));
			*l +=strlen(data)+1;
			data += strlen(data)+1;
		}
		if (o.mod & MOD_bit7) {
			fprintf(outFile, "<explainer>%s</explainer>", accept_data(data, keyword, LEN));
			*l +=strlen(data)+1;
			data += strlen(data)+1;
		}
			fprintf(outFile, "</Jump>");
		  break;
	  case OP_pmuj:
		fprintf(outFile, "<Pmuj id=\"id%d\" reference=\"id%d\" type=\"", o.lab, o.lab-1);
		if (o.mod & MOD_bit3)
			fprintf(outFile, "splice\">");
		else
			fprintf(outFile, "jump\">");
		if (o.mod & MOD_bit4) {
			showstrings(data, "pmuj-vspecs");
			fprintf(outFile, "<viewspecs>%s</viewspecs>", accept_data(data, viewspec, LEN));
			*l +=strlen(data)+1;
			data += strlen(data)+1;
		}
		if (o.mod & MOD_bit1) {
			showstrings(data, "pmuj-dkeys");
			fprintf(outFile, "<dkeywords>%s</dkeywords>", accept_data(data, keyword, LEN));
			*l +=strlen(data)+1;
			data += strlen(data)+1;
		}
		if (o.mod & MOD_bit6) {
			showstrings(data, "pmuj-keys");
			fprintf(outFile, "<keywords>%s</keywords>", accept_data(data, keyword, LEN));
			*l +=strlen(data)+1;
			data += strlen(data)+1;
		}
		if (o.mod & MOD_bit7) {
			showstrings(data, "pmuj-exp");
			fprintf(outFile, "<explainer>%s</explainer>", accept_data(data, keyword, LEN));
			*l +=strlen(data)+1;
			data += strlen(data)+1;
		}
			fprintf(outFile, "</Pmuj>");
		  break;
	  case OP_end_of_page:
		return(FALSE);
		  break;
	  default:
		 fprintf(stderr,"in doCmd, got unknown opcode x'%x'\n",o.op);
   }
   return(TRUE);
} /* End doCmd */

/**************************************************************** main ******
*/
#define MAXNPARMS		10
#define MAXPARMLEN	40
#define MAXNAME		256

typedef Byte		*string;

int main(
	int numfiles,
	char **files
	) {
   int				in;
   char				ucmd[MAXNAME];
   char				*parms[MAXNPARMS];
   char				src[MAXNAME];
			
   int				n,d,q,rc;
   long				l, ct;
   char				done = FALSE;

/* External package objects: */
   ERR1Id			theErr = NULL;
	 
   theErr = ERR1new();	/* Set up error handling package.						*/
#ifdef DEBUG2
   for (int i=0; i < 16; i++) {
	   for (int j=0; j < 16; j++) {
   	   		fprintf(stderr, "%02x ", CP37[i][j]);
   		}
		fputc('\n', stderr);
   }
   for (i=0; i < 16; i++) {
	   for (int j=0; j < 16; j++) {
   	   		fprintf(stderr, " %c ", CP37[i][j]);
   		}	
		fputc('\n', stderr);
   }
#endif
   for (l=0; l<MAXNPARMS; l++)
	  parms[l] = (char *) malloc(MAXPARMLEN+1);
   ERR1msg("main",300,"FRESS file converter (incl. structure).\n");
   
   fprintf(stderr, "size of page strucs: %lu %lu.\n",sizeof(mainPage),sizeof(commonHCB));
   if (numfiles != 1) {
	for (l = 1; l < numfiles; l++) {
		printf("Parsing file %s.\n", files[l]);
		FParseCmd(files[l], NULL);
	}
   } else {
	while (!done) {
			ERR1msg("main",300,"Command (or '?')?\n");
			gets (ucmd);
			parseParms(ucmd,MAXNPARMS,MAXPARMLEN,parms,FALSE);
		
			switch (parms[0][0]) {
				case 'E': /*********************************************** Error pkg */

					switch (parms[1][0]) {
						case 'T': /* Threshold */
							rc = atoi(parms[2]);
							ERR1msg("main",300,"Setting error reporting threshold to %d.\n",rc);
							ERR1setThreshold(rc);
							break;
						case 'S': /* Spooling of transcript to file */
							ERR1setTranscript(parms[2],TRUE/*append*/);
							ERR1msg("main",300,"Transcript going to file '%s' (''=off).",parms[2]);
							break;
						default:
							ERR1msg("main",300,
									"Error subcommands: Threshold n | Spool filename (""=off).\n");
					} /* inner switch under ERROR command */
					break;
			
				case 'Q': /*********************************************** Quit */
					done = TRUE;
					break;
					
				case 'F': /*********************************************** Fress parse */
					FParseCmd(parms[1],parms[2]);
					break;
								
				case '?': /*********************************************** help */
				case 'H':
				default:
					ERR1msg("main",300, "%s\n%s\n%s\n%s\n",
						"Commands:	Quit, Fressparser;	Error Thresh|Spool;",
						"  ");
			} /* switch */
	} /* while */
   } /* if */
   ERR1msg("main",300,"Done.\n");
   return 0;
} /* End main */
