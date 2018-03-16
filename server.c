
/* SERVER TCP concurent + select */
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PORT 4890

char LIT2[2];
int PRIMUL=1;

extern int errno;            /* eroarea returnata de unele apeluri */

/* functie de convertire a adresei IP a clientului in sir de caractere */
char * Conv_Addr (struct sockaddr_in address)
{
  static char str[25];
  char port[7];

  /* adresa IP a clientului */
  strcpy (str, inet_ntoa (address.sin_addr));
  /* portul utilizat de client */
  bzero (port, 7);
  sprintf (port, ":%d", ntohs (address.sin_port));
  strcat (str, port);
  return (str);
}

void Conv_up (char cuv[])
{
  int i;
  char cuvnou[40];

  for (i=0; i<strlen(cuv); i++)
    if (cuv[i]>='a' && cuv[i]<='z')
    {
          cuvnou[i]=toupper(cuv[i]);
      cuv[i]=cuvnou[i];
    }
}

int Este_CuvBun (char cuv[])
{
  if (cuv[0] != LIT2[0] || cuv[1] != LIT2[1])
    return 0;
  return 1;
}

int Exista_Cuv (char cuv[])
{
  int fd;
  fd=open ("Dictionar.txt", O_RDONLY);
  char c;
  char aux[40];
  int k=0;
  while (read (fd, &c, 1))
  {
    if (c == '\n')
    {
      aux[k]='\0';
      k=0;
      if (strcmp (cuv, aux) == 0)
        return 1;
     }
     else
     {
        aux[k]=c;
        k++;
     }
  }
  return 0;
}

int Raspunde(int fd, int sd, int N, int DIFF, fd_set actfds)
{
  int bytes;                     /* numarul de octeti cititi/scrisi */
  char buffer[1000];             /* mesajul */
  char msg[1000];                /* mesajul primit de la client */
  char m[1000];                  /* mesaj de raspuns pentru client */
  char juc[10];
  char j[10];
  bzero(j, 10);
  bzero(juc, 10);
  bzero(m, 1000);
  bzero(msg, 1000);
  bzero(buffer, 1000);

  bytes = read (fd, msg, sizeof (buffer));
  if (bytes < 0)
  {
    perror ("Eroare la read() de la client.\n");
    return -1;
  }
  printf ("[server]Mesajul a fost receptionat...%s\n", msg);

  int c;
  int i=0;
  FILE *file;
  if (strcmp (msg,"_reguli") == 0)
  {
    file = fopen("Regulament.txt", "r");
    if (file)
    {
      while ((c = getc(file)) != EOF)
      {
        m[i]=c;
        i++;
      }
      fclose(file);
    }
    printf("[server]Trimitem mesajul inapoi...%s\n", m);
    fflush(stdout);
    if (bytes && write (fd, m, 1000) < 0)
    {
      perror ("[server]Eroare la write(reguli) catre client.\n");
      return -1;
    }
        else
          return 1000;
  }
  else if (strcmp (msg,"_comenzi") == 0)
  {
    file = fopen("Comenzi.txt", "r");
    if (file)
    {
      while ((c = getc(file)) != EOF)
      {
        m[i]=c;
        i++;
      }
      fclose(file);
    }
        printf("[server]Trimitem mesajul inapoi...%s\n", m);
    fflush(stdout);
    if (bytes && write (fd, m, 1000) < 0)
    {
      perror ("[server]Eroare la write(comenzi) catre client.\n");
      return -1;
    }
        else
          return 1000;
  }
  else
  {
    int fda;
    for (i=1; i<=N; i++)
    {
      fda=i+DIFF;
      if (fda == fd)
      {
        sprintf(j, "Jucător %d", i);
        sprintf(juc, "%s: ", j);
        break;
      }
    }
    if (strcmp (msg, "_renunt") != 0)
    {
      strcat(m, juc);
      strcat(m, msg);
          strcat(m, "\n");
    }
    else
    {
      sprintf(m, "%s a renunțat și a pierdut.\n", j);
    }

    printf("[server]Trimitem mesajul inapoi...%s\n", m);
    fflush(stdout);

    for (i=1; i<=N; i++)
    {
      fda=i+DIFF;
      if (fda != fd && fda != sd && FD_ISSET (fd, &actfds))
        write (fda, m, 100);
    }
        if (strcmp (msg, "_renunt") != 0 && strcmp (msg, "_reguli") != 0 && strcmp (msg, "_comenzi") != 0)
    {
      Conv_up(msg);
      if (PRIMUL != 1)
      {
            if (Este_CuvBun(msg) == 0 || Exista_Cuv(msg) == 0)
        {
          bzero(m, 1000);
          sprintf(m, "%s nu a introdus un cuvânt bun și a pierdut.\n", j);

          printf("[server]Trimitem mesajul inapoi...%s\n", m);
          fflush(stdout);

          for (i=1; i<=N; i++)
          {
            fda=i+DIFF;
            if (fda != sd && fda != fd && FD_ISSET (fd, &actfds))
              write (fda, m, 100);
          }
          bytes=0;
        }
                else
                {
                  LIT2[0]=msg[strlen(msg)-2];
          LIT2[1]=msg[strlen(msg)-1];
                }
      }
      else
          {
            if (Exista_Cuv(msg) == 0)
        {
          bzero(m, 1000);
          sprintf(m, "%s nu a introdus un cuvânt bun și a pierdut.\n", j);

          printf("[server]Trimitem mesajul inapoi...%s\n", m);
          fflush(stdout);

          for (i=1; i<=N; i++)
          {
            fda=i+DIFF;
            if (fda != sd && fda != fd && FD_ISSET (fd, &actfds))
              write (fda, m, 100);
          }
          bytes=0;
        }
                else
                {
                  PRIMUL = 0;
                  LIT2[0]=msg[strlen(msg)-2];
          LIT2[1]=msg[strlen(msg)-1];
                }
          }
    }
  }

  if (strcmp (msg, "_renunt") == 0)
    bytes=0;
  return bytes;
}

