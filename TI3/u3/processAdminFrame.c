#include <stdio.h>
#include <stdlib.h>


/*Implementierung einer Prozessverwaltung:

Implementieren Sie die durch das Framework vorgegebene Prozessverwaltung,
die eine Menge von Prozessen aus der Datei prcs.dat auslesen,und diese
in eine doppelt verkettete Liste einfuegen soll.

Nun soll Prozess um Prozess der Reihenfolge ihrer ids entsprechend
aus der Liste entfernt werden,wobei die uebrig gebliebenen Prozesse in jeder
Iteration ausgegeben werden.

Die Prozesse sind in der Datei prcs.dat Zeile fuer Zeile in der Form:
    ProzessId,Ankunftszeit,Ausfuehrungszeit
gespeichert.

*/

struct pData //Daten der doppelt verketteten Liste
{
 int pId;//ProzessID
 int aTime;//Ankunftszeit
 int sTime;//Ausfuehrungszeit

  /*Struktur vervollstaendigen */
};

//Um Tipparbeit zu sparen
typedef struct pData PROCESS;
typedef PROCESS *LINK;

LINK findNextProcess(LINK head);

void addProcess(LINK head,LINK neu);
void deleteProcess(LINK current);

void printPr(LINK current);//Methode zur Ausgabe eines Prozesses

void printList(LINK head);//Alle Prozesse der Liste ausgeben

void readProcesses(LINK head);//Einlesen der Prozesse

int main(void)
{
 LINK next;
 LINK head;

/*TODO:head initialisieren*/

 readProcesses(head);
 while(head->next!=head)
 {
   printList(head);
   next=findNextProcess(head);
   deleteProcess(next);
 }

return 0;
}

/*==========================================================================
Implementierung der Methoden*/


//Liest die Prozesse ein und erstellt die Liste
void readProcesses(LINK head)
{
 /*TODO:implementieren*/
}


//Einen Prozess an das Ende der Liste (also vor head) einfuegen
void addProcess(LINK head,LINK neu)
{
 /*TODO:implementieren*/
}

//Loeschen des angegebenen Knotens
void deleteProcess(LINK current)
{
 /*TODO:implementieren*/
}


/*gibt den Prozess mit der kleinsten Id aus der Liste zurueck*/

LINK findNextProcess(LINK head)
{
 /*TODO:implementieren*/
}


/*Ausgabemethoden fuer die Prozesse und die gesamte Liste*/
void printPr(LINK current)
{
 printf("\n pId:%d arrival time:%d service time:%d\n ",current->pId,current->aTime,current->sTime);
}

void printList(LINK head)
{
     printf("\n\n Current QUEUE of PROCESSES ---------------------------------------\n\n");
     LINK curr=head->next;
     while(curr!=head)
     {
      printPr(curr);
      curr=curr->next;
     }
}


