
#ifndef COUNTINGMEASUREMENT_H
#define COUNTINGMEASUREMENT_H

#include <QWidget>
#include <QTimer>
#include "showcountingmeasurement.h"
#include "com.h"
#include <QSettings>

#define GET_S_AVERAGE_COUNT_5 1
#define GET_CA_AVERAGE_COUNT_5 2
#define GET_FE_AVERAGE_COUNT_5 3

namespace Ui {
  class countingMeasurement;
}

class countingMeasurement : public QWidget
{
  Q_OBJECT

public:
  explicit countingMeasurement(QWidget *parent = 0);
  void try_inspectoscope();
  ~countingMeasurement();
  static double get_count_5_average(int);
  static QStringList get_count_5_data();
  static int clear_count_5_data();
signals:
  void transmit_time(int);
  void transmit_stop_auto_count();
  void start_steady_summit_measurement();
  void transmit_move_sliding(bool,bool);

public slots:
  int examine_count_5_data_availability();
  void on_pushButton_clicked();
  int on_pushButton_2_clicked();
  void stop_measurement_no_transmit();

private slots:
  void count_second();

  void on_pushButton_3_clicked();

  void on_pushButton_5_clicked();

  void on_pushButton_7_clicked();
  //void set_sliding_disabled(bool,bool);

private:
  Ui::countingMeasurement *ui;
  int turn_second;
  size_t turn_counter,try_inspectoscope_flag;
  QTimer *timer;
  int flag;
  showcountingmeasurement *show_count_mea;
  void updata_count_5_average(QDateTime,unsigned long long);
  void start_count_5_data();
  void disable_button(bool);
};

#endif // COUNTINGMEASUREMENT_H
