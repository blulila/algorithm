#include <stdio.h>
#include <stdlib.h>

struct MEMORY
{
	int page_num;
	int reference_bit;
	int dirty_bit;
};

struct PAGE
{
	int memory_location;
	int valid_bit;
};

int main(int argc, char *argv[])
{
	int memory_size = atoi(argv[1]);
	FILE * input_file = fopen("a.txt","r");
	struct PAGE page[200000];
	struct MEMORY *memory = (struct MEMORY *) malloc(memory_size * sizeof(struct MEMORY));
	int pg, op, i;
	int empty = memory_size;
	int page_fault = 0;
	int clock = 0;
	int disk = 0, disk_d = 0, disk_w = 0, disk_new = 0, disk_old = 0;

	for(i = 0; i < memory_size; i++)
	{
		memory[i].reference_bit = 0;
		memory[i].dirty_bit = 0;
	}
	for(i = 0; i < 200000; i++)
		page[i].valid_bit = 0;
	while(!feof(input_file))
	{
		clock = clock%memory_size;
		fscanf(input_file, "%d %d", &pg, &op);

		if(page[pg].valid_bit == 1)
		{
			memory[page[pg].memory_location].reference_bit =  1;
			if(op == 1)
				memory[page[pg].memory_location].dirty_bit = 1;
		}
		else
		{
			page_fault++;
			if(empty == 0) //빈공간이 없는경우
			{
				while(memory[clock].reference_bit == 1)
				{
					memory[clock].reference_bit = 0; //클락시계바늘이동시키며reference_bit 1은0으로, 0인페이지탐색;
					clock = (clock+1)%memory_size;
				}
				page[memory[clock].page_num].valid_bit = 0;
				if(memory[clock].dirty_bit == 1) //쫓겨나는페이지의dirty_bit이1이면디스크기록을위해
				{	
					disk_w = disk_w + 1;
					disk_new = memory[clock].page_num/1000;
					if(disk_new > disk_old)
						disk = disk_new - disk_old;
					else
						disk = disk_old - disk_new;
					disk_d = disk_d + disk;
					disk_old = disk_new;
					disk_new = 0;
				}
			}
			disk_new = pg/1000;
			if(disk_new > disk_old)
				disk = disk_new - disk_old;
			else
				disk = disk_old - disk_new;
			disk_d = disk_d + disk; //요청된페이지를디스크에서읽어오면서디스크헤드이동거리증가;
			disk_old = disk_new; //클락바늘이가리키는메모리위치에새로들어온페이지번호입력;
			disk_new = 0;
			memory[clock].page_num = pg;
			if(op == 1) //새로들어온페이지에대한요청이쓰기요청인경우dirty_bit 세팅;
				memory[clock].dirty_bit = 1;
			else if (op == 0)
				memory[clock].dirty_bit = 0;
			memory[clock].reference_bit = 0; //새로들어온페이지에대한reference_bit 0으로셋팅;
			page[pg].memory_location = clock; //새로들어온페이지의memory_location에프레임번호지정;
			page[pg].valid_bit = 1;//새로들어온페이지의valid_bit 1로세팅;
			clock++;
			if(empty != 0)
				empty--;
		}
	}
	printf("%d\n%d\n%d\n", page_fault, disk_w, disk_d);
	fclose(input_file);
}
