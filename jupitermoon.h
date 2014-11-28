#ifndef JUPITERMOON_H
#define JUPITERMOON_H

#include <QDialog>
#include <QSignalMapper>

#include "ui_jupitermoon.h"
#include "stdafx.h"
#include "AA+.h"

class jupitermoon : public QDialog, public Ui::jupitermoon
{
	Q_OBJECT

public:
	jupitermoon(QWidget *parent = 0);

protected:
	void paintEvent(QPaintEvent *event);

private:
	CAADate date;
	QString origstyleSheet;
	QSignalMapper signalMapper;

	QTransform m;
	int scale_p;
	void updateWeekday();
	void updateGUI();

	private slots:
		void resetDate();
		void updateDateTime();
		void updateStep();
		void minScale();
		void maxScale();
		void calculate_next(int id);
};

#endif
