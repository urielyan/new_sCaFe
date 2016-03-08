#include "countingmeasurement.h"
#include "ui_countingmeasurement.h"
//#include "widget.h"
#include "global.h"
#include "printer.h"

#include <QMessageBox>
#include <QTimer>
#include <stdio.h>
extern int measurement_flag;

countingMeasurement::countingMeasurement(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::countingMeasurement)
{
    ui->setupUi(this);
    show_count_mea = new showcountingmeasurement();
    turn_counter = 1;//
    turn_second = TURN_SECONDS;

    //while flag = 1,表示没有进行计数测量.
    //while flag = 2,表示正在进行计数测量.
    flag = 0;
    try_inspectoscope_flag = 0;
    ui->label_2->setText(QString("共11次,每次%1秒").arg(TURN_SECONDS));
    ui->label_seconds->setText(QString("%1秒").arg(TURN_SECONDS));

    timer = new QTimer();
    connect(timer,SIGNAL(timeout()),this,SLOT(count_second()));

    this->setFont(QFont(FONT_NAME, FONT_SIZE ,QFont::Normal));
    QList<QLabel *> labellist = this->findChildren<QLabel *>();
    for (int i = 0; i < labellist.count(); ++i) {
        labellist[i]->setFont(QFont(FONT_NAME, FONT_SIZE ,QFont::Normal));
    }
    ui->label_seconds->setFont(QFont(FONT_NAME, FONT_SIZE*2 ,QFont::Normal));
    ui->pushButton_7->setFont(QFont(FONT_NAME, FONT_SIZE*2 ,QFont::Normal));

    //set_sliding_disabled(true,false);
}
countingMeasurement::~countingMeasurement()
{
    delete timer;
    delete show_count_mea;
    delete ui; //QMessageBox::warning(this,"spectrum","printer!");
}

