#include "bluetoothapp.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QStandardPaths>
#include <QFileInfo>
#include <QStyle>
#include <QDebug>

// Здесь меняем QWidget на QDialog в инициализации
BluetoothApp::BluetoothApp(QWidget *parent)
    : QDialog(parent)
    , m_songPath(AUDIO_FILE_PATH)
{
    setWindowTitle("Lab 6: Bluetooth Audio Monitor");
    setFixedSize(1101, 702);

    // --- 1. Создание интерфейса ---
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Верхняя часть: Монитор
    QGroupBox *monitorGroup = new QGroupBox("Bluetooth Монитор", this);
    QVBoxLayout *monitorLayout = new QVBoxLayout(monitorGroup);

    monitorToggleButton = new QPushButton("Начать мониторинг (Авто-сканирование)", this);
    deviceListWidget = new QListWidget(this);
    pairButton = new QPushButton("Сопряжение", this);
    pairButton->setEnabled(false);
    statusLabel = new QLabel("Статус: Готов", this);

    monitorLayout->addWidget(monitorToggleButton);
    monitorLayout->addWidget(deviceListWidget);
    monitorLayout->addWidget(pairButton);
    monitorLayout->addWidget(statusLabel);
    mainLayout->addWidget(monitorGroup);

    // Нижняя часть: Плеер (Отправка звука)
    QGroupBox *playerGroup = new QGroupBox("Отправка аудио (Плеер)", this);
    QVBoxLayout *playerLayout = new QVBoxLayout(playerGroup);

    QHBoxLayout *fileLayout = new QHBoxLayout();
    songLabel = new QLabel("Файл: " + QFileInfo(m_songPath).fileName(), this);
    selectSongButton = new QPushButton("Выбрать другой файл...", this);
    fileLayout->addWidget(songLabel);
    fileLayout->addWidget(selectSongButton);
    playerLayout->addLayout(fileLayout);

    playPauseButton = new QPushButton("Отправить звук (Play)", this);
    playPauseButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));

    positionSlider = new QSlider(Qt::Horizontal, this);
    positionSlider->setEnabled(false);

    playerLayout->addWidget(new QLabel("Выберите устройство вывода (Наушники):"));
    audioOutputComboBox = new QComboBox(this);

    playerLayout->addWidget(audioOutputComboBox);
    playerLayout->addWidget(playPauseButton);
    playerLayout->addWidget(positionSlider);
    mainLayout->addWidget(playerGroup);


    // --- 2. Инициализация логики ---

    // Bluetooth
    m_localDevice = new QBluetoothLocalDevice(this);
    m_discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);

    m_scanTimer = new QTimer(this);
    m_scanTimer->setInterval(5000); // 5 секунд

    connect(monitorToggleButton, &QPushButton::clicked, this, &BluetoothApp::onMonitorToggleClicked);
    connect(m_scanTimer, &QTimer::timeout, this, &BluetoothApp::startScanLoop);

    connect(m_discoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, &BluetoothApp::addDiscoveredDevice);
    connect(m_discoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished, this, &BluetoothApp::scanFinished);

    connect(deviceListWidget, &QListWidget::itemClicked, this, &BluetoothApp::onDeviceListClicked);
    connect(pairButton, &QPushButton::clicked, this, &BluetoothApp::onPairClicked);
    connect(m_localDevice, &QBluetoothLocalDevice::pairingFinished, this, &BluetoothApp::pairingDone);

    // Audio
    m_player = new QMediaPlayer(this);
    m_audioOutput = new QAudioOutput(this);
    m_player->setAudioOutput(m_audioOutput);

    if (QFileInfo::exists(m_songPath)) {
        m_player->setSource(QUrl::fromLocalFile(m_songPath));
    }

    connect(new QMediaDevices(this), &QMediaDevices::audioOutputsChanged, this, &BluetoothApp::updateAudioDevices);
    connect(audioOutputComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &BluetoothApp::onAudioOutputChanged);

    connect(selectSongButton, &QPushButton::clicked, this, &BluetoothApp::onSelectSongClicked);
    connect(playPauseButton, &QPushButton::clicked, this, &BluetoothApp::onPlayPauseClicked);

    connect(m_player, &QMediaPlayer::positionChanged, this, &BluetoothApp::updatePosition);
    connect(m_player, &QMediaPlayer::durationChanged, this, &BluetoothApp::updateDuration);
    connect(m_player, &QMediaPlayer::playbackStateChanged, this, &BluetoothApp::updatePlaybackState);
    connect(positionSlider, &QSlider::sliderMoved, this, &BluetoothApp::onPositionChanged);

    checkBluetoothStatus();
    updateAudioDevices();
}

BluetoothApp::~BluetoothApp() {}

// --- ЛОГИКА МОНИТОРИНГА ---

void BluetoothApp::checkBluetoothStatus() {
    if (m_localDevice->hostMode() == QBluetoothLocalDevice::HostPoweredOff) {
        statusLabel->setText("Bluetooth выключен!");
        monitorToggleButton->setEnabled(false);
    } else {
        monitorToggleButton->setEnabled(true);
    }
}

void BluetoothApp::onMonitorToggleClicked()
{
    if (m_scanTimer->isActive()) {
        m_scanTimer->stop();
        if (m_discoveryAgent->isActive()) m_discoveryAgent->stop();
        monitorToggleButton->setText("Начать мониторинг (Авто-сканирование)");
        statusLabel->setText("Мониторинг остановлен.");
    } else {
        monitorToggleButton->setText("Остановить мониторинг");
        startScanLoop();
        m_scanTimer->start();
    }
}

