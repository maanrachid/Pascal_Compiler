#include <stdio.h>
#include "machine.h"
#include "iolib.h"

extern either_type st[];

int write_bool(int val, int totalwidth)
{
    int i;
    if (totalwidth <= 0)
        return IOERROR;
    else if (val == 0) {
        for (i = 0; i < totalwidth - 5; i++)
            putchar(' ');
        printf("false");
        return RUN;
    }
    else if (val == 1) {
        for (i = 0; i < totalwidth - 4; i++)
            putchar(' ');
        printf("true");
        return RUN;
    } else
        return VALUEERROR;
}

int write_char(int val, int totalwidth)
{
    int i;
    if (totalwidth <= 0)
        return IOERROR;
    else {
        for (i = 0; i < totalwidth - 1; i++)
            putchar(' ');
        putchar(val);
        return RUN;
    }
}

int write_int(int val, int totalwidth)
{
    char arr[20];
    if (totalwidth <= 0)
        return IOERROR;
    else {
        sprintf(arr, "%%%dd", totalwidth);
        printf(arr, val);
        return RUN;
    }
}

int write_real2(float val, int totalwidth)
{
    char arr[20];
    int actualwidth;
    if (totalwidth <= 0)
        return IOERROR;
    else {
        if (totalwidth < 8)
            actualwidth = 8;
        else
            actualwidth = totalwidth;
        sprintf(arr, "%%%d.%de", actualwidth, actualwidth - 7);
        printf(arr, val);
        return RUN;
    }
}

int write_string(int addr, int totalwidth, int strlength)
{
    int i;
    if (totalwidth <= 0)
        return IOERROR;
    else {
        for (i = 0; i < totalwidth - strlength; i++)
            putchar(' ');
        for (i = 0; i < strlength; i++)
            putchar(st[addr + i].i);
        return RUN;
    }
}

int write_real3(float val, int totalwidth, int fracdigits)
{
    char arr[20];
    if ((totalwidth <= 0) || (fracdigits <= 0))
        return IOERROR;
    else {
        sprintf(arr, "%%%d.%df", totalwidth, fracdigits);
        printf(arr, val);
        return RUN;
    }
}
