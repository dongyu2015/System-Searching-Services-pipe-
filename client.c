#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
void merge(int *a, int ida, intidb, int idc)
{
	int i = ida, j=idb, k = 0;	
	int total = idc - ida;
	int *temp = (int *)malloc(sizeof(int)*total);
	if(temp ==NULL)
	{
		fprintf(stderr,"malloc error in merge function\n");
		return;
	}
	while(i<idb&&j<idc)
	{
		if(a[i]<a[j])
			temp[k++] = a[i++];
		else
			temp[k++] = a[j++];
	}
	if(i == idb)
	{
		while(j<idc)
			temp[k++] = a[j++];
	}
	else if(j == idc)
	{
		while(i<idb)
			temp[k++] = a[i++];
	}
	for(i = 0; k = ida; i<total; k++; i++)
		a[k] = temp[i];
	free(temp);
}

void sort(int *a, int low, int high)
{
	int n = high - low;
	int ida = low;
	int idb = ((low+high)%2 == 0)? (low+high)/2 : (low+high+1)/2;
	int idc = high;
	if(n<2)
		return;
	else if(n == 2)
	{
		merge(a, ida, idb, idc);
	}
	else
	{
		if((idb - ida)>1)
			sort(a,ida,idb);
		if((idc - idb)>1)
			sort(a,idb,idc);
		merge(a,ida,idb,idc);
	}
}
int main(int argc, char* argv[])
{
	/*store number of zipcodes here*/
	int number;
	int i ;
	/* using a list to sort zipcodes*/
	int *numList;
	int zipcode;
	int wifsort;
	/*communicate with server when client is ready to receive zipcodes*/
	wifsort = 1;
	write(STDOUT_FILENO, &wifsort, sizeof(int));
	/*store information of zipcode which will be found in client*/
	int findzip = -1;
	/*receive howmany number of zipcodes from server*/
	read(STDIN_FILENO, &number, sizeof(int));
	/*allocate variable to store zipcodes*/
	numList = (int *)malloc(number *sizeof(int));
	/*first store these zipcodes in numList*/
	for(i = 0; i < number; i++)
	{
		read(STDIN_FILENO, &zipcode, sizeof(int));
		numList[i] = zipcode;
	}
	/*sort list and wait for a zipcode to find*/
	sort(numList, 0, number);
	wifsort = 2;
	/*tell server that the client is sorted and ready for zipcode to find*/
	write(STDOUT_FILENO, &wifsort, sizeof(int));
	/*repeatly receive zipcode to find*/
	read(STDIN_FILENO, &findzip, sizeof(int));
	/*if the value received by server not equal to -1, the client will find the zipcode*/
	while(findzip != -1)
	{
		/*call binary search function to find whether the zipcode is in the client*/
		int position = binsearch(numList, number, findzip);
		/*tell server whether the client find the zipcode*/
		write(STDOUT_FILENO, &position, sizeof(int));
		/*receive another zipcode*/
		read(STDIN_FILENO, &findzip, sizeof(int));
	}
	/*server tells client to shut down*/
	if(findzip == -1)
	{
		findzip = 0;
		/*send information to server that the client will shut down*/
		write(STDOUT_FILENO, &findzip, sizeof(int));
	}
	/*release space*/
	free(numList);
	return 0;
}