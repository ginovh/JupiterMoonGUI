#include <QtGui>
#include <iostream>
using namespace std;

#include "astroDateTime.h"

astroDateTime::astroDateTime(QWidget *parent)
    : QDateTimeEdit(parent)
{
   setWrapping(true);
}

void astroDateTime::stepBy ( int steps ) {

	switch (currentSection()) {
	case NoSection :
		break;
	case AmPmSection :
		break;
	case MSecSection :
		break;
	case SecondSection :
		if (steps>0) {
			if ( (time().hour() == 23) && (time().minute() == 59) && (time().second() == 59) ) {
				setDate( date().addDays(1) );
			}
			setTime( time().addSecs(1) );
		} else {
			if ( (time().hour() == 0) && (time().minute() == 0) && (time().second() == 0) ) {
				setDate( date().addDays(-1) );
			}
			setTime( time().addSecs(-1) );
		}
		break;
	case MinuteSection :
		if (steps>0) {
			if ( (time().hour() == 23) && (time().minute() == 59) ) {
				setDate( date().addDays(1) );
			}
			setTime( time().addSecs(60) );
		} else {
			if ( (time().hour() == 0) && (time().minute() == 0) ) {
				setDate( date().addDays(-1) );
			}
			setTime( time().addSecs(-60) );
		}
		break;
	case HourSection :
		if (steps>0) {
			if (time().hour() == 23) {
				setDate( date().addDays(1) );
			}
			setTime( time().addSecs(60*60) );
		} else {
			if (time().hour() == 0) {
				setDate( date().addDays(-1) );
			}
			setTime( time().addSecs(-60*60) );
		}
		break;
	case DaySection :
		if (steps>0) {
			setDate( date().addDays(1) );
		} else {
			setDate( date().addDays(-1) );
		}
		break;
	case MonthSection :
		if (steps>0) {
			setDate( date().addMonths(1) );
		} else {
			setDate( date().addMonths(-1) );
		}
		break;
	case YearSection :
		if (steps>0) {
			setDate( date().addYears(1) );
		} else {
			setDate( date().addYears(-1) );
		}
		break;
	default:
		break;
	}
	setSelectedSection( currentSection() );

};

QAbstractSpinBox::StepEnabled astroDateTime::stepEnabled ( ) {
	return (StepUpEnabled| StepDownEnabled);
};
