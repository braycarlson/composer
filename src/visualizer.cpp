#include "include/visualizer.h"


Visualizer::Visualizer(QWidget* parent) : QOpenGLWidget(parent), m_player(nullptr) {
    m_probe = new QAudioProbe(this);

    if (m_probe->setSource(m_player)) {
        this->connect(m_probe, &QAudioProbe::audioBufferProbed, this, &Visualizer::onProcessAudioBuffer);
    }
}

void Visualizer::initializeGL() {
    this->initializeOpenGLFunctions();

    QPalette palette = this->palette();
    QColor background = palette.color(QPalette::Window);
    float r = background.redF();
    float g = background.greenF();
    float b = background.blueF();

    this->glClearColor(r, g, b, 1.0f);
}

void Visualizer::onSongFinished() {
    this->level.clear();
    this->update();
}

void Visualizer::onProcessAudioBuffer(const QAudioBuffer& buffer) {
    this->level.clear();

    const qint16* data = buffer.constData<qint16>();
    int count = buffer.sampleCount();

    const int size = 100;
    int samples = 0;

    double sum = 0;

    for (int i = 0; i < count; ++i) {
        sum = sum + qAbs(data[i]) / 32768.0;
        samples++;

        if (samples == size || i == count - 1) {
            double avg = sum / samples;
            this->level.append(avg);
            sum = 0;
            samples = 0;
        }
    }

    this->update();
}

void Visualizer::paintGL() {
    this->glClear(GL_COLOR_BUFFER_BIT);

    QPalette palette = this->palette();
    QColor background = palette.color(QPalette::Window);
    float r = background.redF();
    float g = background.greenF();
    float b = background.blueF();

    this->glClearColor(r, g, b, 1.0f);

    QPainter painter(this);
    QPainterPath path;

    QColor color = palette.color(QPalette::Highlight);
    QPen pen(color);
    pen.setWidth(3);

    painter.setPen(pen);

    if (!this->level.isEmpty()) {
        int w = width();
        int h = height();

        float step = w / static_cast<float>(this->level.size());

        double offset = h * 0.15;
        path.moveTo(0, (h / 2) + offset);

        for (int i = 0; i < this->level.size(); ++i) {
            path.lineTo(i * step, (h / 2) - (this->level[i] * h / 2) + offset);
        }

        painter.drawPath(path);
    }

    this->update();
}

void Visualizer::resizeGL(int width, int height) {
    this->glViewport(0, 0, width, height);
}

void Visualizer::setPlayer(QMediaPlayer* player) {
    if (m_player == player) return;

    if (m_player) {
        this->disconnect(m_probe, nullptr, this, nullptr);
    }

    m_player = player;

    if (m_player) {
        if (!m_probe->setSource(m_player)) {
            qWarning() << "Failed to set probe source.";
        }

        this->connect(m_probe, &QAudioProbe::audioBufferProbed, this, &Visualizer::onProcessAudioBuffer);
    }
}
