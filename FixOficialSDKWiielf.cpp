// 
// Fix for Official Nintendo Wii SDK compiled elfs
// to allow them to run via the Homebrew channel
// 
// xorloser July 2009
// 

#include <stdio.h>
#include <string.h>
#include "elf/ELFAccess.h"

const char G_TITLE[] =	"WiiElfFix v1.0 - xorloser 2009";
const char G_USAGE[] =	"Usage:   WiiElfFix <elf filename>\n"
						"Fixes Nintendo SDK compiled ELFs to run via the Homebrew Channel";

int main(int argc, const char* argv[])
{
	// handle args
	printf("%s\n", G_TITLE);
	if(argc != 2)
	{
		printf("%s\n", G_USAGE);
		return 1;
	}
	char filename[260];
	strcpy(filename, argv[1]);
	
	
	// open elf file
	FILE* fd = fopen(filename, "r+b");
	if(fd == NULL)
	{
		printf("Error opening %s\n", filename);
		return 1;
	}
	
	
	// read in elf header
	Elf64_Ehdr ehdr;
	bool is32, isbe;
	if( !elf_read_hdr(fd, &ehdr, &is32, &isbe) )
	{
		printf("Error reading ELF header from %s\n", filename);
		fclose(fd);
		return 1;
	}
	

	// fix program headers
	// copy the value for virtual address into the physical address field
	for(int i=0; i<ehdr.e_phnum; i++)
	{
		// read in header
		Elf64_Phdr phdr;
		fseek(fd, ehdr.e_phoff + i*ehdr.e_phentsize, SEEK_SET);
		if( !elf_read_phdr(fd, &phdr, is32, isbe) )
		{
			printf("Error reading program header %d from %s\n", i, filename);
			fclose(fd);
			return 1;
		}
		
		// alter header
		phdr.p_paddr = phdr.p_vaddr;
		
		// write back altered header
		fseek(fd, ehdr.e_phoff + i*ehdr.e_phentsize, SEEK_SET);
		if( !elf_write_phdr(fd, &phdr, is32, isbe) )
		{
			printf("Error writing program header %d to %s\n", i, filename);
			fclose(fd);
			return 1;
		}
	}
	
	
	// done
	printf("done\n");
	fclose(fd);
	return 0;
}