// TODO : 
// - 

#include <QtGui>
#include <iostream>
#include <bitset>
#include <algorithm>

#include "jupitermoon.h"

jupitermoon::jupitermoon(QWidget *parent)
  : QDialog(parent)
  , signalMapper(this)
  , scale_p(1)
{
  setupUi(this);

  // Use QSignalMapper to pass parameter to slot calculate_next(int)
  signalMapper.setMapping(NextButton, 1);
  signalMapper.setMapping(PreviousButton, 2);

  connect(NextButton, SIGNAL(clicked()),
      &signalMapper, SLOT (map()));
  connect(PreviousButton, SIGNAL(clicked()),
      &signalMapper, SLOT (map()));

  connect(&signalMapper, SIGNAL(mapped(int)),
           this, SLOT(calculate_next(int)));

  dateTimeEdit->setWrapping(true);
  // Must get origstyleSheet before resetDate(), because if GRS is visible,
  // stylesheet is set directly to red in resetDate(); and origstyleSheet will be red.
  origstyleSheet = CMII->styleSheet();

  resetDate();
}

void jupitermoon::resetDate()
{
  // automatically calls updateDateTime();
  dateTimeEdit->setDateTime( QDateTime::currentDateTime() );
}

void jupitermoon::updateGUI() {

  CAAPhysicalJupiterDetails JupiterPhyDetails = CAAPhysicalJupiter::Calculate(date.Julian(), true);
  QString temp_str;
  temp_str = "CMII: " + QString::number(JupiterPhyDetails.Apparentw2);
  CMII->setText(temp_str);

  // assume GRS is visible 50 degrees from meridian ( with mewlon and good seeing condtions this will be more like 55 degrees)
  // 01/12/2014 : 229
  // 14/08/2014 : 004
  // see also: http://jupos.privat.t-online.de
  const int redspotCMII = 4;
  const int dist = abs(JupiterPhyDetails.Apparentw2 - redspotCMII);
  if ( std::min(360-dist,dist) < 50 ) {
      CMII->setStyleSheet("QLabel { background-color : red; color : blue; }");
    } else { //reset stylesheet
      CMII->setStyleSheet(origstyleSheet);
    }

  // Turnhout -4.95, 51.32
  // to calculate altitude of saturn and sun
  CAAEllipticalPlanetaryDetails JupiterDetails = CAAElliptical::Calculate(date.Julian(), CAAElliptical::EllipticalObject::JUPITER, true);
  CAAEllipticalPlanetaryDetails SunDetails = CAAElliptical::Calculate(date.Julian(), CAAElliptical::EllipticalObject::SUN, true);
  double AST = CAASidereal::ApparentGreenwichSiderealTime(date.Julian());
  double LongtitudeAsHourAngle = CAACoordinateTransformation::DegreesToHours(-4.95);

  double Alpha = JupiterDetails.ApparentGeocentricRA ;
  double LocalHourAngle = AST - LongtitudeAsHourAngle - Alpha;
  CAA2DCoordinate JupiterHorizontal = CAACoordinateTransformation::Equatorial2Horizontal(LocalHourAngle, JupiterDetails.ApparentGeocentricDeclination, 51.32 );

  Alpha = SunDetails.ApparentGeocentricRA ;
  LocalHourAngle = AST - LongtitudeAsHourAngle - Alpha;
  CAA2DCoordinate SunHorizontal = CAACoordinateTransformation::Equatorial2Horizontal(LocalHourAngle, SunDetails.ApparentGeocentricDeclination, 51.32 );

  temp_str = "Alt jup: " + QString::number( JupiterHorizontal.Y );
  alt_jup->setText(temp_str);
  temp_str = "Alt sun: " + QString::number( SunHorizontal.Y );
  alt_sun->setText(temp_str);

  updateWeekday();
  update();
}

void jupitermoon::updateDateTime() {
  // Update date
  date.Set(dateTimeEdit->date().year(), dateTimeEdit->date().month(), dateTimeEdit->date().day(),
           dateTimeEdit->time().hour(), dateTimeEdit->time().minute(), dateTimeEdit->time().second() ,true);

  // Update other widgets
  QString temp_str;
  temp_str = "JD: " + QString::number(date.Julian(), 'f');
  JDLabel->setText(temp_str);

  // Update GUI
  updateGUI();
}

enum jupevent {
  SAT1_SHADOWTRANSIT,
  SAT1_INTRANSIT,
  SAT2_SHADOWTRANSIT,
  SAT2_INTRANSIT,
  SAT3_SHADOWTRANSIT,
  SAT3_INTRANSIT,
  SAT4_SHADOWTRANSIT,
  SAT4_INTRANSIT,
  num_events
};

