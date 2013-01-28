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
#include <QToolTip>
#include "satview.h"
#include "satview.moc"

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
    setMouseTracking(true);
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
    Satellite sat;
    int radius = qMin(width(), height())-4/*margin*/;
    QPointF centre(0,0);
    
    QPainter painter(this);
    painter.setTransform(transform);
    QPen bgCirclesPen(Qt::gray, 0.3, Qt::DashLine);
    bgCirclesPen.setCosmetic(true);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(bgCirclesPen);
    painter.setBrush(Qt::NoBrush);
    painter.save();
    painter.drawEllipse(centre, 30, 30);
    painter.drawEllipse(centre, 60, 60);
    painter.drawEllipse(centre, 90, 90);
    painter.save();
    for (double i=0; i<360.0; i+=45) {
        painter.rotate(i);
        painter.drawLine(0, -90, 0, 90);
    }
    painter.restore();
    painter.setPen(Qt::darkRed);
    painter.setBrush(Qt::red);
    
    QPainter usedSat(this);
    usedSat.setPen(Qt::darkRed);
    usedSat.setBrush(Qt::red);
    usedSat.setRenderHint(QPainter::Antialiasing, true);
    usedSat.setTransform(transform);
    
    QPainter unusedSat(this);
    unusedSat.setPen(Qt::darkRed);
    unusedSat.setBrush(Qt::NoBrush);
    unusedSat.setTransform(transform);
    unusedSat.setRenderHint(QPainter::Antialiasing, true);
    
    QPainter prnText(this);
    QPen satTextPen(Qt::black, 1);
    satTextPen.setCosmetic(true);
    prnText.setPen(satTextPen);
    QFont f;
    f.setPixelSize(8);
    prnText.setFont(f);
    prnText.setTransform(transform);
    prnText.setRenderHint(QPainter::TextAntialiasing, true);
    
    
    foreach (sat, satellites) {
        QPointF pos = polar2rect(sat.azm, sat.ele);
        
        if (sat.used)
            usedSat.drawEllipse(pos, 2.0, 2.0);
        else
            unusedSat.drawEllipse(pos, 2.0, 2.0);
        
        prnText.drawText(pos.x()+2, pos.y()+2, QString::number(sat.prn));
    }
    painter.restore();
}

void SatView::resizeEvent(QResizeEvent *event)
{
    QTransform t;
    int radius = qMin(width(), height())-4/*margin*/;
    t.translate(width()/2, height()/2);
    t.scale(radius/180.0, radius/180.0);
    transform = t;
}

void SatView::mouseMoveEvent(QMouseEvent *event)
{
    QPointF p = rect2polar(transform.inverted().map(event->pos()));
    p.ry() = 90.0 - p.y();
    if (p.y() >= 0) {
        QString t;
        t = QString("Az: %1 El: %2").arg(p.x(), 3, 'f', 0, '0').arg(p.y(), 2, 'f', 0, '0');
        foreach(Satellite sat, satellites) {
            QPointF r = polar2rect(sat.azm, sat.ele);
            r = transform.inverted().map(event->pos()) - r;
            if ((r.x()*r.x() + r.y()*r.y()) < 3) {
                t.append(QString("\nPRN: %1 Az: %2 El: %3 SNR: %4 %5").arg(sat.prn)
                    .arg(sat.azm, 3, 'f', 0, '0').arg(sat.ele, 2, 'f', 0, '0')
                    .arg(sat.snr).arg(sat.used?"Used":"Not used"));
            }
        }
            
        QToolTip::showText(event->globalPos(), t, this);
    } else {
        QToolTip::hideText();
    }
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

#define DEG2RAD(v) ((v)*3.141592653589793/180.0)
#define RAD2DEG(v) ((v)*180.0/3.141592653589793)
QPointF SatView::polar2rect(double azm, double ele)
{
    QPointF r;
    r.rx() = cos(DEG2RAD(90-azm))*(90-ele);
    r.ry() = -sin(DEG2RAD(90-azm))*(90-ele);
    return r;
}

QPointF SatView::rect2polar(double x, double y)
{
    QPointF p;
    p.rx() = RAD2DEG(atan2(x, -y)); // azm
    p.rx() += (p.rx() < 0 ? 360.0 : 0.0);
    p.ry() = sqrt(x*x + y*y); // el;
    return p;
}

QPointF SatView::rect2polar(QPointF r)
{
    return rect2polar(r.x(), r.y());
}

#undef RAD2DEG
#undef DEG2RAD
