#include "rk4adaptive.h"

double f(double v, double a, double k) {
    return -a*(v - k);
}

double fAlt(double u, double v, double q, double B) {
    return -(B/v)*u + q;
}

double fTrue(double x, double u0, double a, double k) {
    return (u0 - k)*exp(-a*x) + k;
}

std::pair<double, double> getNextRK4Alt(double Xn, double Vn, double Hn, double v, double q, double B, double (*func)(double, double, double, double)) {

    double k1 = func(Vn, v, q, B);
    double k2 = func(Vn + (Hn/2.0) * k1, v, q, B);
    double k3 = func(Vn + (Hn/2.0) * k2, v, q, B);
    double k4 = func(Vn + Hn*k3, v, q, B);

    double Xnext = Xn + Hn;
    double Vnext = Vn + (Hn/6.0) * (k1 + 2.0*k2 + 2.0*k3 + k4);

    return std::pair<double, double> {Xnext, Vnext};
}


std::pair<double, double> getNextRK4(double Xn, double Vn, double Hn, double a, double k, double (*func)(double, double, double)) {

    double k1 = func(Vn, a, k);
    double k2 = func(Vn + (Hn/2.0) * k1, a, k);
    double k3 = func(Vn + (Hn/2.0) * k2, a, k);
    double k4 = func(Vn + Hn*k3, a, k);

    double Xnext = Xn + Hn;
    double Vnext = Vn + (Hn/6.0) * (k1 + 2.0*k2 + 2.0*k3 + k4);

    return std::pair<double, double> {Xnext, Vnext};
}

std::pair<QVector<double>, QVector<double>> getAltSol(double x0, double v0, double h0, double v,
                                                      double q, double B, double border, double borderTol, double stepTol, int maxIters) {

    QVector<double> xVals = {x0};
    QVector<double> vVals = {v0};

    double curH = h0;
    double curX = x0;
    double curV = v0;
    int iters = 0;

    while (fabs(curX - border) > borderTol && iters < maxIters) {

        // Один шаг величины h
        std::pair<double, double> resFS = getNextRK4Alt(curX, curV, curH, v, q, B, fAlt);
        // Два шага величины h/2
        std::pair<double, double> resHSfirst = getNextRK4Alt(curX, curV, curH/2.0, v, q, B, fAlt);
        std::pair<double, double> resHSsecond = getNextRK4Alt(curX + curH/2.0, resHSfirst.second, curH/2.0, v, q, B, fAlt);

        double S = (resHSsecond.second - resFS.second) / (pow(2.0, 4.0) - 1.0);
        double leftB = stepTol/pow(2.0, 5.0);
        double absS = fabs(S);

        if (leftB <= absS && absS <= stepTol) {
            if (resFS.first > border) break;

            curX = resFS.first;
            curV = resFS.second;

            xVals.push_back(curX);
            vVals.push_back(curV);
            iters++;
            continue;
        }

        if (absS < leftB) {
            if (resFS.first > border) break;

            curX = resFS.first;
            curV = resFS.second;
            curH = curH*2.0;

            xVals.push_back(curX);
            vVals.push_back(curV);
            iters++;
            continue;
        }

        if (absS > stepTol) {
            curH = curH/2.0;
            continue;
        }

    }

    return std::pair<QVector<double>, QVector<double>> {xVals, vVals};
}

std::vector<std::vector<double>> getSolution(double x0, double v0, double h0, double a,
                                             double k, double border, double borderTol, double stepTol, int maxIters)
{

    std::vector<std::vector<double>> data;

    double curX = x0;
    double curV = v0;
    double curU = v0;
    double curH = h0;
    double cure = 0.0;
    double curE = 0.0;

    int iters = 0;
    data.push_back({curX, curV, curU, 0.0, cure, curE});

    while (fabs(curX - border) > borderTol && iters < maxIters) {

        if (curX + curH > border + borderTol && curX < border - borderTol) {
            curH = border - curX - borderTol;
        }

        // Один шаг величины h
        std::pair<double, double> resFS = getNextRK4(curX, curV, curH, a, k, f);
        // Два шага величины h/2
        std::pair<double, double> resHSfirst = getNextRK4(curX, curV, curH/2.0, a, k, f);
        std::pair<double, double> resHSsecond = getNextRK4(curX + curH/2.0, resHSfirst.second, curH/2.0, a, k, f);

        double S = (resHSsecond.second - resFS.second) / (pow(2.0, 4.0) - 1.0);
        double leftB = stepTol/pow(2.0, 5.0);
        double absS = fabs(S);
        double eNext = fabs(pow(2.0, 4.0) * S);

        double trueSol = fTrue(resFS.first, v0, a, k);
        double ENext = fabs(trueSol - resFS.second);

        if (leftB <= absS && absS <= stepTol) {
            if (resFS.first > border) break;

            curX = resFS.first;
            curV = resFS.second;
            cure = eNext;

            curU = trueSol;
            curE = ENext;

            data.push_back({curX, curV, curU, curH, cure, curE});
            iters++;
            continue;
        }

        if (absS < leftB) {
            if (resFS.first > border) break;

            curX = resFS.first;
            curV = resFS.second;
            cure = eNext;

            curU = trueSol;
            curE = ENext;

            data.push_back({curX, curV, curU, curH, cure, curE});
            curH = curH*2.0;
            iters++;
            continue;
        }

        if (absS > stepTol) {
            curH = curH/2.0;
            continue;
        }

    }

    return data;
}
