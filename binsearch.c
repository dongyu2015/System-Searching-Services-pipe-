#include<stdlib.h>

int binsearch(int a[],int len,int count)
{
	int mid,high,low;
	low =0;
	high = len - 1;
	while(low<=high)
	{
		mid = (low+high)/2;
		if(count<a[mid])
			high = mid-1;
		else if(count>a[mid])
			low = mid+1;
		else
			return mid;
	}
	return(-1);
}