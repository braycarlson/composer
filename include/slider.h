#ifndef SLIDER_H
#define SLIDER_H

#include <QMouseEvent>
#include <QSlider>


class Slider : public QSlider {
    Q_OBJECT

 public:
    explicit Slider(QWidget* parent = nullptr);
    explicit Slider(Qt::Orientation orientation, QWidget *parent = nullptr);

 protected:
    void mousePressEvent(QMouseEvent*) override;

 signals:
    void seek(int);
};

#endif // SLIDER_H
