#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// retranslate to ebcdic, courtesy hercules

/*--------------------------------------------------------------------------*/
/* 819 (ISO-8859-1 Latin-1) to 037 (EBCDIC US/Canada) */

static unsigned char
cp_819_to_037[] = {
 /*          x0  x1  x2  x3  x4  x5  x6  x7  x8  x9  xA  xB  xC  xD  xE  xF */
 /* 0x */ "\x00\x01\x02\x03\x37\x2D\x2E\x2F\x16\x05\x25\x0B\x0C\x0D\x0E\x0F"
 /* 1x */ "\x10\x11\x12\x13\x3C\x3D\x32\x26\x18\x19\x3F\x27\x1C\x1D\x1E\x1F"
 /* 2x */ "\x40\x5A\x7F\x7B\x5B\x6C\x50\x7D\x4D\x5D\x5C\x4E\x6B\x60\x4B\x61"
 /* 3x */ "\xF0\xF1\xF2\xF3\xF4\xF5\xF6\xF7\xF8\xF9\x7A\x5E\x4C\x7E\x6E\x6F"
 /* 4x */ "\x7C\xC1\xC2\xC3\xC4\xC5\xC6\xC7\xC8\xC9\xD1\xD2\xD3\xD4\xD5\xD6"
 /* 5x */ "\xD7\xD8\xD9\xE2\xE3\xE4\xE5\xE6\xE7\xE8\xE9\xBA\xE0\xBB\xB0\x6D"
 /* 6x */ "\x79\x81\x82\x83\x84\x85\x86\x87\x88\x89\x91\x92\x93\x94\x95\x96"
 /* 7x */ "\x97\x98\x99\xA2\xA3\xA4\xA5\xA6\xA7\xA8\xA9\xC0\x4F\xD0\xA1\x07"
 /* 8x */ "\x20\x21\x22\x23\x24\x15\x06\x17\x28\x29\x2A\x2B\x2C\x09\x0A\x1B"
 /* 9x */ "\x30\x31\x1A\x33\x34\x35\x36\x08\x38\x39\x3A\x3B\x04\x14\x3E\xFF"
 /* Ax */ "\x41\xAA\x4A\xB1\x9F\xB2\x6A\xB5\xBD\xB4\x9A\x8A\x5F\xCA\xAF\xBC"
 /* Bx */ "\x90\x8F\xEA\xFA\xBE\xA0\xB6\xB3\x9D\xDA\x9B\x8B\xB7\xB8\xB9\xAB"
 /* Cx */ "\x64\x65\x62\x66\x63\x67\x9E\x68\x74\x71\x72\x73\x78\x75\x76\x77"
 /* Dx */ "\xAC\x69\xED\xEE\xEB\xEF\xEC\xBF\x80\xFD\xFE\xFB\xFC\xAD\xAE\x59"
 /* Ex */ "\x44\x45\x42\x46\x43\x47\x9C\x48\x54\x51\x52\x53\x58\x55\x56\x57"
 /* Fx */ "\x8C\x49\xCD\xCE\xCB\xCF\xCC\xE1\x70\xDD\xDE\xDB\xDC\x8D\x8E\xDF"
 };       /* x0  x1  x2  x3  x4  x5  x6  x7  x8  x9  xA  xB  xC  xD  xE  xF */

