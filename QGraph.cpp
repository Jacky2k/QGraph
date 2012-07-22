/*
    (c) Copyright 2012 by Fabian Schwartau

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
  To use QGraph simply copy QGraph.h and QGraph.cpp in your project
  and include QGraph.h.
 */

#include "QGraph.h"

#include <QPainter>
#include <iostream>
#include <cmath>
#include <QFileDialog>
#include <QSvgGenerator>
#include <QFontMetrics>
#include <algorithm>
#include <limits>

using namespace std;

QGraph::QGraph(QWidget *parent) :
    QWidget(parent),
    autoRefresh(true),
    antializing(true),
    grid(false),
    limitedX(false),
    limitedY(false),
    zoomLimit(true),
    rightClickMenu(true),
    zooming(false),
    panning(false),
    tracking(false),
    sizeYLabel(0),
    sizeXLabel(0),
    sizeTitle(0),
    sizeYNumbers(0),
    sizeXNumbers(0),
    sizeUndertitle(0),
    titleEnabled(false),
    xLabelEnabled(false),
    yLabelEnabled(false),
    undertitleEnabled(false),
    xNumbersEnabled(true),
    yNumbersEnabled(true),
    leftBorder(10),
    topBorder(10),
    rightBorder(10),
    bottomBorder(10)
{
    scene = new QGraphicsScene();
    scene->addLine(0,10,10,10);
    graphImage = QImage(400, 300, QImage::Format_ARGB32);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    menuSavePicture = menu.addAction(tr("&Save Picture"));
    connect(menuSavePicture, SIGNAL(triggered()), this, SLOT(onMenuSavePicture()));

    menu.addSeparator();

    menuGrid = menu.addAction(tr("&Grid"));
    menuGrid->setCheckable(true);
    menuGrid->setChecked(grid);
    connect(menuGrid, SIGNAL(triggered(bool)), this, SLOT(onMenuGrid(bool)));

    menuAntializing = menu.addAction(tr("&Antializing"));
    menuAntializing->setCheckable(true);
    menuAntializing->setChecked(antializing);
    connect(menuAntializing, SIGNAL(triggered(bool)), this, SLOT(onMenuAntializing(bool)));

    menuTitle = menu.addAction(tr("&Title"));
    menuTitle->setCheckable(true);
    menuTitle->setChecked(false);
    connect(menuTitle, SIGNAL(triggered(bool)), this, SLOT(onMenuTitle(bool)));

    menuUndertitle = menu.addAction(tr("&Undertitle"));
    menuUndertitle->setCheckable(true);
    menuUndertitle->setChecked(false);
    connect(menuUndertitle, SIGNAL(triggered(bool)), this, SLOT(onMenuUndertitle(bool)));

    menuXLabel = menu.addAction(tr("&X Label"));
    menuXLabel->setCheckable(true);
    menuXLabel->setChecked(false);
    connect(menuXLabel, SIGNAL(triggered(bool)), this, SLOT(onMenuXLabel(bool)));

    menuYLabel = menu.addAction(tr("&Y Label"));
    menuYLabel->setCheckable(true);
    menuYLabel->setChecked(false);
    connect(menuYLabel, SIGNAL(triggered(bool)), this, SLOT(onMenuYLabel(bool)));

    menuXNumbers = menu.addAction(tr("&X Numbers"));
    menuXNumbers->setCheckable(true);
    menuXNumbers->setChecked(xNumbersEnabled);
    connect(menuXNumbers, SIGNAL(triggered(bool)), this, SLOT(onMenuXNumbers(bool)));

    menuYNumbers = menu.addAction(tr("&Y Numbers"));
    menuYNumbers->setCheckable(true);
    menuYNumbers->setChecked(yNumbersEnabled);
    connect(menuYNumbers, SIGNAL(triggered(bool)), this, SLOT(onMenuYNumbers(bool)));

    menu.addSeparator();

    menuNoBorder = menu.addAction(tr("&No Border"));
    connect(menuNoBorder, SIGNAL(triggered()), this, SLOT(onMenuNoBorder()));

    menuDefaultBorder = menu.addAction(tr("&Default Border"));
    connect(menuDefaultBorder, SIGNAL(triggered()), this, SLOT(onMenuDefaultBorder()));
}

QGraph::~QGraph()
{

}

void QGraph::setAntializing(bool antializing)
{
    this->antializing = antializing;
    if(autoRefresh)
    {
        repaint();
        update();
    }
}

