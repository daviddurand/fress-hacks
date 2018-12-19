#include <stdlib.h>
#include <string.h>
#include <console.h>
#include <StandardFile.h>
#include <SegLoad.h>
main()
{
	char	*Inputfiles[400];
	short 	message,filecount;
	AppFile theFile;
	int		file;

	console_options.nrows=60;
	console_options.ncols=80;
	console_options.title = "\pFress Conversion";

	CountAppFiles(&message, &filecount);
	if (message != 0)
		exit(1);
	else
		for (file = filecount; file > 0; file--) {
			GetAppFiles(file, &theFile);
			SetVol((void *)0, theFile.vRefNum);
			Inputfiles[file-1] = malloc(300);
			strncpy(Inputfiles[file-1], (char *)theFile.fName, 256);
			PtoCstr(Inputfiles[file-1]);
	}
	_main(Inputfiles, filecount);
}  
