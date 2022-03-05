#include "topo.h"
#include <stdio.h>
#include <stdlib.h>

void fio(
    double   U[NX + 2][NY + 2][NZ + 2][3],
    double   P[NX + 2][NY + 2][NZ + 2],
    double   X[NX + 2][NY + 2][NZ + 2][3],
    double DIV[NX + 2][NY + 2][NZ + 2],
    char*  fname
) {
    double X1, X2, X3, U1, U2, U3, PP, DD;
    FILE  *fo;

    fo = fopen(fname, "w+t");
    if (fo == NULL) {
        printf("\nERROR when opening file %s", fname);
    }
    else {
        fprintf(fo, "x,y,z,u,v,w,p,div\n");
        for (int k = 1; k <= NZ; k ++) {
            for (int j = 1; j <= NY; j ++) {
                for (int i = 1; i <= NX; i ++) {
                    X1 =   X[i][j][k][0];
                    X2 =   X[i][j][k][1];
                    X3 =   X[i][j][k][2];
                    U1 =   U[i][j][k][0];
                    U2 =   U[i][j][k][1];
                    U3 =   U[i][j][k][2];
                    PP =   P[i][j][k];
                    DD = DIV[i][j][k];
                    fprintf(fo, "%5.8lf,%5.8lf,%5.8lf,%5.8lf,%5.8lf,%5.8lf,%5.8lf,%5.8lf\n", X1, X2, X3, U1, U2, U3, PP, DD);
                }
            }
        }
        fclose(fo);
    }
}