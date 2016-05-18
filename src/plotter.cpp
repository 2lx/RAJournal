/*
 * plotter.cpp
 *
 *  Created on: 28.09.2011
 *      Author: Aim
 */

#include "stdafx.h"
#include "plotter.h"

#include "qwt_curve_fitter.h"
#include <qwt_plot_histogram.h>
#include <qwt_point_data.h>
#include "qwt_legend.h"
#include "qwt_math.h"
#include "qwt_plot.h"
#include "qwt_plot_curve.h"
#include "qwt_plot_grid.h"
#include "qwt_series_data.h"
#include "qwt_scale_engine.h"
#include "qwt_symbol.h"
#include "qwt_text.h"

class FunctionData: public QwtSyntheticPointData
{
public:
	FunctionData( double( *y )( double ) ) : QwtSyntheticPointData( 200 ), d_y( y ) {}
	virtual double y( double x ) const { return d_y( x ); }

private:
	double( *d_y )( double );
};

double kA = 1, kB = 2;

double fun( double x )
{
	return ( x < 6 ) ? kB : kA*x + kB;
}

//////////////////////////////////////////////////////////////////////////

BodePlot::BodePlot( QWidget *parent ):
    QwtPlot( parent )
{
	setAutoReplot(false);

    setCanvasBackground(QColor(Qt::white));

    QwtPlotGrid *grid = new QwtPlotGrid;
    grid->enableXMin(true);
    grid->setMajorPen(QPen(Qt::black, 0, Qt::DotLine));
    grid->setMinorPen(QPen(Qt::gray, 0 , Qt::DotLine));
    grid->attach(this);

    setAutoReplot(true);
}

void BodePlot::clearPlot()
{
//	while ( !lstCurves.isEmpty() )
//	{
//		lstCurves.first()->detach();
//		lstCurves.removeFirst();
//	}
	while ( !lstHists.isEmpty() )
	{
		lstHists.first()->detach();
		lstHists.removeFirst();
	}
}

void BodePlot::drawGraphic(  const double start, const double koeff, const int len  )
{
//	const bool doReplot = autoReplot();
    setAutoReplot(false);

	setAxisScale( QwtPlot::xBottom, 0, len );
	setAxisScale( QwtPlot::yLeft, start - 100, start * ( 1.0 + koeff/100.0 ) + 20 );
	kA = koeff/len/100.0 + 1.0;
	kB = start;

/*
    QwtPlotCurve * curve = new QwtPlotCurve();
	curve->setRenderHint(QwtPlotItem::RenderAntialiased);
	curve->setStyle(QwtPlotCurve::Lines);
	curve->setPen( QPen( Qt::red, 2 ) );

	curve->setData( new FunctionData( fun ) );
	lstCurves.append( curve );
	curve->attach( this );
*/
	QwtPlotHistogram * hist = new QwtPlotHistogram();
	hist->setStyle( QwtPlotHistogram::Columns );
	hist->setBrush( Qt::green );
	QVector< QwtIntervalSample > samples( len );
	for ( int i = 0; i < len; i++ )
	{
		QwtInterval interval( double(i), i + 1.0 );
		interval.setBorderFlags( QwtInterval::ExcludeMaximum );

		samples[ i ] = QwtIntervalSample( fun(i), interval);
	}
	hist->setData( new QwtIntervalSeriesData( samples ) );
	lstHists.append( hist );
	hist->attach( this );

    setAutoReplot( true );
    replot();	
}
