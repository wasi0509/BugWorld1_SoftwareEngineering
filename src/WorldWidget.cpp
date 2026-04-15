#include "WorldWidget.h"
#include <QPainter>
#include <cmath>
#include <QColor>

// Constructor
WorldWidget::WorldWidget(QWidget *parent)
    : QWidget(parent)
{
    setMinimumSize(800, 600);
}

// Update world state and add to trace history
void WorldWidget::setWorldState(const WorldState &state)
{
    world = state;

    std::vector<QPair<int, int>> redPositions;
    std::vector<QPair<int, int>> blackPositions;

    for (int r = 0; r < world.rows; ++r)
    {
        if (r >= (int)world.mapData.size())
            break;

        const std::string &row = world.mapData[r];

        for (int c = 0; c < world.cols && c < (int)row.size(); ++c)
        {
            char cell = row[c];

            if (cell == 'R' || cell == 'r')
            {
                redPositions.push_back(qMakePair(r, c));
            }
            else if (cell == 'B' || cell == 'b')
            {
                blackPositions.push_back(qMakePair(r, c));
            }
        }
    }

    // Add new frame positions to trace history
    redTraces.push_front(redPositions);
    blackTraces.push_front(blackPositions);

    // Keep only traceLength frames
    while ((int)redTraces.size() > traceLength)
        redTraces.pop_back();

    while ((int)blackTraces.size() > traceLength)
        blackTraces.pop_back();

    update();
}

//  Set how many past frames to keep in trace history
void WorldWidget::setTraceLength(int length)
{
    traceLength = length;

    while ((int)redTraces.size() > traceLength)
        redTraces.pop_back();

    while ((int)blackTraces.size() > traceLength)
        blackTraces.pop_back();

    update();
}

// Override paint event to draw the world and traces
QPointF WorldWidget::hexToPixel(int row, int col) const
{
    double size = hexSize;
    double width = size * 2;
    double height = std::sqrt(3) * size;

    double x = col * (width * 0.75);
    double y = row * height + (col % 2 ? height / 2 : 0);

    return QPointF(x + 50, y + 50);
}

// Override paint event to draw the world and traces
void WorldWidget::paintEvent(QPaintEvent *)
{
    if (world.rows == 0 || world.mapData.empty())
        return;

    // Auto-scale hex size to fit window
    double maxWidth = width() - 100;
    double maxHeight = height() - 100;

    double sizeByWidth = maxWidth / (world.cols * 1.5);
    double sizeByHeight = maxHeight / (world.rows * std::sqrt(3));

    hexSize = std::min(sizeByWidth, sizeByHeight);

    // Clamp to reasonable minimum
    if (hexSize < 3)
        hexSize = 3;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), Qt::white);

    double size = hexSize;
    QPolygonF hex;
    for (int i = 0; i < 6; ++i)
    {
        double angle = M_PI / 3 * i - M_PI / 6; // start at -30 degrees for pointy top
        hex << QPointF(size * std::cos(angle), size * std::sin(angle));
    }

    // Draw the world map based on world.mapData and bug positions
    for (int r = 0; r < world.rows; ++r)
    {
        if (r >= (int)world.mapData.size())
            break;
        const std::string &row = world.mapData[r];

        for (int c = 0; c < world.cols && c < (int)row.size(); ++c)
        {
            char cell = row[c];
            QPointF center = hexToPixel(r, c);
            QColor color = Qt::white;

            switch (cell)
            {
            case '#':
                color = Qt::gray;
                break;
            case 'R':
                color = Qt::red;
                break;
            case 'r':
                color = Qt::red;
                break;
            case 'B':
                color = Qt::black;
                break;
            case 'b':
                color = Qt::black;
                break;
            case '+':
                color = QColor(255, 150, 150);
                break;
            case '-':
                color = QColor(150, 150, 255);
                break;
            default:
                if (isdigit(cell))
                    color = Qt::green;
                break;
            }

            QPolygonF shape;
            for (const QPointF &pt : hex)
                shape << QPointF(center.x() + pt.x(), center.y() + pt.y());

            painter.setBrush(color);
            painter.setPen(QPen(Qt::lightGray, 1));
            painter.drawPolygon(shape);
        }
    }

    // Draw traces with fading effect
    for (size_t i = 0; i < redTraces.size(); ++i)
    {
        // Calculate fading alpha (transparency)
        int alpha = traceLength > 0 ? 255 - (i * 255 / traceLength) : 0;
        if (alpha < 0)
            alpha = 0;

        painter.setBrush(QColor(255, 0, 0, alpha));
        painter.setPen(Qt::NoPen);

        for (const auto &cell : redTraces[i])
        {
            QPointF pt = hexToPixel(cell.first, cell.second);
            painter.drawEllipse(pt, hexSize / 3, hexSize / 3);
        }
    }
    for (size_t i = 0; i < blackTraces.size(); ++i)
    {
        int alpha = traceLength > 0 ? 255 - (i * 255 / traceLength) : 0;
        if (alpha < 0)
            alpha = 0;

        painter.setBrush(QColor(0, 0, 0, alpha));
        painter.setPen(Qt::NoPen);

        for (const auto &cell : blackTraces[i])
        {
            QPointF pt = hexToPixel(cell.first, cell.second);
            painter.drawEllipse(pt, hexSize / 3, hexSize / 3);
        }
    }
    if (!winnerText.isEmpty())
    {
        painter.setPen(Qt::darkGreen);
        QFont font = painter.font();
        font.setPointSize(30);
        font.setBold(true);
        painter.setFont(font);

        painter.drawText(rect(), Qt::AlignCenter, winnerText);
    }
}

// Set winner text to display on the widget
void WorldWidget::setWinner(const QString &text)
{
    winnerText = text;
    update();
}

// Clear all trace history and update display
void WorldWidget::clearTraces()
{
    redTraces.clear();
    blackTraces.clear();
    update();
}