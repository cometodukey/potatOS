int
isxdigit(int c) {
    return (c >= '0' && c <= '9')
            || (c >= 'a' && c <= 'f')
            || (c >= 'A' && c <= 'F');
}

int
isdigit(int c) {
    return c >= '0' && c <= '9';
}

int
isprint(int c) {
    return c >= ' ' && c <= '~';
}