/* programul */
int main()
{
  int MAXIM;
  struct sockaddr_in server;    /* structurile pentru server si clienti */
  struct sockaddr_in from;
  fd_set readfds;               /* multimea descriptorilor de citire */
  fd_set actfds;                /* multimea descriptorilor activi */
  struct timeval tv;            /* structura de timp pentru select() */
  int sd, client;               /* descriptori de socket */
  int optval=1;                 /* optiune folosita pentru setsockopt()*/
  int fd;                       /* descriptor folosit pentru
                                   parcurgerea listelor de descriptori */
  int nfds;                     /* numarul maxim de descriptori */
  int len;                      /* lungimea structurii sockaddr_in */

  int N=0;
  int DIFF=-1;
  int NJR=0;
  char ms[60];
  bzero(ms, 60);
  int i;
  int k=1;

  /* creare socket */
  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
  {
    perror ("[server]Eroare la socket().\n");
    return errno;
  }

  /*setam pentru socket optiunea SO_REUSEADDR */
  setsockopt(sd, SOL_SOCKET, SO_REUSEADDR,&optval,sizeof(optval));

  /* pregatim structurile de date */
  bzero (&server, sizeof (server));

  /* umplem structura folosita de server */
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = htonl (INADDR_ANY);
  server.sin_port = htons (PORT);

  /* atasam socketul */
  if (bind (sd, (struct sockaddr *) &server, sizeof (struct sockaddr)) == -1)
  {
    perror ("[server]Eroare la bind().\n");
    return errno;
  }

  /* punem serverul sa asculte daca vin clienti sa se conecteze */
  if (listen (sd, 5) == -1)
  {
    perror ("[server]Eroare la listen().\n");
    return errno;
  }

  /* completam multimea de descriptori de citire */
  FD_ZERO (&actfds);            /* initial, multimea este vida */
  FD_SET (sd, &actfds);         /* includem in multime socketul creat */

  tv.tv_sec = 1;                /* se va astepta un timp de 1 sec. */
  tv.tv_usec = 0;

  /* deschid și citesc nr. max de jucători din fisierul de configurare */
  char buffini[50];
  FILE *fdini;
  if ((fdini = fopen("config.ini", "r")) == NULL)
  {
    return errno;
  }
  if (fgets(buffini, 50, fdini) != NULL)
  {
    buffini[0]=buffini[strlen(buffini)-2];
    buffini[1]='\0';
    MAXIM=atoi(buffini);
        NJR=MAXIM;
  }
  fclose(fdini);

  nfds = sd;

  printf ("[server]Asteptam la portul %d %d jucători...\n", PORT, MAXIM);
  fflush (stdout);

  while (1)
  {
    /* ajustam multimea descriptorilor activi (efectiv utilizati) */
    bcopy ((char *) &actfds, (char *) &readfds, sizeof (readfds));

    /* apelul select() */
    if (select (nfds+1, &readfds, NULL, NULL, &tv) < 0)
    {
      perror ("[server]Eroare la select().\n");
      return errno;
    }
    /* vedem daca e pregatit socketul pentru a-i accepta pe clienti */
    if (FD_ISSET (sd, &readfds))
    {
      /* pregatirea structurii client */
      len = sizeof (from);
      bzero (&from, sizeof (from));

      /* a venit un client, acceptam conexiunea */
      client = accept (sd, (struct sockaddr *) &from, &len);

      /* eroare la acceptarea conexiunii de la un client */
      if (client < 0)
      {
        perror ("[server]Eroare la accept().\n");
        continue;
      }
      else
        N=N+1;

      if (N > MAXIM)
          {
            printf("[server]Conexiune respinsă.\n");
        fflush (stdout);

        bzero (ms, 60);
        sprintf (ms, "S-au conectat deja cei %d jucători. Nu se mai acceptă alții.\n", MAXIM);

        write (client, ms, 60);
        close (client);
          }
          else
          {
        if (nfds < client)
          nfds = client;

        FD_SET (client, &actfds);

        if (DIFF == -1)
          DIFF=client-N;

        printf("[server]S-a conectat jucatorul %d cu descriptorul %d, de la adresa %s.\n",N,client, Conv_Addr (from));
        fflush(stdout);

        bzero (ms, 60);
        sprintf (ms, "Bun venit jucător %d.\n", N);

        printf("[server]Trimitem mesajul inapoi...%s\n", ms);
        fflush(stdout);

        write (client, ms, 60);
      }
    }
    int rasp;
    for(fd=0;fd<=MAXIM+DIFF;fd++)
      if (fd != sd && FD_ISSET (fd, &readfds))
      {
             rasp=Raspunde(fd, sd, MAXIM, DIFF, actfds);
         if (rasp == 0)
         {
            PRIMUL=1;
            NJR--;
            write (fd, "_quit", 10);
            printf ("[server]S-a deconectat jucătorul %d cu descriptorul %d.\n", fd-DIFF, fd);
            fflush (stdout);
            close (fd);
            FD_CLR (fd, &actfds);
          }
        }
        if (NJR == 1)
        {
      for (i = 1; i <= MAXIM; i++)
      {
        fd=i+DIFF;
        if (fd != sd && FD_ISSET (fd, &actfds))
        {
           write (fd, "Ai căștigat! Felicitări!", 30);
           NJR--;
           printf ("[server]S-a deconectat jucătorul %d cu descriptorul %d.\n", fd-DIFF, fd);
           fflush (stdout);
           close (fd);
           FD_CLR (fd, &actfds);
           FD_ZERO(&actfds);
           return 0;
        }
      }
    }
  }                         /* while */
}                           /* main */
