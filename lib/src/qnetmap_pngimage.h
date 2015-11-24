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

#ifndef QNETMAP_PNG_H
#define QNETMAP_PNG_H

#include "qnetmap_global.h"

#include <QByteArray>
#include <QPixmap>

namespace qnetmap
{
   class QNETMAP_EXPORT TPNGImage
   {
   public:
      explicit TPNGImage(const QString& FileName_);
      explicit TPNGImage(const QByteArray& PNGImage_);
      virtual ~TPNGImage() {}

      //! \brief Поиск порции
      //! \param Name_ - имя порции
      //! \return позиция порции (начиная с размера), если она существует, иначе 0
      quint32 findChunk(const QByteArray& Name_) const;
      //! \brief Считывает порцию из указанной позиции
      //! \param Position_ - позиция порции, начиная с размера
      //! \param Name_ - возвращается имя порции
      //! \param Data_ - возвращаются данные порции
      //! \param CRC32 - возвращается считанный CRC32 порции
      //! \return true, если порция считана
      bool readChunk(/*in*/const quint32 Position_, 
                     /*out*/QByteArray& Name_, /*out*/QByteArray& Data_, /*out*/quint32 &CRC32_) const;
      //! \brief Считывает указанную порцию
      //! \param Name_ - имя порции
      //! \param Data_ - возвращаются данные порции
      //! \param CRC32 - возвращается считанный CRC32 порции
      //! \return позиция считанной порции, начиная с размера
      quint32 readChunk(
         /*in*/const QByteArray& Name_, /*out*/QByteArray& Data_, /*out*/quint32 &CRC32_) const;
      //! \brief Добавляет порцию перед IEND (проверка на существование этой порции не производится)
      //! \param Name_ - имя порции
      //! \param Data_ - данный порции
      //! \return true, если порция записана
      bool addChunk(const QByteArray& Name_, QByteArray& Data_);
      //! \brief Сохраняет файл под тем-же именем
      //! \return true, если файл записан
      bool save(void);
      //! \brief Сохраняет файл под указанным именем
      //! \param FileName_ - имя файла
      //! \return true, если файл записан
      bool save(QString FileName_);
      //! \brief Проверка правильности загрузки png-файла
      bool imageReaded(void) const { return m_Image.size() > 0; }
      //! \brief Возвращает весь байтовый массив
      QByteArray image(void) const { return m_Image; }
      //! \brief Возвращает версию карт
      QString mapVersion(void);
      //! \brief Возвращает true, если содержит png-файл
      bool isPNGImage(void) const { return m_Image.size() > 0; }
      //! \brief Возвращает pixmap
      QPixmap pixmap(void) const
      {
      QPixmap Pixmap;
      Pixmap.loadFromData(m_Image);
      return Pixmap;
      }

   private:
      //! \brief Преобразование четырех байт в указанной позиции в число
      //! \param Position_ - позиция с начала файла
      //! \return - число
      quint32 uInt(const quint32 Position_) const;
      //! \brief Проверка на правильность имени порции
      //! \param Name_ Имя порции
      bool isChunkName(const QByteArray& Name_) const;
      //! \brief Выравнивает до указанной ширины, дополняя слева \0
      //!        Не усекает, если ширина меньше
      //! \param Data_ - Исходные данные
      //! \param Width_ - Необходимая ширина
      //! \return Результат выравнивания
      QByteArray rightJustified(const QByteArray& Data_, const quint32 Width_) const;
      //! \brief меняет порядок байт
      quint32 reverse(const quint32 Number_) const;

      //! \var имя PNG-файла
      QString    m_FileName;
      //! \var считанный в память png-файл
      QByteArray m_Image;
      //! \var версия карт
      QString    m_MapVersion;
   };
}

#endif // #ifndef QNETMAP_PNG_H
