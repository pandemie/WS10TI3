#include "mm.h"
#include <stdio.h>

//Groesse des Speichers, den die Speicherverwaltung verwalten soll
#define memorySize 10240

//Zustand von einem Speicherblock
enum memBlockState{not_allocated=0, allocated=1};
//zur spaeteren Ausgabe..
char boolStr[2][6] = {{"FALSE"},{"TRUE"}};

//ein Speicherblock
typedef struct _memoryblock {
    void* data;         //Zeiger auf die Daten
    int dataLength;         //Laenge des Blocks
    enum memBlockState state;   //Wird der Block benutzt?
    struct _memoryblock * nextBlock; //Zeiger auf naechsten Speicherblock
} memoryBlock;

#define memoryBlockHeaderSize sizeof(memoryBlock)

//definition von "privaten" funktionen
void mergeFreeBlocks();
memoryBlock* splitBlock(memoryBlock* block, int byteCount);

//Erster Speicherblock in der Liste
memoryBlock* head;
//Unser Speicher, den wir fuer die Daten und deren Verwaltung benutzen koennen
char memory[memorySize];

//Ist unsere Speicherverwaltung initialisiert und bereit benutzt zu werden?
int b_initialized = 0;

//Initialisiere die Datenstruktur(en) fuer unsere Speicherverwaltung
void initialize()
{
    if(!b_initialized)
    {
        //initialisiere head in unserem Speicher <memory>
        b_initialized = 1;

        //TODO INITIALISIERUNG - DONE
        head = (memoryBlock*) &(memory[0]);
        head->data = &(memory[0]) + memoryBlockHeaderSize;
        head->dataLength = memorySize - memoryBlockHeaderSize;
        head->state = not_allocated;
        head->nextBlock = NULL;
    }
}

//liefert den ingesamt freien Speicherplatz zurueck
int get_free_space()
{
    if(!b_initialized)
        return -1;
    int count = 0;
    memoryBlock *block = head;
    while(block != NULL)
    {
        if(block->state == not_allocated)
        {
            count = count + block->dataLength;
        }
        block = block->nextBlock;
    }
    return(count);
}

//versucht einen Speicherbereich mit <byteCount> vielen Bytes zu reservieren
//Falls dies nicht gelingt wird ein NULL (0) Pointer zurueckgeliefert
void* my_malloc(int byteCount)
{
    if(!b_initialized)
    {
        initialize();
    }
    //Wenn der insgesamt verfuegbare Speicherplatz kleiner ist
    //als der angeforderte, koennen wir gleich aufhoeren!
    if(byteCount > get_free_space())
    {
        return(NULL);
    }
    //TODO - DONE
    //SUCHE NACH EINEM GEEIGNETEN FREIEN SPEICHERBLOCK, MIT MEHR ALS <byteCount>
    //VIELEN BYTES
    memoryBlock* block = head;
    while ( 1 ){
        // Keine geeigneter Block gefunden
        if ( block == NULL ){
            // jetzt m�sste man anfangen zu defragmentieren
            // geht nur leider ohne virtuelle Speicheradressen nicht,
            // da sonst die von uns zur�ckgegebenen Pointer ung�ltig werden
            return NULL;
        }

        // Einen Block gefunden, der gro� genug ist
        if ( block->state == not_allocated && block->dataLength >= byteCount ){
            break;
        }

        block = block->nextBlock;
    }

    //UNTERTEILUNG DIESES BLOCKS, SO DASS NICHT UNN�TIG VIEL SPEICHERPLATZ VERBRAUCHT WIRD
    block = splitBlock ( block, byteCount );

    //R�CKGABE DES ZEIGERS AUF DEN ZU BENUTZENDEN SPEICHERBEREICH
    block->state = allocated;
    return (void *) block->data;
}

//Sofern moeglich teilt die Funktion splitBlock einen Block in 2 Bloecke,
//so dass einer der beiden Bloecke <byteCount> viel Speicherplatz enthaelt.
//Die Funktion liefert denjenigen Block zurueck, der <byteCount> viel Speicherplatz
//bereitstellt und passt die verkette Liste an.
memoryBlock* splitBlock(memoryBlock* block, int byteCount)
{
    //TODO - DONE
    // Ist eine aufteilung nicht m�glich?
    if ( block->dataLength < byteCount + memoryBlockHeaderSize + sizeof(char) ){
        return block;
    }

    // Neuen Block nach dem ben�tigten Speicherbereich erstellen
    memoryBlock* newblock;
    newblock = block->data + byteCount;

    // Metainformationen setzen
    newblock->state = not_allocated;
    newblock->data = (void*)((int) newblock + memoryBlockHeaderSize);
    newblock->dataLength = block->dataLength - byteCount - memoryBlockHeaderSize;
    newblock->nextBlock = block->nextBlock;
    block->dataLength = byteCount;
    block->nextBlock = newblock;
    return block;
}


//my_free versucht den Speicherbereich, auf den <p> verweist, wieder freizugeben.
//Sofern <p> in unserem Speicher gefunden wird, muessen benachbarte, freie Speicherbloecke
//miteinander verschmolzen werden.
void my_free(void* p)
{
    if(!b_initialized)
    {
        return;
    }
    //TODO - DONE
    //FREIGEBEN VON DEM ENTSPRECHENDEN SPEICHERBLOCK
    memoryBlock* block;
    block = p - memoryBlockHeaderSize;
    block->state = not_allocated;

    //FREIE SPEICHERBLOECKE MITEINANDER VERSCHMELZEN
    mergeFreeBlocks();
}

//Diese Funktion verschmilzt benachbarte, nicht benutzte Speicherbloecke
void mergeFreeBlocks()
{
    //TODO - DONE
    memoryBlock* block;
    block = head;
    while ( block != NULL ){
        // Können wir den Block mit dem nächsten Block verschmelzen?
        if ( block->state == not_allocated && block->nextBlock != NULL
        	&& block->nextBlock->state == not_allocated ){
            // Die Bl�cke verschmelzen
            block->dataLength += block->nextBlock->dataLength + memoryBlockHeaderSize;
            block->nextBlock = block->nextBlock->nextBlock;
        } else {
			block = block->nextBlock;
        }
    }
}

//Diese Funktion gibt eine Uebersicht ueber die vorhandenen Speicherbl�cke aus
void status()
{
    if(!b_initialized)
    {
        initialize();
        b_initialized = 1;
    }
    memoryBlock *block = head;
    int count = 0;
    printf("Uebersicht des Speichers: %d / %d Speicher frei\n", get_free_space(), memorySize);
    printf("------------------------------------------------\n");
    printf("#  at\t\t allocated\t space\t data\t\t\tnext block\n");
    while(block != NULL)
    {

        printf("%d  %p\t %s \t\t %d\t [%p,%p]\t%p\n", ++count, block, boolStr[block->state], block->dataLength, (block->data), ((char*)block->data + block->dataLength-1), (block->nextBlock));
        block = block->nextBlock;
    }
    printf("\n\n");
}

