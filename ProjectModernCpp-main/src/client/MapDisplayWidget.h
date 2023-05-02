#pragma once

#include "../model/Map.h"
#include "MapDisplayModel.h"
#include "MapGraphics.h"

#include <QGraphicsView>

#include <memory>

class MapDisplayWidget : public QGraphicsView
{
	Q_OBJECT

public:
	MapDisplayWidget(QWidget* parent = nullptr);
	~MapDisplayWidget();

	void setMapDisplayModel(MapDisplayModel* model);
	
protected:
	void resizeEvent(QResizeEvent* event) override;

private:
	MapDisplayModel* m_scene = nullptr;

	friend class MapDisplayModel;
};
