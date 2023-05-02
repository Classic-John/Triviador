#pragma once

#include <QPixmap>
#include <QString>
#include <QPoint>
#include <QDir>

#include <vector>
#include <istream>
#include <string>

struct MapGraphics
{
	QPixmap wholeMap;
	std::vector<QPixmap> territories;
	std::vector<QPixmap> highlights;
	std::vector<QPoint> territoryOffsets;
	std::vector<QPoint> highlightOffsets;
	std::vector<QColor> colors;
	std::vector<QPoint> hitboxCoords;
	std::vector<std::vector<size_t>> polygonIndices;

	MapGraphics(std::istream& in, const QString& mapGraphicsRoot, std::vector<QColor> colors = {});
};

