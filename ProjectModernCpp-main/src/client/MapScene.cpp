#include "MapScene.h"
#include "MapDisplayWidget.h"

#include <QGraphicsColorizeEffect>

MapScene::MapScene(QObject* parent)
	: QGraphicsScene(parent)
{}

MapScene::~MapScene()
{}

TerritoryImageItem::TerritoryImageItem(size_t territoryIndex)
	: territoryIndex(territoryIndex)
{
	setAcceptHoverEvents(true);
}

TerritoryImageItem::~TerritoryImageItem()
{
}

void TerritoryImageItem::setHighlighted(bool highlighted)
{
	this->highlighted = highlighted;
	// TODO change my look only if the territories are selectable
	if(highlighted)
		setPixmap(mapGraphics().highlights[territoryIndex]);
	else
		setPixmap(mapGraphics().territories[territoryIndex]);
	auto effect = new QGraphicsColorizeEffect;
	effect->setColor(Qt::blue);
	setGraphicsEffect(effect);
}

MapGraphics& TerritoryImageItem::mapGraphics()
{
	return *static_cast<MapDisplayWidget*>(static_cast<MapScene*>(scene())->views()[0])->m_mapGraphics;
}

TerritoryHitboxItem::TerritoryHitboxItem(size_t territoryIndex)
	: territoryIndex(territoryIndex)
{
	setAcceptHoverEvents(true);
}

TerritoryHitboxItem::~TerritoryHitboxItem()
{
}

void TerritoryHitboxItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
	imageItem->setHighlighted(true);
}

void TerritoryHitboxItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
	imageItem->setHighlighted(false);
}
