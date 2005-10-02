/* utils.cc - misc. utility functions
 * Copyright (c) 2001-2003 Sam Steele
 *
 * This file is part of DCBlap.
 *
 * DCBlap is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * DCBlap is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */
#ifdef WIN32
#include <io.h>
#include <fcntl.h>
#include <time.h>
#include <stdio.h>
#include <kos_w32.h>
#include <windows.h>
#include <winsock.h>
#include <basetsd.h>
#include <commdlg.h>
#include <mmreg.h>
#include <dxerr9.h>
#include <dsound.h>
#include <dshow.h>
#include "DSUtil.h"
#include "DXUtil.h"
#include "word.h"
#endif
#ifdef DREAMCAST
#include <kos.h>
#endif

#ifdef UNIX
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <kos_w32.h>
#include <stdio.h>
#endif
#include <ctype.h>
#include <string.h>

void debug(char *text);

#ifdef WIN32
IGraphBuilder *pGraph = NULL;
IMediaControl *pControl = NULL;
IMediaEvent   *pEvent = NULL;
IMediaSeeking *pSeek = NULL;
extern HWND		hWnd;

CSoundManager* g_pSoundManager = NULL;

void audio_init() {
  debug("Initializing COM");
	HRESULT hr = CoInitialize(NULL);
    if (FAILED(hr))
    {
        return;
    }
    g_pSoundManager = new CSoundManager();
    if( NULL == g_pSoundManager )
    {
        DXTRACE_ERR_MSGBOX( TEXT("Initialize"), E_OUTOFMEMORY );
        EndDialog( hWnd, IDABORT );
        return;
    }

    if( FAILED( hr = g_pSoundManager->Initialize( hWnd, DSSCL_PRIORITY ) ) )
    {
        DXTRACE_ERR_MSGBOX( TEXT("Initialize"), hr );
        MessageBox( hWnd, "Error initializing DirectSound.  Sample will now exit.", 
                            "DirectSound Sample", MB_OK | MB_ICONERROR );
        EndDialog( hWnd, IDABORT );
        return;
    }
    
    if( FAILED( hr = g_pSoundManager->SetPrimaryBufferFormat( 2, 22050, 16 ) ) )
    {
        DXTRACE_ERR_MSGBOX( TEXT("SetPrimaryBufferFormat"), hr );
        MessageBox( hWnd, "Error setting buffer format.  Sample will now exit.", 
                          "DirectSound Sample", MB_OK | MB_ICONERROR );
        EndDialog( hWnd, IDABORT );
        return;
    }
}

void play_bgm(LPCWSTR fn) {
    // Create the filter graph manager and query for interfaces.
    HRESULT hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, 
                        IID_IGraphBuilder, (void **)&pGraph);
    if (FAILED(hr))
    {
        printf("ERROR - Could not create the Filter Graph Manager.");
        return;
    }

    hr = pGraph->QueryInterface(IID_IMediaControl, (void **)&pControl);
    hr = pGraph->QueryInterface(IID_IMediaEvent, (void **)&pEvent);
    hr = pGraph->QueryInterface(IID_IMediaSeeking, (void **)&pSeek);

    pControl->RenderFile((BSTR)fn);
    pControl->Run();
}

void stop_bgm() {
	if(pControl) {
    pControl->Stop();
    pControl->Release();
    pEvent->Release();
    pGraph->Release();
    pSeek->Release();
    pControl=NULL;
    pEvent=NULL;
    pSeek=NULL;
    pGraph=NULL;
  }
}

void audio_shutdown() {
  stop_bgm();
    CoUninitialize();
}
#endif

void string_tolower(char *in) {
  int i;
  //printf("string_tolower(%s)\n",in);
  for(i=0;i<strlen(in);i++) {
	//printf("tolower(%c)\n",in[i]);
    in[i]=tolower(in[i]);
  }
}

char file_read_byte(int fd) {
	char buf[2];
	fs_read(fd,buf,1);
	return buf[0];
}

void get_string_from_file(int in, char *text, int len) {
  int x;
  for(x=0;x<len;x++) {
    text[x]=file_read_byte(in);
  }
  text[len]='\0';
}

