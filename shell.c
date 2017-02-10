#define EXEC_FG 11
#define KILL 10
#define LIST_DIR 9
#define WRITE_FILE 8
#define DEL 7
#define WRITE_SEC 6
#define TERM 5
#define EXEC 4
#define READ_FILE 3
#define READ_SEC 2
#define READ_STR 1
#define PRINT_STR 0
#define PS1 "lolnu :^)"
#define BAD_CMD "Bad command!\r\n"
#define help
#define color 12

void execute(char* command);
void getFirstArg(char* command, char* name);
void listDir();
void createFile(char* fileName);
void getCommandArgs(char* command);
void clear();
int stringComp(char* first, char* second);

int main() {
    char command[80];
    enableInterrupts();
    clear();
    interrupt(0x21, PRINT_STR, "============================================================================\r\n", 0, 0);
    interrupt(0x21, PRINT_STR, "8 8888         ,o888888o.     8 8888         b.             8 8 8888      88\r\n", 0, 0);
    interrupt(0x21, PRINT_STR, "8 8888      . 8888     `88.   8 8888         888o.          8 8 8888      88\r\n", 0, 0);
    interrupt(0x21, PRINT_STR, "8 8888     ,8 8888       `8b  8 8888         Y88888o.       8 8 8888      88\r\n", 0, 0);
    interrupt(0x21, PRINT_STR, "8 8888     88 8888        `8b 8 8888         .`Y888888o.    8 8 8888      88\r\n", 0, 0);
    interrupt(0x21, PRINT_STR, "8 8888     88 8888         88 8 8888         8o. `Y888888o. 8 8 8888      88\r\n", 0, 0);
    interrupt(0x21, PRINT_STR, "8 8888     88 8888         88 8 8888         8`Y8o. `Y88888o8 8 8888      88\r\n", 0, 0);
    interrupt(0x21, PRINT_STR, "8 8888     88 8888        ,8P 8 8888         8   `Y8o. `Y8888 8 8888      88\r\n", 0, 0);
    interrupt(0x21, PRINT_STR, "8 8888     `8 8888       ,8P  8 8888         8      `Y8o. `Y8 ` 8888     ,8P\r\n", 0, 0);
    interrupt(0x21, PRINT_STR, "8 8888      ` 8888     ,88'   8 8888         8         `Y8o.`   8888   ,d8P \r\n", 0, 0);
    interrupt(0x21, PRINT_STR, "8 888888888888 `8888888P'     8 888888888888 8            `Yo    `Y88888P'  \r\n", 0, 0);
    interrupt(0x21, PRINT_STR, "============================================================================\r\n", 0, 0);
    interrupt(0x21, PRINT_STR, "\r\n\n\n\n\n\n\n\n\n\n\n\n", 0, 0);


    while(1){
       interrupt(0x21, PRINT_STR, PS1, 0, 0);
       interrupt(0x21, READ_STR, command, 0, 0);
       execute(command);
    }
}

