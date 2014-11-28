#ifndef ASTODATETIME_H
#define ASTODATETIME_H

#include <QDateTimeEdit>

class astroDateTime : public QDateTimeEdit
{
    Q_OBJECT

public:
    astroDateTime(QWidget *parent = 0);

protected:
void stepBy ( int steps ) ;
QAbstractSpinBox::StepEnabled stepEnabled () ;

private:
};

#endif
