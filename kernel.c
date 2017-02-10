/* Xingcheng Jiang, Walt Panfil, and Maxim Zaman 
 * Team 1J
 */
typedef struct{
    int isActive;
    int waitingOn;
    int sp;
} pcb;

pcb processTable[8];
int currProcess;

void printString(char* string);
void readString(char* string);
void readSector(char* buffer, int sector);
void writeSector(char* buffer, int sector);
int readFile(char* buffer, char* fileName);
void writeFile(char* buffer, char* fileName, int numSectors);
void deleteFile(char* fileName);
void execPrg(char* fileName);
void execProg(char* fileName, int blockedProc);
void termProg();
void killProcess(int proc);
void listDir();
int mod(a, b);
int div(a, b);
void setColor(char t, char b);

int main() {
    int i;
    char shell[6];
    shell[0] = 's';
    shell[1] = 'h';
    shell[2] = 'e';
    shell[3] = 'l';
    shell[4] = 'l';
    shell[5] = '\0';
    for(i = 0; i < 8; i++) {
        processTable[i].isActive = 0;
        processTable[i].waitingOn = -1;
        processTable[i].sp = 0xff00;
    }
    currProcess = 0;
    makeInterrupt21();
    execProg(shell);
    makeTimerInterrupt();
    while(1);
}

void printString(char* string) {
    int i = 0;
    while(string[i] != '\0') {
        interrupt(0x10, 0xe*256 + string[i], 0, 0, 0);
        i++;
    } 
    return;
}

void readString(char* string) {
    int i = 0;
    char input = 'a';
    while (input != 0xd) {
        input = interrupt(0x16, 0, 0, 0, 0);
        if (input == 0x8) {
            if (i > 0) {
                i--;
                interrupt(0x10, 0xe*256 + input, 0, 0, 0);
                interrupt(0x10, 0xe*256 + ' ', 0, 0, 0);
                interrupt(0x10, 0xe*256 + input, 0, 0, 0);
            }
        } else {
            interrupt(0x10, 0xe*256 + input, 0, 0, 0);
            string[i] = input;
            i++;
        }
    }
    interrupt(0x10, 0xe*256 + 0xa, 0, 0, 0);
    string[i] = 0xa;
    string[i+1] = 0x0;
    return;
}

void readSector(char* buffer, int sector) {
    int ch = div(sector, 36);
    int cl = mod(sector, 18) + 1;
    int dh = mod(div(sector, 18), 2);
    interrupt(0x13, 2*256 + 1, buffer, 256*ch + cl, 256*dh);
    return;
}

void writeSector(char* buffer, int sector) {
    int ch = div(sector, 36);
    int cl = mod(sector, 18) + 1;
    int dh = mod(div(sector, 18), 2);
    interrupt(0x13, 3*256 + 1, buffer, 256*ch + cl, 256*dh);
    return;
}

int readFile(char* buffer, char* fileName) {
    int i, j, flag = 0;
    char directory[512];
    char err[17];
    readSector(directory, 2);
    for (i = 0; i < 512; i += 32) {
        flag = 1;
        for (j = 0; j < 6; j++) {
            if (directory[i+j] == 0x00) {
                if (fileName[j] != 0x0d && fileName[j] != 0x00) {
                    flag = 0;
                }
                j = 6;
                break;
            }
            if (directory[i+j] != fileName[j]) {
                flag = 0;
                break;
            }
        }
        if (flag == 1) {
            while (directory[i + j] != 0x0) {
                readSector(buffer, (int) directory[i + j]);
                buffer += 512;
                j++;
            }
            return 1;
        }
    }
    err[0] = 'F';
    err[1] = 'i';
    err[2] = 'l';
    err[3] = 'e';
    err[4] = ' ';
    err[5] = 'n';
    err[6] = 'o';
    err[7] = 't';
    err[8] = ' ';
    err[9] = 'f';
    err[10] = 'o';
    err[11] = 'u';
    err[12] = 'n';
    err[13] = 'd';
    err[14] = '\r';
    err[15] = '\n';
    err[16] = '\0';
    printString(err);
    return 0;
}

void writeFile(char* buffer, char* fileName, int numSectors) {
    int i, j, sec, flag = 0;
    char directory[512];
    char map[512];
    readSector(directory, 2);
    readSector(map, 1);
    
    /* Find an empty directory entry for the file */
    for (i = 0; i < 512; i+= 32) {
        if (directory[i] == 0x00) {
            for (j = 0; j < 6; j++) {
                directory[i+j] = fileName[j];
                if (fileName[j] == 0x0d) {
                    break;
                }
            }
            for (; j < 6; j++) {
                directory[i+j] = 0x00;
            }
            break;
        }
    }

    /* Check that there is enough space for the file */
    for (j = 0; j < numSectors; j++) {
        while (map[sec] != 0x00) {
            sec++;
            if (sec >= 512) {
                return;
            }
        }
    }

    /* Actually write the file */
    sec = 0;
    for (j = 0; j < numSectors; j++) {
        while (map[sec] != 0x00) {
            sec++;
        }
        map[sec] = 0xFF;
        directory[i+6+j] = sec;
        writeSector(buffer + 512*j, sec);
    }
    writeSector(map, 1);
    writeSector(directory, 2);
}

