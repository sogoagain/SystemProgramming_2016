#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <elf.h>

#define MAX_PATH 256
#define TRUE 1
#define FALSE 0

typedef struct Hdr32{
    Elf32_Ehdr ehdr;
    Elf32_Shdr *shdrs;
    Elf32_Phdr *phdrs;
}Hdr32;

typedef struct Hdr64{
    Elf64_Ehdr ehdr;
    Elf64_Shdr *shdrs;
    Elf64_Phdr *phdrs;
}Hdr64;

int check_elf(Elf32_Ehdr ehdr);
int getBitSystem(char *filename);

void read_headers32(int fd,Hdr32 *hdrs);
void read_headers64(int fd,Hdr64 *hdrs);

void read_elf_header32(int fd, Elf32_Ehdr *ehdr);
void read_elf_header64(int fd, Elf64_Ehdr *ehdr);

void read_sec_header32(int fd, Elf32_Ehdr ehdr, Elf32_Shdr *shdrs);
void read_sec_header64(int fd, Elf64_Ehdr ehdr, Elf64_Shdr *shdrs);

char* read_string_table32(int fd, Elf32_Shdr shdr);
char* read_string_table64(int fd, Elf64_Shdr shdr);

void read_program_header32(int fd, Elf32_Ehdr ehdr, Elf32_Phdr *phdrs);
void read_program_header64(int fd, Elf64_Ehdr ehdr, Elf64_Phdr *phdrs);

void print_elf_header32(Elf32_Ehdr ehdr);
void print_elf_header64(Elf64_Ehdr ehdr);

void print_sec_header32(int fd, Elf32_Ehdr ehdr, Elf32_Shdr shdrs[]);
void print_sec_header64(int fd, Elf64_Ehdr ehdr, Elf64_Shdr shdrs[]);

void print_pro_header32(Elf32_Ehdr ehdr, Elf32_Phdr phdrs[]);
void print_pro_header64(Elf64_Ehdr ehdr, Elf64_Phdr phdrs[]);

int check_elf(Elf32_Ehdr ehdr) {
    /* ELF Magic Number 0x7f, 'E', 'L', 'F' : 총 4바이트 */
    if(!strncmp((char *)ehdr.e_ident, "\177ELF", 4)) {
        printf("ELF파일이 맞습니다.\n");
        return TRUE;
    }
    else {
        printf("ELF파일이 아닙니다.\n");
        return FALSE;
    }
}

int getBitSystem(char *filename){
    char checker[5];
    int fd;
    int i;
    if((fd = open(filename,O_RDONLY)) == -1){
        printf("Please check the file name.\n");
        exit(0);
    }
    read(fd,checker,5);
    close(fd);
    return checker[4] == 1 ? 32 : 64;
}

void read_headers32(int fd,Hdr32 *hdrs){
    read_elf_header32(fd,&(hdrs->ehdr));
    hdrs->shdrs = (Elf32_Shdr *)malloc(hdrs->ehdr.e_shentsize * hdrs->ehdr.e_shnum);
    read_sec_header32(fd,hdrs->ehdr,hdrs->shdrs);
    hdrs->phdrs = (Elf32_Phdr *)malloc(hdrs->ehdr.e_phentsize * hdrs->ehdr.e_phnum);
    read_program_header32(fd,hdrs->ehdr,hdrs->phdrs);
    
}
void read_headers64(int fd,Hdr64 *hdrs){
    read_elf_header64(fd,&(hdrs->ehdr));
    hdrs->shdrs = (Elf64_Shdr *)malloc(hdrs->ehdr.e_shentsize * hdrs->ehdr.e_shnum);
    read_sec_header64(fd,hdrs->ehdr,hdrs->shdrs);
    hdrs->phdrs = (Elf64_Phdr *)malloc(hdrs->ehdr.e_phentsize * hdrs->ehdr.e_phnum);
    read_program_header64(fd,hdrs->ehdr,hdrs->phdrs);
}

