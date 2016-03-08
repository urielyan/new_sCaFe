#include "showcountingmeasurement.h"
#include "ui_showcountingmeasurement.h"
#include "global.h"
#include "issample.h"
#include "ispreheat.h"

#include <QByteArray>
#include <math.h>
//#include "countingmeasurement.h"

//QSettings count_data("shanghaikairen","count_data");

showcountingmeasurement::showcountingmeasurement(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::showcountingmeasurement)
{
  ui->setupUi(this);

  for(int i = 0;i<ui->tableWidget->rowCount();i++){
      for(int j = 0 ; j< ui->tableWidget->columnCount();j++){
          ui->tableWidget->setItem(i,j,new QTableWidgetItem());
        }
    }
  ui->tableWidget->setColumnWidth(0,this->size().width()/4);
  ui->tableWidget->setColumnWidth(1,this->size().width()/4);
  ui->tableWidget->setColumnWidth(2,this->size().width()/4);
  for(int i = 0;i < ui->tableWidget->rowCount();i++){
      ui->tableWidget->setRowHeight(i,this->size().height()/8);
    }
  this->setFont(QFont(FONT_NAME, FONT_SIZE ,QFont::Normal));
  QList<QLabel *> labellist = this->findChildren<QLabel *>();
  for (int i = 0; i < labellist.count(); ++i) {
      labellist[i]->setFont(QFont(FONT_NAME, FONT_SIZE ,QFont::Normal));
    }
  ui->tableWidget->setFont(QFont(FONT_NAME, FONT_SIZE ,QFont::Normal));
}

showcountingmeasurement::~showcountingmeasurement()
{
  delete ui;
}
extern int measurement_flag;
void showcountingmeasurement::adddata(int count,QString data){
#if 1
  QSettings count_data("shanghaikairen","count_data");
  data.remove(0,2);
  QString s_count = data.left(5);
  data.remove(0,5);
  QString ca_count = data.left(5);
  data.remove(0,5);
  QString fe_count = data.left(5);
  QTableWidgetItem *tmpitem1 = new QTableWidgetItem(tr("%1").arg(s_count));
  tmpitem1->setTextAlignment(Qt::AlignHCenter);
  ui->tableWidget->setItem(count - 1,0,tmpitem1);
  QTableWidgetItem *tmpitem2 = new QTableWidgetItem(tr("%1").arg(ca_count));
  tmpitem2->setTextAlignment(Qt::AlignHCenter);
  ui->tableWidget->setItem(count - 1,1,tmpitem2);
  QTableWidgetItem *tmpitem3 = new QTableWidgetItem(tr("%1").arg(fe_count));
  tmpitem3->setTextAlignment(Qt::AlignHCenter);
  ui->tableWidget->setItem(count - 1,2,tmpitem3);
  if(count == 11){
      double y_s = 0 , y_ca = 0 ,y_fe = 0;
      double sumsub_s = 0 ,sumsub_ca = 0,sumsub_fe = 0;
      long long sum_s = 0,sum_ca = 0,sum_fe = 0;
      int tmp_count = 0;
      tmp_count = count_data.value("count_count").toInt() + 1;
      if(tmp_count > COUNT_MEASUREMENT_MOST_STORAGE) tmp_count = 1;
      ui->label_data->setText(QDateTime::currentDateTime().toString("MM月dd日 hh:mm"));
      for(int i = 0;i < 11;i++){
          sum_s += ui->tableWidget->item(i,0)->text().toInt();
          sum_ca += ui->tableWidget->item(i,1)->text().toInt();
          sum_fe += ui->tableWidget->item(i,2)->text().toInt();
        }
 //s
      for(int i = 0;i < 10; i++){
          sumsub_s += pow(sum_s / 11 - ui->tableWidget->item(i,0)->text().toInt(),2);
        }
      y_s = pow((sumsub_s / (double)(sum_s/11)) * (double)(TURN_SECONDS/10),0.5);
      ui->label_s->setText(QString("SO3   计数平均数 = %1").arg(sum_s/11));
      ui->label_sy->setText("   γ= " + QString::number(y_s,'f',4));
//s

//ca
      for(int i = 0;i < 10; i++){
          sumsub_ca += pow(sum_ca / 11 - ui->tableWidget->item(i,1)->text().toInt(),2);
        }
      y_ca = pow((sumsub_ca / (double)(sum_ca/11)) * (double)(TURN_SECONDS/10),0.5);
      ui->label_ca->setText(QString("CaO   计数平均数 = %1").arg(sum_ca/11));
      ui->label_cay->setText("   γ= " + QString::number(y_ca,'f',4));
//ca

//fe
      for(int i = 0;i < 10; i++){
          sumsub_fe += pow(sum_fe / 11 - ui->tableWidget->item(i,2)->text().toInt(),2);
        }
      y_fe = pow((sumsub_fe / (double)(sum_fe/11)) * (double)(TURN_SECONDS/10),0.5);
      ui->label_fe->setText(QString("Fe2O3   计数平均数 = %1").arg(sum_fe/11));
      ui->label_fey->setText("   γ= " + QString::number(y_fe,'f',4));
//fe
      if(issample::global_is_sample == WAIT_BE_LOCATION || ui->widget_2->global_ispreheat >=  2){
          return;
        }
      count_data.setValue(QString("count_data_%1").arg(tmp_count),
                          QString::number(sum_s/11) + ";"+ QString::number(y_s,'f',4) + ";" +
                          QString::number(sum_ca/11) + ";"+ QString::number(y_ca,'f',4) + ";" +
                          QString::number(sum_fe/11) + ";"+ QString::number(y_fe,'f',4) + ";" +
                          QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss"));
      count_data.setValue("count_count",tmp_count);
    }
#endif
}
void showcountingmeasurement::clearall(){
  ui->tableWidget->clearContents();
  ui->label_data->clear();
  //ui->label_x->clear();
  ui->label_ca->clear();
  ui->label_cay->clear();
  ui->label_fe->clear();
  ui->label_fey->clear();
  ui->label_s->clear();
  ui->label_sy->clear();
}

//bool showcountingmeasurement::get_y(){
//  if(ui->label_sy->text() == NULL)return false;
//  if(ui->label_sy->text().toFloat() <= 0.55 || ui->label_sy->text().toFloat() >= 1.45 || \
//     ui->label_cay->text().toFloat() <= 0.55 || ui->label_cay->text().toFloat() >= 1.45 || \
//     ui->label_fey->text().toFloat() <= 0.55 || ui->label_fey->text().toFloat() >= 1.45)return false;

//}
float showcountingmeasurement::get_average_x()
{
//  if(ui->label_x->text() != NULL)
//    return ui->label_x->text().toFloat();
  return 0.0;
}
void showcountingmeasurement::on_pushButton_clicked()
{
  this->close();
}
