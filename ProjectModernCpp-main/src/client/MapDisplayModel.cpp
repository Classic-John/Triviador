#include "MapDisplayModel.h"
#include "MapDisplayWidget.h"

#include <QGraphicsColorizeEffect>

MapDisplayModel::MapDisplayModel(QObject* parent)
	: QGraphicsScene(parent), m_selectionEnabled(true)
{
}

MapDisplayModel::~MapDisplayModel()
{}

bool MapDisplayModel::selectionEnabled() const
{
	return m_selectionEnabled;
}

void MapDisplayModel::setSelectionEnabled(bool enabled)
{
	for (auto& item : m_territoryItems)
		item->setHighlighted(false);
	m_selectionEnabled = enabled;
}

void MapDisplayModel::setSelectableTerritories(std::set<size_t> territories)
{
	m_selectableTerritories = std::move(territories);
}

void MapDisplayModel::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	if (!m_selectionEnabled)
		return;
	
	qDebug() << "Clicked MapModel";
	auto itemsAtCursor = items(event->scenePos());
	for (const auto& item : itemsAtCursor) {
		qDebug() << "Click position has items";
		if (auto hitboxItem = qgraphicsitem_cast<TerritoryHitboxItem*>(item)) {
			qDebug() << "Hitbox item";
			if (m_selectableTerritories.find(hitboxItem->imageItem->territoryIndex) != m_selectableTerritories.end()) {
				qDebug() << "emitted signal with " << hitboxItem->imageItem->territoryIndex;
				emit territorySelected((*m_map)[hitboxItem->imageItem->territoryIndex]);
				return;
			}
		}
	}
}

TerritoryImageItem::TerritoryImageItem(size_t territoryIndex, const QPixmap& defaultImg, const QPixmap& highlightedImg,
	const QPoint& defaultOffset, const QPoint& highlightOffset)
	: territoryIndex(territoryIndex), defaultImg(defaultImg), highlightedImg(highlightedImg),
	defaultOffset(defaultOffset), highlightOffset(highlightOffset)
{
	scoreItem = new QGraphicsSimpleTextItem(this);
	scoreItem->setFont(QFont{ "Arial", 20, 20 });
	scoreItem->setText("0");
	scoreItem->setBrush(Qt::lightGray);
}

TerritoryImageItem::~TerritoryImageItem()
{
}

void TerritoryImageItem::updateTerritory()
{
	setHighlighted(highlighted);
}

void TerritoryImageItem::setHighlighted(bool highlighted)
{
	if (!selectionEnabled() || !territorySelectable())
		highlighted = false;
	this->highlighted = highlighted;
	// TODO change my look only if the territories are selectable
	QFontMetrics fm(scoreItem->font());
	if (highlighted) {
		setPixmap(highlightedImg);
		setPos(highlightOffset);
		auto size = boundingRect().size();
		auto cx = (size.width() - fm.horizontalAdvance(scoreItem->text())) / 2;
		auto cy = (size.height() - fm.height()) / 2;
		scoreItem->setPos(cx, cy);
	}
	else {
		setPixmap(defaultImg);
		setPos(defaultOffset);
		auto size = boundingRect().size();
		auto cx = (size.width() - fm.horizontalAdvance(scoreItem->text())) / 2;
		auto cy = (size.height() - fm.height()) / 2;
		scoreItem->setPos(cx, cy);
	}
	auto effect = new QGraphicsColorizeEffect;
	effect->setColor(color);
	setGraphicsEffect(effect);
}

bool TerritoryImageItem::selectionEnabled()
{
	return static_cast<MapDisplayModel*>(scene())->selectionEnabled();
}

bool TerritoryImageItem::territorySelectable()
{
	return static_cast<MapDisplayModel*>(scene())->m_selectableTerritories.contains(territoryIndex);
}

TerritoryHitboxItem::TerritoryHitboxItem(TerritoryImageItem* imageItem)
	: imageItem(imageItem)
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

void MapDisplayModel::setMap(std::shared_ptr<model::Map> map, std::shared_ptr<MapGraphics> mapGraphics)
{
	assert(map->size() == mapGraphics->territories.size());
	assert(map->size() == mapGraphics->highlights.size());
	assert(map->size() == mapGraphics->polygonIndices.size());
	m_map = map;
	m_mapGraphics = mapGraphics;
	rebuildScene();
}

QPolygon makeHitboxPolygon(const std::vector<QPoint>& vertexCoords, const std::vector<size_t>& indices)
{
	QPolygon polygon;
	for (const auto& index : indices)
		polygon << vertexCoords[index];
	return polygon;
}

void MapDisplayModel::rebuildScene()
{
	// Clear and reset existing members
	m_territoryItems.clear();
	m_territoryItems.resize(m_map->size());
	// Add background map image
	auto background = addPixmap(m_mapGraphics->wholeMap);
	// Add territory images shown as black by default (unselected) and hitboxes
	for (const auto& territory : *m_map) {
		auto index = territory.index();
		auto imageItem = new TerritoryImageItem(index, m_mapGraphics->territories[index], m_mapGraphics->highlights[index],
			m_mapGraphics->territoryOffsets[index], m_mapGraphics->highlightOffsets[index]);
		auto hitboxItem = new TerritoryHitboxItem(imageItem);
		m_territoryItems[index] = imageItem;
		addItem(imageItem);
		addItem(hitboxItem);
		imageItem->setHighlighted(false);
		hitboxItem->setPolygon(makeHitboxPolygon(m_mapGraphics->hitboxCoords, m_mapGraphics->polygonIndices[index]));
		hitboxItem->stackBefore(background);
	}
}

void MapDisplayModel::updateTerritory(const model::Territory& territory, size_t colorIndex)
{
	auto index = territory.index();
	m_territoryItems[index]->color = m_mapGraphics->colors[colorIndex];
	m_territoryItems[index]->scoreItem->setText(QString::number(territory.score()));
	m_territoryItems[index]->updateTerritory();
}