void read_elf_header32(int fd, Elf32_Ehdr *ehdr){
    lseek(fd, (off_t)0, SEEK_SET);
    read(fd, ehdr, sizeof(Elf32_Ehdr));
}

void read_elf_header64(int fd, Elf64_Ehdr *ehdr){
    lseek(fd, (off_t)0, SEEK_SET);
    read(fd, ehdr, sizeof(Elf64_Ehdr));
}

void read_sec_header32(int fd, Elf32_Ehdr ehdr,  Elf32_Shdr *shdrs){
    int i;
    
    lseek(fd, (off_t)ehdr.e_shoff, SEEK_SET);
    for(i =0; i<ehdr.e_shnum;i++){
        read(fd,&shdrs[i],ehdr.e_shentsize);
    }
}
void read_sec_header64(int fd, Elf64_Ehdr ehdr,  Elf64_Shdr *shdrs){
    int i;
    
    lseek(fd, (off_t)ehdr.e_shoff, SEEK_SET);
    for(i =0; i<ehdr.e_shnum;i++){
        read(fd,&shdrs[i],ehdr.e_shentsize);
    }
}

char *read_string_table32(int fd, Elf32_Shdr shdr){
    char* buf = (char*)malloc(shdr.sh_size);
    
    lseek(fd,(off_t)shdr.sh_offset, SEEK_SET);
    read(fd,buf, shdr.sh_size);
    
    return buf;
}

char *read_string_table64(int fd, Elf64_Shdr shdr){
    char* buf = (char*)malloc(shdr.sh_size);
    
    lseek(fd,(off_t)shdr.sh_offset, SEEK_SET);
    read(fd,buf, shdr.sh_size);
    
    return buf;
}

void read_program_header32(int fd, Elf32_Ehdr ehdr, Elf32_Phdr *phdrs){
    int i;
    
    lseek(fd, (off_t)ehdr.e_phoff, SEEK_SET);
    for(i = 0;i<ehdr.e_phnum;i++){
        read(fd,&phdrs[i],ehdr.e_phentsize);
    }
    
}
void read_program_header64(int fd, Elf64_Ehdr ehdr, Elf64_Phdr *phdrs){
    int i;
    
    lseek(fd, (off_t)ehdr.e_phoff, SEEK_SET);
    for(i = 0;i<ehdr.e_phnum;i++){
        read(fd,&phdrs[i],ehdr.e_phentsize);
    }
}

