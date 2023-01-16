
char *strcpy(char *s, const char *t, unsigned int n) {
    
    char *ps;

    ps = s;
    if(n <= 0) {
        return ps;
    }
    while(--n > 0 && (*s++ = *t++) != 0) {
        ;
    }
    *s = 0;
    return ps;
}
