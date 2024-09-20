// Debouncer.cpp
#include "Debouncer.h"

Debouncer::Debouncer(int delayMs, QObject *parent)
    : QObject(parent), delay(delayMs), timer(new QTimer(this)) {
    timer->setInterval(delayMs);
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, this, [this]() {
        if (callback)
            callback();
    });
}

void Debouncer::trigger(const std::function<void()> &func) {
    callback = func;
    timer->start();
}

void Debouncer::setDelay(int delayMs) {
    delay = delayMs;
    timer->setInterval(delay);
}

bool Debouncer::isDebouncing() const { return timer->isActive(); }

void Debouncer::stop() {
    timer->stop();
    callback = nullptr;
}

void Debouncer::clearCallback() { callback = nullptr; }