void QGraph::clearData()
{
    lines.clear();
    scene->clear();
}

void QGraph::setData(QVector<double> xData, QVector<double> yData, GraphStyle style, double barWidth, QPen pen, QBrush brush)
{
    clearData();
    appendData(xData, yData, style, barWidth, pen, brush);
}

void QGraph::setData(QVector< QVector<double> > xData, QVector< QVector<double> > yData, QVector<GraphStyle> styles, QVector<double> barWidths, QVector<QPen> pens, QVector<QBrush> brushes)
{
    lines.clear();
    for(int set=0; set<xData.size(); set++)
    {
        LineInfo line;
        line.xData = xData[set];
        line.yData = yData[set];
        if(styles.size() != xData.size())
            line.style =Line;
        else
            line.style = Line;

        if(barWidths.size() != xData.size())
            line.barWidth = barWidths[set];
        else
            line.barWidth = 0.9;

        if(pens.size() != xData.size())
            line.pen = QPen(Qt::black);
        else
            line.pen = pens[set];

        if(brushes.size() != xData.size())
            line.brush = QBrush(Qt::transparent);
        else
            line.brush = brushes[set];
    }
    if(autoRefresh)
    {
        dataMinMax();
        insertLines();
    }
}

void QGraph::appendData(QVector<double> xData, QVector<double> yData, GraphStyle style, double barWidth, QPen pen, QBrush brush)
{
    LineInfo line;
    line.xData = xData;
    line.yData = yData;
    line.style = style;
    line.barWidth = barWidth;
    line.pen = pen;
    line.brush = brush;
    lines.push_back(line);
    if(autoRefresh)
    {
        dataMinMax();
        insertLines();
    }
}

void QGraph::useLimit(bool limitedX, bool limitedY)
{
    this->limitedX = limitedX;
    this->limitedY = limitedY;
    if(autoRefresh)
        dataMinMax();
}

void QGraph::useZoomLimit(bool zoomLimit)
{
    this->zoomLimit =  zoomLimit;
    if(autoRefresh)
    {
        dataMinMax();
        insertLines();
    }
}

void QGraph::limitX(double xmin, double xmax)
{
    limitedX = true;
    dataMinX = xmin;
    dataMaxX = xmax;
    if(autoRefresh)
    {
        dataMinMax();
        insertLines();
    }
}

void QGraph::limitY(double ymin, double ymax)
{
    limitedY = true;
    dataMinY = ymin;
    dataMaxY = ymax;
    if(autoRefresh)
    {
        dataMinMax();
        insertLines();
    }
}

void QGraph::refresh()
{
    dataMinMax();
    textSize();
    insertLines();
    update();
}

/**
  \fn void QGraph::setAutoRefresh(bool autoRefresh)
  Sets the variable autoRefresh.
  This variable controls if the graph should be automatically refreshed
  when it is required. For exaple when you add data with appenData() it
  might be unefficient to redraw the whole graph because you want to add
  more data. You can call refresh() to manually update the graph.
 **/
void QGraph::setAutoRefresh(bool autoRefresh)
{
    this->autoRefresh = autoRefresh;
}

void QGraph::setRightClickMenu(bool menu)
{
    rightClickMenu = menu;
}

bool QGraph::getRightClickMenu()
{
    return rightClickMenu;
}

void QGraph::setTitle(QString title, QFont font, QPen pen)
{
    this->titleFont = font;
    this->title = title;
    this->titlePen = pen;
    if(title.isEmpty())
        titleEnabled = false;
    else
        titleEnabled = true;
    menuTitle->setChecked(titleEnabled);
    if(autoRefresh)
    {
        repaint();
        update();
    }
}

QString QGraph::getTitle()
{
    return title;
}

void QGraph::setXLabel(QString xLabel, QFont font, QPen pen)
{
    this->xLabel = xLabel;
    this->xLabelFont = font;
    this->xLabelPen = pen;
    if(xLabel.isEmpty())
        xLabelEnabled = false;
    else
        xLabelEnabled = true;
    menuXLabel->setChecked(xLabelEnabled);
    if(autoRefresh)
    {
        repaint();
        update();
    }
}

QString QGraph::getXLabel()
{
    return xLabel;
}

