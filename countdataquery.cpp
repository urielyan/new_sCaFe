#include "countdataquery.h"
#include "ui_countdataquery.h"
#include "showcountingmeasurement.h"
#include "global.h"

#include <stdio.h>

//extern QSettings count_data;

countdataquery::countdataquery(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::countdataquery)
{
  ui->setupUi(this);
  QSettings count_data("shanghaikairen","count_data");
  if(!count_data.contains("count_count")){
      count_data.setValue("count_count",0);
    }
  display_count = count_data.value("count_count").toInt();

  for(int i = 0;i<ui->tableWidget->rowCount();i++){
      for(int j = 0 ; j< ui->tableWidget->columnCount();j++){
          ui->tableWidget->setItem(i,j,new QTableWidgetItem());
        }
    }

  for(int i = 0; i < 6; i++){
      ui->tableWidget->setColumnWidth(i,FONT_SIZE * 6);
    }
  ui->tableWidget->setColumnWidth(6,FONT_SIZE * 15);
  for(int i_2 = 0 ; i_2 < ui->tableWidget->rowCount();i_2++){
      ui->tableWidget->setRowHeight(i_2,this->height()/12);
    }
  ui->tableWidget->setFont(QFont("symbol",11,QFont::Normal));

  this->setFont(QFont(FONT_NAME, FONT_SIZE ,QFont::Normal));
  QList<QPushButton *> labellist = this->findChildren<QPushButton *>();
  for (int i = 0; i < labellist.count(); ++i) {
      labellist[i]->setFont(QFont(FONT_NAME, FONT_SIZE ,QFont::Normal));
    }
  ui->tableWidget->setFont(QFont(FONT_NAME, FONT_SIZE ,QFont::Normal));
  ui->label->setFont(QFont(FONT_NAME, FONT_SIZE * 2 ,QFont::Normal));
}

countdataquery::~countdataquery()
{
  delete ui;
}

void countdataquery::on_pushButton_clicked()
{
  QSettings count_data("shanghaikairen","count_data");
  display_count = count_data.value("count_count").toInt();

  int i;
  for(i = 0; i < 6 ;i++){
      QStringList datalist = count_data.value(QString("count_data_%1").arg(display_count)).toString().split(";");
      if(datalist.size()  != 7){
          break;
        }
      for(int k = 0 ; k < 6 ;k++){
          ui->tableWidget->item(i,k)->setText(datalist[k]);
          ui->tableWidget->item(i,k)->setTextAlignment(Qt::AlignCenter);
        }
      //      ui->tableWidget->item(i,0)->setText(datalist[0]);
      //      ui->tableWidget->item(i,1)->setText(datalist[1]);
      //      ui->tableWidget->item(i,2)->setText(datalist[2]);
      //      ui->tableWidget->item(i,0)->setTextAlignment(Qt::AlignCenter);
      //      ui->tableWidget->item(i,1)->setTextAlignment(Qt::AlignCenter);
      //      ui->tableWidget->item(i,2)->setTextAlignment(Qt::AlignCenter);
      display_count--;
    }
  ui->label_current->setText(QString("%1-%2").arg(display_count + i).arg(display_count + 1));
  this->close();
}

void countdataquery::on_pushButton_2_clicked()
{
  QSettings count_data("shanghaikairen","count_data");
  for(int i = 0; i < ui->tableWidget->rowCount() ;i++){
      for(int k = 0 ; k < ui->tableWidget->columnCount(); k ++){
          ui->tableWidget->item(i,k)->setText("");
        }
    }
  ui->label_current->setText("");
  int i;
  for(i= 0; i < 6 ;i++){
      if(display_count  <= 0) break;
      QStringList datalist = count_data.value(QString("count_data_%1").arg(display_count)).toString().split(";");
      if(datalist.size() != 7){
          display_count = count_data.value("count_count").toInt();
          return;
        }
      display_count--;
      for(int k = 0;k <ui->tableWidget->columnCount();k++){
          ui->tableWidget->item(i,k)->setText(datalist[k]);
          ui->tableWidget->item(i,k)->setTextAlignment(Qt::AlignCenter);
        }
//      ui->tableWidget->item(i,0)->setText(datalist[0]);
//      ui->tableWidget->item(i,1)->setText(datalist[1]);
//      ui->tableWidget->item(i,2)->setText(datalist[2]);
//      ui->tableWidget->item(i,0)->setTextAlignment(Qt::AlignCenter);
//      ui->tableWidget->item(i,1)->setTextAlignment(Qt::AlignCenter);
//      ui->tableWidget->item(i,2)->setTextAlignment(Qt::AlignCenter);
    }
  if(display_count <= 0){
      ui->label_current->setText(QString("%1-%2").arg(0).arg(display_count + i));
      display_count = count_data.value("count_count").toInt();
      return;
    }
  ui->label_current->setText(QString("%1-%2").arg(display_count + i).arg(display_count + 1));
}

void countdataquery::show_and_refresh(){
  QSettings count_data("shanghaikairen","count_data");
  display_count = count_data.value("count_count").toInt();
  on_pushButton_2_clicked();
  this->showFullScreen();
}
