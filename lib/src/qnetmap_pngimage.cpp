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

#include "qnetmap_pngimage.h"

#include <QFileInfo>

namespace qnetmap
{
   //////////////////////////////////////////////////////////////////////////
   TPNGImage::TPNGImage(const QString& FileName_) : m_FileName(FileName_)
   {
   static const char Signature[] = { (char)0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a };
   QByteArray PNGSignature = QByteArray::fromRawData(Signature, sizeof(Signature));
   //
   QFile File_(FileName_);
   if(File_.open(QIODevice::ReadOnly)) {
      // считываем в байтовый массив
      m_Image = File_.readAll();
      File_.close();
      // проверка на то, что это PNG-файл
      if(m_Image.size() > 56 /*сигнатура + 4 пустые порции*/) {
         // проверяем сигнатуру
         if(m_Image.mid(0, 8) != PNGSignature) m_Image.clear();
         }
      else m_Image.clear();
      }
   }

   //////////////////////////////////////////////////////////////////////////
   TPNGImage::TPNGImage(const QByteArray& PNGImage_) : m_Image(PNGImage_)
   {
   static const char Signature[] = { (char)0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a };
   QByteArray PNGSignature = QByteArray::fromRawData(Signature, sizeof(Signature));
   // проверка на то, что это PNG-файл
   if(m_Image.size() > 56 /*сигнатура + 4 пустые порции*/) {
      // проверяем сигнатуру
      if(m_Image.mid(0, 8) != PNGSignature) m_Image.clear();
      }
   }

   //-----------------------------------------------------------------------
   quint32 TPNGImage::findChunk(const QByteArray& Name_) const 
   {
   quint32 RetValue = 0;
   if(m_Image.size() && isChunkName(Name_)) {
      // ищем
      QByteArray NameEnd("IEND");
      qint32 CurrentPosition = 0;
      quint32 Size = 0;
      bool Found;
      QByteArray ChunkName;
      do {
         CurrentPosition += Size + 8;
         if(CurrentPosition > m_Image.size()) break;
         Size = uInt(CurrentPosition);
         CurrentPosition += 4;
         ChunkName = m_Image.mid(CurrentPosition, 4);
         Found = ChunkName == Name_;
         } while (!Found && ChunkName != NameEnd);
      if(Found) {
         RetValue = CurrentPosition - 4;
         }
      }
   return RetValue;
   }

   //-----------------------------------------------------------------------
   quint32 TPNGImage::readChunk(const QByteArray& Name_, QByteArray& Data_, quint32 &CRC32_) const
   {
   quint32 RetValue = 0;
   if(m_Image.size() && isChunkName(Name_)) {
      // ищем
      QByteArray NameEnd("IEND");
      quint32 CurrentPosition = 0;
      quint32 Size = 0;
      bool Found;
      QByteArray ChunkName;
      do {
         CurrentPosition += Size + 8;
         if(CurrentPosition > (quint32)m_Image.size()) break;
         Size = uInt(CurrentPosition);
         CurrentPosition += 4;
         ChunkName = m_Image.mid(CurrentPosition, 4);
         Found = ChunkName == Name_;
         } while (!Found && ChunkName != NameEnd);
      if(Found) {
         RetValue = CurrentPosition - 4;;
         CurrentPosition += 4;
         Data_ = m_Image.mid(CurrentPosition, Size);
         CRC32_ = uInt(CurrentPosition + Size);
         }
      else Data_.clear();
      }
   return RetValue;
   }

   //-----------------------------------------------------------------------
   bool TPNGImage::readChunk(
      const quint32 Position_, QByteArray& Name_, QByteArray& Data_, quint32 &CRC32_) const
   {
   bool RetValue = false;
   if(m_Image.size() && (Position_ < (quint32)m_Image.size() - 11)) {
      quint32 CurrentPosition = Position_;
      Name_ = m_Image.mid(CurrentPosition + 4, 4);
      if(isChunkName(Name_)) {
         quint32 Size = uInt(CurrentPosition);
         if(CurrentPosition + Size + 11 <= (quint32)m_Image.size()) {
            CurrentPosition += 8;
            Data_ = m_Image.mid(CurrentPosition, Size);
            CRC32_ = uInt(CurrentPosition + Size);
            RetValue = true;
            }
         }
      else {
         Name_.clear();
         Data_.clear();
         CRC32_ = 0;
         }
      }
   return RetValue;
   }

