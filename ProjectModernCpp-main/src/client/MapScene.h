#pragma once

#include "MapGraphics.h"

#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsPolygonItem>

class TerritoryImageItem : public QGraphicsPixmapItem
{
public:
	TerritoryImageItem(size_t territoryIndex);
	~TerritoryImageItem();

public:
	void setHighlighted(bool highlighted);

private:
	MapGraphics& mapGraphics();
public:
	size_t territoryIndex;
	bool highlighted = false;
};

class TerritoryHitboxItem : public QGraphicsPolygonItem
{
public:
	TerritoryHitboxItem(size_t territoryIndex);
	~TerritoryHitboxItem();

protected:
	void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
	void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

public:
	size_t territoryIndex;
	TerritoryImageItem* imageItem;
};

class MapScene : public QGraphicsScene
{
	Q_OBJECT

public:
	MapScene(QObject *parent = nullptr);
	~MapScene();

	
};