void execute(char* input) {
    char arg[80];
    char buff[13312];
    int sec;
    int index;
    int bool;
    buff[0] = 0x00;
        
    getFirstArg(&input, arg);
    if (stringComp(arg, "execute")) {
        interrupt(0x21, EXEC, input, 0, 0);
    } else if (stringComp(arg, "execFG")) {
        interrupt(0x21, EXEC_FG, input, 0, 0);
    } else if (stringComp(arg, "type")) {
        interrupt(0x21, READ_FILE, input, buff, 0);
        interrupt(0x21, PRINT_STR, buff, 0, 0);
    } else if(stringComp(arg, "delete")) {
        interrupt(0x21, DEL, input, 0, 0);
    } else if(stringComp(arg, "copy")) {
        getFirstArg(&input, arg);
        interrupt(0x21, READ_FILE, arg, buff, 0);
        for (sec = 0; buff[512*sec] != 0x00; sec++) {
            /* just for counting file size */
        }
        interrupt(0x21, WRITE_FILE, input, buff, sec);
    } else if(stringComp(arg, "dir")) {
        interrupt(0x21, LIST_DIR, 0, 0, 0);
    } else if(stringComp(arg, "clear")) {
        clear();
    } else if(stringComp(arg, "kill")) {
        interrupt(0x21, KILL, input[0] - 48, 0, 0);
    } else if(stringComp(arg, "create")) {
        createFile(input);
    } else if (stringComp(arg, "help")){
        interrupt(0x21, PRINT_STR, ">execute --  execute <program_name> - Executes the program.\r\n", 0, 0);
        interrupt(0x21, PRINT_STR, ">type -- type <file_name> - Prints out the contents of the file.\r\n", 0, 0);
        interrupt(0x21, PRINT_STR, ">delete -- delete <file_name> - Deletes the file.\r\n", 0, 0);
        interrupt(0x21, PRINT_STR, ">copy -- copy <file_name> <file_name> - Copies the contents of the first file\r\n into the second file and creates it.\r\n", 0, 0);
        interrupt(0x21, PRINT_STR, ">dir -- dir - Lists the contents of the directory.\r\n", 0, 0);
        interrupt(0x21, PRINT_STR, ">clear -- clear - clear the console\r\n", 0, 0);
        interrupt(0x21, PRINT_STR, ">create -- create <file_name> - Creates text file with the given name. Accepts\r\n lines of input until empty line.\r\n", 0, 0);
        interrupt(0x21, PRINT_STR, ">help -- help - view help menu\r\n", 0, 0);
        interrupt(0x21, PRINT_STR, ">kill -- kill - kill the current process\r\n", 0, 0);
        interrupt(0x21, PRINT_STR, ">color-help -- color-help - view the color list\r\n", 0, 0);
    } else if(stringComp(arg, "color")){        
        interrupt(0x21, 12, (input[0] - 48)*10+(input[1] - 48), (input[3] - 48)*10+(input[4] - 48), 0);
    }else if(stringComp(arg, "color-help")){
        interrupt(0x21, PRINT_STR, "No  CLR\r\n", 0, 0);
        interrupt(0x21, PRINT_STR, "--  --\r\n", 0, 0);
        interrupt(0x21, PRINT_STR, "00  black\r\n", 0, 0);
        interrupt(0x21, PRINT_STR, "01  dark blue\r\n", 0, 0);
        interrupt(0x21, PRINT_STR, "02  green\r\n", 0, 0);
        interrupt(0x21, PRINT_STR, "03  light blue\r\n", 0, 0);
        interrupt(0x21, PRINT_STR, "04  red\r\n", 0, 0);
        interrupt(0x21, PRINT_STR, "05  purple\r\n", 0, 0);
        interrupt(0x21, PRINT_STR, "06  orange\r\n", 0, 0);
        interrupt(0x21, PRINT_STR, "07  gray\r\n", 0, 0);
        interrupt(0x21, PRINT_STR, "(below works best for FG)\r\n", 0, 0);
        interrupt(0x21, PRINT_STR, "08  dark gray\r\n", 0, 0);
        interrupt(0x21, PRINT_STR, "09  cerulean\r\n", 0, 0);
        interrupt(0x21, PRINT_STR, "10  light green\r\n", 0, 0);
        interrupt(0x21, PRINT_STR, "11  light blue\r\n", 0, 0);
        interrupt(0x21, PRINT_STR, "12  light red\r\n", 0, 0);
        interrupt(0x21, PRINT_STR, "13  pink\r\n", 0, 0);
        interrupt(0x21, PRINT_STR, "14  yellow\r\n", 0, 0);
        interrupt(0x21, PRINT_STR, "15  white\r\n", 0, 0);
    }else {
        interrupt(0x21, PRINT_STR, BAD_CMD, 0, 0);
    }
    return;
}

void getFirstArg(char** command, char* name) {
    int i = 0;
    while((*command)[i] != 0xd  && (*command)[i] != ' ') {
        name[i] = (*command)[i];
        i++;
    }
    *command += i + 1;
    name[i] = '\0';
}

void createFile(char* fileName) {
    char buffer[13312];
    char line[80];
    int pos = 0, len, sectors = 0;

    while (1) {
        interrupt(0x21, READ_STR, line, 0, 0);
        if (line[1] == 0xa) {
            break;
        }
        for (len = 0; line[len] != 0x00; len++) {
            buffer[pos + len] = line[len];
        }
        pos += len;
    } 
    buffer[pos + 1] = 0x00;

    /* Calculate number of sectors from number of bytes */ 
    while ((sectors+1)*512 <= pos) {
        sectors  = sectors + 1;
    }

    interrupt(0x21, WRITE_FILE, fileName, buffer, sectors + 1);
    return;
}

int stringComp(char* first, char* second) {
    int i = 0;
    while (first[i] != '\0' && second[i] != '\0') {
        if (first[i] != second[i]) {
            return 0;
        }
        i++;
    }
    return first[i] == '\0' && second[i] == '\0';
}

void clear() {
    int i;
    char* empty = "\r\n\0";
    for (i=0; i < 24; i++) {
        interrupt(0x21, PRINT_STR, empty, 0, 0);
    }
}
