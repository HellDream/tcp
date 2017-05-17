#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdlib.h>
#define BUFFERSIZE 1024

int main(int argc, char* argv[]){
	int in,out;
	void *src, *dest;
	struct stat statbuf;
	DIR *dir=opendir("./");
	int dirfd2 = dirfd(dir);
	if(argc != 3){
		printf("use %s file1 file2", argv[1]);
		exit(1);
	}
	in = open(argv[1], O_RDONLY,S_IRUSR);
	if(in<0){
		printf("file open error!\n");
		close(in);
		exit(1);
	}
	if(fstat(in, &statbuf)<0){
		printf("fstat fail");
		exit(1);
	}
	out = open(argv[2],O_RDWR|O_CREAT|O_TRUNC,0600);
	if(out<0){
		dir = opendir(argv[2]);
		if(dir==NULL){
			perror("open dir fail!");
			return 1;
		}
		dirfd2 = dirfd(dir);
		out = openat(dirfd2, argv[1], O_CREAT|O_RDWR|O_TRUNC,0600);
		fchmod(out,S_IRWXU|S_IRWXG|S_IRWXO);
	}
	if(lseek(out,statbuf.st_size-1,SEEK_SET)<0){
		printf("fail lseek out");
		exit(1);
	}
	if(write(out, &statbuf,1)!=1){
		printf("write error");
		exit(1);
	}
	src = mmap(NULL,(size_t)statbuf.st_size,PROT_READ,MAP_PRIVATE| MAP_NORESERVE,in,0);
	if(src==MAP_FAILED){
		printf("src fail");
		return 1;
	}
	dest = mmap(0,(size_t)statbuf.st_size,PROT_WRITE,MAP_SHARED,out,0);
	if(dest==MAP_FAILED){
		printf("dest2 fail");
		return 1;
	}
	memcpy(dest, src,(size_t)statbuf.st_size);

	munmap(src,statbuf.st_size);
	munmap(dest, statbuf.st_size);
	close(in);
	close(out);
	exit(0);	
}
