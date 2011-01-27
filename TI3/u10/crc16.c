#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

#define BIT char
#define CRC unsigned short

int getFileSize (FILE* f){
    fseek(f, 0, SEEK_END);
    int len = ftell(f)*8;
    fseek(f, 0, SEEK_SET);
    return len;
}

// Liest das nächste Bit aus einer Datei
// (funktioniert nur für eine einzige Datei)
BIT getNextBit (FILE* f){
    static unsigned int position;
    static unsigned char byte;

    // Müssen wir ein neues Byte lesen?
    if ( position % 8 == 0 ){
        fread(&byte,1,1,f);
    }

    // Das verlangte Bit liefern
    int bitpos = (position++)%8;
    return (byte & (1<<bitpos) ? 1 : 0);
}

// CRC-16 erstellen
CRC crc16 (FILE* f, int len){
    // Ein 16-Bit großes Schieberegister für die Polynomdivision
    CRC crc = 0;
    int i;
    BIT bit;

    // Die letzten 16 sind für die 16 Nullen, die man hinten anhängt
    for (i = 0; i < len + 16; ++i){
        // Nächstes Bit lesen
        if ( i < len )
            bit = getNextBit(f);
        else
            bit = 0;

        // Polynomdivision ausführen (s. Wikipedia)
        #define FIRSTBIT 0x8000
        #define POLYNOM_CRC16 0xA001
        if (((crc & FIRSTBIT) ? 1 : 0) != bit)
            crc = (crc << 1) ^ POLYNOM_CRC16;
        else
            crc <<= 1;
    }
    printf("berechneter CRC ist 0x%04X\n",crc);
    return crc;
}

// Entfernt .crc oder liefert 0, falls die Datei anders endet
char* removeDotCRC (char* fname){
    int len = strlen(fname);
    if ( len > 4 ){
        if ( strcmp ( fname + len - 4, ".crc" ) == 0 ){
            char* target = (char*) malloc (len);
            strcpy (target, fname);
            target [len-4] = 0;
            return target;
        }
    }
    return 0;
}

// Liest den CRC aus einer Datei
CRC getCrcFromFile (FILE* f){
    CRC crc;
    fseek ( f, -2, SEEK_END );
    fread ( &crc, sizeof(crc), 1, f );
    fseek ( f, 0, SEEK_SET );
    return crc;
}

int main (int argc, char** argv)
{
    char* fname = argv[1];
    char* target;
    FILE* f = fopen(fname,"r");
    int len = getFileSize(f);

    // Dateiendung .crc?
    if ( target = removeDotCRC (fname) ){
        // CRC aus Datei lesen
        CRC crc = getCrcFromFile(f);
        printf("gelesener CRC: 0x%04X\n",crc);

        // Ist CRC korrekt?
        if ( crc16(f,len) == crc )
        {
            printf("CRC korrekt. Erstelle Datei ohne CRC...\n");

            // Datei kopieren
            FILE* to = fopen (target,"w");
            int i;
            fseek(f,0,SEEK_SET);
            for ( i = 0; i < len - 16; i += 8 ){
                fputc (fgetc(f), to);
            }
            fclose(to);
        }
        else
        {
            printf("CRC fehlerhaft!\n");
        }
        free(target);
    }

    // Ansonsten .crc erstellen
    else
    {
        // CRC berechnen
        CRC crc = crc16(f,len);

        // Neuer Dateiname
        char* target = (char*) malloc (strlen(fname)+5);
        sprintf(target,"%s.crc",fname);
        printf("Erstelle %s...\n",target);

        // Datei kopieren
        FILE* to = fopen (target,"w");
        fseek(f,0,SEEK_SET);
        int i;
        for ( i = 0; i < len; i += 8 ){
            fputc (fgetc(f), to);
        }

        // CRC anhängen
        fwrite (&crc,sizeof(crc),1,to);

        // Aufräumen;
        fclose (to);
        free (target);
        fseek(f,0,SEEK_SET);
    }

    fclose(f);
    return 0;
}

