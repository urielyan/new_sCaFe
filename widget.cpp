﻿#include "widget.h"
#include "ui_widget.h"
#include <QDate>
#include<QTime>
#include<QTimer>
#include<QDebug>
#include <QMessageBox>
#include "global.h"
#include "com.h"

/*区分现在是在做什么:
 * 每次测量之后都要把measurement_flag清零
 *  measurement_flag = 0 :不做任何事
 *  measurement_flag = 1 :能谱测量
 *  measurement_flag = 2 :计数测量
 *  measurement_flag = 3 :标定测量
 *  measurement_flag = 4 :含量测量
 *  measurement_flag = 5 :10分钟自动进行的计数参考样测量
 *  measurement_flag = 6 :循环计数测量
*/
int measurement_flag = 0;

Widget::Widget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::Widget)
{
  ui->setupUi(this);

  com *tmp_com= new com(this);
  tmp_com->transmit(0,0);

  cm = new countingMeasurement();
  cm_auto = new countingMeasurement();
  sm = new spectrummeasurement();
  dq = new dataquery();
  samplem = new sampleMeasurement();
  pw = new passwd();
  inspectoscope_measurement = new inspectoscope();
  //ui->b4->hide();
  //ui->b1->setDisabled(true);
  QTimer *timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(update_count_5_data()));
  timer->start(1000 * 2);

  QTimer *timer_auto_count_measurement = new QTimer(this);
  connect(timer_auto_count_measurement, SIGNAL(timeout()), this, SLOT(auto_start_count_measument()));
  timer_auto_count_measurement->start(1000);

  //当按其他测量时自动的计数测量会停止。通过下面两个连接一级一级的吧信号传到cm_auto对象中。
  connect(samplem,SIGNAL(transmit_stop_auto_count()),cm_auto,SLOT(stop_measurement_no_transmit()));
  connect(pw,SIGNAL(transmit_stop_auto_count()),cm_auto,SLOT(stop_measurement_no_transmit()));
  connect(pw,SIGNAL(transmit_stop_auto_count()),cm,SLOT(stop_measurement_no_transmit()));
  connect(this,SIGNAL(transmit_stop_auto_count()),cm,SLOT(stop_measurement_no_transmit()));
  connect(this,SIGNAL(transmit_stop_auto_count()),cm_auto,SLOT(stop_measurement_no_transmit()));

  //当按其他测量时自动的稳峰测量会停止。通过下面两个连接一级一级的吧信号传到cm_auto对象中。
  connect(this,SIGNAL(transmit_stop_auto_count()),sm,SLOT(stop_measurement_no_transmit()));
  connect(pw,SIGNAL(transmit_stop_auto_count()),sm,SLOT(stop_measurement_no_transmit()));
  connect(samplem,SIGNAL(transmit_stop_auto_count()),sm,SLOT(stop_measurement_no_transmit()));

  QTimer *timer_get_pos = new QTimer(this);
  connect(timer_get_pos, SIGNAL(timeout()), this, SLOT(get_slide_current_position()));
  timer_get_pos->start(1000);

  //开始一个含量测量和移动滑板会使稳峰测量停止
  connect(samplem,SIGNAL(transmit_stop_auto_count()),pw,SIGNAL(transmit_stop_auto_count()));
  connect(this,SIGNAL(transmit_stop_auto_count()),pw,SIGNAL(transmit_stop_auto_count()));

  //在计数和能谱测量下按移动滑板会使主菜单下的移动滑板按钮改变状态
  connect(pw,SIGNAL(change_widget_sliding(bool,bool)),this,SLOT(set_sliding_disabled(bool,bool)));
  //在主菜单下按移动滑板会使计数和能谱测量下的移动滑板按钮改变状态
  connect(this,SIGNAL(change_sliding_disabled(bool,bool)),pw,SIGNAL(change_sliding_disabled(bool,bool)));

  //当一个自动计数测量结束时开始一个测量峰位的能谱测量，下面是连接两个类中的信号和槽的
  connect(cm_auto,SIGNAL(start_steady_summit_measurement()),sm,SLOT(start_steady_summit()));
  connect(sm,SIGNAL(transmit_steady_summit_result(double)),\
          this,SLOT(judge_spectrument_measurement_result(double)));

  if(!mysettings.contains("count_voltage")){
      mysettings.setValue("count_voltage",578);
    }
  this->setFont(QFont(FONT_NAME, FONT_SIZE ,QFont::Normal));
  QList<QLabel *> labellist = this->findChildren<QLabel *>();
  for (int i = 0; i < labellist.count(); ++i) {
      labellist[i]->setFont(QFont(FONT_NAME, FONT_SIZE ,QFont::Normal));
    }
  ui->label->setFont(QFont("symbol.ttf", FONT_SIZE*2,QFont::Normal));
}

