#include "MapDisplayWidget.h"

MapDisplayWidget::MapDisplayWidget(QWidget* parent)
	: QGraphicsView(parent)
{}

MapDisplayWidget::~MapDisplayWidget()
{}

void MapDisplayWidget::setMapDisplayModel(MapDisplayModel * model)
{
	m_scene = model;
	setScene(model);
}

void MapDisplayWidget::resizeEvent(QResizeEvent* event)
{
	if (m_scene) {
		fitInView(m_scene->sceneRect(), Qt::KeepAspectRatio);
	}
}
