#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    solPlotted = false;
    diffPlotted = false;

    ui->setupUi(this);
    this->setWindowTitle("КСР | Зыков Александр");
    this->setWindowFlags(Qt::Window | Qt:: WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);

    // Первая вкладка
    QWidget *tab1 = new QWidget(this);
    QGridLayout *layout1 = new QGridLayout(this);

    layout1->addWidget(new QLabel("Левая граница интегрирования:"), 0, 0);
    minX = new QLineEdit(this);
    layout1->addWidget(minX, 0, 1, 1, 2);

    layout1->addWidget(new QLabel("Правая граница интегрирования:"), 1, 0);
    maxX = new QLineEdit();
    layout1->addWidget(maxX, 1, 1, 1, 2);

    layout1->addWidget(new QLabel("Начальный шаг:"), 2, 0);
    initStep = new QLineEdit();
    layout1->addWidget(initStep, 2, 1, 1, 2);

    layout1->addWidget(new QLabel("Максимальное количество шагов:"), 3, 0);
    maxIters = new QLineEdit();
    layout1->addWidget(maxIters, 3, 1, 1, 2);

    layout1->addWidget(new QLabel("Точность выхода на правую границу:"), 4, 0);
    borderTol = new QLineEdit();
    layout1->addWidget(borderTol, 4, 1, 1, 2);

    layout1->addWidget(new QLabel("Параметр контроля локальной погрешности:"), 5, 0);
    stepTol = new QLineEdit();
    layout1->addWidget(stepTol, 5, 1, 1, 2);

    layout1->addWidget(new QLabel("u0:"), 6, 0);
    u0 = new QLineEdit();
    layout1->addWidget(u0, 6, 1, 1, 2);

    layout1->addWidget(new QLabel("a:"), 7, 0);
    a = new QLineEdit();
    layout1->addWidget(a, 7, 1, 1, 2);

    layout1->addWidget(new QLabel("k:"), 8, 0);
    k = new QLineEdit();
    layout1->addWidget(k, 8, 1, 1, 2);

    withComp = new QCheckBox("Альтернативные графики");
    layout1->addWidget(withComp, 9, 1);

    startBtn = new QPushButton();
    startBtn->setText("Начать расчёт");
    layout1->addWidget(startBtn, 9, 0);
    tab1->setLayout(layout1);
    connect(startBtn, &QPushButton::clicked, this, &MainWindow::startCalculation);

    //Вторая вкладка
    QWidget *tab2 = new QWidget(this);
    QGridLayout *layout2 = new QGridLayout(this);

    table = new QTableWidget(this);
    table->setColumnCount(6);
    table->setRowCount(20);
    table->setHorizontalHeaderLabels(QStringList() << "xi" << "vi"
                                    << "|ОЛП|" << "hi" << "ui" << "|ui - vi|");
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    layout2->addWidget(table);
    tab2->setLayout(layout2);

    // Третья вкладка
    QWidget *tab3 = new QWidget(this);
    QGridLayout *layout3 = new QGridLayout(this);

    info = new QTextEdit();
    info->setReadOnly(true);

    info->setStyleSheet(
        "QTextEdit {"
        "   padding: 5px;"                /* Внутренние отступы */
        "   font-size: 12pt;"             /* Размер шрифта */
        "   line-height: 150%;"           /* Отступ между строками */
        "}"
        "QTextEdit::item {"
        "   border-bottom: 1px solid lightgray;" /* Разделительные линии */
        "}"
        );

    layout3->addWidget(info);
    tab3->setLayout(layout3);

    // Четвёртая вкладка
    QWidget *tab4 = new QWidget(this);
    QGridLayout *layout4 = new QGridLayout(this);

    graph = new QCustomPlot();

    layout4->addWidget(graph);
    tab4->setLayout(layout4);

    // Пятая вкладка
    QWidget *tab5 = new QWidget(this);
    QGridLayout *layout5 = new QGridLayout(this);

    comparison1 = new QCustomPlot();
    comparison2 = new QCustomPlot();
    comparison3 = new QCustomPlot();
    comparison4 = new QCustomPlot();

    layout5->addWidget(comparison1, 0, 0);
    layout5->addWidget(comparison2, 0, 1);
    layout5->addWidget(comparison3, 1, 0);
    layout5->addWidget(comparison4, 1, 1);
    tab5->setLayout(layout5);

    //Шестая вкладка
    QWidget *tab6 = new QWidget(this);
    QGridLayout *layout6 = new QGridLayout(this);

    comparison5 = new QCustomPlot();
    comparison6 = new QCustomPlot();

    layout6->addWidget(comparison5, 0, 0);
    layout6->addWidget(comparison6, 1, 0);

    tab6->setLayout(layout6);


    // Вкладки погрешностей
    QWidget *tab7 = new QWidget(this);
    QGridLayout *layout7 = new QGridLayout(this);
    comparison7 = new QCustomPlot();
    layout7->addWidget(comparison7, 0, 0);

    comparison8 = new QCustomPlot();
    layout7->addWidget(comparison8, 1, 0);

    tab7->setLayout(layout7);

    QTabWidget *tabWidget = new QTabWidget();
    // Добавляем вкладки
    tabWidget->addTab(tab1, "Начальные условия");
    tabWidget->addTab(tab2, "Таблица результатов");
    tabWidget->addTab(tab3, "Справка");
    tabWidget->addTab(tab4, "График решения");
    tabWidget->addTab(tab5, "Влияние параметров задачи");
    tabWidget->addTab(tab6, "Влияние параметров метода");
    tabWidget->addTab(tab7, "Погрешности");
    this->setCentralWidget(tabWidget);
}

