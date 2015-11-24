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

#ifndef QNETMAP_MAPADAPTERPARAMETERS_H
#define QNETMAP_MAPADAPTERPARAMETERS_H

#include "qnetmap_global.h"

class QString;

namespace qnetmap 
{
   //! \class TMapAdapterParameters
   //! \brief управляет параметрами адаптера карты
   class TMapAdapterParameters
   {
   public:
      TMapAdapterParameters();
      virtual ~TMapAdapterParameters() {}

      // методы, возвращающие необходимые свойства
      QString name(void) const { return m_Name; }
      QString description(void) const { return m_Description; }
      int     type(void) const { return m_Type; }
      int     transparency(void) const { return m_Transparency; }
      int     projectionType(void) const { return m_ProjectionType; }
      int     basis(void) const { return m_Basis; }
      uint    tileSizeX(void) const { return m_TileSizeX; }
      uint    tileSizeY(void) const { return m_TileSizeY; }
      int     tilesNumbering(void) const { return m_TilesNumbering; }
      int     zoomMinimum(void) const { return m_ZoomMinimum; }
      int     zoomMaximum(void) const { return m_ZoomMaximum; }
      int     zoomShift(void) const { return m_ZoomShift; }
      QString folderName(void) const { return m_FolderName; }
      uint    updatability(void) const { return m_Updatability; }
      // методы, устанавливающие свойства с необходимыми проверками
      bool setName(const QString& Name_);
      bool setDescription(const QString& Description_);
      bool setType(const uint Type_);
      bool setType(const QString& Type_);
      bool setTransparency(const uint Transparency_);
      bool setTransparency(const QString& Transparency_);
      bool setProjectionType(const uint ProjectionType_);
      bool setProjectionType(const QString& ProjectionType_);
      bool setBasis(const uint Basis_);
      bool setBasis(const QString& Basis_);
      bool setTileSizeX(const uint TileSizeX_);
      bool setTileSizeY(const uint TileSizeY_);
      bool setTilesNumbering(const uint TilesFing_);
      bool setTilesNumbering(const QString& TilesNumbering_);
      bool setZoomMinimum(const int ZoomMinimum_);
      bool setZoomMaximum(const int ZoomMaximum_);
      bool setZoomShift(const int ZoomShift_);
      bool setFolderName(const QString& FolderName_);
      bool setUpdatability(const uint Updatability_);
      bool setUpdatability(const QString& Updatability_);
      //! \brief проверка на правильность ввода данных в класс
      bool isValid(void) const;

   private:
      //! \var наименование карты
      QString m_Name;
      //! \var описание карты
      QString m_Description;
      //! \var тип карты (спутник, схема, дороги)
      int m_Type;
      //! \var прозрачность карты
      int m_Transparency;
      //! \var тип проекции карты
      int m_ProjectionType;
      //! \var геофизическая основа карты (сфера, эллипс)
      int m_Basis;
      //! \var размер тайла по координате X
      uint m_TileSizeX;
      //! \var размер тайла по координате Y
      uint m_TileSizeY;
      //! \var принцип нумерации тайлов
      int m_TilesNumbering;
      //! \var минимальный масштаб (как степень двойки)
      int m_ZoomMinimum;
      //! \var максимальный масштаб (как степень двойки)
      int m_ZoomMaximum;
      //! \var сдвиг масштаба относительно нумерации масштабов google
      int m_ZoomShift;
      //! \var признак ввода нижней границы масштаба
      bool m_ZoomMinimumSet;
      //! \var признак ввода верхней границы масштаба
      bool m_ZoomMaximumSet;
      //! \var имя папки карты в хранилище
      QString m_FolderName;
      //! \var тип обновляемости тайлов
      uint m_Updatability;
   };
}
#endif
