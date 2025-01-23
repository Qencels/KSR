#ifndef RK4ADAPTIVE_H
#define RK4ADAPTIVE_H
#include <utility>
#include <cmath>
#include <vector>
#include <QVector>

double f(double v, double a, double k);
double trueF(double x, double v, double a, double k);
std::pair<double, double> getNextRK4(double Xn, double Vn, double Hn, double (*func)(double, double, double));
std::vector<std::vector<double>> getSolution(double x0, double v0, double h0, double a,
                                             double k, double border, double borderTol, double stepTol, int maxIters);
double fAlt(double u, double v, double q, double B);
std::pair<double, double> getNextRK4Alt(double Xn, double Vn, double Hn, double v, double q, double B, double (*func)(double, double, double, double));
std::pair<QVector<double>, QVector<double>> getAltSol(double x0, double v0, double h0, double v,
                                                      double q, double B, double border, double borderTol, double stepTol, int maxIters);

#endif // RK4ADAPTIVE_H
