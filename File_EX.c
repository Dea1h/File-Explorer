#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/types.h>
#include <errno.h>
#include <time.h>

#define KILO_BYTE 1024

void errno_show(){
	switch(errno){
		case EACCES: 	printf("Permission denied");			break;
		case EBADF:	printf("Bad file descriptor");			break;
		case EBADFD: 	printf("File descriptor in bad state.");	break;
		case EDQUOT: 	printf("Disk quota exceeded");			break;
		case EFBIG:  	printf("File Too Large");			break;
		case EINVAL: 	printf("Invalid argument");			break;
		case ENOENT: 	printf("No such File or Directory");		break;
		case ENOTEMPTY:	printf("Directory not empty");			break;
		case ENOTDIR:	printf("Not a directory");			break;
		case EROFS:	printf("Read-Only Filesystem");			break;
		default:		  					break;
	}
}

int listDIR(char *path){
	DIR *DirP;

	DirP = opendir(path);
	if(DirP == NULL){
		errno_show();
		return -1;
	}

	struct dirent *Fp;
	printf("Date Modified\t\tType\tSize\tName\n");

	while((Fp = readdir(DirP)) != NULL) {
		if(strcmp(Fp->d_name,".") == 0 || strcmp(Fp->d_name,"..") == 0) {
			continue;
		}

		char root_path[PATH_MAX];
		snprintf(root_path,sizeof(root_path),"%s/%s",path,Fp->d_name);

		struct stat statBuf;
		if(lstat(root_path,&statBuf) < 0) {
			errno_show();
			return -1;
		}
		
		time_t modtime = statBuf.st_mtime;
		struct tm *tm_info = localtime(&modtime);
		char modTimeStr[20];
		strftime(modTimeStr,sizeof(modTimeStr),"%Y-%m-%d %H:%M:%S",tm_info);
		printf("%s\t",modTimeStr);
		
		switch(Fp->d_type){
			case DT_REG: printf("FILE\t");		break;
			case DT_DIR: printf("DIR\t");		break;
			default:     printf("UN\t");		break;
		}

		if(Fp->d_type == DT_REG) {
			printf("%.2lf\t",(double)statBuf.st_size / KILO_BYTE);
		} else {
			printf("\t");
		}

		printf("%s",Fp->d_name);
		printf("\n");
	}
	if(closedir(DirP) < 0) {
		errno_show();
		return -1;
	}
	
	return 0;
}

int main(int argc,char **argv) {
	char *path = "/mnt";
	listDIR(path);
	return 1;
}
