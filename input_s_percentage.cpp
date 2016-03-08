#include "input_s_percentage.h"
#include "ui_input_s_percentage.h"
#include "global.h"

#include <QTableWidgetItem>
#include <QMessageBox>
#include <QDebug>

//extern QSettings mysettings;

input_S_percentage::input_S_percentage(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::input_S_percentage)
{
  ui->setupUi(this);

  input_select_element = OPERATE_IN_S;
  ui->b_switch->setText("当前输入为SO3");
  int tmpnumber , row= 0 , column = 0;
  QString tmpstr_s = "calibrate_input_s_";
  QString tmpstr_ca = "calibrate_input_ca_";
  QString tmpstr_fe = "calibrate_input_fe_";

  //mysettings.clear();
  /*
        If mysettings have not calibrate_input_s_ + i ,
        build them and set them "0.0000".
    */
  if(!mysettings.contains("calibrate_input_s_1")){
      for(tmpnumber = 1; tmpnumber <= CALIBRATE_DATA_COUNT ;tmpnumber++){
          tmpstr_s.append(QString("%1").arg(tmpnumber));
          tmpstr_ca.append(QString("%1").arg(tmpnumber));
          tmpstr_fe.append(QString("%1").arg(tmpnumber));
          mysettings.setValue(tmpstr_s,"0.0000");
          mysettings.setValue(tmpstr_ca,"0.0000");
          mysettings.setValue(tmpstr_fe,"0.0000");
          tmpstr_s = "calibrate_input_s_";
          tmpstr_ca = "calibrate_input_ca_";
          tmpstr_fe = "calibrate_input_fe_";
        }
    }

  for(column = 1 ; column < 4 ; column += 2){
      for(row = 0 ; row < ui->tableWidget->rowCount() ; row++){
          //初始化表格使它为所有的硫的化学含量
          QString tmpsettingsstr = QString("calibrate_input_s_%1").arg(((1 == column) ? (row + column) : (row + column +4)));
          QString tmpsettings = mysettings.value(tmpsettingsstr).toString();
          ui->tableWidget->setItem(row,column,new QTableWidgetItem(tmpsettings));
        }
    }

  //define the number key slots.
  QList<QPushButton *> allPButtons = this->findChildren<QPushButton *>();
  for(int i=0;i<allPButtons.count();i++){
      QString tmpstr=allPButtons[i]->objectName();
      //  tmpstr=tmpstr.remove("b_");
      if(tmpstr.length()==3){
          QObject::connect(allPButtons[i],SIGNAL(clicked()),this,SLOT(slot_keyNumPressed()));
        }
    }

  ui->tableWidget->setColumnWidth(0,this->size().width()/7);
  ui->tableWidget->setColumnWidth(1,this->size().width()/15 * 4);
  ui->tableWidget->setColumnWidth(2,this->size().width()/7);
  ui->tableWidget->setColumnWidth(3,this->size().width()/15 * 4);
  for(column = 0 ; column < 3 ; column += 2){
      for(row = 0 ; row < ui->tableWidget->rowCount() ; row++){
          //设置tablewidget为不可编辑
          ui->tableWidget->item(row,column)->setFlags(ui->tableWidget->item(row,column)->flags() & (~Qt::ItemIsEnabled));
        }
    }

  ui->tableWidget->setCurrentCell(0,1);

  this->setFont(QFont(FONT_NAME, FONT_SIZE ,QFont::Normal));
  QList<QLabel *> labellist = this->findChildren<QLabel *>();
  for (int i = 0; i < labellist.count(); ++i) {
      labellist[i]->setFont(QFont(FONT_NAME, FONT_SIZE ,QFont::Normal));
    }
  ui->label->setFont(QFont(FONT_NAME, FONT_SIZE*2 ,QFont::Normal));
}

input_S_percentage::~input_S_percentage()
{
  delete ui;
}

void input_S_percentage::clear_all_tablewidget(){
  /*
        clear tablewidget and mysettings.value("calibrate_input_s_ + i");
        It's used to clear data in calibrate widget.

        clear calibratemeasurement_count settings;
    */
  int tmpnumber,row,column;
  QString tmpstr = "calibrate_input_s_";
  for(tmpnumber = 1; tmpnumber <= CALIBRATE_DATA_COUNT ;tmpnumber++){
      tmpstr.append(QString("%1").arg(tmpnumber));
      //qDebug() << tmpstr;
      mysettings.setValue(tmpstr,"0.0000");
      tmpstr = "calibrate_input_s_";
    }

  for(row = 0;row < ui->tableWidget->rowCount();row++)
    for(column = 1 ;column < 4 ; column += 2){
        ui->tableWidget->item(row,column)->setText("0.0000");
      }
  mysettings.setValue("calibratemeasurement_count",0);
}