/*下面四个函数是为了含量测量而设计的最新5个计数的平均值
 *  count_data_5:一个以时间排序的QMap对象，一个时间对应一个计数值，最多5个
    第一个函数：实现了更新计数数据，保证5个数据是最新的
    第二个函数：一个静态公有函数，得到5个最新计数值的平均值。当计数值的时间超过规定时间，则返回0
    第三个函数：开始计数测量，开始之前不发送停止信号。
    第四个函数：是一个静态公有槽函数，检查5个数据是否有效,若无效，则进行计数测量并返回-1，否则返回0，这个槽函数要每分钟进行一次。若正在进行计数测量，则不做任何事。
*/
static QMap<QDateTime,unsigned long long> count_data_5;
void countingMeasurement::updata_count_5_average(QDateTime count_datetime,unsigned long long data){
    //printf("in\n");
    QSettings count_data("shanghaikairen","count_data");

    if(count_data_5.size() < COUNT_MEASUREMENT_DATA_COUNT){
        count_data_5.insert(count_datetime,data);
        return;
    }else {
        count_data_5.remove(count_data_5.begin().key());
        count_data_5.insert(count_datetime,data);
    }

    if(measurement_flag == MEASUREMENT_10_AUTO && turn_counter == COUNT_MEASUREMENT_DATA_COUNT){
        //printf("ready\n");
        static int in_this_cunction_time = 0;
        in_this_cunction_time ++;
        int tmp_count = count_data.value("count_count").toInt() + 1;
        if(tmp_count > COUNT_MEASUREMENT_MOST_STORAGE) tmp_count = 1;
#if 0
        QString last_count_meaurement_datetime;
        if(tmp_count == 1){
            //printf("in1111\n");
            if(count_data.contains(QString("count_data_%1").arg(COUNT_MEASUREMENT_MOST_STORAGE))){
                last_count_meaurement_datetime = count_data.value(QString("count_data_%1").arg(COUNT_MEASUREMENT_MOST_STORAGE)).toString();
            }else {
                count_data.setValue(QString("count_data_%1").arg(tmp_count),QString("%1").arg((int)get_count_5_average()) \
                                    + ";" + "   " + ";" + QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss"));
                count_data.setValue("count_count",tmp_count);
                return;
            }
        }else{
            //printf("in222\n");
            last_count_meaurement_datetime = count_data.value(QString("count_data_%1").arg(tmp_count -1)).toString();
        }
        if(last_count_meaurement_datetime.split(";").size() != 3)return;
        if(-QDateTime::currentDateTime().secsTo(QDateTime::fromString(last_count_meaurement_datetime.split(";")[2],"yyyy.MM.dd hh:mm:ss")) > 10 * 60){
            //printf("3333333\n");
            return;
        }
        if(tmp_count >= COUNT_MEASUREMENT_MOST_STORAGE) tmp_count = 1;
        //QString tmp_y = QString("无");
        //printf("444444in\n");
#endif
        if(in_this_cunction_time  < 2){
            //printf("no\n");
            return;
        }
        in_this_cunction_time = 0;
        count_data.setValue(QString("count_data_%1").arg(tmp_count),
                            QString("%1").arg((int)get_count_5_average(GET_S_AVERAGE_COUNT_5))   + ";" + "  " + ";" +
                            QString("%1").arg((int)get_count_5_average(GET_CA_AVERAGE_COUNT_5)) + ";" + "  " + ";" +
                            QString("%1").arg((int)get_count_5_average(GET_FE_AVERAGE_COUNT_5))  + ";" + "  " + ";" +
                            QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss"));
        count_data.setValue("count_count",tmp_count);
    }
}
double countingMeasurement::get_count_5_average(int select){
    double average = 0;
    QMap<QDateTime,unsigned long long>::iterator i;
    for(i = count_data_5.begin();i != count_data_5.end();i++){
        if(select == GET_S_AVERAGE_COUNT_5){
            average += QString::number(i.value()).left(5).toInt();
        }else if(select == GET_CA_AVERAGE_COUNT_5){
            average += QString::number(i.value()).left(10).right(5).toInt();
        }else if (select == GET_FE_AVERAGE_COUNT_5){
            average += QString::number(i.value()).right(5).toInt();
        }
    }
    average /= (double)COUNT_MEASUREMENT_DATA_COUNT;
    return average;
}
void countingMeasurement::start_count_5_data(){
    on_pushButton_clicked();
    measurement_flag = MEASUREMENT_10_AUTO;
}
int countingMeasurement::examine_count_5_data_availability(){
    if(ui->widget_2->global_ispreheat > 0)return 1;
    if(count_data_5.size() < COUNT_MEASUREMENT_DATA_COUNT){
        if(issample::global_is_sample == WAIT_BE_LOCATION){
            //如果待测样在参考样位置，则使ispreheat的lable2变成“需要采样”
            ispreheat::is_sampling_num = NEED_START_SAMPLING;
            return -1;
        }
        start_count_5_data();
        ispreheat::is_sampling_num = COUNT_MEASUREMENT_DATA_COUNT - count_data_5.size();
        //qDebug() << ispreheat::is_sampling_num << count_data_5.size();
        return -1;
    }else{
        int count_5_data_stale_count = 0;
        QMap<QDateTime,unsigned long long>::iterator i;
        //检查5个数据中有几个数据无效
        for(i = count_data_5.begin();i != count_data_5.end();i++){
            //数据有效时间为30分钟
            if(i.key().secsTo(QDateTime::currentDateTime()) > 60 *30)
                count_5_data_stale_count++;
        }
        if(count_5_data_stale_count > 0){
            if(issample::global_is_sample == WAIT_BE_LOCATION){
                //如果待测样在参考样位置，则使ispreheat的lable2变成“需要采样”
                ispreheat::is_sampling_num = NEED_START_SAMPLING;
                return -1;
            }
            start_count_5_data();
            ispreheat::is_sampling_num = count_5_data_stale_count;
            return -1;
        }else{
            //只有在正在计数测量并且计数已经得到更新时，才会进入下面的判断条件
            if(flag == 2){// && measurement_flag == MEASUREMENT_10_AUTO
                on_pushButton_2_clicked();//停止测量
            }
            ispreheat::is_sampling_num = 0;
            return 0;
        }
    }
}
int countingMeasurement::clear_count_5_data(){
    count_data_5.clear();
    //examine_count_5_data_availability();
}
QStringList countingMeasurement::get_count_5_data(){
    QMap<QDateTime,unsigned long long>::iterator i;
    QStringList list;
    for(i = count_data_5.begin();i != count_data_5.end();i++){
        list << QString::number(i.value());
    }
    return list;
}

//自检开始
void countingMeasurement::try_inspectoscope(){
    on_pushButton_clicked();
    try_inspectoscope_flag = 1;
}

//槽函数，由timer控制进行计数测量
static int judge_position_flag = 0;
void countingMeasurement::count_second(){
    QString recv_data,recv_data_real="";
    if(turn_second > 2){
        recv_data = com::receive();
        if(recv_data == NULL){
        }else if(recv_data[1] == (char)0x31 && recv_data.size() == 2){
            ui->widget->change_label_content(WAIT_BE_LOCATION);
        }else if(recv_data[1] == (char)0x32 && recv_data.size() == 2){
            ui->widget->change_label_content(REFERENCE_BE_LOCATON);
        }else if(recv_data[1] == (char)0x33 && recv_data.size() == 2){
            ui->widget->change_label_content(MACHINE_MALFUNCTION);
            on_pushButton_2_clicked();
            return;
        }else if(recv_data[0] == (char)0x02){
            //如果计数被判断位置的接收了则还给他
            judge_position_flag = 1;
            recv_data_real = recv_data;
            QMessageBox::information(this,"err",recv_data);
        }
    }
#if 1
    if(measurement_flag == MEASUREMENT_SPECTRUM || measurement_flag == MEASUREMENT_CALIBRATE || measurement_flag == MEASUREMENT_SAMPLE){
        stop_measurement_no_transmit();
        return;
    }
#endif
    emit transmit_time(11 * TURN_SECONDS - (turn_counter * TURN_SECONDS - turn_second));

    if(turn_second < 0){
        recv_data = com::receive(2);
        if(judge_position_flag == 1 &&recv_data_real.size() == 0){
            //如果计数被判断位置的接收了则还给他
            recv_data = recv_data_real;
            recv_data_real = "";
            judge_position_flag = 0;
        }
        if(recv_data == NULL){
            QSettings communication_err_data("shanghaikairen","communication_error");
            communication_err_data.setValue("com_err_2",communication_err_data.value("com_err_2").toInt() + 1);

            on_pushButton_2_clicked();
            QMessageBox box;
            box.setFont(QFont(FONT_NAME, FONT_SIZE ,QFont::Normal));
            QTimer::singleShot(MESSAGEBOX_AUTO_CLOSE_SECOND * 1000,&box,SLOT(close()));
            box.setText("没有接收到计数数据，已停止测量");
            box.exec();
            return;
        }else if(recv_data[0] != (char)0x02){
            QSettings communication_err_data("shanghaikairen","communication_error");
            communication_err_data.setValue("com_err_2",communication_err_data.value("com_err_2").toInt() + 1);

            on_pushButton_2_clicked();
            QMessageBox box;
            box.setFont(QFont(FONT_NAME, FONT_SIZE ,QFont::Normal));
            QTimer::singleShot(MESSAGEBOX_AUTO_CLOSE_SECOND * 1000,&box,SLOT(close()));
            box.setText("接收到的计数数据有误，已停止测量");
            box.exec();
            return;
        }
        recv_data.remove(0,1);
        show_count_mea->adddata(turn_counter,recv_data);
        recv_data.remove(0,2);
        //只有在是测参考样时才会更新5个计数值
        if(ui->widget->global_is_sample == REFERENCE_BE_LOCATON){
            updata_count_5_average(QDateTime::currentDateTime(),recv_data.toULongLong());
        }
        if (turn_counter == 11){
            show_count_mea->showFullScreen();
            //判断是否是循环计数测量，若是，则在几秒钟之后关闭数据展示窗口，在重新开始另一个测量
            if(measurement_flag == MEASUREMENT_LOOP_COUNT){
                QTimer::singleShot(MESSAGEBOX_AUTO_CLOSE_SECOND * 1000,show_count_mea,SLOT(close()));
                QTimer::singleShot(MESSAGEBOX_AUTO_CLOSE_SECOND * 1000,this,SLOT(showFullScreen()));
                QTimer::singleShot(MESSAGEBOX_AUTO_CLOSE_SECOND  * 1000 + 1000,this,SLOT(on_pushButton_7_clicked()));
            }
            on_pushButton_2_clicked();//停止按钮
            return;
        }

        if(measurement_flag == MEASUREMENT_10_AUTO && turn_counter >= COUNT_MEASUREMENT_DATA_COUNT){
            //当是自动进行的计数测量而且计数达到了COUNT_MEASUREMENT_DATA_COUNT次后停止当前的计数测量
#ifdef DEBUG
            QMessageBox::warning(this,QString(__FILE__),QString(__FUNCTION__) + QString::number(measurement_flag) );
#endif
            on_pushButton_2_clicked();
            emit start_steady_summit_measurement();
            //spectrum_meas->start_steady_summit();
        }
        turn_second = TURN_SECONDS;
        turn_counter += 1;
    }
    QString tmpstr_second = QString("%1 秒").arg(turn_second);
    QString tmpstr_count = QString("第 %1 次").arg(turn_counter);
    ui->label_counter->setText(tmpstr_count);
    ui->label_seconds->setText(tmpstr_second);
    turn_second--;
}

void countingMeasurement::disable_button(bool value){
    ui->pushButton->setDisabled(value);
    ui->pushButton_3->setDisabled(value);

}

void countingMeasurement::on_pushButton_clicked()
{
    //start button
    if(flag == 1){
        turn_counter = 1;
        turn_second = TURN_SECONDS;
    }
    if (flag == 2) return;
    //judge whether have any other measurement.
    if(measurement_flag != MEASUREMENT_NOTHING){
        emit transmit_stop_auto_count();
    }

    //开始之前停止其他任何测量
    if(on_pushButton_2_clicked() != ALL_RIGHT){
        return;
    }
    //开始计数测量
    tcflush(com::fd,TCIOFLUSH);
    if(com::transmit(ACTIVATINE_COUNT,3) <= 0){
        QMessageBox msgbox;
        msgbox.setFont(QFont(FONT_NAME, FONT_SIZE ,QFont::Normal));
        msgbox.setText(TRANSMIT_DATA_ERROR);
        msgbox.exec();
        return;
    }
    //开始之前清除上一次留下的计数数据
    show_count_mea->clearall();
    ui->label_counter->setText(tr("%1 秒").arg(TURN_SECONDS));
    ui->label_counter->setText("第 1 次");
    flag = 2;
    measurement_flag = MEASUREMENT_COUNT;
    tcflush(com::fd,TCIOFLUSH);
    disable_button(true);
    timer->start(1000);
}

void countingMeasurement::on_pushButton_3_clicked()
{
    this->close();
}

int countingMeasurement::on_pushButton_2_clicked()
{
    //stop button
    if(measurement_flag != MEASUREMENT_NOTHING){
        emit transmit_stop_auto_count();
    }
    ui->pushButton_7->setText("计数测量");
    ui->pushButton_7->setPalette(QColor(255,255,0,0));
    timer->stop();
    disable_button(false);//改变按钮状态
    measurement_flag = MEASUREMENT_NOTHING;
    try_inspectoscope_flag = 0;
    turn_counter = 1;
    turn_second = TURN_SECONDS;
    ui->label_seconds->setText("");
    ui->label_counter->setText("");
    flag = 1;
    tcflush(com::fd,TCIOFLUSH);
    if(com::transmit(STOP_ORDER,3) <= 0){
        QMessageBox msgbox;
        msgbox.setFont(QFont(FONT_NAME, FONT_SIZE ,QFont::Normal));
        msgbox.setText(TRANSMIT_DATA_ERROR);
        msgbox.exec();
        return ERRNO_COMMUNICATION_1;
    }
    QString recv_data = com::receive(1);
    if(recv_data == NULL){
        QSettings communication_err_data("shanghaikairen","communication_error");
        communication_err_data.setValue("com_err_10",communication_err_data.value("com_err_10").toInt() + 1);
        QMessageBox msgbox;
        msgbox.setFont(QFont(FONT_NAME, FONT_SIZE ,QFont::Normal));
        msgbox.setText(SLIDING_PLATE_NO_CHANGE_TEXT);
        msgbox.setInformativeText("recv NULL");
        msgbox.exec();
        return ERRNO_COMMUNICATION_1;
    }
    if(recv_data[1] == (char)0x31){//recv_data[0] == (char)0x98 &&
        ui->widget->change_label_content(REFERENCE_BE_LOCATON);
    }else if(recv_data[1] == (char)0x32){
        ui->widget->change_label_content(WAIT_BE_LOCATION);
    }else if(recv_data[1] == (char)0x33){
        ui->widget->change_label_content(MACHINE_MALFUNCTION);
        //        QMessageBox msgbox;
        //        msgbox.setFont(QFont(FONT_NAME, FONT_SIZE ,QFont::Normal));
        //        msgbox.setText(MACHINE_MALFUNCTION_TEXT);
        //        msgbox.exec();
        return ERRNO_SILIDING_POSITION;
    }else{
        QSettings communication_err_data("shanghaikairen","communication_error");
        communication_err_data.setValue("com_err_10",communication_err_data.value("com_err_10").toInt() + 1);
        QMessageBox msgbox;
        msgbox.setFont(QFont(FONT_NAME, FONT_SIZE ,QFont::Normal));
        msgbox.setText(QString("通信不正常") + ":" + recv_data + "," + QString::number(measurement_flag));
        msgbox.exec();
        return ERRNO_COMMUNICATION_1;
    }
    return ALL_RIGHT;
}


void countingMeasurement::stop_measurement_no_transmit(){
    timer->stop();
    disable_button(false);//改变按钮状态
    try_inspectoscope_flag = 0;
    turn_counter = 1;
    turn_second = TURN_SECONDS;
    ui->label_seconds->setText("");
    ui->label_counter->setText("");
    flag = 1;
}


void countingMeasurement::on_pushButton_5_clicked()
{
    //query button
    show_count_mea->showFullScreen();
}

void countingMeasurement::on_pushButton_7_clicked()
{
    //循环测量按钮
    //if(measurement_flag != MEASUREMENT_NOTHING)return;
    on_pushButton_clicked();//start
    measurement_flag = MEASUREMENT_LOOP_COUNT;
    ui->pushButton_7->setText("循环计数测量");
    ui->pushButton_7->setPalette(QColor(255,0,0,200));
}
