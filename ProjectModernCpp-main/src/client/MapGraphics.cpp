#include "MapGraphics.h"

MapGraphics::MapGraphics(std::istream& in, const QString& mapGraphicsRoot, std::vector<QColor> colors)
{
	// TODO validate read input
	this->colors = std::move(colors);
	QDir rootDir(mapGraphicsRoot);

	std::string name;
	in >> name;

	unsigned int numTerritories;
	in >> numTerritories;

	std::string filePath;
	in >> filePath;
	wholeMap = QPixmap(rootDir.absoluteFilePath(QString::fromUtf8(filePath.c_str(), filePath.size())));

	territories.reserve(numTerritories);
	for (unsigned int i = 0; i < numTerritories; ++i) {
		in >> filePath;
		auto territoryImageFilePath = rootDir.absoluteFilePath(QString::fromUtf8(filePath.c_str(), filePath.size()));
		territories.emplace_back(territoryImageFilePath);
	}

	highlights.reserve(numTerritories);
	for (unsigned int i = 0; i < numTerritories; ++i) {
		in >> filePath;
		auto highlightImageFilePath = rootDir.absoluteFilePath(QString::fromUtf8(filePath.c_str(), filePath.size()));
		highlights.emplace_back(highlightImageFilePath);
	}

	territoryOffsets.reserve(numTerritories);
	highlightOffsets.reserve(numTerritories);
	for (unsigned int i = 0; i < numTerritories; ++i) {
		int x, y;
		in >> x >> y;
		territoryOffsets.push_back(QPoint{ x, y });
		in >> x >> y;
		highlightOffsets.push_back(QPoint{ x, y });
	}

	unsigned int numHitboxVertices;
	in >> numHitboxVertices;
	hitboxCoords.reserve(numHitboxVertices);
	for (unsigned int i = 0; i < numHitboxVertices; ++i) {
		int x, y;
		in >> x >> y;
		hitboxCoords.push_back(QPoint(x, y));
	}
	polygonIndices.resize(numTerritories);
	for (unsigned int i = 0; i < numTerritories; ++i) {
		unsigned int numIndices;
		int index;
		in >> numIndices;
		for (unsigned int j = 0; j < numIndices; ++j) {
			in >> index;
			polygonIndices[i].push_back(index);
		}
	}
}
