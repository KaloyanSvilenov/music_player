#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "treewidgetwindow.h"

#include <QMainWindow>
#include <QResizeEvent>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    TreeWidgetWindow *m_treeWindowManager;
};

#endif // MAINWINDOW_H
