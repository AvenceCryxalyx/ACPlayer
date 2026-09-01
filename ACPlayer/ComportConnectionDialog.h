#pragma once
#include "D:\Qt\5.1.5LTS\5.15.2\msvc2019_64\include\QtWidgets\qwidget.h"
#include "ui_ComportConnectionDialog.h"
#include "UARTLibrary.h"
#include <QtWidgets>
#include <QtCore>
#include <QtGui>
#include <QtSerialPort>
#include <QIODevice>
#include <QSerialPortInfo>
#include "Events.h"

using namespace std;
class ComportConnectionDialog : public QDialog
{
	Q_OBJECT
public:
	ComportConnectionDialog(QSerialPort* portConnection, QWidget* parent);
	~ComportConnectionDialog();
	bool OpenComport();
	void Initialize();
	Events onConnectionSuccess;
	Events onConnectionFailed;
private:
	QSerialPort* port;
	Ui_ComportConnectionDialog ui;
	void LoadComportList();
	void FillupDropDown();
	void CloseDialog();
	QList<QSerialPortInfo> comportList;
};