Widget::~Widget()
{
  delete inspectoscope_measurement;
  delete pw;
  delete samplem;
  delete dq;
  delete sm;
  delete cm_auto;
  delete cm;
  delete ui;
}

/*计数测量的自动模块：
 *  当10 seconds没有任何测量，则自动进行计数测量，不显示计数测量页面
 *  通过measuremect_flag 这个全局变量判断是否有操作
 */
void Widget::auto_start_count_measument(){
  if(ui->widget_ispreheat->global_ispreheat > 0){
      //
      return;
    }
  if(ui->widget_ispreheat->is_sampling_num > 0){
      return;
    }
  static int no_behaviour_count_second;
  if(measurement_flag == MEASUREMENT_NOTHING){
      no_behaviour_count_second++;
    }else{
      no_behaviour_count_second = 0;
    }

  if(no_behaviour_count_second > NO_BEHAVIOUR_START_COUNT){
      //cm_auto->showFullScreen();
      no_behaviour_count_second = 0;
      //若待测样在测量位置，则不测量
      if(ui->widget_issample->global_is_sample == WAIT_BE_LOCATION){
          no_behaviour_count_second = NO_BEHAVIOUR_START_COUNT/2;
          return;
        }

      //开始测量
      if(measurement_flag != MEASUREMENT_NOTHING)return;
      cm_auto->on_pushButton_clicked();
      measurement_flag = MEASUREMENT_10_AUTO;
    }
}

void Widget::update_count_5_data(){
  if(measurement_flag != MEASUREMENT_10_AUTO &&  measurement_flag != MEASUREMENT_NOTHING){
     ispreheat::is_sampling_num = 0;
      return;
    }
  cm->examine_count_5_data_availability();
}
void Widget::on_b1_clicked()
{
  //sample measurement.
  samplem->showFullScreen();
}

static int move_sliding_count = 0;
static QDateTime last_move_sliding_datetime = QDateTime::currentDateTime();
void Widget::on_b4_clicked()
{
  cm_auto->on_pushButton_2_clicked();
  cm_auto->showFullScreen();
}

int Widget::on_b6_clicked()
{
  sm->showFullScreen();
}

void Widget::set_sliding_disabled(bool out, bool in){
  ui->b6->setDisabled(out);
  ui->b4->setDisabled(in);
}

void Widget::on_b2_clicked()
{
  dq->showFullScreen();
}

void Widget::on_b3_clicked()
{
  pw->showpass();
}

void Widget::get_slide_current_position(){
    if(measurement_flag == MEASUREMENT_NOTHING){
        //tcflush(com::fd,TCIOFLUSH);
        //com::transmit(STOP_ORDER);
        QString recv_data = com::receive();
        if(recv_data == NULL){
            return;
        }else if(recv_data[1] == (char)0x31){
            ui->widget_issample->change_label_content(WAIT_BE_LOCATION);
            return;
        }else if(recv_data[1] == (char)0x32){
            ui->widget_issample->change_label_content(REFERENCE_BE_LOCATON);
            return;
        }else if(recv_data[1] == (char)0x33){
            ui->widget_issample->change_label_content(MACHINE_MALFUNCTION);
            return;
        }
    }
  //on_b6_clicked();
}

static int steady_summit_count=0;
void Widget::judge_spectrument_measurement_result(double summit){
  if(summit > 1.6 || summit < 1.4){
      int calibrate_summit = (1.5 - summit) * 40 + mysettings.value("count_voltage").toInt();
      if(calibrate_summit > 700 && summit < 1.4){
          QMessageBox msgbox;
          msgbox.setFont(QFont(FONT_NAME, FONT_SIZE ,QFont::Normal));
          msgbox.setText("需要更换计数管高压,请联系厂家18855953618");
          msgbox.exec();
          return;
        }
      steady_summit_count +=1;
      if(steady_summit_count >= 3){
          if(pw->set_count_voltage(calibrate_summit) == ALL_RIGHT){
              mysettings.setValue("count_voltage",calibrate_summit);
              return;
            }
          return;
        }
      sm->start_steady_summit();
    }else {
      steady_summit_count =0;
    }
}