bool MainWindow::validateInput() {
    bool ok;

    int nMax = maxIters->text().toInt(&ok);
    if (!ok || nMax <= 0) {
        QMessageBox::warning(this, "Некорректный ввод", "Введите положительное целое число максимального количества шагов.");
        return false;
    }

    double xMax = maxX->text().toDouble(&ok);
    double xMin = minX->text().toDouble(&ok);
    if (!ok) {
        QMessageBox::warning(this, "Некорректный ввод", "Введите числа для границ интегрирования.");
        return false;
    }
    if (xMax - xMin < 0) {
        QMessageBox::warning(this, "Некорректный ввод", "Левая граница интегрирования не может быть больше правой.");
        return false;
    }

    borderTol->text().toDouble(&ok);
    stepTol->text().toDouble(&ok);
    if (!ok) {
        QMessageBox::warning(this, "Некорректный ввод", "Введите числа для контроля погрешности и выхода на границу.");
        return false;
    }

    u0->text().toDouble(&ok);
    double aVal = a->text().toDouble(&ok);
    k->text().toDouble(&ok);
    initStep->text().toDouble(&ok);

    if(!ok) {
        QMessageBox::warning(this, "Некорректный ввод", "Введённые значения должны быть числами.");
        return false;
    }
    if(aVal <= 0) {
        QMessageBox::warning(this, "Некорректный ввод", "a должна быть положительной.");
        return false;
    }

    return true;
}

std::pair<double, double> MainWindow::intervalOfDiff(const std::vector<std::vector<double>>& obj) {

    double minDiff = 1000.0;
    double maxDiff = -1000.0;
    double tol = 1e-10;

    for (size_t i = 0; i < obj.size() - 1; i++) {
        if (obj[i][3]*2.0 - obj[i+1][3] < tol) {

            double attitude = obj[i+1][4]/obj[i][4];
            if (std::isnan(attitude) || obj[i][4] == 0.0 || obj[i+1][4] == 0.0) continue;

            if (minDiff > attitude) minDiff = attitude;
            if (maxDiff < attitude) maxDiff = attitude;
        }
    }

    return std::pair<double, double> {minDiff, maxDiff};
}

std::pair<double, double> MainWindow::setInfo(const std::vector<std::vector<double>>& obj, double border) {

    size_t step = 0;
    double maxE = 0;
    size_t stepe = 0;
    double maxe = 0;

    for(size_t i = 0; i < obj.size(); i++) {
        if (maxE < obj[i][5]) { maxE = obj[i][5]; step = i; }
        if (maxe < obj[i][4]) { maxe = obj[i][4]; stepe = i; }
    }

    std::pair<double, double> diffs = intervalOfDiff(obj);

    info->setText(QString("Результаты (шагов: %1):\nЧисленная траектория достигла точки (%2, %3), начиная с (%4, %5)\n")
                    .arg(obj.size()).arg(obj.back()[0]).arg(obj.back()[1]).arg(obj.front()[0]).arg(obj.front()[1]) +
                  QString("Траектория точного решения достигла точки (%1, %2), начиная с (%3, %4)\n")
                    .arg(obj.back()[0]).arg(obj.back()[2]).arg(obj.front()[0]).arg(obj.front()[2]) +
                  QString("Максимум модуля глобальной погрешности = %1, при x = %2, на шаге %3\n")
                    .arg(maxE).arg(obj[step][0]).arg(step) +
                  QString("Максимум модуля локальной погрешности = %1, при x = %2, на шаге %3\n")
                        .arg(maxe).arg(obj[stepe][0]).arg(stepe) +
                  QString("Разница между последней точкой траектории и правой границей = %1\n\n").arg(fabs(border - obj.back()[0])) +
                  QString("Проверка порядка метода\nОтношения значений локальных погрешностей при увеличении шага в два раза варьируются в пределах:\n(%1, %2)\n")
                        .arg(diffs.first).arg(diffs.second));

    return std::pair<double, double> {maxe, maxE};
}

