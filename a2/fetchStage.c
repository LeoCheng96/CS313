#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <inttypes.h>
#include <stdlib.h>
#include <errno.h>
#include "printInternalReg.h"

#define ERROR_RETURN -1
#define SUCCESS 0


int main(int argc, char **argv) {
    
    int machineCodeFD = -1;       // File descriptor of file with object code
    uint64_t PC = 0;              // The program counter
    struct fetchRegisters fReg;
    
    // Verify that the command line has an appropriate number
    // of arguments
    
    if (argc < 2 || argc > 3) {
        printf("Usage: %s InputFilename [startingOffset]\n", argv[0]);
        return ERROR_RETURN;
    }
    
    // First argument is the file to open, attempt to open it
    // for reading and verify that the open did occur.
    machineCodeFD = open(argv[1], O_RDONLY);
    
    if (machineCodeFD < 0) {
        printf("Failed to open: %s\n", argv[1]);
        return ERROR_RETURN;
    }
    
    // If there is a 2nd argument present, it is an offset so
    // convert it to a value. This offset is the initial value the
    // program counter is to have. The program will seek to that location
    // in the object file and begin fetching instructions from there.
    if (3 == argc) {
        // See man page for strtol() as to why
        // we check for errors by examining errno
        errno = 0;
        PC = strtol(argv[2], NULL, 0);
        if (errno != 0) {
            perror("Invalid offset on command line");
            return ERROR_RETURN;
        }
    }
    
    printf("Opened %s, starting offset 0x%016llX\n", argv[1], PC);
    
    // Start adding your code here and comment out the line the #define EXAMPLESON line
    
    // #define EXAMPLESON 1
    
    FILE * fp;
    fp = fopen(argv[1], "r");
    fReg.valP = PC;
    
    int current;
    uint8_t iCodeiFun;
    uint8_t registers;
    int wrongifun;
    int halts;
    int bytesRequired;
    int numBytes;
    
    while ((current = fgetc(fp)) != EOF) {
        fReg.PC = fReg.valP;
        fseek(fp, fReg.PC, SEEK_SET);
        fread(&iCodeiFun, sizeof(iCodeiFun), 1, fp);
        fReg.icode = iCodeiFun >> 4 & 0x0F;
        fReg.ifun = iCodeiFun & 0x0F;
        
        
        switch (fReg.icode) {
            case 0x0:
                fReg.instr = "halt";
                fReg.regsValid = 0;
                fReg.valCValid = 0;
                fReg.valP = fReg.PC + 1;
                halts = halts + 1;
                bytesRequired = 0;
                if (fReg.ifun != 0) {
                    printf("Invalid function code %X%X at 0x%X\n", fReg.icode, fReg.ifun, fReg.PC);
                    fclose(fp);
                    return ERROR_RETURN;
                }
                break;
                
            case 0x1:
                fReg.instr = "nop";
                fReg.regsValid = 0;
                fReg.valCValid = 0;
                fReg.valP = fReg.PC + 1;
                bytesRequired = 0;
                halts = 0;
                if (fReg.ifun != 0) {
                    printf("Invalid function code %X%X at 0x%X\n", fReg.icode, fReg.ifun, fReg.PC);
                    fclose(fp);
                    return ERROR_RETURN;
                }
                break;
                
            case 0x2:
                switch (fReg.ifun) {
                    case 0:
                        fReg.instr = "rrmovq";
                        break;
                    case 1:
                        fReg.instr = "cmovle";
                        break;
                    case 2:
                        fReg.instr = "cmovl";
                        break;
                    case 3:
                        fReg.instr = "cmove";
                        break;
                    case 4:
                        fReg.instr = "cmovne";
                        break;
                    case 5:
                        fReg.instr = "cmovge";
                        break;
                    case 6:
                        fReg.instr = "cmovg";
                        break;
                    default:
                        printf("Invalid function code %X%X at 0x%X\n", fReg.icode, fReg.ifun, fReg.PC);
                        fclose(fp);
                        return ERROR_RETURN;
                }
                
                fReg.regsValid = 1;
                fReg.valCValid = 0;
                fReg.valP = fReg.PC + 2;
                halts = 0;
                bytesRequired = 1;
                break;
                
            case 0x3:
                fReg.instr = "irmovq";
                fReg.regsValid = 1;
                fReg.valCValid = 1;
                fReg.valP = fReg.PC + 10;
                bytesRequired = 9;
                halts = 0;
                if (fReg.ifun != 0) {
                    printf("Invalid function code %X%X at 0x%X\n", fReg.icode, fReg.ifun, fReg.PC);
                    fclose(fp);
                    return ERROR_RETURN;
                }
                break;
                
            case 0x4:
                fReg.instr = "rmmovq";
                fReg.regsValid = 1;
                fReg.valCValid = 1;
                fReg.valP = fReg.PC + 10;
                halts = 0;
                bytesRequired = 9;
                if (fReg.ifun != 0) {
                    printf("Invalid function code %X%X at 0x%X\n", fReg.icode, fReg.ifun, fReg.PC);
                    fclose(fp);
                    return ERROR_RETURN;
                }
                break;
                
            case 0x5:
                fReg.instr = "mrmovq";
                fReg.regsValid = 1;
                fReg.valCValid = 1;
                fReg.valP = fReg.PC + 10;
                halts = 0;
                bytesRequired = 9;
                if (fReg.ifun != 0) {
                    printf("Invalid function code %X%X at 0x%X\n", fReg.icode, fReg.ifun, fReg.PC);
                    fclose(fp);
                    return ERROR_RETURN;
                }
                break;
                
            case 0x6:
                
                switch (fReg.ifun) {
                    case 0x0:
                        fReg.instr = "addq";
                        break;
                    case 0x1:
                        fReg.instr = "subq";
                        break;
                    case 0x2:
                        fReg.instr = "andq";
                        break;
                    case 0x3:
                        fReg.instr = "xorq";
                        break;
                    case 0x4:
                        fReg.instr = "mulq";
                        break;
                    case 0x5:
                        fReg.instr = "divq";
                        break;
                    case 0x6:
                        fReg.instr = "modq";
                        break;
                    default:
                        printf("Invalid function code %X%X at 0x%X\n", fReg.icode, fReg.ifun, fReg.PC);
                        fclose(fp);
                        return ERROR_RETURN;
                }
                fReg.regsValid = 1;
                fReg.valCValid = 0;
                fReg.valP = fReg.PC + 2;
                halts = 0;
                bytesRequired = 1;
                break;
                
            case 0x7:
                
                switch (fReg.ifun) {
                    case 0:
                        fReg.instr = "jmp";
                        break;
                    case 1:
                        fReg.instr = "jle";
                        break;
                    case 2:
                        fReg.instr = "jl";
                        break;
                    case 3:
                        fReg.instr = "je";
                        break;
                    case 4:
                        fReg.instr = "jn";
                        break;
                    case 5:
                        fReg.instr = "jge";
                        break;
                    case 6:
                        fReg.instr = "jg";
                        break;
                    default:
                        printf("Invalid function code %X%X at 0x%X\n", fReg.icode, fReg.ifun, fReg.PC);
                        fclose(fp);
                        return ERROR_RETURN;
                }
                fReg.regsValid = 0;
                fReg.valCValid = 1;
                fReg.valP = fReg.PC + 9;
                halts = 0;
                bytesRequired = 8;
                break;
                
            case 0x8:
                fReg.instr = "call";
                fReg.regsValid = 0;
                fReg.valCValid = 1;
                fReg.valP = fReg.PC + 9;
                halts = 0;
                bytesRequired = 8;
                if (fReg.ifun != 0) {
                    printf("Invalid function code %X%X at 0x%X\n", fReg.icode, fReg.ifun, fReg.PC);
                    fclose(fp);
                    return ERROR_RETURN;
                }
                break;
                
            case 0x9:
                fReg.instr = "ret";
                fReg.regsValid = 0;
                fReg.valCValid = 0;
                fReg.valP = fReg.PC + 1;
                halts = 0;
                bytesRequired = 0;
                if (fReg.ifun != 0) {
                    printf("Invalid function code %X%X at 0x%X\n", fReg.icode, fReg.ifun, fReg.PC);
                    fclose(fp);
                    return ERROR_RETURN;
                }
                break;
                
            case 0xA:
                fReg.instr = "pushq";
                fReg.regsValid = 1;
                fReg.valCValid = 0;
                fReg.valP = fReg.PC + 2;
                halts = 0;
                bytesRequired = 1;
                if (fReg.ifun != 0) {
                    printf("Invalid function code %X%X at 0x%X\n", fReg.icode, fReg.ifun, fReg.PC);
                    fclose(fp);
                    return ERROR_RETURN;
                }
                break;
                
            case 0xB:
                fReg.instr = "popq";
                fReg.regsValid = 1;
                fReg.valCValid = 0;
                fReg.valP = fReg.PC + 2;
                halts = 0;
                bytesRequired = 1;
                if (fReg.ifun != 0) {
                    printf("Invalid function code %X%X at 0x%X\n", fReg.icode, fReg.ifun, fReg.PC);
                    fclose(fp);
                    return ERROR_RETURN;
                }
                break;
                
            default:
                printf("Invalid opcode %X%X at 0x%X\n", fReg.icode, fReg.ifun, fReg.PC);
                fclose(fp);
                return ERROR_RETURN;
        }
        
        numBytes = 0;
        while ((current = fgetc(fp)) != EOF && numBytes < bytesRequired) {
            numBytes = numBytes + 1;
        }
        
        if (numBytes < bytesRequired) {
            fReg.rA = 0;
            fReg.rB = 0;
            fReg.valC = 0;
            printRegS(&fReg);
            printf("Memory access at 0x%X, required %X bytes, read %X bytes \n", fReg.PC + fReg.regsValid + 1, bytesRequired, numBytes);
            fclose(fp);
            return ERROR_RETURN;
        }
        
        if (fReg.regsValid == 1) {
            fseek(fp, fReg.PC + 1, SEEK_SET);
            fread(&registers, sizeof(registers), 1, fp);
            fReg.rA = registers >> 4 & 0x0F;
            fReg.rB = registers & 0x0F;
        }

        if (fReg.valCValid == 1) {
            fseek(fp, fReg.PC + fReg.regsValid + 1, SEEK_SET);
            fread(&fReg.valC, sizeof(fReg.valC), 1, fp);
            fseek(fp, fReg.PC + fReg.regsValid + 1, SEEK_SET);
            fread(&fReg.byte0, sizeof(fReg.byte0), 8, fp);
        }
        
        if (halts < 3) {
            printRegS(&fReg);
        }
        
    }
    
    printf("Normal Termination \n");
    fclose(fp);
    
#ifdef  EXAMPLESON
    
    // The next few lines are examples of various types of output. In the comments is
    // an instruction, the address it is at and the associated binary code that would
    // be found in the object code file at that address (offset). Your program
    // will read that binary data and then pull it appart just like the fetch stage.
    // Once everything has been pulled apart then a call to printRegS is made to
    // have the output printed. Read the comments in printInternalReg.h for what
    // the various fields of the structure represent. Note: Do not fill in fields
    // into the structure that aren't used by that instruction.
    
    
    /*************************************************
     irmovq $1, %rsi   0x008: 30f60100000000000000
     ***************************************************/
    
    fReg.PC = 8; fReg.icode = 3; fReg.ifun = 0;
    fReg.regsValid = 1; fReg.rA = 15;  fReg.rB = 6;
    fReg.valCValid = 1; fReg.valC = 1;
    fReg.byte0 = 1;  fReg.byte1 = 0; fReg.byte2 = 0; fReg.byte3 = 0;
    fReg.byte4 = 0;  fReg.byte5 = 0; fReg.byte6 = 0; fReg.byte7 = 0;
    fReg.valP = 8 + 10;  fReg.instr = "irmovq";
    
    //printRegS(&fReg);
    
    
    /*************************************************
     je target   x034: 733f00000000000000     Note target is a label
     
     ***************************************************/
    
    
    fReg.PC = 0x34; fReg.icode = 7; fReg.ifun = 3;
    fReg.regsValid = 0;
    fReg.valCValid = 1; fReg.valC = 0x3f;
    fReg.byte0 = 0x3f;  fReg.byte1 = 0; fReg.byte2 = 0; fReg.byte3 = 0;
    fReg.byte4 = 0;  fReg.byte5 = 0; fReg.byte6 = 0; fReg.byte7 = 0;
    fReg.valP = 0x34 + 9;  fReg.instr = "je";
    
    //printRegS(&fReg);
    /*************************************************
     nop  x03d: 10
     
     ***************************************************/
    
    fReg.PC = 0x3d; fReg.icode = 1; fReg.ifun = 0;
    fReg.regsValid = 0;
    fReg.valCValid = 0;
    fReg.valP = 0x3d + 1;  fReg.instr = "nop";
    
    //printRegS(&fReg);
    
    /*************************************************
     addq %rsi,%rdx  0x03f: 6062
     
     ***************************************************/
    
    fReg.PC = 0x3f; fReg.icode = 6; fReg.ifun = 0;
    fReg.regsValid = 1; fReg.rA = 6; fReg.rB = 2;
    fReg.valCValid = 0;
    fReg.valP = 0x3f + 2;  fReg.instr = "add";
    
    //printRegS(&fReg);
#endif
    
    
    return SUCCESS;
    
}
