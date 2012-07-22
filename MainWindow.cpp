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

#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QMessageBox>
#include <cmath>
#include <iostream>

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setGeometry(geometry().x(), geometry().y(), 800, 600);
    graph = new QGraph(this);
    graph->setAutoRefresh(false);
    graph->show();

    graph->clearData();
    QVector<double> x;
    for(double tmpx=-1; tmpx<=1; tmpx+=0.1)
        x<<tmpx;
    cout<<x.size()<<endl;
    QVector<double> y;
    y.clear();
    for(int i=0; i<x.size(); i++)
        y<<x[i]*x[i]+0.3;
    graph->appendData(x, y, QGraph::Line, 0.9, QPen(Qt::red), QBrush(Qt::black));

    y.clear();
    for(int i=0; i<x.size(); i++)
        y<<x[i]*x[i]*x[i]*x[i]+0.2;
    graph->appendData(x, y, QGraph::Stem, 0.9, QPen(Qt::green), QBrush(Qt::black));

    y.clear();
    for(int i=0; i<x.size(); i++)
        y<<x[i]*x[i]*x[i]*x[i]*x[i]*x[i]+0.1;
    graph->appendData(x, y, QGraph::Bar, 0.8, QPen(Qt::blue), QBrush(Qt::transparent));

    graph->update();
    graph->show();
    graph->setGrid(true);
    graph->setTitle("Test title");
    graph->setYLabel("y Axis");
    graph->setXLabel("x Axis");
    graph->setUndertitle("This is what an undertitle looks like. You can write here any text you may use to describe the graphs above. \nAs you can see the text will automatically wrap and newlines are supported.");
    graph->refresh();
}

MainWindow::~MainWindow()
{

}

void MainWindow::resizeEvent(QResizeEvent*)
{
    graph->setGeometry(50, 50, width()-100, height()-100);
}