void BluetoothApp::startScanLoop()
{
    if (m_discoveryAgent->isActive()) return;
    deviceListWidget->clear();
    statusLabel->setText("Сканирование эфира...");
    m_discoveryAgent->start();
}

void BluetoothApp::addDiscoveredDevice(const QBluetoothDeviceInfo &device)
{
    if (device.name().isEmpty()) return;

    QString label = QString("%1 (%2)").arg(device.name(), device.address().toString());
    QListWidgetItem *item = new QListWidgetItem(label);
    item->setData(Qt::UserRole, QVariant::fromValue(device));

    deviceListWidget->addItem(item);
}

void BluetoothApp::scanFinished()
{
    statusLabel->setText("Список обновлен. Ждем следующего цикла...");
    updateDeviceHighlights();
}

void BluetoothApp::updateDeviceHighlights()
{
    QString currentAudioName;
    if (audioOutputComboBox->count() > 0) {
        currentAudioName = audioOutputComboBox->currentText();
    }

    for (int i = 0; i < deviceListWidget->count(); ++i) {
        QListWidgetItem *item = deviceListWidget->item(i);
        QBluetoothDeviceInfo info = item->data(Qt::UserRole).value<QBluetoothDeviceInfo>();

        item->setForeground(palette().color(QPalette::WindowText));

        if (!currentAudioName.isEmpty() && currentAudioName.contains(info.name(), Qt::CaseInsensitive)) {
            item->setForeground(Qt::blue);
            item->setText(item->text() + " [ПОДКЛЮЧЕНО]");
        }
        else if (info.majorDeviceClass() == QBluetoothDeviceInfo::AudioVideoDevice) {
            item->setForeground(QColor(0, 100, 0));
        }
    }
}

// --- ЛОГИКА СОПРЯЖЕНИЯ ---

void BluetoothApp::onDeviceListClicked(QListWidgetItem *item)
{
    if (!item) return;
    QBluetoothDeviceInfo info = item->data(Qt::UserRole).value<QBluetoothDeviceInfo>();
    m_selectedDeviceAddress = info.address();

    QBluetoothLocalDevice::Pairing status = m_localDevice->pairingStatus(m_selectedDeviceAddress);

    if (status == QBluetoothLocalDevice::Paired) {
        pairButton->setText("Уже сопряжено");
        pairButton->setEnabled(false);
    } else {
        pairButton->setText("Сопряжение");
        pairButton->setEnabled(true);
    }
}

void BluetoothApp::onPairClicked()
{
    if (m_selectedDeviceAddress.isNull()) return;
    statusLabel->setText("Запрос сопряжения...");
    m_localDevice->requestPairing(m_selectedDeviceAddress, QBluetoothLocalDevice::Paired);
}

void BluetoothApp::pairingDone(const QBluetoothAddress &address, QBluetoothLocalDevice::Pairing status)
{
    Q_UNUSED(address);
    if (status == QBluetoothLocalDevice::Paired) {
        QMessageBox::information(this, "Успех", "Сопряжение выполнено!\nТеперь выберите устройство в списке плеера.");
        pairButton->setText("Уже сопряжено");
        pairButton->setEnabled(false);
    }
}

// --- ЛОГИКА ПЛЕЕРА ---

void BluetoothApp::updateAudioDevices()
{
    audioOutputComboBox->clear();
    m_audioDevices = QMediaDevices::audioOutputs();
    for (const QAudioDevice &device : m_audioDevices) {
        audioOutputComboBox->addItem(device.description(), QVariant::fromValue(device));
    }
    updateDeviceHighlights();
}

void BluetoothApp::onAudioOutputChanged(int index)
{
    if (index >= 0 && index < m_audioDevices.size()) {
        m_audioOutput->setDevice(m_audioDevices.at(index));
        statusLabel->setText("Вывод звука: " + m_audioDevices.at(index).description());
        updateDeviceHighlights();
    }
}

void BluetoothApp::onSelectSongClicked()
{
    QString path = QFileDialog::getOpenFileName(this, "Выбрать MP3", QStandardPaths::writableLocation(QStandardPaths::MusicLocation), "*.mp3");
    if (!path.isEmpty()) {
        m_songPath = path;
        m_player->setSource(QUrl::fromLocalFile(m_songPath));
        songLabel->setText("Файл: " + QFileInfo(m_songPath).fileName());
    }
}

void BluetoothApp::onPlayPauseClicked()
{
    if (m_player->playbackState() == QMediaPlayer::PlayingState) {
        m_player->pause();
    } else {
        m_player->play();
    }
}

void BluetoothApp::updatePlaybackState(QMediaPlayer::PlaybackState newState)
{
    if (newState == QMediaPlayer::PlayingState) {
        playPauseButton->setText("Пауза");
        playPauseButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
        statusLabel->setText("Отправка аудиопотока...");
    } else {
        playPauseButton->setText("Отправить звук (Play)");
        playPauseButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        statusLabel->setText("Остановлено.");
    }
}

void BluetoothApp::onPositionChanged(int position) { m_player->setPosition(position); }
void BluetoothApp::updatePosition(qint64 position) { positionSlider->setValue(position); }
void BluetoothApp::updateDuration(qint64 duration) {
    positionSlider->setEnabled(duration > 0);
    positionSlider->setMaximum(duration);
}
