#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main()
{
	int index = 1;
	int status;
//	while(1)
//	{
		int pid;
		pid = fork();
		if(pid == 0) {
			char filename[10];
			snprintf(filename,sizeof(filename),"%d.text",index);
//			sleep(2);
			FILE *fp = fopen(filename,"w");
			if( fp == NULL ) {
				fprintf(stderr,"Unable to open file");
				exit(0);
			}
			int i;
			fprintf(fp,"This is file %d",index);
			fflush(fp);
			fsync(fileno(fp));
			fclose(fp);
		
//			sleep(2);
			fp = fopen(filename,"r");
			if( fp == NULL ) {
                                fprintf(stderr,"Unable to open file");
                                exit(0);
                        }
			fseek(fp,2,SEEK_CUR);
			char line[512];
			fgets(line,sizeof(line),fp);
			printf("Read: %s\n",line);
			fclose(fp);	
			printf("File %d\n",index);
			exit(0);
		}
		index++;
//		sleep(2);
//		if(index == 4)
//			goto out;
//	}
	out:
	while(waitpid(-1,&status,0) != -1);
}
