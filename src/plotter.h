/*
 * plotter.h
 *
 *  Created on: 28.09.2011
 *      Author: Aim
 */

#ifndef PLOTTER_H
#define PLOTTER_H

#include "qwt_plot.h"

class QwtPlotHistogram;

class BodePlot: public QwtPlot
{
public:
	BodePlot(QWidget *parent = 0);
	virtual ~BodePlot() { clearPlot(); }

public:
	void clearPlot();
	void drawGraphic( const double start, const double koeff, const int len );

private:
//	QList< QwtPlotCurve * > lstCurves;
	QList< QwtPlotHistogram * > lstHists;
};

#endif // PLOTTER_H
