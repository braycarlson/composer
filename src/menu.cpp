#include "include/menu.h"


Menu::Menu(QWidget* parent) : QMenuBar(parent) {
    QMenu* menu = addMenu(tr("&File"));
    QMenu* themeMenu = addMenu(tr("&Themes"));

    exitAction = new QAction(tr("Exit"), this);
    menu->addAction(exitAction);

    draculaThemeAction = new QAction(tr("Dracula"), this);
    draculaThemeAction->setCheckable(true);

    deuteranomalyThemeAction = new QAction(tr("Deuteranomaly"), this);
    deuteranomalyThemeAction->setCheckable(true);

    gruvboxThemeAction = new QAction(tr("Gruvbox"), this);
    gruvboxThemeAction->setCheckable(true);

    oneDarkThemeAction = new QAction(tr("One Dark"), this);
    oneDarkThemeAction->setCheckable(true);

    paperColorThemeAction = new QAction(tr("Paper Color"), this);
    paperColorThemeAction->setCheckable(true);

    protanomalyThemeAction = new QAction(tr("Protanomaly"), this);
    protanomalyThemeAction->setCheckable(true);

    protanopiaThemeAction = new QAction(tr("Protanopia"), this);
    protanopiaThemeAction->setCheckable(true);

    tritanomalyThemeAction = new QAction(tr("Tritanomaly"), this);
    tritanomalyThemeAction->setCheckable(true);

    tritanopiaThemeAction = new QAction(tr("Tritanopia"), this);
    tritanopiaThemeAction->setCheckable(true);

    rosePineThemeAction = new QAction(tr("Rose Pine"), this);
    rosePineThemeAction->setCheckable(true);
    rosePineThemeAction->setChecked(true);

    themeAction = {
        deuteranomalyThemeAction,
        draculaThemeAction,
        gruvboxThemeAction,
        oneDarkThemeAction,
        paperColorThemeAction,
        protanomalyThemeAction,
        protanopiaThemeAction,
        tritanomalyThemeAction,
        tritanopiaThemeAction,
        rosePineThemeAction
    };

    themeMenu->addAction(deuteranomalyThemeAction);
    themeMenu->addAction(draculaThemeAction);
    themeMenu->addAction(gruvboxThemeAction);
    themeMenu->addAction(oneDarkThemeAction);
    themeMenu->addAction(paperColorThemeAction);
    themeMenu->addAction(rosePineThemeAction);
    themeMenu->addAction(protanomalyThemeAction);
    themeMenu->addAction(protanopiaThemeAction);
    themeMenu->addAction(tritanomalyThemeAction);
    themeMenu->addAction(tritanopiaThemeAction);

    this->connect(deuteranomalyThemeAction, &QAction::triggered, this, &Menu::onDeuteranomalyThemeApplied);
    this->connect(exitAction, &QAction::triggered, this, &Menu::onExitClicked);
    this->connect(draculaThemeAction, &QAction::triggered, this, &Menu::onDraculaThemeApplied);
    this->connect(gruvboxThemeAction, &QAction::triggered, this, &Menu::onGruvboxThemeApplied);
    this->connect(oneDarkThemeAction, &QAction::triggered, this, &Menu::onOneDarkThemeApplied);
    this->connect(paperColorThemeAction, &QAction::triggered, this, &Menu::onPaperColorThemeApplied);
    this->connect(protanomalyThemeAction, &QAction::triggered, this, &Menu::onProtanomalyThemeApplied);
    this->connect(protanopiaThemeAction, &QAction::triggered, this, &Menu::onProtanopiaThemeApplied);
    this->connect(tritanomalyThemeAction, &QAction::triggered, this, &Menu::onTritanomalyThemeApplied);
    this->connect(tritanopiaThemeAction, &QAction::triggered, this, &Menu::onTritanopiaThemeApplied);
    this->connect(rosePineThemeAction, &QAction::triggered, this, &Menu::onRosePineThemeApplied);
}

void Menu::onExitClicked() {
    emit this->exitApplication();
}

void Menu::onDraculaThemeApplied() {
    emit this->applyDraculaTheme();
    this->updateCheckState(this->draculaThemeAction);
}

void Menu::onGruvboxThemeApplied() {
    emit this->applyGruvboxTheme();
    this->updateCheckState(this->gruvboxThemeAction);
}

void Menu::onOneDarkThemeApplied() {
    emit this->applyOneDarkTheme();
    this->updateCheckState(this->oneDarkThemeAction);
}

void Menu::onPaperColorThemeApplied() {
    emit this->applyPaperColorTheme();
    this->updateCheckState(this->paperColorThemeAction);
}

void Menu::onRosePineThemeApplied() {
    emit this->applyRosePineTheme();
    this->updateCheckState(this->rosePineThemeAction);
}

void Menu::onDeuteranomalyThemeApplied() {
    emit this->applyDeuteranomalyTheme();
    this->updateCheckState(this->deuteranomalyThemeAction);
}

void Menu::onProtanomalyThemeApplied() {
    emit this->applyProtanomalyTheme();
    this->updateCheckState(this->protanomalyThemeAction);
}

void Menu::onProtanopiaThemeApplied() {
    emit this->applyProtanopiaTheme();
    this->updateCheckState(this->protanopiaThemeAction);
}

void Menu::onTritanomalyThemeApplied() {
    emit this->applyTritanomalyTheme();
    this->updateCheckState(this->tritanomalyThemeAction);
}

void Menu::onTritanopiaThemeApplied() {
    emit this->applyTritanopiaTheme();
    this->updateCheckState(this->tritanopiaThemeAction);
}

void Menu::updateCheckState(QAction* name) {
    for (auto &action : this->themeAction) {
        action->setChecked(action == name);
    }
}