void print_elf_header64(Elf64_Ehdr ehdr){
    printf("\t\t\t*****ELF HEADER*****\n");
    printf("\t\t-ELF identification-\n");
    printf("\t1. Magic Number: %c%c%c%c\n", ehdr.e_ident[EI_MAG0], ehdr.e_ident[EI_MAG1], ehdr.e_ident[EI_MAG2], ehdr.e_ident[EI_MAG3]);
    
    printf("\t2. File Class: ");
    switch(ehdr.e_ident[EI_CLASS]) {
        case ELFCLASS32:
            printf("32-bit objects\n");
            break;
        case ELFCLASS64:
            printf("64-bit objects\n");
            break;
        default:
            printf("Invalid class\n");
    }
    
    printf("\t3. DATA Encoding: ");
    switch(ehdr.e_ident[EI_DATA]) {
        case ELFDATA2LSB:
            printf("little endian\n");
            break;
        case ELFDATA2MSB:
            printf("big endian\n");
            break;
        default:
            printf("Invalid data encoding\n");
    }
    
    printf("\t4. ELF header version: %d\n", ehdr.e_ident[EI_VERSION]);
    
    printf("\t5. ELF file type: ");
    switch(ehdr.e_type) {
        case ET_NONE:
            printf("No file type\n");
            break;
        case ET_REL:
            printf("Relocatable file\n");
            break;
        case ET_EXEC:
            printf("Executable file\n");
            break;
        case ET_DYN:
            printf("Shared object file\n");
            break;
        case ET_CORE:
            printf("Core file\n");
            break;
        case ET_LOPROC:
            printf("Processor-specific\n");
            break;
        case ET_HIPROC:
            printf("Processor-specific\n");
            break;
        default:
            printf("Unknown (0x%x)\n", ehdr.e_type);
    }
    
    printf("\t6. Processor Architecture: ");
    switch(ehdr.e_machine) {
        case EM_NONE:
            printf("No machine\n");
            break;
        case EM_M32:
            printf("At&T WE 32100\n");
            break;
        case EM_SPARC:
            printf("SPARC\n");
            break;
        case EM_386:
            printf("Intel 80386\n");
            break;
        case EM_68K:
            printf("Motorola 68000\n");
            break;
        case EM_88K:
            printf("Motorola 88000\n");
            break;
        case EM_860:
            printf("Intel 80860\n");
            break;
        case EM_MIPS:
            printf("MIPS RS3000\n");
            break;
        default:
            printf("Unknown (0x%x)\n", ehdr.e_machine);
    }
    
    printf("\t7. ELF file version: ");
    switch(ehdr.e_version) {
        case EV_NONE:
            printf("Invalid version\n");
            break;
        case EV_CURRENT:
            printf("Current version\n");
            break;
        default:
            printf("Unknown (0x%x)\n", ehdr.e_version);
    }
    
    printf("\t8. 실행시작 가상 어드레스: (0x%jx)\n", ehdr.e_entry);
    
    printf("\t9. Program Header Table(PHT)\n");
    printf("\t\tPHT의 file offset: (0x%jx)\n", ehdr.e_phoff);
    printf("\t\tPHT의 entry 크기: %d\n", ehdr.e_phentsize);
    printf("\t\tPHT의 entry 개수: %d\n", ehdr.e_phnum);
    
    printf("\t10. Section Header Table(SHT)\n");
    printf("\t\tSHT의 file offset: (0x%jx)\n", ehdr.e_shoff);
    printf("\t\tSHT의 entry 크기: %d\n", ehdr.e_shentsize);
    printf("\t\tSHT의 entry 개수: %d\n", ehdr.e_shnum);
    printf("\t\tstring table의 SHT index: %d\n", ehdr.e_shstrndx);
    
    printf("\t11. processor-specific flag: (0x%x)\n", ehdr.e_flags);
    
    printf("\t\t\t***************\n");
}


void print_sec_header64(int fd, Elf64_Ehdr ehdr, Elf64_Shdr shdrs[]){
    int i;
    char *string_table;
    int flag;
    int remain_space = 0;
    string_table = read_string_table64(fd, shdrs[ehdr.e_shstrndx]);
    
    printf("\n*** SECTION HEADERS ***\n");
    printf("[Num] Name              Type               Address           Offset  \n"
           "      Size              EntSize            Flags  Link  Info  Align\n");
    for(i=0;i<ehdr.e_shnum;i++){
        printf("[%-3d] ",i);
        printf("%-16s ",(string_table+shdrs[i].sh_name));
        printf("0x%016X ",shdrs[i].sh_type);
        printf("0x%016jX ",shdrs[i].sh_addr);
        printf("0x%08jX ",shdrs[i].sh_offset);
        
        printf("\n      ");
        
        printf("0x%016jX ",shdrs[i].sh_size);
        printf("0x%016jX ",shdrs[i].sh_entsize);
        
        remain_space = 7;
        flag = (int)shdrs[i].sh_flags;
        
        printf("%7jX ",shdrs[i].sh_flags);
        printf("%-6X ",shdrs[i].sh_link);
        printf("%-6X ",shdrs[i].sh_info);
        printf("%-6jX ",shdrs[i].sh_addralign);
        
        printf("\n");
        
    }
    
}

