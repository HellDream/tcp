#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <regex.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fnmatch.h> 
#define TRUE 1
#define FALSE 0
#define MAX 512

static char *result[MAX];
int pos=0;
int matchName(char *string,char* pattern);
int matchNameBlur(char *string, char* pattern);
char* getPath(char* path,char *filename);
void findFile(int argc, char* argv[], char* path);
int matchContent(char* ,char*);
int nameTag=0;
int namePosition=0;
int contentTag=0;
int contentPosition=0;

int main(int argc, char *argv[]){
	if (argc!=4&&!(strcmp(argv[1],"-name")==0||strcmp(argv[1],"-content")==0)){
		perror("Usage: rfind [-name][-content] <filename> <pathname>");
		return 1;
	}
	for(int i=1;i<argc;i++){
		if(argv[i][0]!='-')
			continue;
		if(strcmp(argv[i],"-name")==0){
			nameTag=TRUE;
			namePosition=i;
		}
		else if(strcmp(argv[i],"-content")==0){
			contentTag=TRUE;
			contentPosition=i;
		}
	}
	findFile(argc,argv, argv[argc-1]);
	printf("finish\n");
	return 0;
}

int matchName(char *string, char* pattern){
	regex_t reg;
	int r = regcomp(&reg, pattern,REG_NOSUB|REG_ICASE);
	if(r!=0){
		perror("regex compile error");
		return 1;
	}
	int status=regexec(&reg,string,(size_t)0,NULL,0);
	regfree(&reg);
	if(status==0)
		return TRUE;
	return FALSE;
}
int matchNameBlur(char* string, char*pattern){
	int ret = fnmatch(pattern, string, FNM_PATHNAME|FNM_PERIOD);
	if(ret==0)
		return TRUE;
	else if(ret==FNM_NOMATCH)
		return FALSE;
	else{
		printf("error file=%s\n", string);
		return FALSE;
	}
}

char* getPath(char* path,char *filename){
	int pStr = strlen(path);
	int fStr = strlen(filename);
	char* newPath = (char*)malloc(pStr+fStr+2);
	newPath[0]= '\0';
	newPath=strcat(newPath,path);
	if(path[pStr-1]!='/'){
		newPath=strcat(newPath,"/");
	}
	newPath =strcat(newPath,filename);
	// printf("%s\n", newPath);
	return newPath;
}

int matchContent(char* content,char* pattern){
	int ret = fnmatch(pattern, content, FNM_PATHNAME|FNM_PERIOD);
	if(ret==0)
		return TRUE;
	else if(ret==FNM_NOMATCH)
		return FALSE;
	else{
		printf("error file content:%s\n", content);
		return FALSE;
	}
}



void findFile(int argc, char*argv[], char *path){
	struct dirent *dirp;
	struct stat statbuf;
	DIR *dp;
	char* filepath;
	if(path==NULL)
		path=argv[argc-1];
	if((dp=opendir(path))==NULL){
		printf("%s\n", path);
		perror("Open dir error");
		return;
	}
		do{
		errno=0;
		if((dirp=readdir(dp))!=NULL){
			int match=TRUE;
			filepath = getPath(path,dirp->d_name);
			if(lstat(filepath,&statbuf)!=0){
				perror("lstat error.");
				continue;
			}
			if(nameTag==TRUE){
				char* filename = argv[namePosition+1];
				if(!matchNameBlur(dirp->d_name, filename)){
					match=FALSE;
				}
			}
			if(contentTag==TRUE){
				char* compareStr = argv[contentPosition+1];
				char comparebuf[1024];
				int fd;
				if ((fd = open(filepath, O_RDONLY)) == -1){
					match = FALSE;
				}
				int n;
				while((n=read(fd, comparebuf, 1024)) >0){
					if(!matchContent(comparebuf, compareStr)){
						match=FALSE;
					}
					else{
						match=TRUE;
						break;
					}
				}
				if(n<0){
					match=FALSE;

				}
				close(fd);

			}
			if(match==TRUE){
				result[pos++]=filepath;
				printf("%s\n", filepath);
			}
			if(S_ISDIR(statbuf.st_mode) &&
				!(strcmp(dirp->d_name,".")==0||strcmp(dirp->d_name,"..")==0)){
				findFile(argc,argv,filepath);
			}


		}
	}while(dirp!=NULL);
	if(errno!=0){
		perror("readdir fail.");
		exit(EXIT_FAILURE);
	}
	closedir(dp);
}
