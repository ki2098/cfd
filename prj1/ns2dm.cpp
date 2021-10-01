#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <algorithm>

using namespace std;

const int nc = 128;
const double u0 = 1.0;
const double L0 = 1.0;
const double nu = 1.004e-6;
const double cfl = 1.0;
const double dd = 1.0 / nc;
const double dx = dd;
const double dy = dd;
const double B0 = 2.0 / (dx * dx) + 2.0 / (dy * dy);
const double dtau = 1.0 / B0;
double dt;
double Re;
double ter;
int iter;

// a[x][y]
const int UX = nc + 1;
const int UY = nc + 2;
const int VX = nc + 2;
const int VY = nc + 1;
const int PX = nc + 2;
const int PY = nc + 2;
const int NG = nc + 1;
double U1[UX][UY], U2[UX][UY], UG[NG][NG];
double V1[VX][VY], V2[VX][VY], VG[NG][NG];
double P1[PX][PY], P2[PX][PY], PG[NG][NG];
double (*u)[UY] = U1;
double (*v)[VY] = V1;
double (*p)[PY] = P1;
double (*ut)[UY] = U2;
double (*vt)[VY] = V2;
double (*pt)[PY] = P2;
double (*ug)[NG] = UG;
double (*vg)[NG] = VG;
double (*pg)[NG] = PG;
double maxdiv;

// void getmax(void) {
//     double tmp = 0;
//     for (int i = 0; i < UX; i ++) {
//         for (int j = 0; j < UY; j ++) {
//             if (abs(u[i][j]) > tmp) {
//                 tmp = u[i][j];
//             }
//         }
//     }
//     maxu = tmp;
//     tmp = 0;
//     for (int i = 0; i < VX; i ++) {
//         for (int j = 0; j < VY; j ++) {
//             if (abs(v[i][j]) > tmp) {
//                 tmp = v[i][j];
//             }
//         }
//     }
//     maxv = tmp;
// }

// void calcdt(void) {
//     // getmax();
//     dt = min(cfl * dd / (sqrt(2.0)), Re * dx * dx * dy * dy / (2 * (dx * dx + dy * dy)));
// }

void calcdiv(void) {
    double tdiv = 0;
    for(int i = 1; i < UX - 1; i ++) {
        for (int j = 1; j < UY - 1; j ++) {
            double div = (u[i + 1][j] - u[i - 1][j]) / (2 * dx) + (u[i][j + 1] - u[i][j - 1]) / (2 * dy);
            if(abs(div) > tdiv) {
                tdiv = abs(div);
            }
        }
    }
    for(int i = 1; i < VX - 1; i ++) {
        for (int j = 1; j < VY - 1; j ++) {
            double div = (v[i + 1][j] - v[i - 1][j]) / (2 * dx) + (v[i][j + 1] - v[i][j - 1]) / (2 * dy);
            if(abs(div) > tdiv) {
                tdiv = abs(div);
            }
        }
    }
    maxdiv = tdiv;
}

void init(void) {
    for (int i = 0; i < UX; i ++) {
        for (int j = 0; j < UY; j ++) {
            u[i][j] = 0;
        }
    }
    for (int i = 1; i < UX - 1; i ++) {
        u[i][UY - 1] = 1;
        u[i][UY - 2] = 1;
    }
    for (int i = 0; i < UX; i ++) {
        for (int j = 0; j < UY; j ++) {
            ut[i][j] = 0;
        }
    }
    for (int i = 1; i < UX - 1; i ++) {
        ut[i][UY - 1] = 1;
        ut[i][UY - 2] = 1;
    }
    for (int i = 0; i < VX; i ++) {
        for (int j = 0; j < VY; j ++) {
            v[i][j] = 0;
        }
    }
    for (int i = 0; i < VX; i ++) {
        for (int j = 0; j < VY; j ++) {
            vt[i][j] = 0;
        }
    }
    for (int i = 0; i < PX; i ++) {
        for (int j = 0; j < PX; j ++) {
            p[i][j] = 0;
        }
    }
    for (int i = 0; i < PX; i ++) {
        for (int j = 0; j < PX; j ++) {
            pt[i][j] = 0;
        }
    }
}

void bcu(void) {
    for (int i = 1; i < UX - 1; i ++) {
        u[i][0] = - u[i][1];
        u[i][UY - 1] = 2.0 - u[i][UY - 2];
    }
    for (int j = 0; j < UY; j ++) {
        u[0][j] = 0;
        u[UX - 1][j] = 0;
    }
}

void bcv(void) {
    for (int i = 0; i < VX; i ++) {
        v[i][0] = 0;
        v[i][VY - 1] = 0;
    }
    for (int j = 1; j < VY - 1; j ++) {
        v[0][j] = - v[1][j];
        v[VX - 1][j] = - v[VX - 2][j];
    }
}

void bcp(void) {
    for (int i = 0; i < PX; i ++) {
        p[i][0] = p[i][1];
        p[i][PY - 1] = p[i][PY - 2];
    }
    for (int j = 0; j < PY; j ++) {
        p[0][j] = p[1][j];
        p[PX - 1][j] = p[PX - 2][j];
    }
}