void print_pro_header64(Elf64_Ehdr ehdr, Elf64_Phdr *phdrs){
    int i;
    printf("\n*** PROGRAM HEADERS ***\n");
    printf("Type            Offset             VirtAddr           PhysAddr\n"
           "                FileSize           MemSize            Flags   Align\n");
    for(i=0;i<ehdr.e_phnum;i++){
        switch(phdrs[i].p_type){
            case PT_LOAD:
                printf("LOAD            ");
                break;
            case PT_DYNAMIC:
                printf("DYNAMIC         ");
                break;
            case PT_INTERP:
                printf("INTERP          ");
                break;
            case PT_NOTE:
                printf("NOTE            ");
                break;
            case PT_SHLIB:
                printf("SHLIB           ");
                break;
            case PT_PHDR:
                printf("PHDR            ");
                break;
            case PT_TLS:
                printf("TLS             ");
                break;
            case PT_LOOS:
                printf("LOOS            ");
                break;
            case PT_GNU_EH_FRAME:
                printf("GNU_EH_FRAME    ");
                break;
            case PT_GNU_STACK:
                printf("GNU_STACK       ");
                break;
            case PT_GNU_RELRO:
                printf("GNU_RELRO       ");
                break;
            case PT_SUNWBSS:
                printf("SUNWBSS         ");
                break;
            case PT_SUNWSTACK:
                printf("SUNWSTACK       ");
                break;
            case PT_HISUNW:
                printf("HISUNW          ");
                break;
            case PT_LOPROC:
                printf("PT_LOPROC       ");
                break;
 
        }
        printf("0x%016jX ",phdrs[i].p_offset);
        printf("0x%016jX ",phdrs[i].p_vaddr);
        printf("0x%016jX ",phdrs[i].p_paddr);
        printf("\n                ");
        printf("0x%016jX ",phdrs[i].p_filesz);
        printf("0x%016jX ",phdrs[i].p_memsz);
        printf("%-8X ",phdrs[i].p_flags);
        printf("%7jX ",phdrs[i].p_align);
        printf("\n");
    }
}