void QGraph::setYLabel(QString yLabel, QFont font, QPen pen)
{
    this->yLabel = yLabel;
    this->yLabelFont = font;
    this->yLabelPen = pen;
    if(yLabel.isEmpty())
        yLabelEnabled = false;
    else
        yLabelEnabled = true;
    menuYLabel->setChecked(yLabelEnabled);
    if(autoRefresh)
    {
        repaint();
        update();
    }
}

QString QGraph::getYLabel()
{
    return yLabel;
}

void QGraph::setUndertitle(QString undertitle, QFont font, QPen pen)
{
    this->undertitle = undertitle;
    this->undertitleFont = font;
    this->undertitlePen = pen;
    if(undertitle.isEmpty())
        undertitleEnabled = false;
    else
        undertitleEnabled = true;
    menuUndertitle->setChecked(undertitleEnabled);
    if(autoRefresh)
    {
        repaint();
        update();
    }
}

QString QGraph::getUndertitle()
{
    return undertitle;
}

void QGraph::setXNumbersEnabled(bool xNumbersEnabled)
{
    this->xNumbersEnabled = xNumbersEnabled;
    if(autoRefresh)
    {
        repaint();
        update();
    }
}

void QGraph::setYNumbersEnabled(bool yNumbersEnabled)
{
    this->yNumbersEnabled = yNumbersEnabled;
    if(autoRefresh)
    {
        repaint();
        update();
    }
}

void QGraph::setLeftBorder(int leftBorder)
{
    this->leftBorder = leftBorder;
}

void QGraph::setTopBorder(int topBorder)
{
    this->topBorder = topBorder;
}

void QGraph::setRightBorder(int rightBorder)
{
    this->rightBorder = rightBorder;
}

void QGraph::setBottomBorder(int bottomBorder)
{
    this->bottomBorder = bottomBorder;
}

void QGraph::setDefaultBorder()
{
    leftBorder = 10;
    topBorder = 10;
    rightBorder = 10;
    bottomBorder = 10;
}

void QGraph::setNoBorder()
{
    leftBorder = 0;
    topBorder = 0;
    rightBorder = 0;
    bottomBorder = 0;
}