void bfu(void) {
    for (int i = 0; i < UX; i ++) {
        ut[i][0] = u[i][0];
        ut[i][UY - 1] = u[i][UY - 1];
    }
    for (int j = 0; j < UY; j ++) {
        ut[0][j] = u[0][j];
        ut[UX - 1][j] = u[UX - 1][j];
    }
}

void bfv(void) {
    for (int i = 0; i < VX; i ++) {
        vt[i][0] = v[i][0];
        vt[i][VY - 1] = v[i][VY - 1];
    }
    for (int j = 0; j < VY; j ++) {
        vt[0][j] = v[0][j];
        vt[VX - 1][j] = v[VX - 1][j];
    }
}

void bfp(void) {
    for (int i = 0; i < PX; i ++) {
        pt[i][0] = p[i][0];
        pt[i][PY - 1] = p[i][PY - 1];
    }
    for (int j = 0; j < PY; j ++) {
        pt[0][j] = p[0][j];
        pt[PX - 1][j] = p[PX - 1][j];
    }
}

void fs1(void) {
    // double duudx, duvdy, dvvdy, duvdx, ddudxx, ddudyy, ddvdxx, ddvdyy;
    for (int i = 1; i < UX - 1; i ++) {
        for (int j = 1; j < UY - 1; j ++) {
            // duu / dx
            double umr = (u[i][j] + u[i + 1][j]) / 2.0;
            double uml = (u[i][j] + u[i - 1][j]) / 2.0;
            double duudx = (umr * umr - uml * uml) / dx;
            //duv / dy
            double umu = (u[i][j] + u[i][j + 1]) / 2.0;
            double umd = (u[i][j] + u[i][j - 1]) / 2.0;
            double vmu = (v[i][j] + v[i + 1][j]) / 2.0;
            double vmd = (v[i][j - 1] + v[i + 1][j - 1]) / 2.0;
            double duvdy = (umu * vmu - umd * vmd) / dy;
            // viscousity
            double ddudxx = (u[i - 1][j] - 2 * u[i][j] + u[i + 1][j]) / (dx * dx);
            double ddudyy = (u[i][j - 1] - 2 * u[i][j] + u[i][j + 1]) / (dy * dy);
            ut[i][j] = u[i][j] + dt * (- duudx - duvdy + (ddudxx + ddudyy) / Re);
        }
    }
    for (int i = 1; i < VX - 1; i ++) {
        for (int j = 1; j < VY - 1; j ++) {
            // dyy / dy
            double vmu = (v[i][j] + v[i][j + 1]) / 2.0;
            double vmd = (v[i][j] + v[i][j - 1]) / 2.0;
            double dvvdy = (vmu * vmu - vmd * vmd) / dy;
            // duv / dx
            double umr = (u[i][j] + u[i][j + 1]) / 2.0;
            double uml = (u[i - 1][j] + u[i - 1][j + 1]) / 2.0;
            double vmr = (v[i][j] + v[i + 1][j]) / 2.0;
            double vml = (v[i][j] + v[i - 1][j]) / 2.0;
            double duvdx = (umr * vmr - uml * vml) / dx;
            // viscousity
            double ddvdxx = (v[i - 1][j] - 2 * v[i][j] + v[i + 1][j]) / (dx * dx);
            double ddvdyy = (v[i][j - 1] - 2 * v[i][j] + v[i][j + 1]) / (dy * dy);
            vt[i][j] = v[i][j] + dt * (- dvvdy - duvdx + (ddvdxx + ddvdyy) / Re);
        }
    }
    bfu();
    bfv();
    double (*uex)[UY] = u;
    u = ut;
    ut = uex;
    double (*vex)[VY] = v;
    v = vt;
    vt = vex;
}

void poisson(void) {
    double E = 0.02;
    int MAXITER = 10000;
    bool con = false;
    int it = 0;
    while (con == false) {
        con = true;
        for (int i = 1; i < PX - 1; i ++) {
            for (int j = 1; j < PY - 1; j ++) {
                double dudx = (u[i][j] - u[i - 1][j]) / dx;
                double dvdy = (v[i][j] - v[i][j - 1]) / dy;
                double psi = (dudx + dvdy) / dt;
                double ddpdxx = (p[i - 1][j] - 2 * p[i][j] + p[i + 1][j]) / (dx * dx);
                double ddpdyy = (p[i][j - 1] - 2 * p[i][j] + p[i][j + 1]) / (dy * dy);
                double res = dtau * (ddpdxx + ddpdyy - psi);
                pt[i][j] = p[i][j] + res;
                if (abs(res) > abs(pt[i][j]) * E) {
                    con = false;
                }
            }
        }
        bfp();
        double (*pex)[PY] = p;
        p = pt;
        pt = pex;
        it += 1;
        if (it >= MAXITER) {
            break;
        }
    }
    // if (con == true) {
    //     printf("Convergence in %d iterations\n", it);
    // }
    iter = it;
    bcp();
}

