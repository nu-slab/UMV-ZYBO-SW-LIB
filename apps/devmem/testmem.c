/*
	 access to physical address
	 by izumi@ieee.org ver.2016/10/07a
	 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <slab/bsp/xparameters.h>

#define BASE_ADDR XPAR_SYSTEM_ZYNQ_PROCESSOR_0_S00_AXI_BASEADDR

int main(int argc,char *argv[]){
	
	int fd;
	int dirout=0;
	unsigned int value=0;
	unsigned int addr=0;
	unsigned int reg_num=0;
	unsigned page_addr,page_offset;
	unsigned page_size=sysconf(_SC_PAGESIZE);
	void *ptr;

	if (argc==3) {
		/* output */
		dirout=1;
		reg_num=strtoul(argv[1],NULL,0);
		value=strtoul(argv[2],NULL,0);
	} else if (argc==2) {
		/* input */
		dirout=0;
		reg_num=strtoul(argv[1],NULL,0);
	} else {
		printf("IO access through /dev/mem.\n");
		printf("%s <Reg Number> [<VALUE>]\n",argv[0]);
		printf("	<Reg Number>  IO physical address\n");
		printf("	<VALUE> output value to IO\n");
		return -1;
	}

	/* Open /dev/mem file */
	fd=open("/dev/mem",O_RDWR | O_SYNC);
	if (fd<=0) {
		fprintf(stderr,"can't open /dev/mem\n");
		return -1;
	}

	addr = BASE_ADDR + reg_num * 4; 

	/* mmap the memory device */
	page_addr=addr&~(page_size-1);
	page_offset=addr-page_addr;
	ptr=mmap(NULL,page_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,page_addr);

	if (ptr==0) {
		fprintf(stderr,"failed to mmap the memory device.\n");
		return -1;
	}

	if (dirout) {
		/* write to IO */
		*((unsigned int *)(ptr+page_offset))=value;
		printf("reg%d[%08x] <= %08x\n",reg_num, addr, value);
	} else {
		/* read from IO */
		value= *((unsigned int *)(ptr+page_offset));
		printf("reg%d[%08x] => %08x\n",reg_num, addr, value);
	}

	munmap(ptr,page_size);
	close(fd);

	return 0;
}
