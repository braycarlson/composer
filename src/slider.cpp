#include "include/slider.h"


Slider::Slider(QWidget* parent) : QSlider(parent) {}

Slider::Slider(Qt::Orientation orientation, QWidget* parent) : QSlider(orientation, parent) {}

void Slider::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        double ratio = static_cast<double>(event->pos().x()) / width();
        int range = maximum() - minimum();
        int position = static_cast<int>(ratio * range) + minimum();
        this->setValue(position);

        emit this->seek(position);
        QSlider::mousePressEvent(event);
    }
}
