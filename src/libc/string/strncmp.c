
int strncmp(const char *a, const char *b, unsigned int n) {
    while (n && *a && (*a == *b)) {
        n--;
        a++;
        b++;
    }
    if (n == 0) {
        return 0;
    }
    else {
        return (unsigned char)*a - (unsigned char)*b;
    }
}
