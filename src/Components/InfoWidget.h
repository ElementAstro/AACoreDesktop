#ifndef INFO_WIDGET_H
#define INFO_WIDGET_H

#include <QWidget>
#include <QColor>

class ElaText;
class QPropertyAnimation;

class InfoWidget : public QWidget {
    Q_OBJECT
    Q_PROPERTY(double currentValue READ getCurrentValue WRITE setCurrentValue)

public:
    explicit InfoWidget(QWidget *parent = nullptr);
    ~InfoWidget() override;

    // Setters
    void setName(const QString &name);
    void setUnit(const QString &unit);
    void setRange(double min, double max);
    void setColors(const QColor &normal, const QColor &warning, const QColor &error);
    void setFontSizes(int nameSize, int valueSize, int unitSize, int rangeSize);
    void setBorderStyle(const QString &style);
    void setAnimationDuration(int ms);

public slots:
    void updateValue(double value);
    void updateValueExternally(double value);

private:
    void setupUI();
    void setupAnimations();
    void refreshDisplay();
    double getCurrentValue() const;
    void setCurrentValue(double value);

private:
    ElaText *m_valueLabel;
    ElaText *m_unitLabel;
    ElaText *m_nameLabel;
    ElaText *m_rangeLabel;
    
    QPropertyAnimation *m_valueAnimation;
    
    double m_minValue;
    double m_maxValue;
    double m_currentValue;
    double m_warningThreshold;
    
    QColor m_normalColor;
    QColor m_warningColor;
    QColor m_errorColor;
    
    int m_animationDuration;
};

#endif // INFO_WIDGET_H