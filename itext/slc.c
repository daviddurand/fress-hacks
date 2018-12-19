#include <stdio.h>
#include <strings.h>
char *CP37[16] = {
      /*              0123456789ABCDEF  */
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
#define COLS 80
#define PRINT_COLS "%80s"

int read_card(FILE* in, unsigned char buffer[]) {
   int i;
   int c;
   for(i = 0; i < COLS; i++)
      buffer[i] = c = fgetc(in);
   return c; // EOF if eof was found in card
}

void translate_card(unsigned char *buffer) {
   int i;
   for(i = 0; i < COLS; i++)
      buffer[i] = CP37[buffer[i]/16][buffer[i]%16];
}

unsigned char SLC[] = {0xe2, 0xd3, 0xc3, 0x40};
unsigned char END[] = {0xc5, 0xd5, 0xc4, 0x40};
unsigned char REP[] = {0xd9, 0xc5, 0xd7, 0x40};

void process_item(FILE * in, unsigned char mem[], unsigned char card_buf[]) {
   int i;
   if (strncmp((char *)card_buf, (char *)SLC, 4) == 0) {
      int start = (card_buf[4] << 8) | card_buf[5];
      int count = (card_buf[6] << 8) | card_buf[7];
      printf("SLC start = %o (%d), count = %o (%d)", start, start, count, count);
      if (count > 0) {
         for (i= 0; i < count*2; i++) {
            if (i%80 == 0) {
               read_card(in, card_buf);
               printf("\nD.card %d: ", i/80);
            }
            mem[start+i] = card_buf[i%80];
            printf("%02x", card_buf[i%80]);
            if (i%2==1) printf(" ");
         }
     }
      printf("\n");
   } else if (strncmp((char *)card_buf, (char *)END, 3) == 0) {
      int start = (card_buf[4] << 8) | card_buf[5];
      int count = (card_buf[6] << 8) | card_buf[7];
      printf("\nEND start = %o (%d), count = %o (%d)\n", start, start, count, count);
         read_card(in, card_buf);
         translate_card(card_buf);
         printf("Final: " PRINT_COLS "\n", card_buf);
         return;
   } else if (strncmp((char *)card_buf, (char *)REP, 3) == 0) {
      int start, len;
      translate_card(card_buf);
      printf("REP: %64s\n", card_buf);
      card_buf[79] = '\0';
      sscanf((char *)&card_buf[4], "%o %d", &start, &len);
      printf("rep: start = %o (%d), len = %o (%d)\n", start, start, len, len);
      if (len != 0) {
         read_card(in, card_buf);
         translate_card(card_buf);
         printf("REP-values: %76s\n", card_buf);
         printf("values: ");
      }
      for (i = 0; i <len; i++) {
         unsigned int value;
         sscanf((char *)&card_buf[i*7], "%o", &value);
         mem[start+(2*i)] = value >> 8;
         mem[start+2*i+1] = value &0xff;
         printf(" %o (%d), ", value,value);
      }
      printf("\n");
   } else {
      translate_card(card_buf);
      printf("Bad card found: " PRINT_COLS "\n", card_buf);
   }
}


int main(int argc, char** argv) {
   FILE *init_mem = fopen(argv[1], "r");
   FILE *in = fopen(argv[2], "r");
   FILE *out =fopen(argv[3], "w");
   size_t i;
   int c;
   int please_zero = 1;
   unsigned char card_buf[COLS+1];
   unsigned char mem[1024*16];
   card_buf[COLS]='\0';
   bzero(mem, sizeof(mem));
   c = fgetc(init_mem);
   for(int i=0; !feof(init_mem) && i<sizeof(mem); i++) {
      mem[i] = c;
      c = fgetc(init_mem);
      please_zero = 0;
   }
   if (please_zero) {
      printf("zeroing.\n");
      bzero(mem, sizeof(mem));
   }
   fclose(init_mem);
   for(i = 0; i < 6;i++) {
      read_card(in, card_buf);
      translate_card(card_buf);
      fprintf(stdout, "skipped: " PRINT_COLS "\n", card_buf);
   }
   while (read_card(in, card_buf) != EOF) {
      process_item(in, mem, card_buf);
   }
   for(int i=0; i<sizeof(mem); i++)
      fputc(mem[i], out);
   fclose(out);
   fclose(in);
}
