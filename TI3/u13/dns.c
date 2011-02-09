#include <pcap.h>
#include <stdio.h>

#define DNS_FILTER "udp port 53"
#define DNS_HEADER_LEN 12
#define DNS_RESPONSE_FLAG 0x8000
#define DNS_TYPE_CNAME 0x05
#define DNS_TYPE_ADDRESS 0x01

/* am besten mit wireshark herausfinden: */
struct DNSPacket {
    char MACHeader[14];
    char IPHeader[12];
    unsigned char srcIP[4];
    unsigned char destIP[4];
    char UDPHeader[8];
    short DNS_Id;
    char DNS_Operation_high;
    char DNS_Operation_low;
    char DNS_QDCount_high;
    char DNS_QDCount_low;
    char DNS_ANCount_high;
    char DNS_ANCount_low;
    short DNS_NSCount;
    short DNS_ARCount;
};
typedef struct DNSPacket tDNSPacket;


pcap_t* initSniffer ( char* device, char* errbuf );
void sniffDNS ( pcap_t* sniffer, int packetCount );
void readHostname (unsigned char* packet, int* pos, char praefix);

int main(int argc, char *argv[])
{
    int packetCount;
    char *device;
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t* sniffer;

    // usage
    printf("usage: %s [n [d]]\n",argv[0]);
    printf("  n: anzahl zu sniffender packets\n");
    printf("  d: device (z.B. eth0, wlan0)\n");

    // n
    if ( argc >= 2 ){
        packetCount = atoi(argv[1]);
    } else {
        packetCount = 10000;
    }

    // device
    if ( argc >= 3 ){
        device = argv[2];
    } else {
        char errbuf[PCAP_ERRBUF_SIZE];
        device = pcap_lookupdev(errbuf);
        if (device == NULL) {
            fprintf(stderr, "Couldn't find default device: %s\n", errbuf);
            return 2;
        }
    }

    /* sniffer initialisieren */
    sniffer = initSniffer ( device, errbuf );
    if ( sniffer == NULL ){
        fprintf(stderr, "Fehler: %s\n", errbuf);
        return 2;
    }

    /* DNS Pakete abfangen */
    sniffDNS ( sniffer, packetCount );
    return 0;
}

pcap_t* initSniffer ( char* device, char* errbuf ){
    pcap_t *handle;         /* sniffer */
    struct bpf_program fp;  /* The compiled filter */
    bpf_u_int32 mask;       /* Our netmask */
    bpf_u_int32 net;        /* Our IP */

    /* Find the properties for the device */
    if (pcap_lookupnet(device, &net, &mask, errbuf) == -1) {
        net = 0;
        mask = 0;
    }

    /* Open the session in promiscuous mode */
    handle = pcap_open_live(device, BUFSIZ, 1, 1000, errbuf);
    if (handle == NULL) {
        return NULL;
    }

    /* Compile and apply the filter */
    if (pcap_compile(handle, &fp, DNS_FILTER, 0, net) == -1) {
        return NULL;
    }
    if (pcap_setfilter(handle, &fp) == -1) {
        return NULL;
    }
    return handle;
}

void sniffDNS ( pcap_t* sniffer, int packetCount ){
    unsigned char* packet;       /* The actual packet */
    struct pcap_pkthdr header;  /* The header that pcap gives us */
    tDNSPacket* dnsPacket;
    int i, j, k;

    for (i=0; i<packetCount; ++i){
        packet = (unsigned char*) pcap_next(sniffer, &header);
        if (packet == 0){
            printf("Timeout.\n");
            fflush(stdout);
        } else {
            // IP Adressen ausgeben
            dnsPacket = (tDNSPacket*) packet;
            unsigned char* src = dnsPacket->srcIP;
            unsigned char* dest = dnsPacket->destIP;
            printf("%u.%u.%u.%u an %u.%u.%u.%u:",
                src[0],src[1],src[2],src[3],dest[0],dest[1],dest[2],dest[3]);

            // Angefragte Adresse(n) ausgeben
            int pos = sizeof(tDNSPacket);
            for(j = 0; j < dnsPacket->DNS_QDCount_low; ++j){
                readHostname(packet,&pos,' ');
                pos += 4; // Type und Class sind egal
            }

            // evtl. eine Antwort?
            if ( dnsPacket->DNS_Operation_high & DNS_RESPONSE_FLAG ){
                printf(" Antwort:\n");

                // Alle (!) Antworten ausgeben
                for(j = 0; j < dnsPacket->DNS_ANCount_low; ++j){
                    // Wiederholung der angefragten Adresse
                    printf("->");
                    readHostname(packet,&pos,' ');

                    // Typ bestimmen
                    unsigned char answerType = packet[++pos];
                    pos += 9; // uninteressante Felder

                    if(answerType == DNS_TYPE_ADDRESS){
                        printf(" steht fuer %u.%u.%u.%u\n",
                            packet[pos++],packet[pos++],
                            packet[pos++],packet[pos++]);
                    }
                    else if (answerType == DNS_TYPE_CNAME){
                        printf(" ist ein Alias fuer");
                        readHostname(packet,&pos,' ');
                        printf("\n");
                    } else {
                        printf(" ERROR: unknown answer type\n");
                    }
                }
            } else {
                printf("\n");
            }
        }
    }
    pcap_close(sniffer);
}

// hostnamen lesen und ausgeben
void readHostname (unsigned char* packet, int* pos, char praefix){
    int i, len;
    while(1){
        // Länge lesen
        len = packet[(*pos)++];

        // Sind wir fertig?
        if ( len == 0 ){
            return;
        }

        // Handelt es sich um einen wiederholten Hostnamen in der Antwort?
        else if ( len == 0xC0 ){
            // den String ausgeben, der durch den Pointer referenziert wird
            int pointer = (len << 8) & 0x3F; // die ersten beiden Bits weg
            pointer += packet[(*pos)++]; // das nächste Byte zählt dazu
            pointer += sizeof(tDNSPacket)-DNS_HEADER_LEN; // header offset
            readHostname(packet,&pointer,praefix);
            return;
        }

        // Sonst ist es die Länge des Namens
        else {
            // Erstes Trennzeichen ist ein Leerzeichen, danach Punkte
            printf("%c",praefix);
            praefix = '.';

            // Jetzt die Zeichen lesen
            for(i=0;i<len;++i){
                printf("%c",packet[(*pos)++]);
            }
        }
    }
}
