#fisier folosit pentru compilarea serverului și clientului

all:
        gcc server.c -o server
        gcc client.c -o client
clean:
        rm -f *~client server