std::pair<QCPRange, QCPRange> MainWindow::plotSol(const std::vector<std::vector<double>>& obj) {

    graph->clearPlottables();
    graph->clearGraphs();
    graph->clearItems();
    graph->clearMask();

    if (!solPlotted) {
        QCPTextElement *title = new QCPTextElement(graph, "Графики истинного и численного решений", QFont("Arial", 12, QFont::Bold));
        graph->plotLayout()->insertRow(0);  // Добавляем строку в макет сверху
        graph->plotLayout()->addElement(0, 0, title);  // Вставляем заголовок в верхнюю строку
        solPlotted = true;
    }

    QVector<double> x;
    QVector<double> v;
    QVector<double> u;

    for (size_t i = 0; i < obj.size(); i++) {
        x.push_back(obj[i][0]);
        v.push_back(obj[i][1]);
        u.push_back(obj[i][2]);
    }

    double offset = 1e-1;
    //double maxX = *std::max_element(x.begin(), x.end());
    double maxY = std::max(*std::max_element(v.begin(), v.end()), *std::max_element(u.begin(), u.end()));
    //double minX = *std::min_element(x.begin(), x.end());
    double minY = std::min(*std::min_element(v.begin(), v.end()), *std::min_element(u.begin(), u.end()));

    graph->addGraph();
    graph->graph(0)->setData(x, u);
    graph->graph(0)->setPen(QPen(Qt::red, 3));
    graph->graph(0)->setName("Истинное решение");
    graph->addGraph();
    graph->graph(1)->setData(x, v);
    QVector<qreal> dashes;
    dashes << 1 << 3;
    auto pen = QPen(Qt::blue, 2, Qt::DashLine);
    pen.setDashPattern(dashes);
    graph->graph(1)->setPen(pen);
    graph->graph(1)->setName("Численное решение");

    graph->xAxis->setLabel("Время (x)");
    graph->yAxis->setLabel("Температура (u(x))");

    graph->setBackgroundScaled(false);

    graph->yAxis->setRange(minY - offset, maxY + offset);
    graph->xAxis->rescale(true);

    graph->legend->setVisible(true);

    graph->replot();

    return std::pair<QCPRange, QCPRange> {graph->xAxis->range(), graph->yAxis->range()} ;
}

void MainWindow::plotDiff(const std::pair<QVector<double>, QVector<double>>& obj, QCustomPlot* dgraph,
                          const std::pair<QCPRange, QCPRange>& ranges, QString name, bool autoscale) {
    // type: true - with u, false - without
    int newId = dgraph->graphCount();

    dgraph->addGraph();
    dgraph->graph(newId)->setData(obj.first, obj.second);
    dgraph->graph(newId)->setPen(QPen(Qt::GlobalColor(newId+7), 2));
    dgraph->graph(newId)->setName(name);

    dgraph->xAxis->setLabel("Время (x)");
    dgraph->yAxis->setLabel("Температура (u(x))");

    if (!autoscale) {
        dgraph->xAxis->rescale(false);
        dgraph->yAxis->rescale(false);
        dgraph->xAxis->setRange(ranges.first);
        dgraph->yAxis->setRange(ranges.second);
    } else {
        dgraph->xAxis->rescale(true);
        dgraph->yAxis->rescale(true);
    }

    dgraph->legend->setVisible(true);
    dgraph->replot();
}