double calc_sun_h(CAADate date){
  CAAEllipticalPlanetaryDetails SunDetails = CAAElliptical::Calculate(date.Julian(), CAAElliptical::EllipticalObject::SUN, true);
  double AST = CAASidereal::ApparentGreenwichSiderealTime(date.Julian());
  double LongtitudeAsHourAngle = CAACoordinateTransformation::DegreesToHours(CAACoordinateTransformation::DMSToDegrees(-4, 59, 0));

  double Alpha = SunDetails.ApparentGeocentricRA ;
  double LocalHourAngle = AST - LongtitudeAsHourAngle - Alpha;
  CAA2DCoordinate SunHorizontal = CAACoordinateTransformation::Equatorial2Horizontal(LocalHourAngle, SunDetails.ApparentGeocentricDeclination, CAACoordinateTransformation::DMSToDegrees(51, 18, 0) );
  return SunHorizontal.Y;
}

double calc_jup_h(CAADate date){
  CAAEllipticalPlanetaryDetails PDetails = CAAElliptical::Calculate(date.Julian(), CAAElliptical::EllipticalObject::JUPITER, true);
  double AST = CAASidereal::ApparentGreenwichSiderealTime(date.Julian());
  double LongtitudeAsHourAngle = CAACoordinateTransformation::DegreesToHours(CAACoordinateTransformation::DMSToDegrees(-4, 59, 0));

  double Alpha = PDetails.ApparentGeocentricRA ;
  double LocalHourAngle = AST - LongtitudeAsHourAngle - Alpha;
  CAA2DCoordinate Horizontal = CAACoordinateTransformation::Equatorial2Horizontal(LocalHourAngle, PDetails.ApparentGeocentricDeclination, CAACoordinateTransformation::DMSToDegrees(51, 18, 0) );
  return Horizontal.Y;
}

void jupitermoon::calculate_next(int id) {

  bool new_event_happened=false;
  std::bitset<num_events> jup_events;
//  jupevent last_event;
  CAAGalileanMoonsDetails GalileanDetails;

  // First calculate if event was already in progress
  GalileanDetails = CAAGalileanMoons::Calculate(date.Julian(), true);
  if (GalileanDetails.Satellite1.bInShadowTransit ) {
      jup_events[SAT1_SHADOWTRANSIT] = true;
    }
  if (GalileanDetails.Satellite1.bInTransit ) {
      jup_events[SAT1_INTRANSIT] = true;
    }
  if (GalileanDetails.Satellite2.bInShadowTransit ) {
      jup_events[SAT2_SHADOWTRANSIT] = true;
    }
  if (GalileanDetails.Satellite2.bInTransit ) {
      jup_events[SAT2_INTRANSIT] = true;
    }
  if (GalileanDetails.Satellite3.bInShadowTransit ) {
      jup_events[SAT3_SHADOWTRANSIT] = true;
    }
  if (GalileanDetails.Satellite3.bInTransit ) {
      jup_events[SAT3_INTRANSIT] = true;
    }
  if (GalileanDetails.Satellite4.bInShadowTransit ) {
      jup_events[SAT4_SHADOWTRANSIT] = true;
    }
  if (GalileanDetails.Satellite4.bInTransit ) {
      jup_events[SAT4_INTRANSIT] = true;
    }

  unsigned int progress_counter=0;
  EventLabel->setText(QString((int)(progress_counter/200),'*'));
  while (!new_event_happened) {
      if (id ==1) {
          date.Set( date.Julian() + 1.0/(24*60) ,true); // 1 min
        } else {
          date.Set( date.Julian() - 1.0/(24*60) ,true); // 1 min
        }
      if ( (calc_sun_h(date) < 0.0) && (calc_jup_h(date) > 10.0) ) {
          GalileanDetails = CAAGalileanMoons::Calculate(date.Julian(), true);
          if (GalileanDetails.Satellite1.bInShadowTransit ) {
              if (!jup_events[SAT1_SHADOWTRANSIT]) {
                  new_event_happened = true;
                }
            } else if (jup_events[SAT1_SHADOWTRANSIT]) {
              new_event_happened = true;
            }
          if (GalileanDetails.Satellite1.bInTransit ) {
              if (!jup_events[SAT1_INTRANSIT]) {
                  new_event_happened = true;
                }
            } else if (jup_events[SAT1_INTRANSIT]) {
              new_event_happened = true;
            }
          if (GalileanDetails.Satellite2.bInShadowTransit ) {
              if (!jup_events[SAT2_SHADOWTRANSIT]) {
                  new_event_happened = true;
                }
            } else if (jup_events[SAT2_SHADOWTRANSIT]) {
              new_event_happened = true;
            }
          if (GalileanDetails.Satellite2.bInTransit ) {
              if (!jup_events[SAT2_INTRANSIT]) {
                  new_event_happened = true;
                }
            } else if (jup_events[SAT2_INTRANSIT]) {
              new_event_happened = true;
            }
          if (GalileanDetails.Satellite3.bInShadowTransit ) {
              if (!jup_events[SAT3_SHADOWTRANSIT]) {
                  new_event_happened = true;
                }
            } else if (jup_events[SAT3_SHADOWTRANSIT]) {
              new_event_happened = true;
            }
          if (GalileanDetails.Satellite3.bInTransit ) {
              if (!jup_events[SAT3_INTRANSIT]) {
                  new_event_happened = true;
                }
            } else if (jup_events[SAT3_INTRANSIT]) {
              new_event_happened = true;
            }
          if (GalileanDetails.Satellite4.bInShadowTransit ) {
              if (!jup_events[SAT4_SHADOWTRANSIT]) {
                  new_event_happened = true;
                }
            } else if (jup_events[SAT4_SHADOWTRANSIT]) {
              new_event_happened = true;
            }
          if (GalileanDetails.Satellite4.bInTransit ) {
              if (!jup_events[SAT4_INTRANSIT]) {
                  new_event_happened = true;
                }
            } else if (jup_events[SAT4_INTRANSIT]) {
              new_event_happened = true;
            }
        }
      progress_counter++;
      if (progress_counter%200==0){
        EventLabel->setText(QString((int)(progress_counter/200),'*'));
      }
      if (progress_counter%4000==0){
        progress_counter=0;
      }
      QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    }

  // Update other widgets
  QDate date_t(date.Year(), date.Month(), date.Day());
  QTime time_t(date.Hour(), date.Minute(), date.Second());
  dateTimeEdit->setDateTime( QDateTime(date_t, time_t) );
  EventLabel->setText(QString("Done"));

  // Update GUI
  updateGUI();

}