void input_S_percentage::slot_keyNumPressed(){
  QString tmpstr=sender()->objectName();
  tmpstr.remove("b_");

  QTableWidgetItem *tmpitem;
  if((tmpitem = ui->tableWidget->currentItem()) == NULL){
      return;
    }
  if(!ui->tableWidget->currentItem()->text().contains(".") &&ui->tableWidget->currentItem()->text() != NULL &&  ui->tableWidget->currentItem()->text().toDouble() == 0)return;
  QString itemtext = tmpitem->text();
  if(!itemtext.compare("0.0000")){itemtext.clear();}
  itemtext += tmpstr;

  //判断是否是第0,2列，若是则返回
  if(ui->tableWidget->currentItem()->column() == 0 || ui->tableWidget->currentColumn() == 2){
      return;
    }
  if(itemtext.size() <= 6){
      ui->tableWidget->currentItem()->setText(itemtext);
    }
  if(itemtext.size() >= 6){
      int r = ui->tableWidget->currentRow();
      int c = ui->tableWidget->currentColumn();
      r++;
      if(r > 5) {
          c = 3;
          r = 0;
        }

      ui->tableWidget->setCurrentCell(r,c);
    }

  //setup current item is want to change item
  //ui->tableWidget->setCurrentItem(tmpitem);
  //if(ui)
}

void input_S_percentage::on_b_abandon_clicked()
{
  //don't save button
  this->close();
}

void input_S_percentage::on_b_clear_clicked()
{
  //clear button
  //判断是否是第0,2列，若是则返回
  if(ui->tableWidget->currentItem()->column() == 0 || ui->tableWidget->currentColumn() == 2){
      return;
    }

  QTableWidgetItem *tmpitem;
  if((tmpitem = ui->tableWidget->currentItem()) == NULL){
      return;
    }
  ui->tableWidget->currentItem()->setText("");

  ui->tableWidget->setCurrentItem(tmpitem);
}

void input_S_percentage::on_b_backspace_clicked()
{
  //backspace button

  //判断是否是第0,2列，若是则返回
  if(ui->tableWidget->currentItem()->column() == 0 || ui->tableWidget->currentColumn() == 2){
      return;
    }

  QTableWidgetItem *tmpitem;
  if((tmpitem = ui->tableWidget->currentItem()) == NULL){
      return;
    }
  QString itemtext = tmpitem->text();
  itemtext.chop(1);
  ui->tableWidget->currentItem()->setText(itemtext);

  ui->tableWidget->setCurrentItem(tmpitem);
}

void input_S_percentage::on_b_point_clicked()
{
  //point button

  //判断是否是第0,2列，若是则返回
  if(ui->tableWidget->currentItem()->column() == 0 || ui->tableWidget->currentColumn() == 2){
      return;
    }

  QTableWidgetItem *tmpitem;
  if((tmpitem = ui->tableWidget->currentItem()) == NULL){
      return;
    }
  QString itemtext = tmpitem->text();
  if (itemtext.size() == 0)return;
  //if It have "." : return;
  if(itemtext.contains("."))return;
  itemtext += ".";
  ui->tableWidget->currentItem()->setText(itemtext);

  ui->tableWidget->setCurrentItem(tmpitem);
}

