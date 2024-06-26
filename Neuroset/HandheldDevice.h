#ifndef HANDHELDDEVICE_H
#define HANDHELDDEVICE_H

#include "defs.h"
#include "Headset.h"
#include "Session.h"
#include <QTimer>
#include <QObject>
#include <QDebug>
#include <list>
#include "pcwindow.h"

using namespace std;

/**
 * @file HandheldDevice.h
 * @brief Declaration of the HandheldDevice class.
 * 
 * This file contains the declaration of the HandheldDevice class, which represents
 * a handheld device used to control and monitor the Neureset EEG device.
 * It includes methods for managing sessions, controlling the device's power,
 * and interacting with the PC.
 */

class HandheldDevice : public QObject {
    Q_OBJECT

public:
    explicit HandheldDevice(Headset* headset, int batteryLevel = 100, QObject* parent = nullptr);
    ~HandheldDevice();

    int getBattery() { return batteryPercent; }
    RunStatus getRunStatus() { return runStatus; }
    QList<Session> getSessions();
    Headset* getHeadset();

public slots:
    void createSession();
    void shutdown();
    void beginSession();
    void stop();
    void pause();
    void resume();
    void dateTimeSelection();

    void menuToggle();
    void uploadToPC(int);
    void updateMenu();
    void powerToggle();
    void reduceBattery();
    void chargeBatteryToFull();
    bool disconnect();
    bool reconnect();
    void connectionToggle();
    void receiveSession(const Session&);
    void updateRunStatus(RunStatus runStatus);

signals:
    void sessionStarted();
    void sessionStopped();
    void stopHeadset();
    void runStatusChanged(RunStatus runStatus);

private:
    Headset* headset;
    DeviceStatus deviceStatus;
    RunStatus runStatus;
    int batteryPercent;
    bool contact;
    bool treatmentSig;
    Session* currentSession;

    QTimer stopTimer;
    QTimer runTimer;
    QList<Session> sessions;

    static const int size = MAX_SIZE;
    PCWindow *pcWindow;
};

#endif // HANDHELDDEVICE_H