void print_elf_header32(Elf32_Ehdr ehdr){
    printf("\t\t\t*****ELF HEADER*****\n");
    printf("\t\t-ELF identification-\n");
    printf("\t1. Magic Number: %c%c%c%c\n", ehdr.e_ident[EI_MAG0], ehdr.e_ident[EI_MAG1], ehdr.e_ident[EI_MAG2], ehdr.e_ident[EI_MAG3]);
    
    printf("\t2. File Class: ");
    switch(ehdr.e_ident[EI_CLASS]) {
        case ELFCLASS32:
            printf("32-bit objects\n");
            break;
        case ELFCLASS64:
            printf("64-bit objects\n");
            break;
        default:
            printf("Invalid class\n");
    }
    
    printf("\t3. DATA Encoding: ");
    switch(ehdr.e_ident[EI_DATA]) {
        case ELFDATA2LSB:
            printf("little endian\n");
            break;
        case ELFDATA2MSB:
            printf("big endian\n");
            break;
        default:
            printf("Invalid data encoding\n");
    }
    
    printf("\t4. ELF header version: %d\n", ehdr.e_ident[EI_VERSION]);
    
    printf("\t5. ELF file type: ");
    switch(ehdr.e_type) {
        case ET_NONE:
            printf("No file type\n");
            break;
        case ET_REL:
            printf("Relocatable file\n");
            break;
        case ET_EXEC:
            printf("Executable file\n");
            break;
        case ET_DYN:
            printf("Shared object file\n");
            break;
        case ET_CORE:
            printf("Core file\n");
            break;
        case ET_LOPROC:
            printf("Processor-specific\n");
            break;
        case ET_HIPROC:
            printf("Processor-specific\n");
            break;
        default:
            printf("Unknown (0x%x)\n", ehdr.e_type);
    }
    
    printf("\t6. Processor Architecture: ");
    switch(ehdr.e_machine) {
        case EM_NONE:
            printf("No machine\n");
            break;
        case EM_M32:
            printf("At&T WE 32100\n");
            break;
        case EM_SPARC:
            printf("SPARC\n");
            break;
        case EM_386:
            printf("Intel 80386\n");
            break;
        case EM_68K:
            printf("Motorola 68000\n");
            break;
        case EM_88K:
            printf("Motorola 88000\n");
            break;
        case EM_860:
            printf("Intel 80860\n");
            break;
        case EM_MIPS:
            printf("MIPS RS3000\n");
            break;
        default:
            printf("Unknown (0x%x)\n", ehdr.e_machine);
    }
    
    printf("\t7. ELF file version: ");
    switch(ehdr.e_version) {
        case EV_NONE:
            printf("Invalid version\n");
            break;
        case EV_CURRENT:
            printf("Current version\n");
            break;
        default:
            printf("Unknown (0x%x)\n", ehdr.e_version);
    }
    
    printf("\t8. 실행시작 가상 어드레스: (0x%x)\n", ehdr.e_entry);
    
    printf("\t9. Program Header Table(PHT)\n");
    printf("\t\tPHT의 file offset: (0x%x)\n", ehdr.e_phoff);
    printf("\t\tPHT의 entry 크기: %d\n", ehdr.e_phentsize);
    printf("\t\tPHT의 entry 개수: %d\n", ehdr.e_phnum);
    
    printf("\t10. Section Header Table(SHT)\n");
    printf("\t\tSHT의 file offset: (0x%x)\n", ehdr.e_shoff);
    printf("\t\tSHT의 entry 크기: %d\n", ehdr.e_shentsize);
    printf("\t\tSHT의 entry 개수: %d\n", ehdr.e_shnum);
    printf("\t\tstring table의 SHT index: %d\n", ehdr.e_shstrndx);
    
    printf("\t11. processor-specific flag: (0x%x)\n", ehdr.e_flags);
    
    printf("\t\t\t***************\n");
}


void print_sec_header32(int fd, Elf32_Ehdr ehdr, Elf32_Shdr shdrs[]){
    int i;
    char *string_table;
    int flag;
    int remain_space = 0;
    string_table = read_string_table32(fd, shdrs[ehdr.e_shstrndx]);
    
    printf("\n*** SECTION HEADERS ***\n");
    printf("[Num] Name              Type               Address           Offset  \n"
           "      Size              EntSize            Flags  Link  Info  Align\n");
    for(i=0;i<ehdr.e_shnum;i++){
        printf("[%-3d] ",i);
        printf("%-16s ",(string_table+shdrs[i].sh_name));
        printf("0x%016X ",shdrs[i].sh_type);
        printf("0x%016X ",shdrs[i].sh_addr);
        printf("0x%08X ",shdrs[i].sh_offset);
        
        printf("\n      ");
        
        printf("0x%016X ",shdrs[i].sh_size);
        printf("0x%016X ",shdrs[i].sh_entsize);
        
        remain_space = 7;
        flag = (int)shdrs[i].sh_flags;
        
        printf("%7X ",shdrs[i].sh_flags);
        printf("%-6X ",shdrs[i].sh_link);
        printf("%-6X ",shdrs[i].sh_info);
        printf("%-6X ",shdrs[i].sh_addralign);
        
        printf("\n");
        
    }
    
}