static unsigned char
cp_037_to_819[] = {
 /*          x0  x1  x2  x3  x4  x5  x6  x7  x8  x9  xA  xB  xC  xD  xE  xF*/
 /* 0x */ "\x00\x01\x02\x03\x9C\x09\x86\x7F\x97\x8D\x8E\x0B\x0C\x0D\x0E\x0F"
 /* 1x */ "\x10\x11\x12\x13\x9D\x85\x08\x87\x18\x19\x92\x8F\x1C\x1D\x1E\x1F"
 /* 2x */ "\x80\x81\x82\x83\x84\x0A\x17\x1B\x88\x89\x8A\x8B\x8C\x05\x06\x07"
 /* 3x */ "\x90\x91\x16\x93\x94\x95\x96\x04\x98\x99\x9A\x9B\x14\x15\x9E\x1A"
 /* 4x */ "\x20\xA0\xE2\xE4\xE0\xE1\xE3\xE5\xE7\xF1\xA2\x2E\x3C\x28\x2B\x7C"
 /* 5x */ "\x26\xE9\xEA\xEB\xE8\xED\xEE\xEF\xEC\xDF\x21\x24\x2A\x29\x3B\xAC"
 /* 6x */ "\x2D\x2F\xC2\xC4\xC0\xC1\xC3\xC5\xC7\xD1\xA6\x2C\x25\x5F\x3E\x3F"
 /* 7x */ "\xF8\xC9\xCA\xCB\xC8\xCD\xCE\xCF\xCC\x60\x3A\x23\x40\x27\x3D\x22"
 /* 8x */ "\xD8\x61\x62\x63\x64\x65\x66\x67\x68\x69\xAB\xBB\xF0\xFD\xFE\xB1"
 /* 9x */ "\xB0\x6A\x6B\x6C\x6D\x6E\x6F\x70\x71\x72\xAA\xBA\xE6\xB8\xC6\xA4"
 /* Ax */ "\xB5\x7E\x73\x74\x75\x76\x77\x78\x79\x7A\xA1\xBF\xD0\xDD\xDE\xAE"
 /* Bx */ "\x5E\xA3\xA5\xB7\xA9\xA7\xB6\xBC\xBD\xBE\x5B\x5D\xAF\xA8\xB4\xD7"
 /* Cx */ "\x7B\x41\x42\x43\x44\x45\x46\x47\x48\x49\xAD\xF4\xF6\xF2\xF3\xF5"
 /* Dx */ "\x7D\x4A\x4B\x4C\x4D\x4E\x4F\x50\x51\x52\xB9\xFB\xFC\xF9\xFA\xFF"
 /* Ex */ "\x5C\xF7\x53\x54\x55\x56\x57\x58\x59\x5A\xB2\xD4\xD6\xD2\xD3\xD5"
 /* Fx */ "\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\xB3\xDB\xDC\xD9\xDA\x9F"
 };       /* x0  x1  x2  x3  x4  x5  x6  x7  x8  x9  xA  xB  xC  xD  xE  xF */



void set_tab_str(char** tab, int pos, char *val) {
   int len = strlen(val);
   char* res = malloc (len+1);
   res[0] = strlen(val);
   strncpy(&res[1],  val, len);
   tab[pos] = res;
}

void set_tab_bytes(char** tab, int pos, int len, char *val) {
   char* res = malloc (len+1);
   res[0] = len;
   memcpy(&res[1],  val, len);
   tab[pos] = res;
}

void init(char** tab) {
   for (int i = 0; i <256; i++)
      set_tab_str(tab, i, ".");
}

int load_trans(char **code_tab, char *filename) {
   char inbuff[500];
   FILE *tab_file = fopen(filename, "rb");
   char replacement[500];
   int code;
   while(fgets(inbuff, 500, tab_file) != NULL) {
      // fprintf(stderr, "line: %s\n", inbuff);
      if (inbuff[0]=='#')
         continue;
      if (sscanf(inbuff, "0x%x %s", &code, replacement) != 2) {
         fprintf(stderr, "Line ignored: %s", inbuff);
         continue;
      }
      // fprintf(stderr, "scan: %02x %s\n", code, replacement);
      if (code >255 || code < 0) {
         fprintf(stderr, "Bad code read %x.\n", code);
         return 1;
      }
      size_t rep_len = strlen(replacement);
      char *rep;
      switch (replacement[0]) {
         case '"':
            if (rep_len == 1) {
               set_tab_str(code_tab, code, "\"");
            } else {
               replacement[rep_len-2] = '\0';
               set_tab_str(code_tab, code, replacement+1);
            }
            break;
         case '0':
            {
               char *codebuf = malloc((rep_len)/2);
               for(int i = 2; i < rep_len; i+=2) {
                  char hexbuf[3];
                  strncpy(hexbuf, replacement+i, 2);
                  int byte;
                  sscanf(hexbuf, "%2x", &byte);
                  codebuf[i/2-1] = byte;
               }
               set_tab_bytes(code_tab, code, (rep_len)/2-1, codebuf);
            }
            break;
         default:
            set_tab_str(code_tab, code, replacement);
      }
   }
   fclose(tab_file);
   return 0;
}

int main(int argc, char **argv) {
   char* code_tab[256];
   init(code_tab);
   if (argc != 2) {
      fprintf(stderr, "translate file required.\n");
      return 1;
   }
   int loadres = load_trans(code_tab, argv[1]);
   if (loadres != 0)
      return loadres;
   /* dumpt table for debugging
   for (int i = 0; i < 256; i++) {
      fprintf(stderr, "%2x: ", i);
      for (int j=1; j<code_tab[i][0]+1; j++)
         fputc(code_tab[i][j], stderr);
   }
   fputc('\n', stderr);
   */
   int c;
   while ((c = getchar()) != EOF) {
      c = cp_819_to_037[c];
      for (int j=1; j<((unsigned char)code_tab[c][0])+1; j++)
         putchar(code_tab[c][j]);
   }
}
