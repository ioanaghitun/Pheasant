/* CLIENT */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include<netinet/in.h>
#include<signal.h>

#define PORT 4890
#define ADRESA "0.0.0.0"

int MERGE = 1;

/* codul de eroare returnat de anumite apeluri */
extern int errno;

int main (int argc, char *argv[])
{
  int sd;                       // descriptorul de socket
  struct sockaddr_in server;    // structura folosita pentru conectare
  char msg[1000];               // mesajul trimis

  if(argc > 1)
  {
    printf("\nclientul se apeleaza fara parametrii\n");
    return -1;
  }

  /* cream socketul */
  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
  {
    perror ("Eroare la socket().\n");
    return errno;
  }

  /* umplem structura folosita pentru realizarea conexiunii cu serverul */
  /* familia socket-ului */
  server.sin_family = AF_INET;
  /* adresa IP a serverului */
  server.sin_addr.s_addr = inet_addr(ADRESA);
  /* portul de conectare */
  server.sin_port = htons(PORT);

  /* ne conectam la server */
  if (connect (sd, (struct sockaddr *) &server,sizeof (struct sockaddr)) == -1)
  {
    perror ("Eroare la connect().\n");
    return errno;
  }

  if (read (sd, msg, 1000) < 0)
  {
    perror ("Eroare la read() de la server.\n");
    return errno;
  }

  if (strstr(msg, "Nu se mai acceptă alții") != NULL)
  {
    printf ("%s\n", msg);
    fflush(stdout);
    bzero(msg,100);
    MERGE=0;
    close(sd);
    return 0;
  }
  else
  {
    printf ("%s\n", msg);
    fflush(stdout);
    printf("Bun venit la jocul FAZAN!\n\n");
    fflush(stdout);
    int c;
    FILE *file;
    file = fopen("Comenzi.txt", "r");
    if (file)
    {
      while ((c = getc(file)) != EOF)
        putchar(c);
      fclose(file);
    }
    printf("\n");
    fflush(stdout);

    if (fork())
    {
      while (1)
      {
        if (MERGE == 0)
        {
          exit(0);
        }
        else
        {
          bzero (msg, 1000);
          read (0, msg, 1000);
          msg[strlen(msg)-1]='\0';
          if (write (sd, msg, 1000) <= 0)
          {
            perror ("Eroare la write() spre server.\n");
            return errno;
          }
          if (strcmp(msg,"_renunt") == 0)
          {
            printf("Ai renunțat.\n");
            fflush(stdout);
            exit(0);
          }
        }
      }    /* while */
    }
    else
    {
      while (1)
      {
        if (read (sd, msg, 1000) < 0)
        {
          perror ("Eroare la read() de la server.\n");
          return errno;
        }
        else if (strstr(msg, "_quit") != NULL)
        {
          printf ("Ai pierdut.\n");
          fflush(stdout);
          MERGE=0;
          exit(0);
        }
        else
        {
          printf ("%s\n", msg);
          fflush(stdout);
          if (strstr(msg, "Ai căștigat! Felicitări!") != NULL)
          {
            MERGE=0;
            exit(0);
          }
        }
      }   /* while */
    }     /* else */
    close(sd);
    return 0;
  }
}        /* main */
