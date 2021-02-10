//
// Created by MIKE on 10.02.2021.
//

int func() {
    int a12345 = 0, d = 3, f = 4, c;
    unsigned char _b_;
    c = d * f / 2; /* this is a comment #1 */
    int g;
    int i;
    for (i = 0; i < 5; i = i + 1) {
        if (i < 3) {
            g = g + (i - 1);
        } else if (i == 3) {
            g = g + 3;
        } else {
            g = g + (i + 1);
        }
        for (c = 0; c < 3; c = c + 1) {
            g = g + c;
        }
    }
/*
int xxx = i + 50;
*/
    do {
        i = i - 1;
    } while (i > 0);
    while (i < 3) {
        i = i + 1;
    }
    int j = 0; // this is a comment #3
    if (i == 3) {
        j = 100;
    } else if (i != 2) {
        j = 200;
    }
    return 0;
}