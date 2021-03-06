#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<fcntl.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/stat.h>
int main(int argc, char*argv[])
{
		/*terminate the program if lacking of file name*/
	if(argc!=2)
	{
		fprintf(stderr, "Usage: %s\n", argv[0]);
		exit(1);
	}
		/*the server will create two clients with pid: pid1 and pid2*/
		/*create wait1 and wait2 to wait for client shut down in the future*/
	pid_t pid2, pid1, wait1, wait2;
		/*initialize pid1 and pid2 with value 0*/
	pid1 = pid2 =0;
		/*create variable to store filename*/
	FILE *file;
	char *filename;
	filename = malloc(strlen(argv[1])+1);
	strcpy(filename, argv[1]);
		/*store number of zipcodes in howmany*/
		/*use i in'for'loop*/
		/*num1 is the number of zipcodes transfer to client1*/
		/*num2 is the number of zipcodes transfer to client2*/
	int howmany;
	int i , num1, num2;
		/*create four pipes for communication*/
	int pipe1[2];
	int pipe2[2];
	int pipe3[2];
	int pipe4[2];
		/*receive exit status from clients*/
	int status1, status2, value1, value2;
		/*store zipcode which will be found in clients*/
	int zip;
	char z[10];
		/*store result which received from clients*/
	int result1,result2;
		/*received value from clients*/
	int WIFSORT1,WIFSORT2;
		/*open file to read and store number of zipcodes*/
	file = fopen(filename, "z");
	fscanf(file, "%d", &howmany);
	int zipcode;
		/*nearly half of the zipcode to be sent to each client*/
		/*num1 stores number of zipcodes which send to one of clients*/
		/*num2 stores number of zipcodes which send to the other*/
	num1 = howmany / 2;
	num2 = howmany - num1;
		/*create four pipes here and check whether they create successfully*/
		/*one of the client will use pipe3 to write information to server*/
		/*the other will use pipe4 to write information to server*/
	if(pipe(pipe1)<0)
	{
		perror("pipe not create");
		exit(1);	
	}
	if(pipe(pipe2)<0)
	{
		perror("pipe not create");
		exit(1);
	}
	if(pipe(pipe3)<0)
	{
		perror("pipe not create");
		exit(1);	
	}
	if(pipe(pipe4)<0)
	{
		perror("pipe not create");
		exit(1);
	}
		/*server create clients(this is the server)*/
	if((pid1 == fork())&&(pid2 == fork()))
	{
			/*close pipes which are not used*/
		close(pipe1[0]);
		close(pipe2[0]);
		close(pipe3[1]);
		close(pipe4[1]);
			/*check whether clients are created*/
		read(pipe3[0], &WIFSORT1, sizeof(int));
		read(pipe4[0], &WIFSORT2, sizeof(int));
			/*clients are created*/
		if((WIFSORT1 ==1)&&(WIFSORT2 ==1))
		{
			write(pipe1[1], &num1, sizeof(int));
			write(pipe2[1], &num2, sizeof(int));
			for(i = 0;i< num1; i++)
			{
				fscanf(file, "%d", &zipcode);
				write(pipe1[1], &zipcode, sizeof(int));
			}
			for(i -0 ;i<num2;i++)
			{
				fscanf(file,"%d", &zipcode);
				write(pipe2[1], &zipcode, sizeof(int));
			}
			fclose(file);
			free(filename);
		}
		else
		{
			fprintf(stderr, "fail to receive response from clients\n");
			exit(1);
		}
		read(pipe3[0], &WIFSORTS1, sizeof(int));
		read(pipe4[0], &WIFSORTS2, sizeof(int));
		if((WIFSORT1 == 2)&&(WIFSORT==2))
		{
			fprintf(stderr,"please input zipcode to find\n");
			while(scanf("%d", &zip))
			{
				sprintf(z, "%d", zip);
				if((strlen(z)!=5)&&(zip != -1))
				{
					fprintf(stderr, "zipcode must be five numbers\n");
					fprintf(stderr, "please input zipcode to find\n");
					continue;
				}
				write(pipe1[1], &zip, sizeof(int));
				write(pipe2[1], &zip, sizeof(int));
				read(pipe3[0], &result1,sizeof(int));
				read(pipe4[0], &result2, sizeof(int));
				if((result1 == -1)&&(result2 == -1))
					fprintf(stderr, "NOT FIND\n");
				else if(result1 == -1)
					fprintf(stderr, "FOUND IN CLIENT 2\n");
				else if(result2 == -1)
					fprintf(stderr, "FOUND IN CLIENT 1\n");
				else
				{
					fprintf(stderr, "CLIENTS ARE SHUT DOWN\n");
					break;
				}
				fprintf(stderr," please input zipcode to find\n");
			}
		}
		close(pipe1[1]);
		close(pipe2[1]);
		close(pipe3[0]);
		close(pipe4[0]);
	}
	if(pid1<0||pid2<0)
	{
		perror("error create child pid");
		exit(1);
	}
	if(pid1 == 0)
	{
		close(pipe2[0]);
		close(pipe2[1]);		
		close(pipe4[0]);
		close(pipe4[1]);
		close(pipe1[1]);
		close(pipe3[0]);
		
		dup2(pipe1[0], STDIN_FILENO);
		dup2(pipe3[1], STDOUT_FILENO);

		close(pipe1[0]);
		close(pipe3[1]);
		
		execl("./client", "client", NULL);
	}
	if(pid2 == 0)
	{
		close(pipe1[0]);
		close(pipe1[1]);		
		close(pipe3[0]);
		close(pipe3[1]);
		close(pipe2[1]);
		close(pipe4[0]);
		
		dup2(pipe2[0], STDIN_FILENO);
		dup2(pipe4[1], STDOUT_FILENO);

		close(pipe2[0]);
		close(pipe4[1]);
		
		execl("./client", "client", NULL);
	}
	wait1 = waitpid(pid1, &status1, WUNTRACED);
	wait2 = waitpid(pid2, &status2, WUNTRACED);

	if((wait1 == pid1)&&(wait2 == pid2))
	{
		if(WIFEXITED(status1)&&(WIFEXITED(status2)))
		{
			value1 = WEXITSTATUS(status1);
			value2 = WEXITSTATUS(status2);
			if(value1 == value2)
				fprintf(stderr, "CLIENT EXIT NORMALLY WITH VALUE1 %d, VALUE2 %d\n", value1, value2);
			else
				fprintf(stderr, "CLIENT EXIT NORMALLY\n");
		}
		else
			fprintf(stderr, "CLIENT EXIT ABNORMALLY\n");
	}
	else
		fprintf(stderr, "a signal must interrupt the wait\n");
	return 0;
	
}
