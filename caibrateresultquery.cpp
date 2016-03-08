#include "caibrateresultquery.h"
#include "ui_caibrateresultquery.h"
#include <QDebug>
#include "global.h"

//extern QSettings mysettings;
#define MYSETTINGS_RESULT "calibrtation_results_in_result_"
#define MYSETTINGS_DATA "calibrtation_results_in_data_"

/*
The module will create QSETTINGS of "calibrate_result",the format is:
    calibrtation_results_in_result_:"datetime;work_curve;k=1 b=2 r=3;sample_number"
    calibrtation_results_in_data_:"S%;sample;ref|S%;sample;ref|..."

    We need to translate the string in order to use the data from upside formate!;
    The data store in mysettings file.
*/

caibrateresultquery::caibrateresultquery(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::caibrateresultquery)
{
  //printf("in...%s\n",__FUNCTION__);
  int tmp;
  ui->setupUi(this);
  //得到一个下一次标定会记录在哪的数，所以减一就是最新的数
  display_number = mysettings.value("calibratemeasurement_count_record").toInt() - 1;
  //mysettings.clear();
  if((!mysettings.contains("calibration_results_in_result_1"))\
     && (!mysettings.contains("calibration_results_in_data_10"))){
      for(tmp = 1;tmp <= 10;tmp++){
          //qDebug() << "calibrate_results";
          mysettings.setValue(QString("calibration_results_in_result_%1").arg(tmp)," ; ; ; ");
          mysettings.setValue(QString("calibration_results_in_data_%1").arg(tmp),"");
        }
    }
  update_page(display_number);

  //调整tablewidget的大小以适应屏幕。
  for(int i = 0; i < ui->tableWidget->columnCount();i++){
      ui->tableWidget->setColumnWidth(i,FONT_SIZE * 5);
    }
  ui->tableWidget->setRowHeight(0,FONT_SIZE * 2);
  ui->tableWidget->setRowHeight(1,FONT_SIZE * 2);
  ui->tableWidget->setRowHeight(2,FONT_SIZE * 2);
  //printf("%s\n",__FUNCTION__);

  this->setFont(QFont(FONT_NAME, FONT_SIZE ,QFont::Normal));
  QList<QLabel *> label_all = this->findChildren<QLabel *>();
  for(int i = 0;i <label_all.size();i++){
      label_all[i]->setFont(QFont(FONT_NAME, FONT_SIZE/5*4,QFont::Normal));
    }
  QList<QPushButton *> buttonlist = this->findChildren<QPushButton *>();
  for (int i = 0; i < buttonlist.count(); ++i) {
      buttonlist[i]->setFont(QFont(FONT_NAME, FONT_SIZE ,QFont::Normal));
    }
  ui->tableWidget->setFont(QFont(FONT_NAME, FONT_SIZE ,QFont::Normal));
  ui->label->setFont(QFont(FONT_NAME, FONT_SIZE * 2 ,QFont::Normal));
}

caibrateresultquery::~caibrateresultquery()
{
  delete ui;
}

void caibrateresultquery::update_page(int count){
  //init
  ui->tableWidget->clearContents();

  QString tmp_result,tmp_data;
  QStringList tmp_result_list,tmp_data_list,tmp_data_list2;

  //result:
  tmp_result = mysettings.value(QString("calibration_results_in_result_%1").arg(count)).toString();
  tmp_result_list = tmp_result.split(";");
  if(tmp_result_list.size() != 4) return;
  ui->label_datetime->setText(QString("标定测量时间：") + tmp_result_list[0]);
  ui->label_work_curve->setText(tmp_result_list[1]);
  ui->label_kbr->setText(QString("标定结果：") + tmp_result_list[2]);
  ui->label_number->setText(tmp_result_list[3]);

  //data:tablewidget
  tmp_data = mysettings.value(QString("calibration_results_in_data_%1").arg(count)).toString();
  tmp_data_list = tmp_data.split(";");
  //判断数据是否会出错，即此标定数据的个数与此标定结果的相应数据是否对应
#if 0
  if(tmp_data_list.size() != tmp_result_list[3].toInt())
    return;
#endif
  int i,j;

  for(i = 0;i < tmp_data_list.size()  ;i++){
      tmp_data_list2 = tmp_data_list[i].split("/");
      if(tmp_data_list2.size() != 3) continue;
      for(j = 0 ; j < 3 ; j++){
          QTableWidgetItem *item = new QTableWidgetItem(tmp_data_list2[j]);
          item->setTextAlignment(Qt::AlignCenter);
          ui->tableWidget->setItem(j,i,item);
        }
    }
}

void caibrateresultquery::show_and_update(){
  display_number = mysettings.value("calibratemeasurement_count_record").toInt() - 1;
  update_page(display_number);
  this->showFullScreen();
}
void caibrateresultquery::on_pushButton_clicked()
{
  //return button;
  display_number = mysettings.value("calibratemeasurement_count_record").toInt() - 1;
  update_page(display_number);
  this->close();
}

void caibrateresultquery::on_pushButton_2_clicked()
{
  //next button;
  if(display_number <= 1)
    display_number = CALIBRATE_RESULT_MAX_RECORD;
  //display_number = mysettings.value("calibratemeasurement_count_record").toInt() - 1;
  update_page(--display_number);
}
