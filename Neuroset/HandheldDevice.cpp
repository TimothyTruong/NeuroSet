#include "HandheldDevice.h"

HandheldDevice::HandheldDevice(Headset* headset, int battery, QObject* parent)
: QObject(parent), headset(headset), deviceStatus(ON), runStatus(INACTIVE),
  batteryPercent(battery), contact(false), treatmentSig(false){

    // Initialize timers
    connect(&stopTimer, &QTimer::timeout, this, &HandheldDevice::shutdown);
    connect(&runTimer, &QTimer::timeout, this, &HandheldDevice::reduceBattery);
    connect(headset, &Headset::sendSession, this, &HandheldDevice::receiveSession);

    runTimer.start(3000); // battery lasts 5min. battery is supposed to fully deplete every 2-3 treatments (29s per treatment for testing)

    pcWindow = new PCWindow();
    pcWindow->hide();

    //temp sessions
    QList<Frequency> freq;
    Session s1(freq);
    Session s2(freq);
    Session s3(freq);

    sessions.append(s1);
    sessions.append(s2);
    sessions.append(s3);

}

HandheldDevice::~HandheldDevice() {}

void HandheldDevice::createSession(){
    if (deviceStatus == OFF || batteryPercent <= 0) {
        qDebug() << "Cannot start session: Device is off or battery is low.";
        return;
    }
    runStatus = ACTIVE;
    // create a session
    qDebug() << "New session created and ready to start.";
    beginSession();
}

void HandheldDevice::receiveSession(const Session& session) {
    sessions.append(session);
}

void HandheldDevice::shutdown() {
    if (runStatus == ACTIVE || runStatus == PAUSED) {
        stop();
    }
    qDebug() << "Device shutting down...";
    deviceStatus = OFF;
    batteryPercent = 0;
}

void HandheldDevice::beginSession() {
    if (deviceStatus != ON) {
        qDebug() << "Trying to begin a session without it being set to active.";
        return;
    }
    qDebug() << "Session has begun.";
    headset->startSimulation(500);
//    emit sessionStarted();
}

QList<Session> HandheldDevice::getSessions(){
    return sessions;
}

void HandheldDevice::stop() {
    if (runStatus == INACTIVE || runStatus == DISCONNECTED) {
        qDebug() << "No session is currently running to stop.";
        return;
    }

    runStatus = INACTIVE;
    qDebug() << "Session and simulation stopped.";
    headset->stopSimulation();
}

void HandheldDevice::pause() {
    if (runStatus != ACTIVE) {
        qDebug() << "No active session to pause.";
        return;
    }

    runStatus = PAUSED;
    qDebug() << "Session paused.";
}

void HandheldDevice::resume() {
    if (deviceStatus == OFF) {
        qDebug() << "Cannot resume: Device is turned off.";
        return;
    } else if (deviceStatus == ON && runStatus == DISCONNECTED) {
        qDebug() << "Cannot resume: Device is disconnected.";
        return;
    }

    if (runStatus == INACTIVE) {
        qDebug() << "No active session found. Starting a new session.";
        createSession();
    } else if (runStatus == PAUSED) {
        runStatus = ACTIVE;
        qDebug() << "Session resumed.";
        //resume type shi
    } else if (runStatus == ACTIVE) {
        qDebug() << "Session is already active.";
    } else {
        qDebug() << "Session cannot be resumed due to unexpected state.";
    }
}

void HandheldDevice::dateTimeSelection() {
    qDebug() << "Date and time selection accessed.";
    // ui component
}

void HandheldDevice::menuToggle() {
    qDebug() << "Menu toggled.";
}

void HandheldDevice::uploadToPC(int index) {
    qDebug() << "Session data being prepared for upload to PC...";
    Session sessionUpload = sessions.at(index);

    pcWindow->newUpload(sessionUpload);
    pcWindow->show();
}

void HandheldDevice::updateMenu() {
    qDebug() << "Updating menu display...";
}

void HandheldDevice::powerToggle() {
    if (deviceStatus == OFF) {
        deviceStatus = ON;
        qDebug() << "Device powered on. Ready for operation.";
        runStatus = INACTIVE;
        chargeBatteryToFull();
        runTimer.start(3000);
    } else {
        qDebug() << "Shutting down the device...";
        shutdown();
    }
}

void HandheldDevice::reduceBattery() {
    if (batteryPercent > 0 && runStatus == ACTIVE) {
        batteryPercent--;
        qDebug() << "Battery level now at " << batteryPercent << "%.";
        if (batteryPercent == 0) {
            shutdown();
        }
    }
}

void HandheldDevice::chargeBatteryToFull() { //link this to some button i guess
    batteryPercent = 100;
    qDebug() << "Battery fully charged.";
}

bool HandheldDevice::disconnect() {
    if (runStatus == ACTIVE || runStatus == PAUSED) {
        stop();
        runStatus = DISCONNECTED;
        qDebug() << "Device disconnected. Awaiting reconnection...";
        // reconnect type shi

        return true;
    }

    return false;
}

bool HandheldDevice::reconnect() {
    if (runStatus == DISCONNECTED) {
        runStatus = INACTIVE;
        resume();
        qDebug() << "Device reconnected. Resuming operations.";
        
        return true;
    }

    return false;
}

void HandheldDevice::connectionToggle() {
    if (deviceStatus == ON && runStatus != INACTIVE) {
        (runStatus == DISCONNECTED ? reconnect() : disconnect());
    } else {
        qDebug() << "Cannot toggle connection: Device is" << (deviceStatus == OFF ? "off." : (runStatus == INACTIVE ? "inactive." : "???."));
    }
}

