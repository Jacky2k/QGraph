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

#ifndef QGRAPH_H
#define QGRAPH_H

#include <QMainWindow>
#include <QVector>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QWidget>
#include <QImage>
#include <QRectF>
#include <QRect>
#include <QSize>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QPoint>
#include <QMenu>
#include <QFont>

class QGraph : public QWidget
{
    Q_OBJECT
public:
    explicit QGraph(QWidget *parent = 0);
    virtual ~QGraph();

    enum GraphStyle {
        Line,
        Bar,
        Stem
    };

    struct LineInfo {
        QVector<double> xData;
        QVector<double> yData;
        QPen pen;
        QBrush brush;
        GraphStyle style;
        double barWidth;
    };

    void setAntializing(bool antializing);
    void setGrid(bool grid);
    void clearData();
    void setData(QVector<double> xData, QVector<double> yData, GraphStyle style = Line, double barWidth = 0.9, QPen pen = QPen(Qt::black), QBrush brush = QBrush(Qt::transparent));
    void setData(QVector< QVector<double> > xData, QVector< QVector<double> > yData, QVector<GraphStyle> styles = QVector<GraphStyle>(), QVector<double> barWidths = QVector<double>(), QVector<QPen> pens = QVector<QPen>(), QVector<QBrush> brushes = QVector<QBrush>());
    void appendData(QVector<double> xData, QVector<double> yData, GraphStyle style = Line, double barWidth = 0.9, QPen pen = QPen(Qt::black), QBrush brush = QBrush(Qt::transparent));

    void useLimit(bool limitedX, bool limitedY);
    void useZoomLimit(bool zoomLimit);
    void limitX(double xmin, double xmax);
    void limitY(double ymin, double ymax);

    void refresh();
    void setAutoRefresh(bool autoRefresh);

    void setRightClickMenu(bool menu);
    bool getRightClickMenu();

    void setTitle(QString title, QFont font = QFont("Helvetica", 22), QPen pen = QPen(Qt::black, Qt::SolidLine));
    QString getTitle();

    void setXLabel(QString xLabel, QFont font = QFont(), QPen pen = QPen(Qt::black, Qt::SolidLine));
    QString getXLabel();

    void setYLabel(QString yLabel, QFont font = QFont(), QPen pen = QPen(Qt::black, Qt::SolidLine));
    QString getYLabel();

    void setUndertitle(QString undertitle, QFont font = QFont(), QPen pen = QPen(Qt::black, Qt::SolidLine));
    QString getUndertitle();

    void setXNumbersEnabled(bool xNumbersEnabled);
    void setYNumbersEnabled(bool yNumbersEnabled);

    void setLeftBorder(int leftBorder);
    void setTopBorder(int topBorder);
    void setRightBorder(int rightBorder);
    void setBottomBorder(int bottomBorder);
    void setDefaultBorder();
    void setNoBorder();

protected:
    void dataMinMax();
    void xyPoints();
    void repaint();
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseDoubleClickEvent(QMouseEvent*);
    void wheelEvent(QWheelEvent* event);
    void resizeEvent(QResizeEvent* event);
    void paintEvent(QPaintEvent* );
    void keyPressEvent(QKeyEvent* event);
    void insertLines();
    void calcPoints(QVector<double>& points, double min, double max);
    void checkZoomLimit();
    int src2dstX(double srcX);
    int src2dstY(double srcY);
    double dst2srcX(int dstX);
    double dst2srcY(int dstY);
    double dst2srcW(int dstW);
    double dst2srcH(int dstH);
    void textSize();
    void calcDstRect();
    void updatePanning();
    void findGraphAt(QPoint pos);

    QGraphicsScene* scene;
    QImage graphImage;
    QRectF srcRect;
    QRect dstRect;
    QFont axisLabelFont;

    QVector<LineInfo> lines;

    bool autoRefresh;
    bool antializing;
    bool grid;
    bool limitedX, limitedY;
    bool zoomLimit;

    bool rightClickMenu;
    QMenu menu;
    QAction* menuSavePicture;
    QAction* menuGrid;
    QAction* menuAntializing;
    QAction* menuTitle;
    QAction* menuUndertitle;
    QAction* menuXLabel;
    QAction* menuYLabel;
    QAction* menuXNumbers;
    QAction* menuYNumbers;
    QAction* menuNoBorder;
    QAction* menuDefaultBorder;

    double dataMinX, dataMinY, dataMaxX, dataMaxY;
    QVector<double> xPoints;
    QVector<double> yPoints;

    bool zooming;
    QRect zoomRect;
    bool panning;
    QPoint panStart;
    QPoint panCurrent;

    bool tracking;
    int trackingSet;
    int trackingIndex;

    int sizeYLabel, sizeXLabel, sizeTitle, sizeYNumbers, sizeXNumbers, sizeUndertitle;

    bool titleEnabled;
    QString title;
    QFont titleFont;
    QPen titlePen;

    bool xLabelEnabled;
    QString xLabel;
    QFont xLabelFont;
    QPen xLabelPen;

    bool yLabelEnabled;
    QString yLabel;
    QFont yLabelFont;
    QPen yLabelPen;

    bool undertitleEnabled;
    QString undertitle;
    QFont undertitleFont;
    QPen undertitlePen;

    bool xNumbersEnabled;
    bool yNumbersEnabled;

    int leftBorder, topBorder, rightBorder, bottomBorder;
    
private slots:
    void onMenuGrid(bool grid);
    void onMenuAntializing(bool antializing);
    void onMenuSavePicture();
    void onMenuTitle(bool enableTitle);
    void onMenuUndertitle(bool enableUndertitle);
    void onMenuXLabel(bool enableXLabel);
    void onMenuYLabel(bool enableYLabel);
    void onMenuXNumbers(bool enableXNumbers);
    void onMenuYNumbers(bool enableYNumbers);
    void onMenuNoBorder();
    void onMenuDefaultBorder();
};

#endif // QGRAPH_H
