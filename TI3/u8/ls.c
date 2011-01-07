#include <stdio.h>
#include <stdlib.h>

// datastructure for bootsector
// pragma disables compiler padding
#pragma pack(push,1)
typedef struct {
    unsigned char jmpBoot [3];
    unsigned char OEMName [8];
    unsigned short BytsPerSec;
    unsigned char SecPerClus;
    unsigned short RsvdSecCnt;
    unsigned char NumFATs;
    unsigned short RootEntCnt;
    unsigned short TotSec16;
    unsigned char Media;
    unsigned short FATSz16;
    unsigned short SecPerTrk;
    unsigned short NumHeads;
    unsigned int HiddSec;
    unsigned int TotSec32;
} BOOTSECTOR;
#pragma pack(pop)

// datastructure for a file entry
// pragma disables compiler padding
#pragma pack(push,1)
typedef struct {
    unsigned char name [8];
    unsigned char ext [3];
    unsigned char flags;
    unsigned char stuff [14];
    unsigned short data;
    unsigned int filesize;
} FILEENTRY;
#pragma pack(pop)
#define FILE_ENTRY_IGNORABLE     0x0F
#define FILE_ENTRY_DIRECTORY     0x10
#define FILE_ENTRY_ERASED        0xE5

// read bootsector
BOOTSECTOR readBootsector (FILE * f){
    BOOTSECTOR bs;
    fread(&bs,1,sizeof(bs),f);
    return bs;
}

// returns pointer to root directory
int getRootDir (BOOTSECTOR* bs){
   return (bs->RsvdSecCnt + (bs->NumFATs*bs->FATSz16))*bs->BytsPerSec;
}

// read a file entry
FILEENTRY readFileEntry (FILE * f){
    FILEENTRY fe;
    fread(&fe,1,sizeof(fe),f);
    return fe;
}

// compare two directory names
int dirstrcmp (char* fatname, char* myname){
    int i;
    for ( i = 0; i < 8; ++i ){
        if ( fatname[i] != myname[i] )
            return 0;
    }
    return 1;
}

// set file pointer to cluster x
void seekCluster (FILE* f, BOOTSECTOR* bs, int x){
    int pos = (bs->RsvdSecCnt + bs->NumFATs*bs->FATSz16 + (x-2)*bs->SecPerClus)
              *(bs->BytsPerSec) + bs->RootEntCnt*sizeof(FILEENTRY);
    fseek(f,pos,SEEK_SET);
}

// change work dir
int changeWorkDir (FILE* f, BOOTSECTOR* bs, char* dir){
    FILEENTRY fe;
    int pos;
    while ( 1 ){
        // read file entry
        fe = readFileEntry(f);

        // last entry reached?
        if ( fe.name [0] == 0 ){
            printf("dir '%s' not found\n",dir);
            return 0;
        }

        // dir found?
        if ( fe.name [0] != FILE_ENTRY_ERASED && fe.flags == FILE_ENTRY_DIRECTORY ){
            if ( dirstrcmp(fe.name,dir) ){
                // yes! we've found it
                printf("changing work dir to '%s'\n",dir);
                seekCluster(f,bs,fe.data);
                return 1;
            }
        }
    }
}

// print a file entry
void printFileEntry (FILEENTRY* fe){
    fwrite(fe->name,8,1,stdout);
    printf(" ");
    fwrite(fe->ext,3,1,stdout);
    printf(" ");
    if ( fe->flags & FILE_ENTRY_DIRECTORY )
        printf("<dir>\n");
    else
        printf("%d\n",fe->filesize);
}

int main(int argc, char** argv){
    int i;
    FILEENTRY fe;

    // open image and read bootsector
    FILE * f = fopen(argv[1],"r");
    BOOTSECTOR bs = readBootsector (f);

    // change to /
    fseek(f,getRootDir(&bs),SEEK_SET);

    // change to requested directory
    char* path = argv[2];
    char buffer [9]; // buffer to read until next /
    int j = 0;
    for ( i = 1; path[i-1] != 0; ++i ){
        if ( path[i] == '/' || path[i] == 0 ){
            if ( j == 0 ) break;
            while ( j < 8 ) buffer[j++] = ' ';
            if ( !changeWorkDir(f,&bs,buffer) ) return 1;
            j = 0;
        } else {
            buffer[j] = path[i];
            j++;
        }
    }

    // list files
    while ( 1 ){
        // read file entry
        fe = readFileEntry(f);

        // print file entries
        if ( fe.name [0] == 0 ) break;
        if ( fe.name [0] != FILE_ENTRY_ERASED && fe.flags != FILE_ENTRY_IGNORABLE ){
            printFileEntry(&fe);
        }
    }

    return 0;
}

