#include "main.h"
#include "utils.h"
#include "docm.h"
#include "guim.h"
#include "filem.h"
#include <signal.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>


void handle_winch(int sig){
    signal(SIGWINCH, SIG_IGN);
    GUI_REINIT(NULL);
    GUI_DRAW();
    signal(SIGWINCH, handle_winch);
}

int main(int argc, char** argv){
    setlocale(LC_CTYPE, "en_US.UTF-8");
    Document* doc = alloc_document(1, STR_REALLOC_DEF_INTERVAL);
    doc->mode = MODE_DETECT_LINUX;
    if(doc == NULL)
    {
        perror("yace: memory allocation failed, malloc returned NULL\n");
        exit(-1);
    }
    //executing command:
    char cwd[PATH_MAX + MAX_FNAME_SIZE];
    char document_name[MAX_FNAME_SIZE];
    if(getcwd(cwd, sizeof(cwd)) == NULL){
        perror("yace: getcwd() returned NULL\n");
        dealloc_document(doc);
        exit(-1);
    }
    if(argc > 1){
        strcpy(document_name, argv[1]);
        int result = load_file_name(document_name, doc);
        if(result < 0){
            switch(result){
                case ERR_NOT_FOUND:
                    printf("%s: no such file (%s)\n", document_name, cwd);
                    dealloc_document(doc);
                    exit(-1);
            }
        }
    }
    else
    {
        sprintf(document_name, "Untitled.txt");
    }

    //initialization:
    sprintf(doc->docname, "%s", document_name);
    int quit = false, control_receiving_mode = false;
    wchar_t ch;

    GUI_INIT(doc);

    struct timeval last_escapeseq_detected, current_t;
    gettimeofday(&last_escapeseq_detected, NULL);

    signal(SIGWINCH, handle_winch); //register signal handler for terminal resize

    GUI_DRAW();
    while(!quit){ //MAIN LOOP
        ch = getwchar();
        gettimeofday(&current_t, NULL);
        if(ch == ESCAPE_KEY)
        {
            gettimeofday(&last_escapeseq_detected, NULL);
        }
        if(ch != ESCAPE_KEY) control_receiving_mode = check_control_mode(last_escapeseq_detected, current_t);
        int res = GUI_HANDLE_INPUT(ch, control_receiving_mode);
        if(res < 0){
            GUI_DEINIT();
            dealloc_document(doc);
            printf("ERROR");
            return 0;
        }
        int current = GUI_GET_CURRENT_NAME();
        if(current < 0){
            GUI_DEINIT();
            dealloc_document(doc);
            printf("ERROR");
        }
        else{
            if(current == UC_NAME_MAINW){  //temp, all this stuff must be moved inside GUI_HANDLE_INPUT
                int feedback;
                switch(res){
                    case R_REFRESH:
                        GUI_REINIT(doc);
                        GUI_DRAW();
                        break;
                    case R_QUIT:
                        quit = true;
                        break;
                    case R_SAVEFILE:
                        feedback = save_file_name(doc->docname, doc);
                }
            }
        }
        GUI_DRAW();
    }

    GUI_DEINIT(doc);
    dealloc_document(doc);
    return 0;
}
