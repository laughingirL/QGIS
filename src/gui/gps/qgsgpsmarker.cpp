/***************************************************************************
    qgsgpsmarker.cpp  - canvas item which shows a gps position marker
    ---------------------
    begin                : December 2009
    copyright            : (C) 2009 by Tim Sutton
    email                : tim at linfiniti dot com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QPainter>

#include "qgsgpsmarker.h"
#include "qgscoordinatetransform.h"
#include "qgsmapcanvas.h"
#include "qgsexception.h"
#include "qgsproject.h"

QgsGpsMarker::QgsGpsMarker( QgsMapCanvas *mapCanvas )
  : QgsMapCanvasItem( mapCanvas )
{
  mSize = 16;
  mWgs84CRS = QgsCoordinateReferenceSystem::fromOgcWmsCrs( QStringLiteral( "EPSG:4326" ) );
  mSvg.load( QStringLiteral( ":/images/north_arrows/gpsarrow2.svg" ) );
  if ( ! mSvg.isValid() )
  {
    qDebug( "GPS marker not found!" );
  }
}

void QgsGpsMarker::setSize( int size )
{
  mSize = size;
}

void QgsGpsMarker::setCenter( const QgsPointXY &point )
{
  //transform to map crs
  if ( mMapCanvas )
  {
    QgsCoordinateTransform t( mWgs84CRS, mMapCanvas->mapSettings().destinationCrs(), QgsProject::instance() );
    try
    {
      mCenter = t.transform( point );
    }
    catch ( QgsCsException &e ) //silently ignore transformation exceptions
    {
      Q_UNUSED( e );
      return;
    }
  }
  else
  {
    mCenter = point;
  }

  QPointF pt = toCanvasCoordinates( mCenter );
  setPos( pt );
}

void QgsGpsMarker::setDirection( double direction )
{
  mDirection = direction;
}

void QgsGpsMarker::paint( QPainter *p )
{
  if ( ! mSvg.isValid() )
  {
    return;
  }

  // this needs to be done when the canvas is repainted to make for smoother map rendering
  // if not done the map could be panned, but the cursor position won't be updated until the next valid GPS fix is received
  QPointF pt = toCanvasCoordinates( mCenter );
  setPos( pt );

  p->save();
  p->rotate( mDirection );
  mSvg.render( p, QRectF( - mSize, - mSize, 2 * mSize, 2 * mSize ) );
  p->restore();
}


QRectF QgsGpsMarker::boundingRect() const
{
  float myHalfSize = mSize / 2.0;
  return QRectF( -myHalfSize, -myHalfSize, 2.0 * myHalfSize, 2.0 * myHalfSize );
}

void QgsGpsMarker::updatePosition()
{
  setCenter( mCenter );
}
