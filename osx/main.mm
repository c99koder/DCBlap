#import <Cocoa/Cocoa.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>

int main(int argc, const char *argv[])
{
	char text[PATH_MAX];
	char text2[100];
	int len=0;
	char *val;
	struct stat s;

	srand(time(NULL));

	if(readlink(argv[0],text,100)==-1) strcpy(text,argv[0]);

	if(text[0]!='/' && text[0]!='.') {
		
		val=strtok(getenv("PATH"),":");
		do {
		  sprintf(text2,"%s/%s",val,argv[0]);
			//printf("Searching: %s\n",text2);
			if(lstat(text2,&s)!=-1) {
				//printf("Found it!\n");
				if(S_ISLNK(s.st_mode)) {
					readlink(text2,text,100);
				} else {
				  strcpy(text,text2);
				}
				break;
			}
		  val=strtok(NULL,":");
		} while(val!=NULL);
	}

	for(len=strlen(text);len>=0;len--) {
	  if(text[len]=='/') {
	    text[len]='\0';
	    break;
	  }
	}

	chdir(text);
	//chdir("../../..");
	chdir("../Resources/data");
  return NSApplicationMain(argc, argv);
}
