#include "funclist.h"
#include "varlist.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    geom(X);

    #pragma acc enter data copyin(X, U, UD, UC, UU, P, DIV, KX, J, G, C, SGS)

    init(U, UD, UC, UU, P, SGS, X, KX, J, C, G);
    bcu(U, UD, KX, DT);

    for (STEP = 1; STEP <= NSTEP; STEP ++) {
        fs1(U, UD, UU, SGS, KX, J, C, ALPHA, DT, RI);
        contra(U, UC, UU, KX, J);
        solid(U, UU);
        diver(UU, J, DIV, AD);

        printf("d(%5.8lf)", AD);
        fflush(stdout);

        do {
            sor(P, DIV, C, OMEGA, EPOI, MAXIT, DT, IT, R);
            // jacob(P, DIV, C, OMEGA, EPOI, MAXIT, DT, IT, R);
            avp0(P);
            bcp(P, U, C, KX, RI);
            fs2(U, UU, P, KX, G, DT);
            solid(U, UU);
            bcu(U, UD, KX, DT);
            diver(UU, J, DIV, AD);

            printf("\rstep %5d: p(%3d, %5.8lf), d(%5.8lf)", STEP, IT, R, AD);
            fflush(stdout);

        } while (AD > EDIV);
    }
    printf("\n");

    #pragma acc exit data copyout(X, U, UD, UC, UU, P, DIV, KX, J, G, C, SGS)

    fio(U, P, X, DIV, (char*)"o.csv");

    return 0;
}