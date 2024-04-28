#ifndef WINDOW_H
#define WINDOW_H

#include <QApplication>
#include <QDateTime>
#include <QDesktopWidget>
#include <QDir>
#include <QFile>
#include <QGuiApplication>
#include <QHeaderView>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QListWidget>
#include <QMainWindow>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QMessageBox>
#include <QPoint>
#include <QPushButton>
#include <QScreen>
#include <QSlider>
#include <QSplitter>
#include <QStackedWidget>
#include <QTableWidget>
#include <QVBoxLayout>

#include "include/control.h"
#include "include/menu.h"
#include "include/palette.h"
#include "include/playlist.h"
#include "include/table.h"
#include "include/theme.h"
#include "include/visualizer.h"


class Window : public QMainWindow {
    Q_OBJECT

 public:
    explicit Window(QWidget* parent = nullptr);
    virtual ~Window();

    void applyTheme(const Theme&);

 public slots:
    void onDeuteranomalyThemeApplied();
    void onDraculaThemeApplied();
    void onGruvboxThemeApplied();
    void onOneDarkThemeApplied();
    void onPaperColorThemeApplied();
    void onPlaylistView();
    void onProtanomalyThemeApplied();
    void onProtanopiaThemeApplied();
    void onTritanomalyThemeApplied();
    void onTritanopiaThemeApplied();
    void onRosePineThemeApplied();
    void onToggleView();

 private:
    QString current;

    Control* control;
    Playlist* playlist;
    Table* table;
    QStackedWidget* stack;
    Visualizer* visualizer;
};

#endif // WINDOW_H
