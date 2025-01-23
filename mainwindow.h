#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qtabwidget.h>
#include <QLabel>
#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QTableWidget>
#include <QHeaderView>
#include <qcustomplot.h>
#include <QTextEdit>
#include "rk4adaptive.h"
#include <vector>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    bool solPlotted;
    bool diffPlotted;

private slots:
    void startCalculation();
    bool validateInput();
    std::pair<double, double> setInfo(const std::vector<std::vector<double>>& obj, double border);
    std::pair<double, double> intervalOfDiff(const std::vector<std::vector<double>>& obj);
    std::pair<QCPRange, QCPRange> plotSol(const std::vector<std::vector<double>>& obj);
    void plotDiff(const std::pair<QVector<double>, QVector<double>>& obj, QCustomPlot* dgraph,
                  const std::pair<QCPRange, QCPRange>& ranges, QString name, bool autoscale);
    std::pair<QVector<double>, QVector<double>> getDataForPlot(const std::vector<std::vector<double>>& obj, bool type, int column);

private:
    Ui::MainWindow *ui;

    QLineEdit *maxX, *maxIters, *minX, *borderTol, *stepTol, *initStep, *u0, *a, *k;
    QPushButton *startBtn;
    QTableWidget *table;
    QCustomPlot *graph, *comparison1, *comparison2, *comparison3, *comparison4, *comparison5, *comparison6, *comparison7, *comparison8;
    QTextEdit *info;
    QCheckBox *withComp;

};
#endif // MAINWINDOW_H
