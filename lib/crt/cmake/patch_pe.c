#include <stdio.h>

int main(int argc, char** argv) {
    unsigned int pe_offset, sig;
    unsigned short val;
    FILE* f;

    if (argc != 2) return 1;

    f = fopen(argv[1], "rb+");
    if (!f) return 1;

    fseek(f, 0x3c, SEEK_SET);
    fread(&pe_offset, 4, 1, f);

    fseek(f, pe_offset, SEEK_SET);
    fread(&sig, 4, 1, f);

    if (sig != 0x4550) { 
        fclose(f); 
        return 1; 
    }

    val = 1; 
    fseek(f, pe_offset + 0x40, SEEK_SET); 
    fwrite(&val, 2, 1, f);
    
    val = 0; 
    fseek(f, pe_offset + 0x42, SEEK_SET); 
    fwrite(&val, 2, 1, f);
    
    val = 3; 
    fseek(f, pe_offset + 0x48, SEEK_SET); 
    fwrite(&val, 2, 1, f);
    
    val = 10; 
    fseek(f, pe_offset + 0x4a, SEEK_SET); 
    fwrite(&val, 2, 1, f);

    fclose(f);
    return 0;
}