void input_S_percentage::on_b_sure_clicked()
{
  //to save button
  int row,column;
  QMessageBox msgbox;
  msgbox.setFont(QFont(FONT_NAME, FONT_SIZE ,QFont::Normal));
  msgbox.setText("你要保存你输入的数据么!");
  msgbox.setStandardButtons(QMessageBox::Save | QMessageBox::Cancel);
  msgbox.setDefaultButton(QMessageBox::Save);
  int ret = msgbox.exec();
  if (QMessageBox::Save == ret){
      for(column = 1 ; column < 4 ; column += 2){
          for(row = 0 ; row < ui->tableWidget->rowCount() ; row++){
              if(input_select_element == OPERATE_IN_S){
                  QString tmpsettingsstr = QString("calibrate_input_s_%1").arg(((1 == column) ? (row + column) : (row + column +4)));
                  mysettings.setValue(tmpsettingsstr,ui->tableWidget->item(row,column)->text());
              }else if(input_select_element == OPERATE_IN_CA){
                  QString tmpsettingsstr = QString("calibrate_input_ca_%1").arg(((1 == column) ? (row + column) : (row + column +4)));
                  mysettings.setValue(tmpsettingsstr,ui->tableWidget->item(row,column)->text());
              }else if(input_select_element == OPERATE_IN_FE){
                  QString tmpsettingsstr = QString("calibrate_input_fe_%1").arg(((1 == column) ? (row + column) : (row + column +4)));
                  mysettings.setValue(tmpsettingsstr,ui->tableWidget->item(row,column)->text());
              }
            }
        }
      QMessageBox box;
      msgbox.setFont(QFont(FONT_NAME, FONT_SIZE ,QFont::Normal));
      box.setText("您已保存了您输入的数据，现在可以计算kb值了!");
      box.exec();
      this->close();
    }else{
      this->close();
    }
}

void input_S_percentage::on_b_switch_clicked()
{
    if(input_select_element == OPERATE_IN_S){
        input_select_element = OPERATE_IN_CA;
        ui->b_switch->setText("当前输入为CaO");
        for(int column = 1 ; column < 4 ; column += 2){
            for(int row = 0 ; row < ui->tableWidget->rowCount() ; row++){
                //初始化表格使它为所有的硫的化学含量
                QString tmpsettingsstr = QString("calibrate_input_ca_%1").arg(((1 == column) ? (row + column) : (row + column +4)));
                QString tmpsettings = mysettings.value(tmpsettingsstr).toString();
                //ui->tableWidget->setItem(row,column,new QTableWidgetItem(tmpsettings));
                ui->tableWidget->item(row,column)->setText(tmpsettings);
              }
          }
    }else if (input_select_element == OPERATE_IN_CA) {
        input_select_element = OPERATE_IN_FE;
        ui->b_switch->setText("当前输入为Fe2O3");
        for(int column = 1 ; column < 4 ; column += 2){
            for(int row = 0 ; row < ui->tableWidget->rowCount() ; row++){
                //初始化表格使它为所有的硫的化学含量
                QString tmpsettingsstr = QString("calibrate_input_fe_%1").arg(((1 == column) ? (row + column) : (row + column +4)));
                QString tmpsettings = mysettings.value(tmpsettingsstr).toString();
                //ui->tableWidget->setItem(row,column,new QTableWidgetItem(tmpsettings));
                ui->tableWidget->item(row,column)->setText(tmpsettings);
              }
          }
    }else if (input_select_element == OPERATE_IN_FE) {
        input_select_element = OPERATE_IN_S;
        ui->b_switch->setText("当前输入为SO3");
        for(int column = 1 ; column < 4 ; column += 2){
            for(int row = 0 ; row < ui->tableWidget->rowCount() ; row++){
                //初始化表格使它为所有的硫的化学含量
                QString tmpsettingsstr = QString("calibrate_input_s_%1").arg(((1 == column) ? (row + column) : (row + column +4)));
                QString tmpsettings = mysettings.value(tmpsettingsstr).toString();
                //ui->tableWidget->setItem(row,column,new QTableWidgetItem(tmpsettings));
                ui->tableWidget->item(row,column)->setText(tmpsettings);
              }
          }
    }
}

void input_S_percentage::show_and_refresh(){
    input_select_element = OPERATE_IN_S;
    ui->b_switch->setText("当前输入为SO3");
    for(int column = 1 ; column < 4 ; column += 2){
        for(int row = 0 ; row < ui->tableWidget->rowCount() ; row++){
            //初始化表格使它为所有的硫的化学含量
            QString tmpsettingsstr = QString("calibrate_input_s_%1").arg(((1 == column) ? (row + column) : (row + column +4)));
            QString tmpsettings = mysettings.value(tmpsettingsstr).toString();
            //ui->tableWidget->setItem(row,column,new QTableWidgetItem(tmpsettings));
            ui->tableWidget->item(row,column)->setText(tmpsettings);
          }
      }
    this->showFullScreen();
}