void jupitermoon::updateStep()
{
}


void jupitermoon::updateWeekday()
{
  switch ( date.DayOfWeek() ) {
    case CAADate::DAY_OF_WEEK::SUNDAY :
      WeekdayLabel->setText("Sunday");
      break;
    case CAADate::DAY_OF_WEEK::MONDAY :
      WeekdayLabel->setText("Monday");
      break;
    case CAADate::DAY_OF_WEEK::TUESDAY :
      WeekdayLabel->setText("Tuesday");
      break;
    case CAADate::DAY_OF_WEEK::WEDNESDAY :
      WeekdayLabel->setText("Wednesday");
      break;
    case CAADate::DAY_OF_WEEK::THURSDAY :
      WeekdayLabel->setText("Thursday");
      break;
    case CAADate::DAY_OF_WEEK::FRIDAY :
      WeekdayLabel->setText("Friday");
      break;
    case CAADate::DAY_OF_WEEK::SATURDAY :
      WeekdayLabel->setText("Saturday");
      break;
    default:
      WeekdayLabel->setText("Error");
      break;
      ;
    }
}

void jupitermoon::minScale() {
  if (scale_p>=2) {
      scale_p /= 2;
    }
  update();
}

void jupitermoon::maxScale() {
  if (scale_p<=5) {
      scale_p *= 2;
    }
  update();
}

