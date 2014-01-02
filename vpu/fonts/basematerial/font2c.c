#include <stdio.h>
#include <stdlib.h>

#define IFILENAME "VGA-ROM.F14"
#define OFILENAME "FONT-VGA8x14.c"
#define SIZE_X      8
#define SIZE_Y      14
#define RIGHTSHIFT  1

int convert(FILE *ifp, FILE *ofp, int sizex, int sizey);

int main(void)
{
    FILE *ifp, *ofp;
    
    if ((ifp = fopen(IFILENAME, "r")) == NULL) {
        perror("Error opening " IFILENAME);
        exit(1);
    }
    if ((ofp = fopen(OFILENAME, "w")) == NULL) {
        perror("Error opening " OFILENAME);
        fclose(ifp);
        exit(1);
    }
    
    convert(ifp, ofp, SIZE_X, SIZE_Y);
    
    fclose(ofp);
    fclose(ifp);
    
    return 0;
}

int convert(FILE *ifp, FILE *ofp, int sizex, int sizey)
{
    unsigned char byte;
    size_t count = 0;
    
    fprintf(ofp, "  ");
    while (fread(&byte, 1, 1, ifp) == 1) {
        byte >>= RIGHTSHIFT;
        fprintf(ofp, "0x%02x", byte);
        if (++count % SIZE_Y == 0)
            fprintf(ofp, ",\n  ");
        else
            fprintf(ofp, ", ");
    }
    if (!feof(ifp)) {
        perror("Error reading input file");
        exit(1);
    }
    
    return 1;
}