#pragma once
#include <QMainWindow>
#include <QResizeEvent>
#include <QEnterEvent>
#include <QEvent>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pausePlay_clicked();

    void on_loopButton_clicked();

    void onMenuActionTriggered(QAction *action);

private:
    Ui::MainWindow *ui;
};
