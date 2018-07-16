#define ENDSTRING '\0'
int str_len(char *string){
    int len = 0;

    while(*string++ != ENDSTRING){
        len++;
    }
    return len;
}