void delay(float seconds) {
#ifdef WIN32
	clock_t delayend=clock()+(seconds * CLOCKS_PER_SEC);
	while(clock() < delayend);
#endif
#ifdef DREAMCAST
  uint32 s,ms,tm;
  timer_ms_gettime(&s,&ms);
  tm=(s*1000)+ms;
  do {
    timer_ms_gettime(&s,&ms);
  } while ((s*1000)+ms < tm + (seconds*1000));
#endif
#ifdef linux
  struct timeval tm;
  long ms=(seconds*1000.0f);
  long end;
  gettimeofday(&tm,0);
  end=tm.tv_sec*1000000+tm.tv_usec+(ms*1000);
  while(tm.tv_sec*1000000+tm.tv_usec < end) {
    gettimeofday(&tm,0);
  }
#endif
}

#ifdef WIN32
void c99_net_init() {
    WSAData wsaData;
    WSAStartup(MAKEWORD(1, 1), &wsaData);
}

void socket_write_line(SOCKET socket, char *text) {
	send(socket,text,strlen(text),0);
}

void socket_read_line(SOCKET socket, char *return_text) {
  int i=0;
  static int bufpos=0;
  static int bufend=0;
  static char buf[256];
 
  if(bufpos>=bufend) {
    bufend=recv(socket,buf,256,0);
    bufpos=0;
    if(bufend<1) { return_text[0]='\0'; return; }
  }
  do {
    if(bufpos>=bufend) {
      bufpos=0; 
      bufend=recv(socket,buf,256,0);
		//fprintf(stderr,"Waiting for new data...\n");
    }
    return_text[i]=buf[bufpos++];
    i++;
  } while(return_text[i-1]!='\n');
  return_text[i-1]='\0';
  //fprintf(stderr,"Returning: %s\n",return_text);
}

SOCKET connect_to_host(char *ip, int port) {
	SOCKET file_descriptor;	// File descriptor that represents the client socket.
	struct sockaddr_in target_host_address;	// Structure to hold the target address of the connection. (The i.p. address and port of the host we want to connect to.)
	unsigned char *address_holder;	// Pointer to simplify ip address designation.
  unsigned long nonblock = 1;
	file_descriptor=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(file_descriptor<0) {
		printf("Cannot create socket!\n");
		return -1;
	}
 	memset((void*)&target_host_address, 0, sizeof(target_host_address));
	target_host_address.sin_family=AF_INET;
	target_host_address.sin_port=htons(port);
	wordify(ip,'.');
	address_holder=(unsigned char*)&target_host_address.sin_addr.s_addr;
	address_holder[0]=atoi(get_word(0));
	address_holder[1]=atoi(get_word(1));
	address_holder[2]=atoi(get_word(2));
	address_holder[3]=atoi(get_word(3)); 
	if (connect(file_descriptor, (struct sockaddr*)&target_host_address, sizeof(target_host_address))<0) {
		printf("Couldn't connect to host.\n");
		return -1;
	}
ioctlsocket(file_descriptor, FIONBIO, &nonblock);
	return file_descriptor;
}

SOCKET wait_for_connect(int port) {
	// Next, create the listening socket
	SOCKET listeningSocket;

	listeningSocket = socket(AF_INET,		// Go over TCP/IP
			         SOCK_STREAM,   	// This is a stream-oriented socket
				 IPPROTO_TCP);		// Use TCP rather than UDP

	// Use a SOCKADDR_IN struct to fill in address information
	SOCKADDR_IN serverInfo;

	serverInfo.sin_family = AF_INET;
	serverInfo.sin_addr.s_addr = INADDR_ANY;	// Since this socket is listening for connections,
							// any local address will do
	serverInfo.sin_port = htons(port);		// Convert integer 8888 to network-byte order
							// and insert into the port field


	// Bind the socket to our local server address
	bind(listeningSocket, (LPSOCKADDR)&serverInfo, sizeof(struct sockaddr));

	// Make the socket listen
	listen(listeningSocket, 10);		// Up to 10 connections may wait at any
							// one time to be accept()'ed

	// Wait for a client
	SOCKET theClient;

	theClient = accept(listeningSocket,
			   NULL,			// Address of a sockaddr structure (see explanation below)
			   NULL);			// Address of a variable containing size of sockaddr struct

	closesocket(listeningSocket);


	// Shutdown Winsock
  return theClient;
}
#endif
