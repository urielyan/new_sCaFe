#ifndef HIDE_SYSTEM_H
#define HIDE_SYSTEM_H

#include <QWidget>
//#include <passwd.h>
#include "proportion.h"
#include "input_machine_use_time.h"
#include "query_change_voltage.h"

#include <QSettings>

namespace Ui {
class hide_system;
}

class hide_system : public QWidget
{
    Q_OBJECT

public:
    explicit hide_system(QWidget *parent = 0);
    ~hide_system();

signals:
  void change_voltage_electricity(int);
private slots:
    void on_pushButton_clicked();

    void on_b_count_voltage_clicked();

    void on_b_light_voltage_clicked();

    void on_b_light_electricity_clicked();

    void on_b_clear_clicked();

    void on_pushButton_2_clicked();

    void on_b_input_serial_clicked();

    void time_use_count();

    void on_pushButton_3_clicked();

private:
    Ui::hide_system *ui;
    proportion  *proportion_widget;
    int used_time;
    int most_use_time;
    QTimer *timer;
    input_machine_use_time *input_time;
    QSettings mysettings;
    query_change_voltage *query_change_vol;
};

#endif // HIDE_SYSTEM_H