void QGraph::repaint()
{
    // Fill image white
    graphImage.fill(Qt::white);

    // Generate destination rect and mirror y coordinate
    QPainter painter(&graphImage);

    // Turn off antializing
    painter.setRenderHint(QPainter::NonCosmeticDefaultPen);

    // Draw surrounding rect
    painter.setBrush(Qt::transparent);
    painter.setPen(Qt::black);
    painter.drawRect(dstRect);

    // Draw the coordinate lines
    for(int i=0; i<xPoints.size(); i++)
    {
        painter.drawLine(src2dstX(xPoints[i]), dstRect.y(), src2dstX(xPoints[i]), dstRect.y()-7);
        painter.drawLine(src2dstX(xPoints[i]), dstRect.y()+dstRect.height(), src2dstX(xPoints[i]), dstRect.y()+dstRect.height()+7);
    }
    for(int i=0; i<yPoints.size(); i++)
    {
        painter.drawLine(dstRect.x(), src2dstY(yPoints[i]), dstRect.x()+7, src2dstY(yPoints[i]));
        painter.drawLine(dstRect.x()+dstRect.width()-7, src2dstY(yPoints[i]), dstRect.x()+dstRect.width(), src2dstY(yPoints[i]));
    }

    // Draw the values for the coordinate lines
    if(xNumbersEnabled)
    {
        for(int i=0; i<xPoints.size(); i++)
        {
            painter.drawText(QRectF(src2dstX(xPoints[i])-40, dstRect.y()+5, 80, 20), Qt::AlignCenter, QString::number(xPoints[i]));
        }
    }
    if(yNumbersEnabled)
    {
        for(int i=0; i<yPoints.size(); i++)
        {
            painter.drawText(QRectF(dstRect.x()-sizeYNumbers - 5, src2dstY(yPoints[i])-8, sizeYNumbers, 20), Qt::AlignRight, QString::number(yPoints[i]));
        }
    }

    // Draw the grid
    if(grid)
    {
        painter.setPen(QPen(Qt::black, 1, Qt::DotLine));
        for(int i=0; i<xPoints.size(); i++)
        {
            painter.drawLine(src2dstX(xPoints[i]), dstRect.y(), src2dstX(xPoints[i]), dstRect.y()+dstRect.height());
        }
        for(int i=0; i<yPoints.size(); i++)
        {
            painter.drawLine(dstRect.x(), src2dstY(yPoints[i]), dstRect.x()+dstRect.width(), src2dstY(yPoints[i]));
        }
        painter.setPen(QPen(Qt::black, Qt::SolidLine));
    }

    // Draw the title
    if(!title.isEmpty() && titleEnabled)
    {
        QFont oldFont = painter.font();
        painter.setFont(titleFont);
        painter.setPen(titlePen);
        painter.drawText(QRectF(0, 5, graphImage.width(), sizeTitle), Qt::AlignCenter | Qt::TextWordWrap, title);
        painter.setFont(oldFont);
    }

    // Draw the undertitle
    if(!undertitle.isEmpty() && undertitleEnabled)
    {
        QFont oldFont = painter.font();
        painter.setFont(undertitleFont);
        painter.setPen(undertitlePen);
        painter.drawText(QRectF(0, graphImage.height()-sizeUndertitle, graphImage.width(), sizeUndertitle), Qt::AlignCenter | Qt::TextWordWrap, undertitle);
        painter.setFont(oldFont);
    }

    // Draw the y label
    if(!yLabel.isEmpty() && yLabelEnabled)
    {
        QFont oldFont = painter.font();
        painter.setFont(yLabelFont);
        painter.setPen(yLabelPen);
        painter.rotate(-90);
        painter.drawText(QRectF(-dstRect.y(), leftBorder, -dstRect.height(), sizeYLabel), Qt::AlignCenter | Qt::TextWordWrap, yLabel);
        painter.rotate(90);
        painter.setFont(oldFont);
    }

    // Draw the x label
    if(!xLabel.isEmpty() && xLabelEnabled)
    {
        QFont oldFont = painter.font();
        painter.setFont(xLabelFont);
        painter.setPen(xLabelPen);
        painter.drawText(QRectF(dstRect.x(), graphImage.height()-sizeXLabel - sizeUndertitle - bottomBorder, dstRect.width(), sizeXLabel), Qt::AlignCenter | Qt::TextWordWrap, xLabel);
        painter.setFont(oldFont);
    }

    // Turn on antializing if required
    painter.setRenderHint(QPainter::Antialiasing, antializing);

    // Render the graph
    scene->render(&painter, dstRect, srcRect, Qt::IgnoreAspectRatio);

    // Turn off antializing
    painter.setRenderHint(QPainter::Antialiasing, false);

    // Draw tracking point
    if(tracking)
    {
        int tracX = src2dstX(lines[trackingSet].xData[trackingIndex]);
        int tracY = src2dstY(lines[trackingSet].yData[trackingIndex]);
        painter.setPen(Qt::black);
        //painter.drawRect(tracX-7, tracY-7, 15, 15);
        painter.drawLine(tracX+1, tracY, tracX+10, tracY);
        painter.drawLine(tracX-1, tracY, tracX-10, tracY);
        painter.drawLine(tracX, tracY+1, tracX, tracY+10);
        painter.drawLine(tracX, tracY-1, tracX, tracY-10);

        double posX = lines[trackingSet].xData[trackingIndex];
        double posY = lines[trackingSet].yData[trackingIndex];
        painter.drawText(QRect(dstRect.x()+20, dstRect.y()+dstRect.height()+20, 200, 20), tr("X: ")+QString::number(posX));
        painter.drawText(QRect(dstRect.x()+20, dstRect.y()+dstRect.height()+40, 200, 20), tr("Y: ")+QString::number(posY));
    }

    // Draw zoom rect
    if(zooming)
    {
        painter.drawRect(zoomRect);
    }

    // Finish
    painter.end();
}