void print_pro_header32(Elf32_Ehdr ehdr, Elf32_Phdr *phdrs){
    int i;
    printf("\n*** PROGRAM HEADERS ***\n");
    printf("Type            Offset             VirtAddr           PhysAddr\n"
           "                FileSize           MemSize            Flags   Align\n");
    for(i=0;i<ehdr.e_phnum;i++){
        switch(phdrs[i].p_type){
            case PT_LOAD:
                printf("LOAD            ");
                break;
            case PT_DYNAMIC:
                printf("DYNAMIC         ");
                break;
            case PT_INTERP:
                printf("INTERP          ");
                break;
            case PT_NOTE:
                printf("NOTE            ");
                break;
            case PT_SHLIB:
                printf("SHLIB           ");
                break;
            case PT_PHDR:
                printf("PHDR            ");
                break;
            case PT_TLS:
                printf("TLS             ");
                break;
            case PT_LOOS:
                printf("LOOS            ");
                break;
            case PT_GNU_EH_FRAME:
                printf("GNU_EH_FRAME    ");
                break;
            case PT_GNU_STACK:
                printf("GNU_STACK       ");
                break;
            case PT_GNU_RELRO:
                printf("GNU_RELRO       ");
                break;
            case PT_SUNWBSS:
                printf("SUNWBSS         ");
                break;
            case PT_SUNWSTACK:
                printf("SUNWSTACK       ");
                break;
            case PT_HISUNW:
                printf("HISUNW          ");
                break;
            case PT_LOPROC:
                printf("PT_LOPROC       ");
                break;
        }
        printf("0x%016X ",phdrs[i].p_offset);
        printf("0x%016X ",phdrs[i].p_vaddr);
        printf("0x%016X ",phdrs[i].p_paddr);
        printf("\n                ");
        printf("0x%016X ",phdrs[i].p_filesz);
        printf("0x%016X ",phdrs[i].p_memsz);
        printf("%-8X ",phdrs[i].p_flags);
        printf("%7X ",phdrs[i].p_align);
        printf("\n");
    }
}

void print_ELF_Header(Elf32_Ehdr ehdr) {
    printf("\t\t\t*****ELF HEADER*****\n");
    printf("\t\t-ELF identification-\n");
    printf("\t1. Magic Number: %c%c%c%c\n", ehdr.e_ident[EI_MAG0], ehdr.e_ident[EI_MAG1], ehdr.e_ident[EI_MAG2], ehdr.e_ident[EI_MAG3]);
    
    printf("\t2. File Class: ");
    switch(ehdr.e_ident[EI_CLASS]) {
        case ELFCLASS32:
            printf("32-bit objects\n");
            break;
        case ELFCLASS64:
            printf("64-bit objects\n");
            break;
        default:
            printf("Invalid class\n");
    }

    printf("\t3. DATA Encoding: ");
    switch(ehdr.e_ident[EI_DATA]) {
        case ELFDATA2LSB:
            printf("little endian\n");
            break;
        case ELFDATA2MSB:
            printf("big endian\n");
            break;
        default:
            printf("Invalid data encoding\n");
    }

    printf("\t4. ELF header version: %d\n", ehdr.e_ident[EI_VERSION]);

    printf("\t5. ELF file type: ");
    switch(ehdr.e_type) {
        case ET_NONE:
            printf("No file type\n");
            break;
        case ET_REL:
            printf("Relocatable file\n");
            break;
        case ET_EXEC:
            printf("Executable file\n");
            break;
        case ET_DYN:
            printf("Shared object file\n");
            break;
        case ET_CORE:
            printf("Core file\n");
            break;
        case ET_LOPROC:
            printf("Processor-specific\n");
            break;
        case ET_HIPROC:
            printf("Processor-specific\n");
            break;
        default:
            printf("Unknown (0x%x)\n", ehdr.e_type);
    }

    printf("\t6. Processor Architecture: ");
    switch(ehdr.e_machine) {
        case EM_NONE:
            printf("No machine\n");
            break;
        case EM_M32:
            printf("At&T WE 32100\n");
            break;
        case EM_SPARC:
            printf("SPARC\n");
            break;
        case EM_386:
            printf("Intel 80386\n");
            break;
        case EM_68K:
            printf("Motorola 68000\n");
            break;
        case EM_88K:
            printf("Motorola 88000\n");
            break;
        case EM_860:
            printf("Intel 80860\n");
            break;
        case EM_MIPS:
            printf("MIPS RS3000\n");
            break;
        default:
            printf("Unknown (0x%x)\n", ehdr.e_machine);
    }

    printf("\t7. ELF file version: ");
    switch(ehdr.e_version) {
        case EV_NONE:
            printf("Invalid version\n");
            break;
        case EV_CURRENT:
            printf("Current version\n");
            break;
        default:
            printf("Unknown (0x%x)\n", ehdr.e_version);
    }

    printf("\t8. 실행시작 가상 어드레스: (0x%x)\n", ehdr.e_entry);

    printf("\t9. Program Header Table(PHT)\n");
    printf("\t\tPHT의 file offset: (0x%x)\n", ehdr.e_phoff);
    printf("\t\tPHT의 entry 크기: %d\n", ehdr.e_phentsize);
    printf("\t\tPHT의 entry 개수: %d\n", ehdr.e_phnum);

    printf("\t10. Section Header Table(SHT)\n");
    printf("\t\tSHT의 file offset: (0x%x)\n", ehdr.e_shoff);
    printf("\t\tSHT의 entry 크기: %d\n", ehdr.e_shentsize);
    printf("\t\tSHT의 entry 개수: %d\n", ehdr.e_shnum);
    printf("\t\tstring table의 SHT index: %d\n", ehdr.e_shstrndx);

    printf("\t11. processor-specific flag: (0x%x)\n", ehdr.e_flags);

    printf("\t\t\t***************\n");
}

