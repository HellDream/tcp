#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#define BUFFERSIZE 1024

void copy(int in, int out, char* buffer){
	int r, w;
	while((r=read(in, buffer, BUFFERSIZE))>0){
		while((w=write(out, buffer, r))){
			if((w==-1)){
			break;
			}
			else if(w==r){
			break;
			}
		}
		if(w<0){
			printf("write error");
		}	
	}	
}	

int main(int argc, char *argv[]){

	if (argc!=3){
		printf("Use %s file1 file2\n",argv[0]);
		return 1;
	}
	DIR *dir=opendir("./");
	int dirfd2 = dirfd(dir);	
	char buffer[BUFFERSIZE];
	int in = open(argv[1], O_RDONLY,S_IRUSR);
	if(in<0){
		printf("file open error!\n");
		close(in);
		return 1;
	}
	int out = open(argv[2],O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR);
	if(out<0){
		dir = opendir(argv[2]);
		if(dir==NULL){
			perror("open dir fail!");
			return 1;
		}
		dirfd2 = dirfd(dir);
		out = openat(dirfd2, argv[1], O_CREAT|O_WRONLY|O_TRUNC);
		fchmod(out, S_IRWXU|S_IRWXG|S_IRWXO);
	}
	copy(in, out, buffer);	
	close(in);
	close(out);
	return 0;
}