void QGraph::dataMinMax()
{
    // Make sure an area is defined when no data is given
    if(!limitedX)
    {
        dataMinX=0.0;
        dataMaxX=1.0;
    }
    if(!limitedX)
    {
        dataMinY=0.0;
        dataMaxY=1.0;
    }
    bool gotElement = false;
    for(int set=0; set<lines.size(); set++)
    {
        if(!limitedX && lines[set].xData.size())
        {
            dataMinX = lines[set].xData[0];
            dataMaxX = dataMinX;
            for(int i=1; i<lines[set].xData.size(); i++)
            {
                if(lines[set].xData[i]<dataMinX)
                    dataMinX = lines[set].xData[i];
                if(lines[set].xData[i]>dataMaxX)
                    dataMaxX = lines[set].xData[i];
            }
        }
        if(!limitedY && lines[set].yData.size())
        {
            for(int i=0; i<lines[set].yData.size(); i++)
            {
                if(limitedX && (lines[set].xData[i]<dataMinX || lines[set].xData[i]>dataMaxX))
                    continue;
                if(!gotElement)
                {
                    dataMinY = lines[set].yData[i];
                    dataMaxY = lines[set].yData[i];
                    gotElement = true;
                }
                else
                {
                    if(!gotElement || lines[set].yData[i]<dataMinY)
                        dataMinY = lines[set].yData[i];
                    if(!gotElement || lines[set].yData[i]>dataMaxY)
                        dataMaxY = lines[set].yData[i];
                }
            }
        }
    }
    srcRect = QRectF(dataMinX, dataMinY, dataMaxX-dataMinX, dataMaxY-dataMinY);
}

void QGraph::xyPoints()
{
    // Calculate the points
    calcPoints(xPoints, srcRect.x(), srcRect.x()+srcRect.width());
    calcPoints(yPoints, srcRect.y(), srcRect.y()+srcRect.height());
}

void QGraph::insertLines()
{
    textSize();
    scene->clear();
    for(int set=0; set<lines.size(); set++)
    {
        switch(lines[set].style)
        {
        case Line:
        {
            QPainterPath path(QPointF(lines[set].xData[0], lines[set].yData[0]));
            for(int i=1; i<lines[set].xData.size(); i++)
                path.lineTo(lines[set].xData[i], lines[set].yData[i]);
            scene->addPath(path, lines[set].pen);
        }
            break;
        case Bar:
        {
            for(int i=0; i<lines[set].xData.size(); i++)
            {
                double width;
                if(i==0)
                    width = lines[set].xData[0]-lines[set].xData[1];
                else
                    width = lines[set].xData[i]-lines[set].xData[i-1];
                width *= lines[set].barWidth;
                scene->addRect(lines[set].xData[i]-width/2, lines[set].yData[i], width, -lines[set].yData[i], lines[set].pen, lines[set].brush);
            }
        }
            break;
        case Stem:
        {
            for(int i=0; i<lines[set].xData.size(); i++)
            {
                scene->addLine(lines[set].xData[i], lines[set].yData[i], lines[set].xData[i], 0, lines[set].pen);
                scene->addEllipse(lines[set].xData[i]-dst2srcW(9)/2, lines[set].yData[i]-dst2srcH(9)/2, dst2srcW(9), dst2srcH(9), lines[set].pen);
            }
        }
            break;
        }

    }
    xyPoints();
    repaint();
}

void QGraph::calcPoints(QVector<double>& points, double min, double max)
{
    double dxIdeal = (max-min)/4.0;
    double dx = exp(floor(log(dxIdeal)/log(10.0))*log(10.0));
    double diff = log(dxIdeal)/log(10.0)-floor(log(dxIdeal)/log(10.0));
    if(diff > 0.69897)
        dx *= 5;
    else if(diff > 0.30103)
        dx *= 2;

    double start = ceil(min/dx);
    double end = floor(max/dx);
    points.clear();
    for(double i = start; i<=end; i++)
        points.push_back(dx*i);
}

void QGraph::checkZoomLimit()
{
    if(zoomLimit)
    {
        if(limitedX && srcRect.x()<dataMinX)
            srcRect.setX(dataMinX);
        if(limitedX && srcRect.x()+srcRect.width()>dataMaxX)
            srcRect.setWidth(dataMaxX-srcRect.x());
        if(limitedY && srcRect.y()<dataMinY)
            srcRect.setY(dataMinY);
        if(limitedY && srcRect.y()+srcRect.height()>dataMaxY)
            srcRect.setHeight(dataMaxY-srcRect.y());
    }
}

void QGraph::mousePressEvent(QMouseEvent* event)
{
    if(event->button() == Qt::LeftButton)
    {
        zooming = true;
        zoomRect = QRect(event->x(), event->y(), 0, 0);
    }
    else if(event->button() == Qt::RightButton)
    {
        if(rightClickMenu)
        {
            menu.popup(QCursor::pos());
        }
    }
    else if(event->button() == Qt::MiddleButton)
    {
        panning = true;
        panStart = QCursor::pos();
        panCurrent = panStart;
    }
}

