#include "include/window.h"


Window::Window(QWidget* parent) : QMainWindow(parent) {
    this->setWindowTitle("Composer");
    this->setMinimumSize(1280, 720);

    QScreen* screen = QGuiApplication::primaryScreen();
    QRect geometry = screen->geometry();
    int x = (geometry.width() - this->width()) / 2;
    int y = (geometry.height() - this->height()) / 2;
    this->move(x, y);

    Menu* menu = new Menu(this);
    this->setMenuBar(menu);

    QSplitter* splitter = new QSplitter(Qt::Horizontal, this);

    QWidget* pane = new QWidget();

    QVBoxLayout* layout = new QVBoxLayout(pane);
    layout->setMargin(0);
    layout->setSpacing(0);

    control = new Control(this);
    QMediaPlayer* player = control->getPlayer();

    table = new Table(pane);

    playlist = new Playlist(this);

    visualizer = new Visualizer(this);
    visualizer->setPlayer(player);

    stack = new QStackedWidget();
    stack->addWidget(table);
    stack->addWidget(visualizer);

    layout->addWidget(stack);
    layout->addWidget(control);
    control->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    splitter->addWidget(playlist);
    splitter->addWidget(pane);
    splitter->setStretchFactor(1, 1);

    QVBoxLayout* main = new QVBoxLayout();
    main->addWidget(splitter);

    QWidget* widget = new QWidget(this);
    widget->setLayout(main);
    this->setCentralWidget(widget);

    this->connect(control, &Control::metadataRequested, table, &Table::sendMetadata);
    this->connect(control, &Control::nextTrackRequested, table, &Table::sendMetadata);
    this->connect(control, &Control::previousTrackRequested, table, &Table::sendMetadata);
    this->connect(control, &Control::songChanged, table, &Table::onPlaylistIndexChanged);
    this->connect(control, &Control::songFinished, visualizer, &Visualizer::onSongFinished);
    this->connect(control, &Control::songPlaying, table, &Table::onPlaylistIndexChanged);
    this->connect(control, &Control::visualizeClicked, this, &Window::onToggleView);

    this->connect(menu, &Menu::applyDeuteranomalyTheme, this, &Window::onDeuteranomalyThemeApplied);
    this->connect(menu, &Menu::applyDraculaTheme, this, &Window::onDraculaThemeApplied);
    this->connect(menu, &Menu::applyGruvboxTheme, this, &Window::onGruvboxThemeApplied);
    this->connect(menu, &Menu::applyOneDarkTheme, this, &Window::onOneDarkThemeApplied);
    this->connect(menu, &Menu::applyPaperColorTheme, this, &Window::onPaperColorThemeApplied);
    this->connect(menu, &Menu::applyProtanomalyTheme, this, &Window::onProtanomalyThemeApplied);
    this->connect(menu, &Menu::applyProtanopiaTheme, this, &Window::onProtanopiaThemeApplied);
    this->connect(menu, &Menu::applyRosePineTheme, this, &Window::onRosePineThemeApplied);
    this->connect(menu, &Menu::applyTritanomalyTheme, this, &Window::onTritanomalyThemeApplied);
    this->connect(menu, &Menu::applyTritanopiaTheme, this, &Window::onTritanopiaThemeApplied);
    this->connect(menu, &Menu::exitApplication, this, &Window::close);

    this->connect(playlist, &Playlist::playlistLoaded, table, &Table::onLoadPlaylist);
    this->connect(playlist, &Playlist::playlistNameSelected, table, &Table::onPlaylistSelected);
    this->connect(playlist, &Playlist::playlistSelected, this, &Window::onPlaylistView);
    this->connect(playlist, &Playlist::songAdded, table, &Table::onSongAdded);

    this->connect(table, &Table::addSongRequested, playlist, &Playlist::onBrowseForSong);
    this->connect(table, &Table::doubleClicked, control, &Control::onPlaySong);
    this->connect(table, &Table::doubleClicked, control, &Control::onUpdateSong);
    this->connect(table, &Table::itemSelected, control, &Control::onUpdateSong);
    this->connect(table, &Table::nextSongRequested, control, &Control::onGetNextSong);
    this->connect(table, &Table::playSongRequested, control, &Control::onPlaySong);
    this->connect(table, &Table::playSongRequested, control, &Control::onUpdateSong);
    this->connect(table, &Table::playlistLoaded, control, &Control::onStorePlaylist);
    this->connect(table, &Table::playlistSorted, control, &Control::onPlaylistSorted);
    this->connect(table, &Table::previousSongRequested, control, &Control::onGetPreviousSong);
    this->connect(table, &Table::requestPlaylist, playlist, &Playlist::onHandlePlaylistSelected);
    this->connect(table, &Table::songAdded, control, &Control::queueAddSongToPlaylist);
    this->connect(table, &Table::songDeleted, playlist, &Playlist::onDeleteSong);
    this->connect(table, &Table::songRemoved, control, &Control::queueRemoveSongFromPlaylist);
    this->connect(table, &Table::songUpdated, playlist, &Playlist::onUpdateSong);
    this->connect(table, &Table::songEdited, control, &Control::onUpdateSong);
    this->connect(table, &Table::doubleClicked, playlist, &Playlist::onDoubleClicked);

    this->playlist->setFirstPlaylist();
}