void deleteFile(char* fileName) {
    int i, j, flag = 0;
    char directory[512];
    char map[512];
    char err[17];
    readSector(directory, 2);
    for (i = 0; i < 512; i += 32) {
        flag = 1;
        for (j = 0; j < 6; j++) {
            if (directory[i+j] == 0x00) {
                if (fileName[j] != 0x0d && fileName[j] != 0x00) {
                    flag = 0;
                }
                j = 6;
                break;
            }
            if (directory[i+j] != fileName[j]) {
                flag = 0;
                break;
            }
        }
        if (flag == 1) {
            readSector(map, 1);
            while (directory[i + j] != 0x00) {
                map[directory[i + j]] = 0x00;
                j++;
            }
            writeSector(map, 1);
            directory[i] = 0x00;
            directory[i+6] = 0x00;
            writeSector(directory, 2);
            return;
        }
    }
    err[0] = 'F';
    err[1] = 'i';
    err[2] = 'l';
    err[3] = 'e';
    err[4] = ' ';
    err[5] = 'n';
    err[6] = 'o';
    err[7] = 't';
    err[8] = ' ';
    err[9] = 'f';
    err[10] = 'o';
    err[11] = 'u';
    err[12] = 'n';
    err[13] = 'd';
    err[14] = '\r';
    err[15] = '\n';
    err[16] = '\0';
    printString(err);
    return;

}

void execPrg(char* fileName) {
    execProg(fileName, -1);
}

void execProg(char* fileName, int blockedProc) {
    char buffer[13312];
    register int seg;
    int j;
    setKernelDataSegment();
    for (seg = 0; seg < 8; seg++) {
        if (processTable[seg].isActive == 0) {
            processTable[seg].sp = 0xff00;
            break;
        }
    }
    restoreDataSegment();
    if (readFile(buffer, fileName) == 0) {
        return;
    }
    for (j = 0; j < 13312; j++) {
        putInMemory((seg + 2) * 0x1000, j, buffer[j]);
    }

    if (seg == 8) {
        return;
    }
    initializeProgram((seg + 2) * 0x1000);
    setKernelDataSegment();
    processTable[seg].isActive = 1;
    if (blockedProc >= 0) {
        processTable[blockedProc].waitingOn = seg;
    }
    restoreDataSegment();
    return;
}

void termProg() {
    register int i;
    setKernelDataSegment();
    processTable[currProcess].isActive = 0;
    processTable[currProcess].sp = 0xff00;
    for (i = 0; i < 8; i++) {
        if(processTable[i].waitingOn == currProcess)
            processTable[i].waitingOn = -1;
    }
    while(1);
}

void killProcess(int proc) {
    register int i;
    setKernelDataSegment();
    processTable[proc].isActive = 0;
    processTable[currProcess].sp = 0xff00;
    for (i = 0; i < 8; i++) {
        if(processTable[i].waitingOn == proc)
            processTable[i].waitingOn = -1;
    }
    restoreDataSegment();
    return;
}

void listDir() {
    char buff[512];
    char name[9];
    int i = 0, j;
    interrupt(0x21, 2, buff, 2, 0);
    name[6] = '\r';
    name[7] = '\n';
    name[8] = '\0';
    while (i < 512) {
        if (buff[i] != 0x00) {
            for (j = 0; j < 6; j++) {
                if (buff[i+j] == 0x00) {
                    name[j] = ' ';
                } else {
                    name[j] = buff[i+j];
                }
            }
            interrupt(0x21, 0, name, 0, 0);
        }
        i += 32;
    }
    return;
}

int mod(a, b) {
    while (a >= b) {
        a = a - b;
    }
    return a;
}

int div(a, b) {
    int quotient = 0;
    while ((quotient+1)*b <= a) {
        quotient  = quotient  + 1;
    }
    return quotient;
}

void handleInterrupt21(int ax, int bx, int cx, int dx) {
    char err[17];
    switch (ax) {
        case 0:
            printString((char*) bx);
            break;

        case 1:
            readString((char*) bx);
            break;

        case 2:
            readSector((char*) bx, cx);
            break;

        case 3:
            readFile((char*) cx, (char*) bx);
            break;

        case 4:
            execPrg((char*) bx);
            break;

        case 5:
            termProg();
            break;

        case 6:
            writeSector((char*) bx, cx);
            break;

        case 7:
            deleteFile((char*) bx);
            break;

        case 8:
            writeFile((char*) cx, (char*) bx, dx);
            break;

        case 9:
            listDir();
            break;

        case 10:
            killProcess(bx);
            break;

         case 12:
         	setColor(bx, cx);
         	break;

        case 11:
            execProg((char*) bx, cx);
            break;
        default:
            err[0] = 'I';
            err[1] = 'n';
            err[2] = 'v';
            err[3] = 'a';
            err[4] = 'l';
            err[5] = 'i';
            err[6] = 'd';
            err[7] = ' ';
            err[8] = 'o';
            err[9] = 'p';
            err[10] = 'c';
            err[11] = 'o';
            err[12] = 'd';
            err[13] = 'e';
            err[14] = '\r';
            err[15] = '\n';
            err[16] = '\0';
            printString(err);
    }
    return;
}

void handleTimerInterrupt(int segment, int sp) {
    int i, newSegment;

    if((currProcess + 2) * 0x1000 == segment) {
        processTable[currProcess].sp = sp;
    }

    for(i = currProcess + 1; i != currProcess; i++) {
        if (i == 8) {
            i = 0;
        }
        if (processTable[i].isActive  == 1 && processTable[i].waitingOn == -1) {
            currProcess = i;
            newSegment = (i+2) * 0x1000;
            returnFromTimer(newSegment, processTable[i].sp);
        }
    }
    if (processTable[currProcess].isActive == 1 &&
            processTable[currProcess].waitingOn == -1) {
        newSegment = (currProcess+2) * 0x1000;
        returnFromTimer(newSegment, processTable[currProcess].sp);
    }

    returnFromTimer(segment, sp);
}

void setColor(char t, char b){
	setKernelDataSegment();
	interrupt(0x10, 6*256, 256*((b*16)+t), 0, (256*50)+80);
	interrupt(0x10, 0x2*256, 0, 0, 0);
	restoreDataSegment();
}
