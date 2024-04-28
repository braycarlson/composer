#ifndef MENU_H
#define MENU_H

#include <QAction>
#include <QMenuBar>
#include <QObject>


class Menu : public QMenuBar {
    Q_OBJECT

 public:
    explicit Menu(QWidget* parent = nullptr);

    void updateCheckState(QAction*);

 signals:
    void exitApplication();
    void applyDeuteranomalyTheme();
    void applyDraculaTheme();
    void applyGruvboxTheme();
    void applyOneDarkTheme();
    void applyPaperColorTheme();
    void applyProtanomalyTheme();
    void applyProtanopiaTheme();
    void applyTritanomalyTheme();
    void applyTritanopiaTheme();
    void applyRosePineTheme();

 private slots:
    void onDeuteranomalyThemeApplied();
    void onExitClicked();
    void onDraculaThemeApplied();
    void onGruvboxThemeApplied();
    void onOneDarkThemeApplied();
    void onPaperColorThemeApplied();
    void onProtanomalyThemeApplied();
    void onProtanopiaThemeApplied();
    void onTritanomalyThemeApplied();
    void onTritanopiaThemeApplied();
    void onRosePineThemeApplied();

 private:
    QAction* exitAction;
    QAction* deuteranomalyThemeAction;
    QAction* draculaThemeAction;
    QAction* gruvboxThemeAction;
    QAction* oneDarkThemeAction;
    QAction* paperColorThemeAction;
    QAction* protanomalyThemeAction;
    QAction* protanopiaThemeAction;
    QAction* tritanomalyThemeAction;
    QAction* tritanopiaThemeAction;
    QAction* rosePineThemeAction;
    QList<QAction*> themeAction;
};

#endif // MENU_H
