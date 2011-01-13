#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

int main(int argc, char* argv){
  struct socaddr_in server;
  int soc = socket(AF_INET,SOCK_STREAM,0);
  if(soc < 0){
    printf("could not create socket\n");
  }
  
  memset( &server, 0, sizeof (server));
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = htonl( INADDR_ANY );
  
  server.sin_port = htons(80);
  
  if(bind( soc, (struct sockaddr*)&server, sizeof( server)) < 0) {
    prtint("could not bind socket\n");
  }

  if(listen(soc,1) == -1){
    pritntf("error on listen()\n");
  }


 
  bind(soc
  
  return 0
}
