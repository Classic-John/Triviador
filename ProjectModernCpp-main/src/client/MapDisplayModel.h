#pragma once

#include "MapGraphics.h"
#include "../model/Map.h"

#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsPolygonItem>
#include <QGraphicsSceneMouseEvent>

#include <set>

class TerritoryImageItem : public QGraphicsPixmapItem
{
public:
	TerritoryImageItem(size_t territoryIndex, const QPixmap& defaultImg, const QPixmap& highlightedImg,
		const QPoint& defaultOffset, const QPoint& highlightOffset);
	~TerritoryImageItem();

public:
	void setHighlighted(bool highlighted);
	void updateTerritory();

private:
	bool selectionEnabled();
	bool territorySelectable();
public:
	QGraphicsSimpleTextItem* scoreItem;
	QColor color{ Qt::black };
	size_t territoryIndex;
	QPixmap defaultImg;
	QPixmap highlightedImg;
	QPoint defaultOffset;
	QPoint highlightOffset;
	bool highlighted = false;
};

class TerritoryHitboxItem : public QGraphicsPolygonItem
{
public:
	TerritoryHitboxItem(TerritoryImageItem* imageItem);
	~TerritoryHitboxItem();

protected:
	void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
	void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

public:
	TerritoryImageItem* imageItem;
};

class MapDisplayModel : public QGraphicsScene
{
	Q_OBJECT

public:
	MapDisplayModel(QObject *parent = nullptr);
	~MapDisplayModel();

	bool selectionEnabled() const;
	void setMap(std::shared_ptr<model::Map> map, std::shared_ptr<MapGraphics> mapGraphics);
signals:
	void territorySelected(model::Territory& territory);

public slots:
	void updateTerritory(const model::Territory& territory, size_t colorIndex);
	void setSelectionEnabled(bool enabled);
	void setSelectableTerritories(std::set<size_t> territories);
protected:
	void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

private:
	void rebuildScene();

private:
	std::shared_ptr<model::Map> m_map;
	std::shared_ptr<MapGraphics> m_mapGraphics;
	std::vector<TerritoryImageItem*> m_territoryItems;
	bool m_selectionEnabled;
	std::set<size_t> m_selectableTerritories;

	friend TerritoryImageItem;
};
