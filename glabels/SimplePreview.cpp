/*  SimplePreview.cpp
 *
 *  Copyright (C) 2013-2016  Jim Evins <evins@snaught.com>
 *
 *  This file is part of gLabels-qt.
 *
 *  gLabels-qt is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  gLabels-qt is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with gLabels-qt.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "SimplePreview.h"


#include <QGraphicsRectItem>
#include <QGraphicsDropShadowEffect>
#include <QtDebug>


//
// Private Configuration Data
//
namespace
{
        const QColor  paperColor( 255, 255, 255 );
        const QColor  paperOutlineColor( 0, 0, 0 );
	const double  paperOutlineWidthPixels = 1;

        const QColor  shadowColor( 64, 64, 64 );
	const double  shadowOffsetPixels = 3;
	const double  shadowRadiusPixels = 12;

        const QColor  labelColor( 255, 255, 255 );
        const QColor  labelOutlineColor( 128, 128, 255 );
	const double  labelOutlineWidthPixels = 2;

	const QColor  arrowColor( 192, 192, 255, 128 );
	const double  arrowScale = 0.35;

	const QColor  upColor( 192, 192, 255, 128 );
	const double  upScale = 0.15;
	const QString upFontFamily( "Sans" );
}


///
/// Constructor
///
SimplePreview::SimplePreview( QWidget *parent )
	: mTmplate(NULL), mRotateFlag(false), QGraphicsView(parent)
{
	mScene = new QGraphicsScene();
	setScene( mScene );

	setAttribute(Qt::WA_TranslucentBackground);
	viewport()->setAutoFillBackground(false);

	setFrameStyle( QFrame::NoFrame );
	setRenderHints( QPainter::Antialiasing );
}


///
/// Template Property Setter
///
void SimplePreview::setTemplate( const glabels::Template *tmplate )
{
	mTmplate = tmplate;
	update();
}


///
/// Rotate Property Setter
///
void SimplePreview::setRotate( bool rotateFlag )
{
	mRotateFlag = rotateFlag;
	update();
}


///
/// Resize Event Handler
///
void SimplePreview::resizeEvent( QResizeEvent* event )
{
	fitInView( mScene->sceneRect(), Qt::KeepAspectRatio );
}


///
/// Update View
///
void SimplePreview::update()
{
	clearScene();

	if ( mTmplate != NULL )
	{
		// Set scene up with a 5% margin around paper
		glabels::Distance x = -0.05 * mTmplate->pageWidth();
		glabels::Distance y = -0.05 * mTmplate->pageHeight();
		glabels::Distance w = 1.10 * mTmplate->pageWidth();
		glabels::Distance h = 1.10 * mTmplate->pageHeight();

		mScene->setSceneRect( x.pt(), y.pt(), w.pt(), h.pt() );
		fitInView( mScene->sceneRect(), Qt::KeepAspectRatio );

		drawPaper( mTmplate->pageWidth(), mTmplate->pageHeight() );
		drawLabels();
		drawArrow();
	}
}


///
/// Clear View
///
void SimplePreview::clearScene()
{
	foreach ( QGraphicsItem *item, mScene->items() )
	{
		mScene->removeItem( item );
		delete item;
	}
}


///
/// Draw Paper
///
void SimplePreview::drawPaper( const glabels::Distance& pw, const glabels::Distance& ph )
{
	QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect();
	shadowEffect->setColor( shadowColor );
	shadowEffect->setOffset( shadowOffsetPixels );
	shadowEffect->setBlurRadius( shadowRadiusPixels );

	QBrush brush( paperColor );
	QPen pen( paperOutlineColor );
	pen.setCosmetic( true );
	pen.setWidthF( paperOutlineWidthPixels );

	QGraphicsRectItem *pageItem = new QGraphicsRectItem( 0, 0, pw.pt(), ph.pt() );
	pageItem->setBrush( brush );
	pageItem->setPen( pen );
	pageItem->setGraphicsEffect( shadowEffect );
				
	mScene->addItem( pageItem );
}


///
/// Draw Labels on Paper
///
void SimplePreview::drawLabels()
{
	glabels::Frame *frame = mTmplate->frames().first();

	foreach (glabels::Point origin, frame->getOrigins() )
	{
		drawLabel( origin.x(), origin.y(), frame->path() );
	}
}


///
/// Draw a Single Label at x,y
///
void SimplePreview::drawLabel( const glabels::Distance& x,
			       const glabels::Distance& y,
			       const QPainterPath&      path )
{
	QBrush brush( labelColor );
	QPen pen( labelOutlineColor );
	pen.setCosmetic( true );
	pen.setWidthF( labelOutlineWidthPixels );

	QGraphicsPathItem *labelItem  = new QGraphicsPathItem( path );
	labelItem->setBrush( brush );
	labelItem->setPen( pen );
	labelItem->setPos( x.pt(), y.pt() );

	mScene->addItem( labelItem );
}


///
/// Draw Arrow Indicating Top of First Label
///
void SimplePreview::drawArrow()
{
	glabels::Frame *frame = mTmplate->frames().first();

	glabels::Distance w = frame->w();
	glabels::Distance h = frame->h();

	glabels::Distance min = glabels::min( w, h );

	QPen pen( arrowColor );
	pen.setWidthF( 0.25*min.pt()*arrowScale );
	pen.setCapStyle( Qt::FlatCap );
	pen.setJoinStyle( Qt::MiterJoin );

	QBrush brush( upColor );

	glabels::Point origin = frame->getOrigins().first();
	glabels::Distance x0 = origin.x();
	glabels::Distance y0 = origin.y();

	QPainterPath path;
	path.moveTo( 0,                       min.pt()*arrowScale/3 );
	path.lineTo( 0,                      -min.pt()*arrowScale   );
	path.moveTo( -min.pt()*arrowScale/2, -min.pt()*arrowScale/2 );
	path.lineTo( 0,                      -min.pt()*arrowScale   );
	path.lineTo(  min.pt()*arrowScale/2, -min.pt()*arrowScale/2 );

	QGraphicsPathItem *arrowItem = new QGraphicsPathItem( path );
	arrowItem->setPen( pen );
	arrowItem->setPos( (x0+w/2).pt(), (y0+h/2).pt() );
	if ( mRotateFlag )
	{
		arrowItem->setRotation( -90 );
	}

	QGraphicsSimpleTextItem *upItem = new QGraphicsSimpleTextItem( tr("Up") );
	upItem->setBrush( brush );
	upItem->setFont( QFont( upFontFamily, min.pt()*upScale, QFont::Bold ) );
	upItem->setPos( (x0+w/2).pt(), (y0+h/2).pt() );
	QRectF rect = upItem->boundingRect();
	if ( mRotateFlag )
	{
		upItem->setPos( upItem->x()+min.pt()/8, upItem->y()+rect.width()/2 );
		upItem->setRotation( -90 );
	}
	else
	{
		upItem->setPos( upItem->x()-rect.width()/2, upItem->y()+min.pt()/8 );
	}

	mScene->addItem( arrowItem );
	mScene->addItem( upItem );
}
