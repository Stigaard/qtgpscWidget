/**************************************************************************
*   Copyright (C) 2010 by Diego Berge <gpsd@navlost.eu>                   *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 3 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, see <http://www.gnu.org/licenses/>   *
***************************************************************************/

#include <math.h>
#include <QPainter>
#include <QMenu>
#include <QContextMenuEvent>
#include <QApplication>
#include <QClipboard>
#include "satview.h"

Satellite::Satellite(int p, double a, double e,
                     double r, bool u, bool h)
                     : prn(p), azm(a), ele(e),
                     snr(r), used(u), healthy(h)
{
}

Satellite::Satellite()
    : prn(0), azm(0), ele(0),
    snr(0), used(false), healthy(false)
{
}

SatView::SatView(QWidget *parent)
    : QWidget(parent)
{
    actionCopy = new QAction("&Copy to clipboard", this);
    actionCopy->setShortcut(QKeySequence::Copy);
    connect(actionCopy, SIGNAL(triggered()), this, SLOT(copyToClipboard()));
}

void SatView::setSatellites(const SatList &sats)
{
    satellites = sats;
    update();
}

void SatView::paintEvent(QPaintEvent *)
{
    #define DEG2RAD(v) ((v)*3.141592653589793/180.0)
    Satellite sat;
    int radius = qMin(width(), height())-4/*margin*/;
    QPointF centre(0,0);
    
    QPainter painter(this);
    QPen bgCirclesPen(Qt::gray, 1, Qt::DashLine);
    bgCirclesPen.setCosmetic(true);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(bgCirclesPen);
    painter.setBrush(Qt::NoBrush);
    painter.translate(width()/2, height()/2);
    painter.scale(radius/180.0, radius/180.0);
    painter.save();
    painter.drawEllipse(centre, 30, 30);
    painter.drawEllipse(centre, 60, 60);
    painter.drawEllipse(centre, 90, 90);
    painter.drawLine(0, -90, 0, 90);
    painter.drawLine(-90, 0, 90, 0);
    painter.setPen(Qt::darkRed);
    painter.setBrush(Qt::red);
    
    QPainter usedSat(this);
    usedSat.setPen(Qt::darkRed);
    usedSat.setBrush(Qt::red);
    usedSat.setRenderHint(QPainter::Antialiasing, true);
    usedSat.translate(width()/2, height()/2);
    usedSat.scale(radius/180.0, radius/180.0);
    
    QPainter unusedSat(this);
    unusedSat.setPen(Qt::darkRed);
    unusedSat.setBrush(Qt::NoBrush);
    unusedSat.translate(width()/2, height()/2);
    unusedSat.scale(radius/180.0, radius/180.0);
    unusedSat.setRenderHint(QPainter::Antialiasing, true);
    
    QPainter prnText(this);
    QPen satTextPen(Qt::black, 1);
    satTextPen.setCosmetic(true);
    prnText.setPen(satTextPen);
    QFont f;
    f.setPixelSize(8);
    prnText.setFont(f);
    prnText.translate(width()/2, height()/2);
    prnText.scale(radius/180.0, radius/180.0);
    prnText.setRenderHint(QPainter::TextAntialiasing, true);
    
    
    foreach (sat, satellites) {
        double x,y;
        x = cos(DEG2RAD(90-sat.azm))*(90-sat.ele);
        y = -sin(DEG2RAD(90-sat.azm))*(90-sat.ele);
        
        if (sat.used)
            usedSat.drawEllipse(QPointF(x, y), 2.0, 2.0);
        else
            unusedSat.drawEllipse(QPointF(x, y), 2.0, 2.0);
        
        prnText.drawText(x+2, y+2, QString::number(sat.prn));
    }
    painter.restore();
    #undef DEG2RAD
}

void SatView::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);    
    menu.addAction(actionCopy);
    menu.exec(event->globalPos());
}

void SatView::copyToClipboard()
{
    QPixmap pixmap(size());
    render(&pixmap);
    QApplication::clipboard()->setPixmap(pixmap);
}