void Window::applyTheme(const Theme& theme) {
    QPalette palette;

    palette.setColor(QPalette::Window, theme.base);
    palette.setColor(QPalette::WindowText, theme.text);
    palette.setColor(QPalette::Base, theme.surface);
    palette.setColor(QPalette::AlternateBase, theme.overlay);
    palette.setColor(QPalette::ToolTipBase, theme.base);
    palette.setColor(QPalette::ToolTipText, theme.text);
    palette.setColor(QPalette::Text, theme.text);
    palette.setColor(QPalette::Button, theme.base);
    palette.setColor(QPalette::ButtonText, theme.text);
    palette.setColor(QPalette::BrightText, theme.brightText);
    palette.setColor(QPalette::Link, theme.link);
    palette.setColor(QPalette::Highlight, theme.highlight);
    palette.setColor(QPalette::HighlightedText, theme.highlightedText);

    palette.setColor(QPalette::Disabled, QPalette::WindowText, theme.disabledWindowText);
    palette.setColor(QPalette::Disabled, QPalette::Button, theme.base);
    palette.setColor(QPalette::Disabled, QPalette::ButtonText, theme.disabledButtonText);
    palette.setColor(QPalette::Disabled, QPalette::Text, theme.disabledText);
    palette.setColor(QPalette::Disabled, QPalette::Base, theme.surface);
    palette.setColor(QPalette::Disabled, QPalette::Highlight, theme.disabledHighlight);
    palette.setColor(QPalette::Disabled, QPalette::HighlightedText, theme.highlightedText);

    qApp->setPalette(palette);

    QList<QWidget*> widgets = this->findChildren<QWidget*>();

    foreach (QWidget* widget, widgets) {
        widget->setPalette(palette);
    }

    if (control) {
        control->updateTheme();
    }
}

void Window::onDraculaThemeApplied() {
    Theme theme(dracula);
    theme.applyStylesheet(dracula);

    this->applyTheme(theme);
}

void Window::onGruvboxThemeApplied() {
    Theme theme(gruvbox);
    theme.applyStylesheet(gruvbox);

    this->applyTheme(theme);
}

void Window::onOneDarkThemeApplied() {
    Theme theme(oneDark);
    theme.applyStylesheet(oneDark);

    this->applyTheme(theme);
}

void Window::onPaperColorThemeApplied() {
    Theme theme(paperColor);
    theme.applyStylesheet(paperColor);

    this->applyTheme(theme);
}

void Window::onRosePineThemeApplied() {
    Theme theme(rosePine);
    theme.applyStylesheet(rosePine);

    this->applyTheme(theme);
}

void Window::onDeuteranomalyThemeApplied() {
    Theme theme(deuteranomaly);
    theme.applyStylesheet(deuteranomaly);

    this->applyTheme(theme);
}

void Window::onProtanomalyThemeApplied() {
    Theme theme(protanomaly);
    theme.applyStylesheet(protanomaly);

    this->applyTheme(theme);
}

void Window::onProtanopiaThemeApplied() {
    Theme theme(protanopia);
    theme.applyStylesheet(protanopia);

    this->applyTheme(theme);
}

void Window::onTritanomalyThemeApplied() {
    Theme theme(tritanomaly);
    theme.applyStylesheet(tritanomaly);

    this->applyTheme(theme);
}

void Window::onTritanopiaThemeApplied() {
    Theme theme(tritanopia);
    theme.applyStylesheet(tritanopia);

    this->applyTheme(theme);
}

void Window::onToggleView() {
    int index = this->stack->currentIndex();

    if (index == 0) {
        this->stack->setCurrentIndex(1);
        this->control->updateVisualizeIcon(1);

    } else {
        this->stack->setCurrentIndex(0);
        this->control->updateVisualizeIcon(0);
    }
}

void Window::onPlaylistView() {
    this->stack->setCurrentIndex(0);
    this->control->updateVisualizeIcon(0);
}

Window::~Window() {
    if (this->control) delete this->control;
}
