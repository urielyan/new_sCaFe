#ifndef CAIBRATERESULTQUERY_H
#define CAIBRATERESULTQUERY_H

#include <QWidget>
#include <QSettings>

namespace Ui {
  class caibrateresultquery;
}

class caibrateresultquery : public QWidget
{
  Q_OBJECT

public:
  explicit caibrateresultquery(QWidget *parent = 0);
  ~caibrateresultquery();
  void show_and_update();

private slots:
  void on_pushButton_clicked();

  void on_pushButton_2_clicked();

private:
  Ui::caibrateresultquery *ui;
  QStringList calibrate_results;
  int display_number;
  QSettings mysettings;

  void update_page(int);
};

#endif // CAIBRATERESULTQUERY_H
