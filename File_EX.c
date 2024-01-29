#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/types.h>
#include <errno.h>
#include <time.h>

#define KILO_BYTE 1024

void errno_show();
int search(char *search_target,char *path);
int listDIR(char *path);


int search(char *search_target,char *path){
	DIR *DirP = opendir(path);
	if(!DirP){
		errno_show();
	}
	static int Found;

	struct dirent *Fp;
	while(!Found) {
		Fp = readdir(DirP);
		if(!Fp){
			return 1;
		}
		if(strcmp(Fp->d_name,".") == 0 || strcmp(Fp->d_name,"..") == 0){
			continue;
		}

		char root_path[500];

		snprintf(root_path,sizeof(root_path),"%s/%s",path,Fp->d_name);

		if(strstr(Fp->d_name,search_target) != NULL){
			printf("FOUND %s At %s",Fp->d_name,root_path);
			Found = 1;
			return 1;
		} else if(Fp->d_type == DT_DIR){
			search(search_target,root_path);
		}
	}
	if(closedir(DirP) != 0){
		errno_show();
	}
	return 1;
}

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
		default:	exit(0); 			break;
	}
}

int listDIR(char *path){
	DIR *DirP;

	DirP = opendir(path);
	if(DirP == NULL){
		errno_show();
		return 0;
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
			return 0;
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
		return 0;
	}
	return 0;
}

int main(int argc,char **argv) {
	char *path = "/mnt/d";
	listDIR(path);
	char search_target[30];
	printf("What do you want to search?");
	scanf("%s",search_target);
	search(search_target,path);
	return 1;
}
