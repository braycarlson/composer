#ifndef VISUALIZER_H
#define VISUALIZER_H

#include <QAudioProbe>
#include <QElapsedTimer>
#include <QMediaPlayer>
#include <QOpenGLFunctions>
#include <QOpenGLWidget>
#include <QPainter>
#include <QPainterPath>
#include <QWidget>


class Visualizer : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT

 public:
    explicit Visualizer(QWidget* parent = nullptr);

    void setPlayer(QMediaPlayer*);

 protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int, int) override;

 public slots:
    void onSongFinished();

 private slots:
    void onProcessAudioBuffer(const QAudioBuffer&);

 private:
    QMediaPlayer* m_player;
    QAudioProbe* m_probe;
    QVector<float> level;
};

#endif // VISUALIZER_H