void QGraph::mouseMoveEvent(QMouseEvent* event)
{
    if(zooming)
    {
        zoomRect.setWidth(event->x()-zoomRect.x());
        zoomRect.setHeight(event->y()-zoomRect.y());
        repaint();
        update();
    }
    if(panning)
    {
        updatePanning();
    }
}

void QGraph::mouseReleaseEvent(QMouseEvent* event)
{
    if(event->button() == Qt::LeftButton && zooming)
    {
        zoomRect.setWidth(event->x()-zoomRect.x());
        zoomRect.setHeight(event->y()-zoomRect.y());
        zooming = false;
        int zoomWidth = zoomRect.width();
        int zoomX = zoomRect.x();
        int zoomHeight = zoomRect.height();
        int zoomY = zoomRect.y();
        if(zoomWidth==0 && zoomHeight==0)
        {
            // this is a normal click, so find nearest data and highlight it
            findGraphAt(QPoint(zoomX, zoomY));
        }
        else if(zoomWidth==0 || zoomHeight==0)
            return;
        else
        {
            if(zoomWidth<0)
            {
                zoomWidth = -zoomWidth;
                zoomX = zoomX-zoomWidth;
            }
            if(zoomHeight>0)
            {
                zoomHeight = -zoomHeight;
                zoomY = zoomY-zoomHeight;
            }
            srcRect = QRectF(dst2srcX(zoomX), dst2srcY(zoomY), dst2srcW(zoomWidth), dst2srcH(zoomHeight));
            checkZoomLimit();

            insertLines();
            update();
        }
    }
    if(event->button() == Qt::MiddleButton && panning)
    {
        updatePanning();
        panning = false;
    }
}

void QGraph::mouseDoubleClickEvent(QMouseEvent*)
{
    srcRect = QRectF(dataMinX, dataMinY, dataMaxX-dataMinX, dataMaxY-dataMinY);
    insertLines();
    update();
}

void QGraph::wheelEvent(QWheelEvent* event)
{
    int delta = event->delta();
    double factor = (double)delta/240;
    if(factor<0)
        factor = -1/factor;
    double zoomW = srcRect.width();
    double zoomH = srcRect.height();
    double zoomX = srcRect.x();
    double zoomY = srcRect.y();
    double zoomToX = dst2srcX(event->x());
    double zoomToY = dst2srcY(event->y());
    zoomX = zoomToX + (zoomX-zoomToX)*factor;
    zoomY = zoomToY + (zoomY-zoomToY)*factor;
    zoomW *= factor;
    zoomH *= factor;
    if(zoomW == 0.0 || zoomH == 0.0)
        return;
    srcRect = QRectF(zoomX, zoomY, zoomW, zoomH);

    checkZoomLimit();

    insertLines();
    update();
}

void QGraph::resizeEvent(QResizeEvent* event)
{
    graphImage = QImage(event->size().width(), event->size().height(), QImage::Format_ARGB32);
    calcDstRect();
    insertLines();
}

void QGraph::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.drawImage(0, 0, graphImage);
    painter.drawRect(0, 0, width()-1, height()-1);
    painter.end();
    //cout<<"Paint event"<<endl;
}

void QGraph::keyPressEvent(QKeyEvent* event)
{
    int dx = 0;
    if(event->key() == Qt::Key_Left)
        dx = -1;
    else if(event->key() == Qt::Key_Right)
        dx = 1;
    if(dx == 0 || !tracking)
    {
        QWidget::keyPressEvent(event);
        return;
    }
    if(trackingIndex + dx < 0 || trackingIndex + dx >= lines[trackingSet].xData.size())
        return;
    trackingIndex += dx;
    repaint();
    update();
}

int QGraph::src2dstX(double srcX)
{
    return qRound((srcX-srcRect.x())/srcRect.width()*dstRect.width()+dstRect.x());
}

int QGraph::src2dstY(double srcY)
{
    return qRound((srcY-srcRect.y())/srcRect.height()*dstRect.height()+dstRect.y());
}

void QGraph::setGrid(bool grid)
{
    this->grid = grid;
    menuGrid->setChecked(grid);
    if(autoRefresh)
    {
        repaint();
        update();
    }
}

double QGraph::dst2srcX(int dstX)
{
    return (dstX-dstRect.x())*srcRect.width()/dstRect.width()+srcRect.x();
}

