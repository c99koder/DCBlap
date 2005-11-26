/* utils.cc - misc. utility functions
 * Copyright (c) 2001-2005 Sam Steele
 *
 * This file is part of DCBlap.
 *
 * DCBlap is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
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

#if TIKI_PLAT == TIKI_WIN32
#include <windows.h>
#endif

#include <Tiki/tiki.h>
#include <ctype.h>
#include <string.h>
#include <vector>
#include <sys/stat.h>
#if (TIKI_PLAT == TIKI_OSX || TIKI_PLAT == TIKI_SDL)
#include <dirent.h>
#endif

void debug(char *text);

void string_tolower(char *in) {
  int i;
  //printf("string_tolower(%s)\n",in);
  for(i=0;i<strlen(in);i++) {
	//printf("tolower(%c)\n",in[i]);
    in[i]=tolower(in[i]);
  }
}

bool file_exists(char *path) {
#if TIKI_PLAT == TIKI_DC
	file_t fd=fs_open(path,O_RDONLY);
	if(fd!=-1) {
		fs_close(fd);
		return true;
	}
	return false;
#else
	struct stat stat_info;
	return (stat(path,&stat_info)==0);
#endif
}

void scan_directory(char *path, std::vector<std::string> *s, std::vector<bool> *d, int &count) {
#ifdef WIN32
	BOOL            fFinished;
	HANDLE          hList;
	TCHAR           szDir[MAX_PATH+1];
	WIN32_FIND_DATA FileData;
		int x=0;
		
    // Get the proper directory path
    sprintf(szDir, "%s\\*", path);
		
    // Get the first file
    hList = FindFirstFile(szDir, &FileData);
    if (hList == INVALID_HANDLE_VALUE)
    { 
			printf("No files found\n\n");
    }
    else
    {
			// Traverse through the directory structure
			fFinished = FALSE;
			while (!fFinished)
			{
				// Check the object is a directory or not
				if ((strcmp(FileData.cFileName, ".") != 0) &&
						(strcmp(FileData.cFileName, "..") != 0))
				{
					s->push_back(FileData.cFileName);
					d->push_back(FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
					x++;
				}
				
				if (!FindNextFile(hList, &FileData))
				{
					if (GetLastError() == ERROR_NO_MORE_FILES)
					{
						fFinished = TRUE;
					}
				}
			}
    }
		
    FindClose(hList);
		count=x;
#else
#if TIKI_PLAT == TIKI_DC
		dirent_t *de;
		uint32 fd;
		int x=0;
		fd = fs_open(path,O_RDONLY|O_DIR); 
		
		while ( (de=fs_readdir(fd)) ) {
			if(de->name[0]!='.') {
				s->push_back(de->name);
				d->push_back(de->attr == O_DIR);
				x++;
			}
		}
		fs_close(fd);
		count=x;
#else
		DIR *dp;
    struct dirent *dir_entry;
    struct stat stat_info;
		char tmp[100];
		int x=0;
		
    if((dp = opendir(path)) == NULL) {
			fprintf(stderr,"cannot open directory: %s\n", path);
			return;
    }
    while((dir_entry = readdir(dp)) != NULL) {
			/* ignore . and .. */
			if(strcmp(".",dir_entry->d_name) == 0 ||
				 strcmp("..",dir_entry->d_name) == 0)
				continue;
			sprintf(tmp,"%s/%s",path,dir_entry->d_name);
			s->push_back(dir_entry->d_name);
			lstat(tmp,&stat_info);
			d->push_back(S_ISDIR(stat_info.st_mode));
			x++;
    }
    closedir(dp);
		count=x;
#endif
#endif
}
