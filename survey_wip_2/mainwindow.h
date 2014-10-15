#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlRelationalTableModel>
#include <QDataWidgetMapper>
#include <QSqlQuery>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    QSqlRelationalTableModel *model, *model_imp_tw, *model_imp;
    ~MainWindow();

private slots:
    void on_pushButton_write_clicked();
    void on_pushButton_obzor_clicked();

    void on_pushButton_startimport_clicked();
    
private:
    Ui::MainWindow *ui;
    void setupModel();

    QDataWidgetMapper *mapper;
    int id_names,id_marks;

};

#endif // MAINWINDOW_H