double QGraph::dst2srcY(int dstY)
{
    return (dstY-dstRect.y())*srcRect.height()/dstRect.height()+srcRect.y();
}

double QGraph::dst2srcW(int dstW)
{
    return dstW*srcRect.width()/dstRect.width();
}

double QGraph::dst2srcH(int dstH)
{
    return dstH*srcRect.height()/dstRect.height();
}

void QGraph::textSize()
{
    sizeYNumbers = 0;
    sizeXNumbers = 0;
    sizeYLabel = 0;
    sizeXLabel = 0;
    sizeUndertitle = 0;
    sizeTitle = 0;

    QFontMetrics fmAxis(axisLabelFont);
    if(yNumbersEnabled)
    {
        for(int i=0; i<yPoints.size(); i++)
        {
            if(fmAxis.width(QString::number(yPoints[i])) + 10 > sizeYNumbers)
                sizeYNumbers = fmAxis.width(QString::number(yPoints[i])) + 10;
        }
    }

    if(xNumbersEnabled)
        sizeXNumbers = fmAxis.height() + 10;

    if(title.isEmpty() || !titleEnabled)
        sizeTitle = 0;
    else
    {
        QFontMetrics fmTitle(titleFont);
        sizeTitle = fmTitle.boundingRect(0, 0, graphImage.width(), 500, Qt::AlignCenter | Qt::TextWordWrap, title).height() + 10;
    }

    if(undertitle.isEmpty() || !undertitleEnabled)
        sizeUndertitle = 0;
    else
    {
        QFontMetrics fmundertitle(undertitleFont);
        sizeUndertitle = fmundertitle.boundingRect(0, 0, graphImage.width(), 500, Qt::AlignCenter | Qt::TextWordWrap, undertitle).height() + 10;
    }

    if(xLabel.isEmpty() || !xLabelEnabled)
        sizeXLabel = 0;
    else
    {
        QFontMetrics fmTitle(xLabelFont);
        sizeXLabel = fmTitle.boundingRect(0, 0, graphImage.width(), 500, Qt::AlignCenter | Qt::TextWordWrap, xLabel).height() + 10;
    }

    // TODO: sizeUndertitle missing!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    // The current dstRect needs to be calculated in order to use it for the y axis
    calcDstRect();

    if(yLabel.isEmpty() || !yLabelEnabled)
        sizeYLabel = 0;
    else
    {
        QFontMetrics fmTitle(yLabelFont);
        sizeYLabel = fmTitle.boundingRect(0, 0, -dstRect.height(), 500, Qt::AlignCenter | Qt::TextWordWrap, yLabel).height() + 10;
    }

    // calculate the final dstRect
    calcDstRect();
}

void QGraph::calcDstRect()
{
    /*int xSpace = sizeYLabel + sizeYNumbers + 50;
    int ySpace = sizeXLabel + sizeXNumbers + sizeTitle + sizeUndertitle + 35;
    dstRect = QRect(sizeYLabel + sizeYNumbers + 15, graphImage.height() - (sizeXNumbers + sizeXLabel + sizeUndertitle + 20), graphImage.width()-xSpace, -(graphImage.height() - ySpace));*/

    int left = sizeYLabel + sizeYNumbers + leftBorder;
    int top = sizeTitle + topBorder;
    int right = rightBorder;
    int bottom = sizeXNumbers + sizeXLabel + sizeUndertitle + bottomBorder;
    int w = graphImage.width();
    int h = graphImage.height();

    dstRect = QRect(left, h - bottom, w - left - right, -(h - top - bottom));
}