int main(int argc, char **argv) {
    int opt;
    int fd;
    Hdr32 hdr32;
    Hdr64 hdr64;
    char file_path[MAX_PATH];
    int i = 0;
    int fbit = 0;
    
    // 옵션
    int elf_header_opt = 0;
    int sec_header_opt = 0;
    int proc_header_opt = 0;
    int num_of_opt = 0;

    while((opt = getopt(argc, argv, "eps")) != -1) {
        switch(opt) {
            case 'e':
                elf_header_opt = 1;
                break; 
            case 'p':
                proc_header_opt = 1;
                i++;
                break;
            case 's':
                sec_header_opt = 1;
                i++;
                break;
            default:
                printf("%c는 알수없는 옵션 입니다.\n", opt);
                break;
        }
        num_of_opt++;
    }

    if(num_of_opt == 0) {
        elf_header_opt = 1;
    }

    for(; optind < argc; optind++) {
        strcpy(file_path, argv[optind]);
    }

    fd = open(file_path, O_RDONLY|O_SYNC);
    if(fd < 0) {
        fprintf(stderr, "%s파일을 열수없습니다.\n", file_path);
    }
    
    //verify and read ELF file
    read_headers32(fd, &hdr32);
    if(check_elf(hdr32.ehdr) == FALSE) {
        exit(EXIT_FAILURE);
    }

    fbit = getBitSystem(file_path);
    fd = open(file_path, O_RDONLY);
    
    if(fbit==32) {
        // 앞에서 ELF 파일을 확인할 때 읽었으므로 생략한다.
        //read_headers32(fd, &hdr32);
        
        if(elf_header_opt == 1)
            print_elf_header32(hdr32.ehdr);
        if(proc_header_opt == 1)
            print_pro_header32(hdr32.ehdr, hdr32.phdrs);
        if(sec_header_opt == 1)
            print_sec_header32(fd, hdr32.ehdr, hdr32.shdrs);
    } else {
        read_headers64(fd, &hdr64);
        
        if(elf_header_opt == 1)
            print_elf_header64(hdr64.ehdr);
        if(proc_header_opt == 1)
            print_pro_header64(hdr64.ehdr, hdr64.phdrs);
        if(sec_header_opt == 1)
            print_sec_header64(fd, hdr64.ehdr, hdr64.shdrs);
    }
    
    close(fd);
    exit(EXIT_SUCCESS);
}