std::pair<QVector<double>, QVector<double>> MainWindow::getDataForPlot(const std::vector<std::vector<double>>& obj, bool type = false, int column = 1) {
    QVector<double> v;
    QVector<double> x;
    // true - u, false - v
    if (type) {
        for (size_t i = 0; i < obj.size(); i++) {
            v.push_back(obj[i][2]);
            x.push_back(obj[i][0]);
        }
    } else {
        for (size_t i = 0; i < obj.size(); i++) {
            v.push_back(obj[i][column]);
            x.push_back(obj[i][0]);
        }
    }
    return std::pair<QVector<double>, QVector<double>> {x, v};
}

void MainWindow::startCalculation() {
    table->clearContents();
    if(!validateInput()) return;

    int nMax = maxIters->text().toInt();
    double xMax = maxX->text().toDouble();
    double xMin = minX->text().toDouble();
    double bTVal = borderTol->text().toDouble();
    double sTVal = stepTol->text().toDouble();
    double u0val = u0->text().toDouble();
    double aVal = a->text().toDouble();
    double kVal = k->text().toDouble();
    double h0 = initStep->text().toDouble();

    std::vector<std::vector<double>> sol = getSolution(xMin, u0val, h0, aVal, kVal, xMax, bTVal, sTVal, nMax);

    // Заполнение таблицы
    QStringList currentRowData;
    table->clearContents();
    table->setRowCount(sol.size());

    for (size_t row = 0; row < sol.size(); ++row) {
        table->setVerticalHeaderItem(row, new QTableWidgetItem(QString::number(row)));
        // Заполняем ячейки
        currentRowData = {
            QString::number(sol.at(row)[0]), // xi
            QString::number(sol.at(row)[1]), // vi
            QString::number(sol.at(row)[4]), // olp
            QString::number(sol.at(row)[3]), // hi
            QString::number(sol.at(row)[2]), // ui
            QString::number(sol.at(row)[5]), // |ui-vi|
        };

        for (int col = 0; col < currentRowData.size(); col++) {
            table->setItem(row, col, new QTableWidgetItem(currentRowData.at(col)));
        }
    }
    auto maxs = setInfo(sol, xMax);
    std::pair<QCPRange, QCPRange> ranges = plotSol(sol);

    if (!solPlotted) {
        QCPTextElement *title7 = new QCPTextElement(comparison7, "Локальная погрешность", QFont("Arial", 10, QFont::Bold));
        comparison7->plotLayout()->insertRow(0);
        comparison7->plotLayout()->addElement(0, 0, title7);
        QCPTextElement *title8 = new QCPTextElement(comparison8, "Глобальная погрешность", QFont("Arial", 10, QFont::Bold));
        comparison8->plotLayout()->insertRow(0);
        comparison8->plotLayout()->addElement(0, 0, title8);
        solPlotted = true;
    }

    auto tmpe = getDataForPlot(sol, false, 4);

    comparison7->clearGraphs();
    comparison8->clearGraphs();
    comparison7->addGraph();
    comparison7->graph(0)->setData(tmpe.first, tmpe.second);
    comparison7->graph(0)->setPen(QPen(Qt::red, 2));
    comparison7->graph(0)->setName("Модуль оценки локальной погрешности");
    comparison7->yAxis->setNumberFormat("e");  // То же самое для оси Y

    comparison7->xAxis->setLabel("Время (x)");
    comparison7->yAxis->setLabel("|ОЛП|");
    comparison7->rescaleAxes();
    comparison7->legend->setVisible(true);

    comparison7->yAxis->setRange(0, maxs.first*10.0);
    comparison7->replot();

    comparison8->addGraph();
    comparison8->graph(0)->setData(tmpe.first, tmpe.second);
    comparison8->graph(0)->setPen(QPen(Qt::red, 2));
    comparison8->graph(0)->setName("Модуль глобальной погрешности");

    comparison8->xAxis->setLabel("Время (x)");
    comparison8->yAxis->setLabel("|ui - vi|");
    comparison8->rescaleAxes();
    comparison8->legend->setVisible(true);
    comparison8->yAxis->setNumberFormat("e");  // То же самое для оси Y

    comparison8->yAxis->setRange(0, maxs.second*10.0);
    comparison8->replot();


    if (withComp->isChecked()) {

        QCustomPlot* graphs[6] = {comparison1, comparison2, comparison3, comparison4, comparison5, comparison6};

        if (!diffPlotted) {
            QCPTextElement *title1 = new QCPTextElement(comparison1, "Сравнение траекторий при различных k", QFont("Arial", 10, QFont::Bold));
            comparison1->plotLayout()->insertRow(0);
            comparison1->plotLayout()->addElement(0, 0, title1);
            QCPTextElement *title2 = new QCPTextElement(comparison2, "Сравнение траекторий при различных a", QFont("Arial", 10, QFont::Bold));
            comparison2->plotLayout()->insertRow(0);
            comparison2->plotLayout()->addElement(0, 0, title2);
            QCPTextElement *title3 = new QCPTextElement(comparison3, "Сравнение траекторий при различных u0", QFont("Arial", 10, QFont::Bold));
            comparison3->plotLayout()->insertRow(0);
            comparison3->plotLayout()->addElement(0, 0, title3);
            QCPTextElement *title4 = new QCPTextElement(comparison4, "Сравнение траектории c вариантом №8", QFont("Arial", 10, QFont::Bold));
            comparison4->plotLayout()->insertRow(0);
            comparison4->plotLayout()->addElement(0, 0, title4);
            QCPTextElement *title5 = new QCPTextElement(comparison5, "Влияние значения контроля локальной погрешности", QFont("Arial", 10, QFont::Bold));
            comparison5->plotLayout()->insertRow(0);
            comparison5->plotLayout()->addElement(0, 0, title5);
            QCPTextElement *title6 = new QCPTextElement(comparison6, "Влияние начального шага", QFont("Arial", 10, QFont::Bold));
            comparison6->plotLayout()->insertRow(0);
            comparison6->plotLayout()->addElement(0, 0, title6);
            diffPlotted = true;
        } else {
            for (size_t i = 0; i < 6; i++) {
                graphs[i]->clearGraphs();
            }
        }

        auto primeSol = getDataForPlot(sol, false);

        for (size_t i = 0; i < 6; i++) {
            if (i == 4) { plotDiff(primeSol, graphs[i], ranges, "Изначальное решение (Eps = " + QString::number(sTVal) + ")", false); continue; }
            if (i == 5) { plotDiff(primeSol, graphs[i], ranges, "Изначальное решение (H0 = " + QString::number(h0) + ")", false); continue; };
            plotDiff(primeSol, graphs[i], ranges, "Изначальное решение", false);
        }

        std::vector<double> ks;
        std::vector<double> as;
        std::vector<double> u0s;
        double coefs[4] = {1.0/2.0, 1.0/4.0, 2.0, 4.0};
        for (int i = 0; i < 4; i++) {
            ks.push_back(kVal / coefs[i]);
            as.push_back(aVal / coefs[i]);
            u0s.push_back((u0val) / coefs[i]);
        }

        for (size_t i = 0; i < 4; i++) {
            auto tmpU = getDataForPlot(getSolution(xMin, u0s[i], h0, aVal, kVal, xMax, bTVal, sTVal, nMax));
            plotDiff(tmpU, graphs[2], ranges, "u0 = " + QString::number(u0s[i]), true);
            auto tmpK = getDataForPlot(getSolution(xMin, u0val, h0, aVal, ks[i], xMax, bTVal, sTVal, nMax));
            plotDiff(tmpK, graphs[0], ranges, "k = " + QString::number(ks[i]), true);
            auto tmpA = getDataForPlot(getSolution(xMin, u0val, h0, as[i], kVal, xMax, bTVal, sTVal, nMax));
            plotDiff(tmpA, graphs[1], ranges, "a = " + QString::number(as[i]), true);
        }

        auto altSol = getAltSol(xMin, u0val, h0, std::sqrt(aVal*aVal*kVal*kVal), aVal*kVal, std::sqrt(aVal*aVal*kVal*kVal)*aVal, xMax, bTVal, sTVal, nMax);
        plotDiff(altSol, graphs[3], ranges, "Вариант №8", true);

        double numCoefs[3] = {0.1/sTVal, 1.0/sTVal, sTVal/100.0};
        double hCoefs[3] = {1.0/h0, 0.1/h0, h0/100.0};
        for (size_t i = 0; i < 3; i++) {
            auto diffEps = getDataForPlot(getSolution(xMin, u0val, h0, aVal, kVal, xMax, bTVal, sTVal*numCoefs[i], nMax));
            auto diffL = getDataForPlot(getSolution(xMin, u0val, h0*hCoefs[i], aVal, kVal, xMax, bTVal, sTVal, nMax));
            plotDiff(diffEps, graphs[4], ranges, "Eps = " + QString::number(sTVal*numCoefs[i]), true);
            plotDiff(diffL, graphs[5], ranges, "H0 = " + QString::number(h0*hCoefs[i]), true);
        }

    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
