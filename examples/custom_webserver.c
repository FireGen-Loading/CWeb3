#include "../include/soc.h"
#include "../include/CWEB3Http.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>

char* readFile(const char* path, uint64_t* pLength) {
	FILE* file = fopen(path, "rb");

	fseek(file, 0, SEEK_END);
	 *pLength = ftell(file);
	fseek(file, 0, SEEK_SET);

	char* buffer = (char *)malloc((*pLength) + 1);
	fread(buffer, 1 , *pLength, file);
	fclose(file);
	buffer[*pLength] = 0;

	return buffer;
}

int main() {
    CWeb3Config config;
    config.host = "127.0.0.1";
    config.port = 10004;
    config.protocol = TCP;
    config.verbose = 1;

    CWeb3Socket server = newCWeb3Socket(config);
    if (!server.socket) printf("err on serv sock");
    while (1)
    {
        // wait till the client connects
        CWeb3Socket client = CWeb3Listen(server);
        if (!client.socket) printf("err on clin sock");

        // read the client message
        size_t messageSize;
        char* messageBuffer = CWeb3Recv(client, &messageSize);
        CWEB3HTTPRequest request = CWEB3ParseRequest(messageBuffer);

        char user[] = "User-Agent";
        HashItem key;
        key.pItem = user;
        key.size = sizeof(user);

        printf("message size: %zu\n", messageSize);
        printf("path: %s\n", request.path);
        printf("user: %s\n", getHashValue(request.header, key).pItem);
    	// HEADER IS BROKEN
        // making the response message 
        uint64_t len;
        char* File = readFile("index.html", &len);

        // send response
        CWEB3HTTPData data;
        data.codeNum = 200; // 200 OK
        data.conentType = contentHtml;
        data.version.major = 1; // Http 1.1
        data.version.minor = 1;

        CWeb3HttpRespond(client, File, data);
        free(File);

        /*
        data.codeNum = 100;
        data.conentType = contentNone;
        CWeb3HttpRespond(client, File, data);*/

        
          
        // close client socket
        shutdown(client.socket, SHUT_WR);
    }
    // close server socket
    shutdown(server.socket, SHUT_WR);
}