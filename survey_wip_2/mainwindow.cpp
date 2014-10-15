#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QMessageBox>
#include <QSqlTableModel>
#include <QSqlRelationalTableModel>
#include <QMap>
#include <QFileDialog>
#include <QSignalMapper>
#include <QMap>
#include <algorithm>
#include <memory>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)    
  
{
    setupModel();
    ui->setupUi(this);

    QSqlTableModel *relModel = model->relationModel(id_marks);
    ui->comboBox_mark->setModel(relModel);
    ui->comboBox_mark->setModelColumn(relModel->fieldIndex("mark"));
    mapper = new QDataWidgetMapper(this);
    mapper->setModel(model);
    mapper->addMapping(ui->lineEdit_FIO, model->fieldIndex("FIO"));
    mapper->addMapping(ui->lineEdit_pol, model->fieldIndex("pol"));
    mapper->addMapping(ui->lineEdit_kur, model->fieldIndex("kurenie"));
    mapper->addMapping(ui->lineEdit_dr, model->fieldIndex("dr"));
    mapper->addMapping(ui->comboBox_mark, id_marks);
    mapper->toFirst();

    connect(ui->pushButton_first, SIGNAL(clicked()),
            mapper, SLOT(toFirst()));
    connect(ui->pushButton_next, SIGNAL(clicked()),
            mapper, SLOT(toNext()));
    connect(ui->pushButton_prev, SIGNAL(clicked()),
            mapper, SLOT(toPrevious()));
    connect(ui->pushButton_last, SIGNAL(clicked()),
            mapper, SLOT(toLast()));


    QTableView *m = new QTableView;
    m->setModel(model);
    m->show();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_write_clicked()
{
    int rowCnt = model->rowCount();
    model->insertRow(rowCnt);
    rowCnt = rowCnt+1;
    QString writeId_str = "%1";
    QString writeId;
    writeId = writeId_str.arg(rowCnt);

    QString writeFio = ui->lineEdit_FIO_2->text();
    QString writedr = ui->lineEdit_dr_2->text();
    QString writekur = ui->lineEdit_kur_2->text();
    QString writepol = ui->lineEdit_pol_2->text();
    QString writemark = ui->comboBox_mark->currentText();

    QModelIndex idx1 = model->index(rowCnt-1, 0);
    QModelIndex idx2 = model->index(rowCnt-1, 1);
    QModelIndex idx3 = model->index(rowCnt-1, 2);
    QModelIndex idx4 = model->index(rowCnt-1, 3);
    QModelIndex idx5 = model->index(rowCnt-1, 4);
    QModelIndex idx6 = model->index(rowCnt-1, 5);


    model->setData(idx1, writeId);
    model->setData(idx2, writeFio);
    model->setData(idx3, writedr);
    model->setData(idx4, writepol);
    model->setData(idx5, writekur);
    model->setData(idx6, writemark);
  
    model->submitAll();
}

void MainWindow::setupModel()
{
    QSqlDatabase db1 = QSqlDatabase::addDatabase("QSQLITE");
    db1.setDatabaseName("survey.sqlite3");
    if (!db1.open()) {
        QMessageBox::critical(0, tr("Cannot open database"),
            tr("Unable to establish a database connection.\n"
               "This example needs SQLite support. Please read "
               "the Qt SQL driver documentation for information how "
               "to build it."), QMessageBox::Cancel);
        return;
    }

    QSqlQuery query_names, query_marks;
    query_names.exec("create table zxc (id int primary key, "
                  "FIO varchar(20), pol varchar(20), dr varchar(20), kurenie varchar(20), id2 int)");
    query_marks.exec("create table asd (mark varchar(20), "
                " id2 int primary key,kol int, nikotin int)");
    qDebug()<<query_names.lastError();

    model = new QSqlRelationalTableModel(this);
    model->setTable("zxc");
    id_names = model->fieldIndex("id");
    id_marks = model->fieldIndex("id2");
    qDebug()<<id_names;
    model->setRelation(id_marks,
                       QSqlRelation("asd", "id2", "mark"));
    model->select();

    model->setEditStrategy(QSqlTableModel::OnManualSubmit);

}

void MainWindow::on_pushButton_obzor_clicked()
{
    QFileDialog dg;
    dg.setFileMode(QFileDialog::ExistingFile);
    QStringList Qs;
    if(dg.exec() != QDialog::Accepted) {
        return;
    }
    Qs  = dg.selectedFiles();
    QString str_path = Qs.at(0);
    ui->lineEdit_path->setText(str_path);
    QSqlDatabase db = QSqlDatabase::addDatabase ("QSQLITE","connection2");
    db.setDatabaseName(ui->lineEdit_path->text());
    if (!db.open()){qDebug()<<"222";}
    QSqlQuery* qs_ = new QSqlQuery(db);
    qs_->exec("select * from zxc");
    model_imp = new QSqlRelationalTableModel(0,db);
    model_imp->setTable("zxc");
    int id_names_imp = model_imp->fieldIndex("id");
    int id_marks_imp = model_imp->fieldIndex("id2");
    qDebug()<<id_names_imp;
    model_imp->setRelation(id_marks_imp,
           QSqlRelation("asd", "id2", "mark"));
    model_imp->select();
    ui->tableView_imp->setModel(model_imp);
}


struct Cigarette
{
    int id;
    QString name;
    int quantity;
    int nicotine;
};

typedef std::shared_ptr<Cigarette> CigarettePtr;

struct Survey
{
    QString fio;
    QString gender;
    QString birthDate;
    QString smoking;
    int cigaretteId;
    CigarettePtr cigarette;
};


class import {
private:

    typedef QMap<int, CigarettePtr> Cigarette2Map;
    Cigarette2Map cigarette2Map;
    typedef QVector<Survey> Surveys2;
    Surveys2 surveys2;
    typedef QMap<QString, int> Cigarette1Map;
    Cigarette1Map cigarette1Map;


public:
    void cig2mapfilling(QSqlRelationalTableModel *modeltemp)
    {
        for (int i=0; i<=modeltemp->rowCount(); i++)
        {
            QModelIndex idx1 = modeltemp->index(i, 0);
            QModelIndex idx2 = modeltemp->index(i, 1);
            QModelIndex idx3 = modeltemp->index(i, 2);
            QModelIndex idx4 = modeltemp->index(i, 3);
            
            
            CigarettePtr cig(new Cigarette);
            cig->id = modeltemp->data(idx1).toInt();
            cig->name = modeltemp->data(idx2).toString();
            cig->nicotine = modeltemp->data(idx3).toInt();
            cig->quantity = modeltemp->data(idx4).toInt();
            
            cigarette2Map[cig->id] = cig;
        }
    }

    void surveys2filling(QSqlRelationalTableModel *modeltemp)
    {
        
        //        QString surveys2Query = "SELECT FIO,id,pol,dr,kurenie,id2 FROM zxc";
        //        QSqlQuery q(surveys2Query, dbtemp);
        
        for (int i=0; i<=modeltemp->rowCount(); i++)
        {

            QModelIndex idx1 = modeltemp->index(i, 1);
            QModelIndex idx2 = modeltemp->index(i, 2);
            QModelIndex idx3 = modeltemp->index(i, 3);
            QModelIndex idx4 = modeltemp->index(i, 4);
            QModelIndex idx5 = modeltemp->index(i, 5);
            
            Survey s;
            s.fio = modeltemp->data(idx1).toString();
            s.gender = modeltemp->data(idx2).toString();
            s.birthDate = modeltemp->data(idx3).toString();
            s.smoking = modeltemp->data(idx4).toString();
            s.cigaretteId = modeltemp->data(idx5).toInt();
            
            Cigarette2Map::const_iterator it = cigarette2Map.find(s.cigaretteId);
            s.cigarette = it!=cigarette2Map.cend()? *it: 0;
            
            surveys2.push_back(s);
        }
    }
    
    void cig1mapfilling(QSqlRelationalTableModel *modeltemp)
    {

        //        QString cigarette1Query = "SELECT id2, mark FROM asd";
        //        QSqlQuery q(cigarette1Query, dbtemp);

        for (int i=0; i<=modeltemp->rowCount(); i++)
        {
            QModelIndex idx1 = modeltemp->index(i, 0);
            QModelIndex idx2 = modeltemp->index(i, 1);

            QString name = modeltemp->data(idx1).toString();
            int cigId = modeltemp->data(idx2).toInt();
            cigarette1Map[name] = cigId;
        }
    }



    void marksimporting(QSqlRelationalTableModel *modeltemp)
    {

        QVariantList ids;
        QVariantList names;
        QVariantList quantities;
        QVariantList nicotines;

        int unusedId = *std::max_element(cigarette1Map.cbegin(), cigarette1Map.cend());
        ++unusedId;

        for (auto cig2It=cigarette2Map.begin(); cig2It!=cigarette2Map.end(); ++cig2It) {
            CigarettePtr cig = cig2It.value();

            Cigarette1Map::iterator cig1It = cigarette1Map.find(cig->name);
            if (cig1It!=cigarette1Map.end()) {
                cig->id = cig1It.value();
            }
            else {
                cig->id = unusedId++;

                ids.push_back(cig->id);
                names.push_back(cig->name);
                quantities.push_back(cig->quantity);
                nicotines.push_back(cig->nicotine);
            }
        }

        int rowCnt = modeltemp->rowCount();
        QModelIndex idx1 = modeltemp->index(rowCnt, 0);
        QModelIndex idx2 = modeltemp->index(rowCnt, 1);
        QModelIndex idx3 = modeltemp->index(rowCnt, 2);
        QModelIndex idx4 = modeltemp->index(rowCnt, 3);


        modeltemp->setData(idx1, ids);
        modeltemp->setData(idx2, names);
        modeltemp->setData(idx3, quantities);
        modeltemp->setData(idx4, nicotines);

        modeltemp->submitAll();


        //        QString cig1InsertQuery = "INSERT INTO asd(id2,mark,kol,nikotin) VALUES(?,?,?,?)";
        //        QSqlQuery q(cig1InsertQuery,dbtemp);
        //        q.prepare(cig1InsertQuery);
        //        q.addBindValue(ids);
        //        q.addBindValue(names);
        //        q.addBindValue(quantities);
        //        q.addBindValue(nicotines);

        //        qDebug() << "cig1InsertQuery:" << q.execBatch();

    }

    void FIOimporting(QSqlRelationalTableModel *modeltemp)
    {
        int unusedidtemp = modeltemp->rowCount();

        QVariantList ids;
        QVariantList fios;
        QVariantList genders;
        QVariantList birthDates;
        QVariantList smokings;
        QVariantList cigaretteIds;

        for (auto it=surveys2.cbegin(); it!=surveys2.cend(); ++it) {
            const Survey &survey = *it;
            ids.push_back(unusedidtemp++);
            fios.push_back(survey.fio);
            genders.push_back(survey.gender);
            birthDates.push_back(survey.birthDate);
            smokings.push_back(survey.smoking);
            cigaretteIds.push_back(survey.cigaretteId);
        }
        int rowCnt = modeltemp->rowCount();
        QModelIndex idx1 = modeltemp->index(rowCnt, 0);
        QModelIndex idx2 = modeltemp->index(rowCnt, 1);
        QModelIndex idx3 = modeltemp->index(rowCnt, 2);
        QModelIndex idx4 = modeltemp->index(rowCnt, 3);
        QModelIndex idx5 = modeltemp->index(rowCnt, 4);
        QModelIndex idx6 = modeltemp->index(rowCnt, 5);

        modeltemp->setData(idx1, ids);
        modeltemp->setData(idx2, fios);
        modeltemp->setData(idx3, genders);
        modeltemp->setData(idx4, birthDates);
        modeltemp->setData(idx5, smokings);
        modeltemp->setData(idx6, cigaretteIds);

        modeltemp->submitAll();

        //        QString survey1InsertQuery = "INSERT INTO zxc(id, FIO, pol, dr, kurenie, id2) VALUES(?,?,?,?,?,?)";
        //        QSqlQuery q(survey1InsertQuery,dbtemp);
        //        q.prepare(survey1InsertQuery);
        //        q.addBindValue(ids);
        //        q.addBindValue(fios);
        //        q.addBindValue(genders);
        //        q.addBindValue(birthDates);
        //        q.addBindValue(smokings);
        //        q.addBindValue(cigaretteIds);

        //        qDebug() << "survey1InsertQuery:" << q.execBatch();
        //        qDebug() << q.lastError();
    }
};

void MainWindow::on_pushButton_startimport_clicked()
{
    import importstart;

    importstart.cig2mapfilling(model_imp);
    importstart.surveys2filling(model_imp);
    importstart.cig1mapfilling(model);
    importstart.marksimporting(model);
    importstart.FIOimporting(model);
}