void QGraph::updatePanning()
{
    panCurrent = QCursor::pos();
    double dx = dst2srcW(panCurrent.x()-panStart.x());
    double dy = dst2srcH(panCurrent.y()-panStart.y());
    if(zoomLimit)
    {
        if(limitedX && srcRect.x()-dx<dataMinX)
            dx = srcRect.x() - dataMinX;
        if(limitedX && srcRect.x()-dx+srcRect.width()>dataMaxX)
            dx = srcRect.x() + srcRect.width() - dataMaxX;
        if(limitedY && srcRect.y()-dy<dataMinY)
            dy = srcRect.y() - dataMinY;
        if(limitedY && srcRect.y()-dy+srcRect.height()>dataMaxY)
            dy = srcRect.y() + srcRect.height() - dataMaxY;
    }
    srcRect.setX(srcRect.x()-dx);
    srcRect.setY(srcRect.y()-dy);
    srcRect.setWidth(srcRect.width()-dx);
    srcRect.setHeight(srcRect.height()-dy);
    insertLines();
    update();
    panStart = panCurrent;
}

 void QGraph::findGraphAt(QPoint pos)
 {
    if(lines.size() == 0)
        return;
    double clickX = dst2srcX(pos.x());
    double clickY = dst2srcY(pos.y());

    double minDist = numeric_limits<double>::infinity();
    int minSet = -1;
    int minIndex = -1;

    for(int set=0; set<lines.size(); set++)
    {
        QVector<double>::Iterator newMin = std::lower_bound(lines[set].xData.begin(), lines[set].xData.end(), clickX);
        if(newMin-1 != lines[set].xData.end() && abs(*newMin-clickX) > abs(*(newMin-1)-clickX))
            newMin--;
        int index = std::distance(lines[set].xData.begin(), newMin);
        if(index >= lines[set].xData.size())
            index = lines[set].xData.size() - 1;
        double dx = lines[set].xData[index]-clickX;
        double dy = lines[set].yData[index]-clickY;
        double newDist = sqrt(dx*dx+dy*dy);
        if(newDist < minDist)
        {
            minDist = newDist;
            minSet = set;
            minIndex = index;
        }
    }
    if(minSet == -1 || minIndex == -1)
        return;

    if(minDist > sqrt(srcRect.width()*srcRect.width()+srcRect.height()*srcRect.height())/20)
    {
        tracking = false;
        repaint();
        update();
        return;
    }

    tracking = true;
    trackingSet = minSet;
    trackingIndex = minIndex;
    repaint();
    update();
 }

void QGraph::onMenuGrid(bool grid)
{
    this->grid = grid;
    repaint();
    update();
}

void QGraph::onMenuAntializing(bool antializing)
{
    this->antializing = antializing;
    repaint();
    update();
}

void QGraph::onMenuSavePicture()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save file"), tr("."), "PNG (*.png);;JPG (*.jpg);;SVG (*.svg)");
    if(fileName.isEmpty())
        return;
    if(fileName.toLower().endsWith(".svg"))
    {
        QSvgGenerator generator;
        generator.setFileName("bla.svg");
        generator.setSize(QSize(dstRect.width(), -dstRect.height()));
        generator.setViewBox(QRect(0, 0, dstRect.width(), -dstRect.height()));
        generator.setTitle(tr("SVG Generator Example Drawing"));
        generator.setDescription(tr("An SVG drawing created by the SVG Generator Example provided with Qt."));

        QPainter painter;
        painter.begin(&generator);
        scene->render(&painter, QRect(0, 0, dstRect.width(), -dstRect.height()), srcRect, Qt::IgnoreAspectRatio);
        painter.end();
    }
    else
    {
        graphImage.save(fileName);
    }
}

void QGraph::onMenuTitle(bool enableTitle)
{
    if(title.isEmpty())
        menuTitle->setChecked(false);
    else
    {
        this->titleEnabled = enableTitle;
        insertLines();
        update();
    }
}

void QGraph::onMenuUndertitle(bool enableUndertitle)
{
    if(undertitle.isEmpty())
        menuUndertitle->setChecked(false);
    else
    {
        this->undertitleEnabled = enableUndertitle;
        insertLines();
        update();
    }
}

void QGraph::onMenuXLabel(bool enableXLabel)
{
    if(xLabel.isEmpty())
        menuXLabel->setChecked(false);
    else
    {
        this->xLabelEnabled = enableXLabel;
        insertLines();
        update();
    }
}

void QGraph::onMenuYLabel(bool enableYLabel)
{
    if(yLabel.isEmpty())
        menuYLabel->setChecked(false);
    {
        this->yLabelEnabled = enableYLabel;
        insertLines();
        update();
    }
}

void QGraph::onMenuXNumbers(bool enableXNumbers)
{
    this->xNumbersEnabled = enableXNumbers;
    insertLines();
    update();
}

void QGraph::onMenuYNumbers(bool enableYNumbers)
{
    this->yNumbersEnabled = enableYNumbers;
    insertLines();
    update();
}

void QGraph::onMenuNoBorder()
{
    setNoBorder();
    insertLines();
    update();
}

void QGraph::onMenuDefaultBorder()
{
    setDefaultBorder();
    insertLines();
    update();
}