void fs2(void) {
    for (int i = 1; i < UX - 1; i ++) {
        for (int j = 1; j < UY - 1; j ++) {
            ut[i][j] = u[i][j] - dt * (p[i + 1][j] - p[i][j]) / dx;
        }
    }
    for (int i = 1; i < VX - 1; i ++) {
        for (int j = 1; j < VY - 1; j ++) {
            vt[i][j] = v[i][j] - dt * (p[i][j + 1] - p[i][j]) / dy;
        }
    }
    bfu();
    bfv();
    double (*uex)[UY] = u;
    u = ut;
    ut = uex;
    double (*vex)[VY] = v;
    v = vt;
    vt = vex;
    bcu();
    bcv();
}

void ns2d(FILE* f) {
    // printf("dt = %lf\n", dt);
    double t = 0;
    int loo = 0;
    while (t < ter) {
        // calcdt();
        fs1();
        poisson();
        fs2();
        calcdiv();
        fflush(stdout);
        t += dt;
        printf("\rt = %5.8lf, poi = %5d, max div = %5.8lf", t, iter, maxdiv);
        if (loo % 10 == 0) {
            fprintf(f, "%10.5lf,%5d,%10.5lf\n", t, iter, maxdiv);
        }
        loo += 1;
    }
    printf("\n");
}

void calcgrid(void) {
    for (int i = 0; i < NG; i ++) {
        for (int j = 0; j < NG; j ++) {
            ug[i][j] = (u[i][j] + u[i][j + 1]) / 2.0;
            vg[i][j] = (v[i][j] + v[i + 1][j]) / 2.0;
            pg[i][j] = (p[i][j] + p[i + 1][j] + p[i][j + 1] + p[i + 1][j + 1]) / 4.0;
        }
    }
}

void o2f(void) {
    // FILE *uf, *vf, *pf;
    // uf = fopen("u.txt", "w");
    // if (uf == NULL) {
    //     printf("Error! Cannot open u file.\n");
    // }
    // else {
    //     fprintf(uf, "%d\n", NG);
    //     for (int i = 0; i < NG; i ++) {
    //         for (int j = 0; j < NG; j ++) {
    //             fprintf(uf, "%lf\n", ug[i][j]);
    //         }
    //     }
    //     fclose(uf);
    // }
    // vf = fopen("v.txt", "w");
    // if (vf == NULL) {
    //     printf("Error! Cannot open v file.\n");
    // }
    // else {
    //     fprintf(vf, "%d\n", NG);
    //     for (int i = 0; i < NG; i ++) {
    //         for (int j = 0; j < NG; j ++) {
    //             fprintf(vf, "%lf\n", vg[i][j]);
    //         }
    //     }
    //     fclose(vf);
    // }
    // pf = fopen("p.txt", "w");
    // if (pf == NULL) {
    //     printf("Error! Cannot open p file.\n");
    // }
    // else {
    //     fprintf(pf, "%d\n", NG);
    //     for (int i = 0; i < NG; i ++) {
    //         for (int j = 0; j < NG; j ++) {
    //             fprintf(pf, "%lf\n", pg[i][j]);
    //         }
    //     }
    //     fclose(pf);
    // }
    FILE *fo;
    char fname[128];
    sprintf(fname, "UVP_Re%d_t%d.csv", int(Re), int(ter));
    fo = fopen(fname, "w+t");

    if ( fo == NULL ) {
        printf("\nERROR when opening file\n");
    }
    else {
        fprintf( fo, "x,y,z,u,v,p\n");

        for (int j = 0 ; j < NG; j++ ) {
            for (int i = 0 ; i < NG; i++ ) {
                double xpos, ypos;
                xpos = i * dx;
                ypos = j * dy;

                fprintf(fo, "%5.8lf,%5.8lf,%5.8lf,%5.8lf,%5.8lf,%5.8lf\n", xpos, ypos, 0.0, ug[i][j], vg[i][j], pg[i][j]);
            }
        }
        fclose(fo);
    }
}

int main(int argc, char ** argv) {
    if (argc < 3) {
        printf("ns2d Re time\n");
        return 0;
    }
    Re = strtod(argv[1], NULL);
    ter = strtod(argv[2], NULL);
    dt = min(cfl * dd / 2.0, Re * dx * dx * dy * dy / (2 * (dx * dx + dy * dy)));
    printf("Re = %lf, T = %lf, dt = %lf\n", Re, ter, dt);

    char fname[128];
    sprintf(fname, "monitor_Re%d_t%d.csv", int(Re), int(ter));
    FILE* fmo = fopen(fname, "w+t");
    if (fmo == NULL) {
        printf("ERROR when opening file\n");
        return 0;
    }
    fprintf(fmo, "t,iter,maxdiv\n");

    init();
    ns2d(fmo);
    calcgrid();
    o2f();

    fclose(fmo);
    return 0;
}