void jupitermoon::paintEvent(QPaintEvent *event) {

  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing, true);

  // to test painting
  //painter.drawLine( width(), height() / 2 , 0, height() / 2 );
  //painter.drawLine( width() / 2, 0 , width() / 2, height() );
  unsigned int _width=480;
  unsigned int _heigth=180;
  painter.drawRect(10,150,_width,_heigth);
  QPointF delta1(_width/2+10 , _heigth/2+150);
  painter.translate(delta1);

  // be carefull : with this method, only coords will be rescaled, not size
  m.reset();
  //m.translate( width() / 2, height() / 2 );
  m.scale( scale_p, -scale_p );

  // size needs to be adjusted for correct scale
  QPoint jup(0,0);
  painter.drawEllipse(m.map(jup),10*scale_p,10*scale_p); // uses const QPoint & center !

  CAAGalileanMoonsDetails GalileanDetails = CAAGalileanMoons::Calculate(date.Julian(), true);

  // draw satellites, x/y *10 because size jup = 10
  if (!(GalileanDetails.Satellite1.bInEclipse || GalileanDetails.Satellite1.bInOccultation ) ) {
      QPointF sat1(GalileanDetails.Satellite1.ApparentRectangularCoordinates.X *10
                   , GalileanDetails.Satellite1.ApparentRectangularCoordinates.Y *10);
      painter.drawEllipse(m.map(sat1),0.25*scale_p,0.25*scale_p);

      sat1 += QPointF(0.5,5);
      painter.drawText(m.map(sat1),"I");
    }

  if (!(GalileanDetails.Satellite2.bInEclipse || GalileanDetails.Satellite2.bInOccultation ) ) {
      QPointF sat2(GalileanDetails.Satellite2.ApparentRectangularCoordinates.X *10
                   , GalileanDetails.Satellite2.ApparentRectangularCoordinates.Y *10);
      painter.drawEllipse(m.map(sat2),0.25*scale_p,0.25*scale_p);

      sat2 += QPointF(0.5,5);
      painter.drawText(m.map(sat2),"II");
    }

  if (!(GalileanDetails.Satellite3.bInEclipse || GalileanDetails.Satellite3.bInOccultation ) ) {
      QPointF sat3(GalileanDetails.Satellite3.ApparentRectangularCoordinates.X *10
                   , GalileanDetails.Satellite3.ApparentRectangularCoordinates.Y *10);
      painter.drawEllipse(m.map(sat3),0.25*scale_p,0.25*scale_p);

      sat3 += QPointF(0.5,5);
      painter.drawText(m.map(sat3),"III");
    }

  if (!(GalileanDetails.Satellite4.bInEclipse || GalileanDetails.Satellite4.bInOccultation ) ) {
      QPointF sat4(GalileanDetails.Satellite4.ApparentRectangularCoordinates.X *10
                   , GalileanDetails.Satellite4.ApparentRectangularCoordinates.Y *10);
      painter.drawEllipse(m.map(sat4),0.25*scale_p,0.25*scale_p);

      sat4 += QPointF(0.5,5);
      painter.drawText(m.map(sat4),"IV");
    }

  // draw shadows
  if ( GalileanDetails.Satellite1.bInShadowTransit ) {
      QPointF sat(GalileanDetails.Satellite1.Shadow_ApparentRectangularCoordinates.X *10
                  , GalileanDetails.Satellite1.Shadow_ApparentRectangularCoordinates.Y *10);
      painter.setBrush(QBrush(Qt::black, Qt::SolidPattern));
      painter.drawEllipse(m.map(sat),0.25*scale_p,0.25*scale_p);

      sat += QPointF(0.5,5);
      painter.drawText(m.map(sat),"sI");
    }

  if ( GalileanDetails.Satellite2.bInShadowTransit ) {
      QPointF sat(GalileanDetails.Satellite2.Shadow_ApparentRectangularCoordinates.X *10
                  , GalileanDetails.Satellite2.Shadow_ApparentRectangularCoordinates.Y *10);
      painter.setBrush(QBrush(Qt::black, Qt::SolidPattern));
      painter.drawEllipse(m.map(sat),0.25*scale_p,0.25*scale_p);

      sat += QPointF(0.5,5);
      painter.drawText(m.map(sat),"sII");
    }

  if ( GalileanDetails.Satellite3.bInShadowTransit ) {
      QPointF sat(GalileanDetails.Satellite3.Shadow_ApparentRectangularCoordinates.X *10
                  , GalileanDetails.Satellite3.Shadow_ApparentRectangularCoordinates.Y *10);
      painter.setBrush(QBrush(Qt::black, Qt::SolidPattern));
      painter.drawEllipse(m.map(sat),0.25*scale_p,0.25*scale_p);

      sat += QPointF(0.5,5);
      painter.drawText(m.map(sat),"sIII");
    }

  if ( GalileanDetails.Satellite4.bInShadowTransit ) {
      QPointF sat(GalileanDetails.Satellite4.Shadow_ApparentRectangularCoordinates.X *10
                  , GalileanDetails.Satellite4.Shadow_ApparentRectangularCoordinates.Y *10);
      painter.setBrush(QBrush(Qt::black, Qt::SolidPattern));
      painter.drawEllipse(m.map(sat),0.25*scale_p,0.25*scale_p);

      sat += QPointF(0.5,5);
      painter.drawText(m.map(sat),"sIV");
    }

  //QPointF a1(15.0,4.0);
  //painter.drawEllipse(m.map(a1),0.25*scale_p,0.25*scale_p); // uses const QPoint & center !
  //QPointF a2(5.0,4.0);
  //painter.setBrush(QBrush(Qt::black, Qt::SolidPattern));
  //painter.drawEllipse(m.map(a2),0.25*scale_p,0.25*scale_p); // uses const QPoint & center !
}