   //-----------------------------------------------------------------------
   bool TPNGImage::addChunk(const QByteArray& Name_, QByteArray& Data_)
   {
   // проверка на существование порции не производится!!!
   bool RetValue = false;
   if(m_Image.size() && isChunkName(Name_)) {
      // добавлять порцию будем перед завершающей
      quint32 PositionIEND = findChunk(QByteArray("IEND"));
      if(PositionIEND > 0) {
         QByteArray NewChunk;
         // длина данных порции (4 байта)
         quint32 UInt32 = reverse(Data_.size());
         NewChunk.append(QByteArray::fromRawData(reinterpret_cast<const char*>(&UInt32), 4));
         // имя порции (4 байта)
         NewChunk.append(Name_);
         // данные порции
         NewChunk.append(Data_);
         // контрольная сумма порции (4 байта)
         // crc32 вычисляется для имени порции и данных
         UInt32 = reverse(TQNetMapHelper::hashCode(NewChunk.data() + 4, NewChunk.size() - 4));
         NewChunk.append(QByteArray::fromRawData(reinterpret_cast<const char*>(&UInt32), 4));
         // вставляем перед порцией IEND
         m_Image.insert(PositionIEND, NewChunk);
         RetValue = true;
         }
      }
   return RetValue;
   }

   //-----------------------------------------------------------------------
   bool TPNGImage::save(void)
   {
   if(m_Image.size() && !m_FileName.isEmpty()) return save(m_FileName);
   else return false;
   }

   //-----------------------------------------------------------------------
   bool TPNGImage::save(QString FileName_)
   {
   bool RetValue = false;
   if(m_Image.size() && !FileName_.isEmpty()) {
      QFileInfo FileInfo(FileName_);
      QFile File(FileInfo.absoluteFilePath());
      if((!File.exists() || File.remove()) && File.open(QIODevice::WriteOnly)) {
         File.write(m_Image);
         File.close();
         }
      }
   return RetValue;
   }

   //-----------------------------------------------------------------------
   bool TPNGImage::isChunkName(const QByteArray& Name_) const
   {
   bool RetValue = false;
   if(Name_.size() == 4) {
      // проверка на допустимые символы из таблицы ASCII (большие и малые латинские буквы)
      for(unsigned i = 0; i < 4; ++i) {
         char Byte = Name_[i];
         if(Byte < 0x41 || Byte > 0x7a || (Byte > 0x5a && Byte < 0x61)) return RetValue;
         }
      RetValue = true;
      }
   return RetValue;
   }

   //-----------------------------------------------------------------------
   quint32 TPNGImage::uInt(const quint32 Position_) const 
   {
   quint32 Number = 0;
   for(unsigned i = 0; i < 4; ++i) {
      unsigned char Byte = m_Image[Position_ + i];
      Number += (Byte << (8 * (3 - i))); 
      }
   return Number;
   }

   //-----------------------------------------------------------------------
   QByteArray TPNGImage::rightJustified(const QByteArray& Data_, const quint32 Width_) const
   {
   if((quint32)Data_.size() >= Width_) return Data_;
   QByteArray Temp;
   quint32 Delta = Width_ - Data_.size();
   for(unsigned i = 0; i < Delta; ++i) Temp.append('\0');
   Temp.append(Data_);
   return Temp;
   }

   //-----------------------------------------------------------------------
   quint32 TPNGImage::reverse(const quint32 Number_) const
   {
   union {
      quint32 UInt32;
      struct {
         unsigned char b1;
         unsigned char b2;
         unsigned char b3;
         unsigned char b4;
         } Bytes;
      } Union;
   //
   Union.UInt32 = Number_;
   unsigned char b;
   b = Union.Bytes.b1; Union.Bytes.b1 = Union.Bytes.b4; Union.Bytes.b4 = b;
   b = Union.Bytes.b2; Union.Bytes.b2 = Union.Bytes.b3; Union.Bytes.b3 = b;
   //
   return Union.UInt32;
   }

   //----------------------------------------------------------------------
   QString TPNGImage::mapVersion(void)
   {
   static const QByteArray ChunkName("qNMp");
   static const QByteArray AttributeName("Map version");
   //
   if(!m_MapVersion.isNull()) return m_MapVersion;
   //
   if(m_Image.size()) {
      QByteArray Data;
      quint32 CRC32;
      if(readChunk(ChunkName, Data, CRC32)) {
         QList<QByteArray> Attributes = Data.split('\0');
         for(int i = 0; i < Attributes.size(); ++i) {
            if(Attributes.at(i) == AttributeName) {
               m_MapVersion = Attributes.at(i + 1);
               break;
               }
            }
         }
      }
   else m_MapVersion = "";
   //
   return m_MapVersion;
   }
}
