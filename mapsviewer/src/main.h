#include <qnetmap_global.h>

#ifdef QNETMAP_QT_4
   #include <QApplication>
#endif

#include <QObject>
#include <QDebug>

class OnQuitObject : public QObject
{
   Q_OBJECT

public:
   OnQuitObject(QObject* parent = 0) : QObject(parent)
   {
      QObject::connect(QApplication::instance(), SIGNAL(aboutToQuit()), SLOT(onQuit()));
   }
   virtual ~OnQuitObject() {}

private:

public slots:
   void onQuit()
   {
      qDebug() << Q_FUNC_INFO;
   }
};

