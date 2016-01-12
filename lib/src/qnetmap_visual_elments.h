/*
* This file is part of QNetMap,
* an open-source cross-platform map widget
*
* Copyright (c) 2011-2014 JSC Sintels (sintels.ru) and/or its affiliates.
* Copyright (c) 2012-2014 OJSC T8 (t8.ru) and/or its affiliates.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will `be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with QNetMap. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef QNETMAP_VISUAL_ELEMENTS_H
#define QNETMAP_VISUAL_ELEMENTS_H

#include "qnetmap_global.h"

#ifdef QNETMAP_QT_4
   #include <QPushButton>
   #include <QComboBox>
   #include <QFileDialog>
   #include <QVBoxLayout>
   #include <QHBoxLayout>
   #include <QLabel>
#endif
#include <QSettings>
#include <QDir>

namespace qnetmap
{
   //==============================================================================================
   //! \class TSelectPathButton
   //! \brief Реализует модифицированную кнопку, связанную с комбобоксом
   class QNETMAP_EXPORT TPathButton: public QPushButton
   {
      Q_OBJECT

   public:
      //! \struct Consts
      struct Consts {
         static const int BrowseFile      = __LINE__; //!< выбор файла
         static const int BrowseDirectory = __LINE__; //!< выбор папки
         };

      //! \brief Конструктор класса
      //! \param QComboBox* ComboBox_ - указатель на соответствующий кнопке комбобокс
      //! \param int BrowseType_ - тип содержимого окна выбора, всплывающего при нажатии на кнопку
      //!                          может быть Consts::BrowseDirectory или Consts::BrowseFile
      //! \param QString ExtentionFilter_ - фильтр содержимого в окне выбора
      //! \param QWidget *Parent_ - указатель на класс-родитель
      TPathButton(QComboBox* ComboBox_,
                  int BrowseType_ = Consts::BrowseDirectory, 
                  const QString& ExtentionFilter_ = QNetMapTranslator::tr("All files (*.*)" /* Russian: Все файлы (*.*) */),
                  QWidget *Parent_ = 0) 
         : QPushButton(Parent_),
           m_ComboBox(ComboBox_), m_BrowseType(BrowseType_), m_ExtentionFilter(ExtentionFilter_)
      {
      setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
      switch(BrowseType_) {
      case Consts::BrowseDirectory:
         setText(QNetMapTranslator::tr("Folder..."));
         break;
      case Consts::BrowseFile:
         setText(QNetMapTranslator::tr("File..."));
         break;
      default:
         setText(QNetMapTranslator::tr("The object type to browse is invalid."));
         break;
         }
      QNM_DEBUG_CHECK(connect(this, SIGNAL(clicked()), SLOT(browse())));
      }
      virtual ~TPathButton() {}

   private:
      Q_DISABLE_COPY(TPathButton)

      //! \var Указатель на комбобокс
      QComboBox* m_ComboBox;
      //! \var тип содержимого окна выбора, всплывающего при нажатии на кнопку
      int m_BrowseType;
      //! \var фильтр содержимого в окне выбора
      QString m_ExtentionFilter;

   private slots:
      //! \brief Показывает дерево каталогов для выбора нужного и устанавливает
      //!        в качестве значения в связанный комбобокс
      void browse(void)
      {
      QString ComboBoxText = 
         QDir::toNativeSeparators(m_ComboBox->itemText(m_ComboBox->currentIndex()).trimmed());
      QFileInfo fi(ComboBoxText);
      QDir Directory;
      if(fi.isDir()) Directory = QDir(ComboBoxText);
      else           Directory = fi.dir();
      QString Path;
      if(Directory.exists()) Path = Directory.absolutePath();
      else                   Path = QDir::currentPath();
      //
      switch(m_BrowseType) {
      case Consts::BrowseDirectory:
         Path = QDir::toNativeSeparators(
            QFileDialog::getExistingDirectory(this, QNetMapTranslator::tr("Select folder"), Path));
         break;
      case Consts::BrowseFile:
         Path = QDir::toNativeSeparators(
            QFileDialog::getOpenFileName(this, QNetMapTranslator::tr("Select file"), Path, m_ExtentionFilter));
         break;
      default:
         assert(!"The object type to browse is invalid.");
         break;
         }

      if(!Path.isEmpty()) {
         if(m_ComboBox->findText(Path) < 0) m_ComboBox->insertItem(0, Path);
         m_ComboBox->setCurrentIndex(m_ComboBox->findText(Path));
         }
      }
   };

   //==============================================================================================
   //! \class TComboBox
   //! \brief Реализует модифицированный комбобокс, хранящий в реестре список старых значений
   class QNETMAP_EXPORT TComboBox: public QComboBox
   {
      Q_OBJECT

   public:
      //! \brief Конструктор класса
      //! \param QString& Organization_ - ключ организации
      //! \param QString& Application_ - ключ приложения
      //! \param QString& PropertyName_ - имя свойства
      //! \param int Count_ - количество сохраняемых последних значений
      TComboBox(const QString& Organization_, const QString& Application_, const QString& PropertyName_, int Count_=10, 
                QWidget* Parent_=NULL) : QComboBox(Parent_)
      {
      if(Organization_.isEmpty() || Application_.isEmpty() || PropertyName_.isEmpty()) {
         assert(!"Для комбобокса необходимо задать в параметрах не пустые строки");
         return;
         }
      //
      m_Organization = Organization_;
      m_Application  = Application_;
      m_PropertyName = PropertyName_;
      m_Count = Count_;
      //
      QSettings Settings(Organization_, Application_);
      setEditable(true);
      setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
      // заполняем из реестра
      QString TemplatePropertyName = PropertyName_ + "_%1";
      for(int i=0; i<Count_; ++i) {
         QString Key = TemplatePropertyName.arg(i);
         if(Settings.contains(Key)) {
            QString Value = Settings.value(Key).toString();
            if(Value.isEmpty()) break;
            addItem(Value);
            }
         }
      }
      //
      virtual ~TComboBox() 
      {
      QSettings Settings(m_Organization, m_Application);
      // запоминаем введенный текст
      QString Text = currentText();
      if(!Text.isEmpty()) {
         if(findText(Text) < 0) insertItem(0, Text);
         setCurrentIndex(findText(Text));
         }
      // запоминаем не более m_Count строк в реестре
      int CurrentIndex = currentIndex();
      int Count = m_Count > count() ? count() : m_Count;
      QString TemplatePropertyName = m_PropertyName + "_%1";
      for(int i=0, delta=1; i<m_Count; ++i) {
         QString Key;
         if(i == CurrentIndex) { delta = 0; Key = TemplatePropertyName.arg(0); }
         else                  Key = TemplatePropertyName.arg(i + delta);
         //
         if(i < Count) Settings.setValue(Key, itemText(i));
         else if(Settings.contains(Key)) Settings.remove(Key); // удаляем старые
         }
      }

   private:
      Q_DISABLE_COPY(TComboBox)

      QString m_Organization;
      QString m_Application;
      QString m_PropertyName;
      int m_Count;
   };

   //==============================================================================================
   //! \class TPathField
   //! \brief Реализует модифицированный комбобокс, хранящий в реестре список старых значений
   class QNETMAP_EXPORT TPathField: public QFrame
   {
      Q_OBJECT

   public:
      //! \struct Consts
      struct Consts {
         static const int BrowseFile      = TPathButton::Consts::BrowseFile;      //!< выбор файла
         static const int BrowseDirectory = TPathButton::Consts::BrowseDirectory; //!< выбор папки
         };

      //! \brief Конструктор класса
      //! \param QString Title_ - заголовок
      //! \param QString& Organization_ - ключ организации
      //! \param QString& Application_ - ключ приложения
      //! \param QString& PropertyName_ - имя свойства
      //! \param int Count_ - количество сохраняемых последних значений
      //! \param int BrowseType_ - тип содержимого окна выбора, всплывающего при нажатии на кнопку
      //!                          может быть Consts::BrowseDirectory или Consts::BrowseFile
      //! \param QString ExtentionFilter_ - фильтр содержимого в окне выбора
      //! \param QWidget *Parent_ - указатель на класс-родитель
      TPathField(const QString& Title_,
                 const QString& Organization_, 
                 const QString& Application_, 
                 const QString& PropertyName_,
                 int Count_=10, 
                 int BrowseType_ = Consts::BrowseDirectory,
                 const QString& ExtentionFilter_ = QNetMapTranslator::tr("All files (*.*)" /* Russian: Все файлы (*.*) */),
                 QWidget* Parent_ = NULL) 
                 : QFrame(Parent_)
      {
      QVBoxLayout* MainLayout = new QVBoxLayout;
      MainLayout->setMargin(0);
      m_Label = new QLabel(Title_);
      m_Label->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
      MainLayout->addWidget(m_Label);
      m_ComboBox = new TComboBox(Organization_, Application_, PropertyName_, Count_);
      m_Button   = new TPathButton(m_ComboBox, BrowseType_, ExtentionFilter_);
      QHBoxLayout* Layout = new QHBoxLayout;
      Layout->setMargin(0);
      Layout->addWidget(m_ComboBox);
      Layout->addWidget(m_Button);
      MainLayout->addLayout(Layout);
      setLayout(MainLayout);
      //
      QNM_DEBUG_CHECK(connect(m_ComboBox, SIGNAL(editTextChanged(const QString&)), 
                     this,       SIGNAL(editTextChanged(const QString&))));
      }
      virtual ~TPathField() {}

      QString currentText(void) const { return m_ComboBox->currentText(); }
      TComboBox* comboBox(void) const { return m_ComboBox; }
      TPathButton* button(void) const { return m_Button; }
      QLabel* label(void) const { return m_Label; }

   private:
      Q_DISABLE_COPY(TPathField)

      QLabel*      m_Label;
      TComboBox*   m_ComboBox;
      TPathButton* m_Button;

   signals:
      void editTextChanged(const QString&);
   };

   //==============================================================================================
   //! \class TTextField
   //! \brief Реализует модифицированный комбобокс, хранящий в реестре список старых значений
   class QNETMAP_EXPORT TTextField: public QFrame
   {
      Q_OBJECT

   public:
      //! \brief Конструктор класса
      //! \param QString Title_ - заголовок
      //! \param QString& Organization_ - ключ организации
      //! \param QString& Application_ - ключ приложения
      //! \param QString& PropertyName_ - имя свойства
      //! \param int Count_ - количество сохраняемых последних значений
      //! \param QWidget *Parent_ - указатель на класс-родитель
      TTextField(const QString& Title_,
                 const QString& Organization_, const QString& Application_, const QString& PropertyName_, int Count_=10,
                 QWidget* Parent_ = NULL) 
                 : QFrame(Parent_)
      {
      QVBoxLayout* MainLayout = new QVBoxLayout;
      MainLayout->setMargin(0);
      m_Label = new QLabel(Title_);
      m_Label->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
      MainLayout->addWidget(m_Label);
      m_ComboBox = new TComboBox(Organization_, Application_, PropertyName_, Count_);
      MainLayout->addWidget(m_ComboBox);
      setLayout(MainLayout);
      //
      QNM_DEBUG_CHECK(connect(m_ComboBox, SIGNAL(editTextChanged(const QString&)), 
                     this,       SIGNAL(editTextChanged(const QString&))));
      }
      virtual ~TTextField() {}

      QString currentText(void) const { return m_ComboBox->currentText(); }
      TComboBox* comboBox(void) const { return m_ComboBox; }
      QLabel* label(void) const { return m_Label; }
      void setText(const QString& Text_)
      {
      if(!Text_.isEmpty()) {
         if(m_ComboBox->findText(Text_) < 0) m_ComboBox->insertItem(0, Text_);
         m_ComboBox->setCurrentIndex(m_ComboBox->findText(Text_));
         }
      }

   private:
      Q_DISABLE_COPY(TTextField)

      QLabel*    m_Label;
      TComboBox* m_ComboBox;

   signals:
      void editTextChanged(const QString&);
   };
}
#endif // #ifndef QNETMAP_VISUAL_ELEMENTS_H
