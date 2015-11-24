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

#include "qnetmap_universaltilemapxmlhandler.h"
#include "qnetmap_xmltagnames.h"
#include "qnetmap_mapadapter.h"

namespace qnetmap 
{
   //------------------------------------------------------------------
   bool TUniversalTileMapXMLHandler::startElement(const QString&, const QString&,
                                                  const QString& Name_, 
                                                  const QXmlAttributes& Attr_)
   {
   if(!w_TileMapAdapterParametersList) return false;
   //
   static const QString EmptyParametr  = QString("\"\n") + 
		QNetMapTranslator::tr("Parameter '%1' contains empty value"
		/* Russian: Параметр '%1' содержит пустое значение */);
   static const QString TagInside      = QString("\"\n") +
		QNetMapTranslator::tr("must be inside the tag"
		/* Russian: должен быть внутри тега */) + 
		QString(" \"");
   static const QString WrongParameter = QString("\"\n") +
		QNetMapTranslator::tr("contains invalid parameter"
		/* Russian: содержит неверный параметр */) +
		QString(" \"%1=%2\"");
   static const QString HostRestrictions = QString("\"\n") +
      QNetMapTranslator::tr("do not contains hosts numbers for the pattern"
		/* Russian: не содержит диапазона номеров хостов для шаблона */) +
		QString(" \"%1\"");
   static const QString WrongParametersCount = QString("\"\n") +
      QNetMapTranslator::tr("contains invalid number of parameters (must be %1)"
		/* Russian: содержит неверное количество параметров (должно быть %1) */);
   //
   QString ErrorPrefix = QString("TUniversalTileMapXMLParser\n") + 
		QNetMapTranslator::tr("Tag \"%1\"" /* Russian: Тег "%1" */);
   //
   static TTileMapAdapterParametersList::iterator CurrentParameters;
   //
   if(Name_ == XML_TAG_QNETMAP) {
      if(m_TagsStack.empty()) {
         m_CurrentTag = Name_;
         m_TagsStack.push_back(Name_);
         w_TileMapAdapterParametersList->clear();
         w_HttpParametersList.clear();
         }
      else {
         m_ErrorString = ErrorPrefix.arg(sequenceTags() + "::" + Name_) + 
				QString("\"\n") + 
            QNetMapTranslator::tr("must be the root tag."
				/* Russian: должен быть корневым тегом. */);
         return false;
         }
      }//========
   else if(Name_ == XML_TAG_HTTP) {
      if(m_CurrentTag == XML_TAG_QNETMAP) {
         m_CurrentTag = Name_;
         m_TagsStack.push_back(Name_);
         }
      else {
         m_ErrorString = ErrorPrefix.arg(sequenceTags() + "::" + Name_) + 
            TagInside + XML_TAG_QNETMAP + "\"";
         return false;
         }
      }//========
   else if(Name_ == XML_TAG_HTTPFIELDS) {
      if(m_CurrentTag == XML_TAG_HTTP) {
         m_CurrentTag = Name_;
         m_TagsStack.push_back(Name_);
         }
      else {
         m_ErrorString = ErrorPrefix.arg(sequenceTags() + "::" + Name_) + 
            TagInside + XML_TAG_HTTP + "\"";
         return false;
         }
      w_HttpParametersList.push_back(THttpParameters());
      THttpParametersList::iterator CurrentParameters = --(w_HttpParametersList.end());
      uint ParametersCount = Attr_.length();
      for(uint ParameterNumber = 0; ParameterNumber < ParametersCount; ParameterNumber++) {
         QString Name  = Attr_.qName(ParameterNumber);
         QString Value = Attr_.value(ParameterNumber);
         if(Value.isEmpty()) {
            m_ErrorString = ErrorPrefix.arg(sequenceTags()) + EmptyParametr.arg(Name);
            return false;
            }
         bool Result = false;
         if(Name == XML_PARAMETER_USERAGENT) {
            Result = CurrentParameters->setUserAgent(Value);
            }
         if(!Result) {
            m_ErrorString = ErrorPrefix.arg(sequenceTags()) + WrongParameter.arg(Name).arg(Value);
            return false;
            }
         }
      }//========
   else if(Name_ == XML_TAG_TILEMAPADAPTERS) {
      if(m_CurrentTag == XML_TAG_QNETMAP) {
         m_CurrentTag = Name_;
         m_TagsStack.push_back(Name_);
         }
      else {
         m_ErrorString = ErrorPrefix.arg(sequenceTags() + "::" + Name_) + 
            TagInside + XML_TAG_QNETMAP + "\"";
         return false;
         }
      }//========
   else if(Name_ == XML_TAG_TILEMAPADAPTER) {
      if(m_CurrentTag == XML_TAG_TILEMAPADAPTERS) {
         m_CurrentTag = Name_;
         m_TagsStack.push_back(Name_);
         w_TileMapAdapterParametersList->push_back(TTileMapAdapterParameters());
         CurrentParameters = --(w_TileMapAdapterParametersList->end());
         uint ParametersCount = Attr_.length();
         uint Mask = 0;
         for(uint ParameterNumber = 0; ParameterNumber < ParametersCount; ParameterNumber++) {
            QString Name  = Attr_.qName(ParameterNumber);
            QString Value = Attr_.value(ParameterNumber);
            if(Value.isEmpty()) {
               m_ErrorString = ErrorPrefix.arg(sequenceTags()) + EmptyParametr.arg(Name);
               return false;
               }
            bool Result = false;
            if(Name == XML_PARAMETER_ADAPTERNAME) {
               m_CurrentMapAdapter = Value;
               Result = CurrentParameters->setName(Value);
               if(Result) Mask |= 1;
               }
            else if(Name == XML_PARAMETER_ADAPTERDESCRIPTION) {
               Result = CurrentParameters->setDescription(Value);
               if(Result) Mask |= 2;
               }
            else if(Name == XML_PARAMETER_ADAPTERTYPE) {
               Result = CurrentParameters->setType(Value);
               if(Result) Mask |= 4;
               }
            else if(Name == XML_PARAMETER_ADAPTERTRANSPARENCY) {
               Result = CurrentParameters->setTransparency(Value);
               }
            else if(Name == XML_PARAMETER_ADAPTERFOLDERNAME) {
               Result = CurrentParameters->setFolderName(Value);
               if(Result) Mask |= 8;
               }
            else if(Name == XML_PARAMETER_UPDATABILITY) {
               Result = CurrentParameters->setUpdatability(Value);
               if(Result && 
                  CurrentParameters->updatability() == TMapAdapter::Consts::QueryUpdatabilityDynamic) {
                  //
                  Mask |= 16;
                  }
               }
            if(!Result) {
               m_ErrorString = ErrorPrefix.arg(sequenceTags()) + WrongParameter.arg(Name).arg(Value);
               return false;
               }
            }
         // проверка на соответствие параметров
         if(((Mask & 7) != 7) && ((Mask & 0x18) != 0x8) && ((Mask & 0x18) != 0x10)) {
            m_ErrorString = ErrorPrefix.arg(sequenceTags()) + WrongParametersCount.arg(ParametersCount);
            return false;
            }
         }
      else {
         m_ErrorString = ErrorPrefix.arg(sequenceTags() + "::" + Name_) + 
            TagInside + XML_TAG_TILEMAPADAPTERS + "\"";
         return false;
         }
      }//========
   else if(Name_ == XML_TAG_GEOPARAMETERS) {
      if(m_CurrentTag == XML_TAG_TILEMAPADAPTER) {
         m_CurrentTag = Name_;
         m_TagsStack.push_back(Name_);
         }
      else {
         m_ErrorString = ErrorPrefix.arg(sequenceTags() + "::" + Name_) + 
            TagInside + XML_TAG_TILEMAPADAPTER + "\"";
         return false;
         }
      }//========
   else if(Name_ == XML_TAG_URL) {
      if(m_CurrentTag == XML_TAG_TILEMAPADAPTER) {
         m_CurrentTag = Name_;
         m_TagsStack.push_back(Name_);
         }
      else {
         m_ErrorString = ErrorPrefix.arg(sequenceTags() + "::" + Name_) + 
            TagInside + XML_TAG_TILEMAPADAPTER + "\"";
         return false;
         }
      }//========
   else if(Name_ == XML_TAG_MAPVERSION) {
      if(m_CurrentTag == XML_TAG_TILEMAPADAPTER) {
         m_CurrentTag = Name_;
         m_TagsStack.push_back(Name_);
         if(Attr_.length() == 1) {
            QString Name  = Attr_.qName(0);
            QString Value = Attr_.value(0);
            if(Value.isEmpty()) {
               m_ErrorString = ErrorPrefix.arg(sequenceTags()) + EmptyParametr.arg(Name);
               return false;
               }
            bool Result = false;
            if(Name == XML_PARAMETER_DEFAULTMAPVERSION) {
               Result = CurrentParameters->setDefaultMapVersion(Value);
               }
            if(!Result) {
               m_ErrorString = ErrorPrefix.arg(sequenceTags()) + WrongParameter.arg(Name).arg(Value);
               return false;
               }
            }
         else {
            m_ErrorString = ErrorPrefix.arg(sequenceTags()) + WrongParametersCount.arg(1);
            return false;
            }
         }
      else {
         m_ErrorString = ErrorPrefix.arg(sequenceTags() + "::" + Name_) + 
            TagInside + XML_TAG_TILEMAPADAPTER + "\"";
         return false;
         }
      }//========
   else if(Name_ == XML_TAG_PROJECTION) {
      if(m_CurrentTag == XML_TAG_GEOPARAMETERS) {
         m_CurrentTag = Name_;
         m_TagsStack.push_back(Name_);
         if(Attr_.length() == 2) {
            for(uint ParameterNumber = 0; ParameterNumber < 2; ParameterNumber++) {
               QString Name  = Attr_.qName(ParameterNumber);
               QString Value = Attr_.value(ParameterNumber);
               if(Value.isEmpty()) {
                  m_ErrorString = ErrorPrefix.arg(sequenceTags()) + EmptyParametr.arg(Name);
                  return false;
                  }
               bool Result = false;
               if(Name == XML_PARAMETER_PROJECTIONTYPE) Result = CurrentParameters->setProjectionType(Value);
               else if(Name == XML_PARAMETER_PROJECTIONBASIS) {
                  Result = CurrentParameters->setBasis(Value);
                  }
               if(!Result) {
                  m_ErrorString = ErrorPrefix.arg(sequenceTags()) + WrongParameter.arg(Name).arg(Value);
                  return false;
                  }
               }
            }
         else {
            m_ErrorString = ErrorPrefix.arg(sequenceTags()) + WrongParametersCount.arg(2);
            return false;
            }
         }
      else {
         m_ErrorString = ErrorPrefix.arg(sequenceTags() + "::" + Name_) + 
            TagInside + XML_TAG_GEOPARAMETERS + "\"";
         return false;
         }
      }//========
   else if(Name_ == XML_TAG_TILES) {
      if(m_CurrentTag == XML_TAG_GEOPARAMETERS) {
         m_CurrentTag = Name_;
         m_TagsStack.push_back(Name_);
         if(Attr_.length() == 3) {
            for(uint ParameterNumber = 0; ParameterNumber < 3; ParameterNumber++) {
               QString Name  = Attr_.qName(ParameterNumber);
               QString Value = Attr_.value(ParameterNumber);
               if(Value.isEmpty()) {
                  m_ErrorString = ErrorPrefix.arg(sequenceTags()) + EmptyParametr.arg(Name);
                  return false;
                  }
               bool Result = false;
               if(Name == XML_PARAMETER_TILESSIZEX) Result = CurrentParameters->setTileSizeX(Value.toInt());
               else if(Name == XML_PARAMETER_TILESSIZEY) {
                  Result = CurrentParameters->setTileSizeY(Value.toInt());
                  }
               else if(Name == XML_PARAMETER_TILESNUMBERING) {
                  Result = CurrentParameters->setTilesNumbering(Value);
                  }
               if(!Result) {
                  m_ErrorString = ErrorPrefix.arg(sequenceTags()) + WrongParameter.arg(Name).arg(Value);
                  return false;
                  }
               }
            }
         else {
            m_ErrorString = ErrorPrefix.arg(sequenceTags()) + WrongParametersCount.arg(3);
            return false;
            }
         }
      else {
         m_ErrorString = ErrorPrefix.arg(sequenceTags() + "::" + Name_) + 
            TagInside + XML_TAG_GEOPARAMETERS + "\"";
         return false;
         }
      }//========
   else if(Name_ == XML_TAG_ZOOMS) {
      if(m_CurrentTag == XML_TAG_GEOPARAMETERS) {
         m_CurrentTag = Name_;
         m_TagsStack.push_back(Name_);
         uint ParametersCount = Attr_.length();
         if(ParametersCount == 2 || ParametersCount == 3) {
            for(uint ParameterNumber = 0; ParameterNumber < ParametersCount; ParameterNumber++) {
               QString Name  = Attr_.qName(ParameterNumber);
               QString Value = Attr_.value(ParameterNumber);
               if(Value.isEmpty()) {
                  m_ErrorString = ErrorPrefix.arg(sequenceTags()) + EmptyParametr.arg(Name);
                  return false;
                  }
               bool Result = false;
               if(Name == XML_PARAMETER_ZOOMMINIMUM) {
                  Result = CurrentParameters->setZoomMinimum(Value.toInt());
                  }
               else if(Name == XML_PARAMETER_ZOOMMAXIMUM) {
                  Result = CurrentParameters->setZoomMaximum(Value.toInt());
                  }
               else if(Name == XML_PARAMETER_ZOOMSHIFT) {
                  Result = CurrentParameters->setZoomShift(Value.toInt());
                  }
               if(!Result) {
                  m_ErrorString = ErrorPrefix.arg(sequenceTags()) + WrongParameter.arg(Name).arg(Value);
                  return false;
                  }
               }
            }
         else {
            m_ErrorString = ErrorPrefix.arg(sequenceTags()) + WrongParametersCount.arg(
					QNetMapTranslator::tr("2 or 3" /* Russian: 2 или 3 */));
            return false;
            }
         }
      else {
         m_ErrorString = ErrorPrefix.arg(sequenceTags() + "::" + Name_) + 
            TagInside + XML_TAG_GEOPARAMETERS + "\"";
         return false;
         }
      }//========
   else if(Name_ == XML_TAG_HOST) {
      if(m_CurrentTag == XML_TAG_URL) {
         m_CurrentTag = Name_;
         m_TagsStack.push_back(Name_);
         if(Attr_.length() == 1) {
            QString Name  = Attr_.qName(0);
            QString Value = Attr_.value(0);
            if(Value.isEmpty()) {
               m_ErrorString = ErrorPrefix.arg(sequenceTags()) + EmptyParametr.arg(Name);
               return false;
               }
            bool Result = false;
            if(Name == XML_PARAMETER_HOSTTEMPLATE) {
               if(!Value.contains("%1")) Result = CurrentParameters->setHostTemplate(Value);
               else {
                  m_ErrorString = ErrorPrefix.arg(sequenceTags()) + HostRestrictions.arg(Value);
                  return false;
                  }
               }
            if(!Result) {
               m_ErrorString = ErrorPrefix.arg(sequenceTags()) + WrongParameter.arg(Name).arg(Value);
               return false;
               }
            }
         else if(Attr_.length() == 3) {
            for(uint ParameterNumber = 0; ParameterNumber < 3; ParameterNumber++) {
               QString Name  = Attr_.qName(ParameterNumber);
               QString Value = Attr_.value(ParameterNumber);
               if(Value.isEmpty()) {
                  m_ErrorString = ErrorPrefix.arg(sequenceTags()) + EmptyParametr.arg(Name);
                  return false;
                  }
               bool Result = false;
               if(Name == XML_PARAMETER_HOSTTEMPLATE)     Result = CurrentParameters->setHostTemplate(Value);
               else if(Name == XML_PARAMETER_HOSTMINIMUM) Result = CurrentParameters->setHostNumberMinimum(Value.toStdString().c_str()[0]);
               else if(Name == XML_PARAMETER_HOSTMAXIMUM) Result = CurrentParameters->setHostNumberMaximum(Value.toStdString().c_str()[0]);
               //
               if(!Result) {
                  m_ErrorString = ErrorPrefix.arg(sequenceTags()) + WrongParameter.arg(Name).arg(Value);
                  return false;
                  }
               }
            }
         else {
            m_ErrorString = ErrorPrefix.arg(sequenceTags()) + WrongParametersCount.arg(3);
            return false;
            }
         }
      else {
         m_ErrorString = ErrorPrefix.arg(sequenceTags() + "::" + Name_) + 
            TagInside + XML_TAG_URL + "\"";
         return false;
         }
      }//========
   else if(Name_ == XML_TAG_QUERY) {
      if(m_CurrentTag == XML_TAG_URL) {
         m_CurrentTag = Name_;
         m_TagsStack.push_back(Name_);
         uint ParametersCount = Attr_.length();
         if(ParametersCount >= 1 && ParametersCount <= 3) {
            for(uint ParameterNumber = 0; ParameterNumber < ParametersCount; ParameterNumber++) {
               QString Name  = Attr_.qName(ParameterNumber);
               QString Value = Attr_.value(ParameterNumber);
               if(Value.isEmpty()) {
                  m_ErrorString = ErrorPrefix.arg(sequenceTags()) + EmptyParametr.arg(Name);
                  return false;
                  }
               bool Result = false;
               if(Name == XML_PARAMETER_QUERYTEMPLATE)         Result = CurrentParameters->setQueryTemplate(Value);
               else if(Name == XML_PARAMETER_COORDINATESWIDTH) Result = CurrentParameters->setCoordinatesWidth(Value.toInt());
               else if(Name == XML_PARAMETER_ZOOMWIDTH)        Result = CurrentParameters->setZoomWidth(Value.toInt());
               //
               if(!Result) {
                  m_ErrorString = ErrorPrefix.arg(sequenceTags()) + WrongParameter.arg(Name).arg(Value);
                  return false;
                  }
               }
            }
         else {
            m_ErrorString = ErrorPrefix.arg(sequenceTags()) + WrongParametersCount.arg(1);
            return false;
            }
         }
      else {
         m_ErrorString = ErrorPrefix.arg(sequenceTags() + "::" + Name_) + 
            TagInside + XML_TAG_URL + "\"";
         return false;
         }
      }//========
   else if(Name_ == XML_TAG_SOURCE) {
      if(m_CurrentTag == XML_TAG_MAPVERSION) {
         m_CurrentTag = Name_;
         m_TagsStack.push_back(Name_);
         if(Attr_.length() == 3) {
            for(uint ParameterNumber = 0; ParameterNumber < 3; ParameterNumber++) {
               QString Name  = Attr_.qName(ParameterNumber);
               QString Value = Attr_.value(ParameterNumber);
               if(Value.isEmpty()) {
                  m_ErrorString = ErrorPrefix.arg(sequenceTags()) + EmptyParametr.arg(Name);
                  return false;
                  }
               bool Result = false;
               if(Name == XML_PARAMETER_SOURCEURL) Result = CurrentParameters->setMapVersionUrl(Value);
               else if(Name == XML_PARAMETER_SOURCEPREFIX) {
                  Result = CurrentParameters->setMapVersionPrefix(Value);
                  }
               else if(Name == XML_PARAMETER_SOURCEPOSTFIX) {
                  Result = CurrentParameters->setMapVersionPostfix(Value);
                  }
               if(!Result) {
                  m_ErrorString = ErrorPrefix.arg(sequenceTags()) + WrongParameter.arg(Name).arg(Value);
                  return false;
                  }
               }
            }
         else {
            m_ErrorString = ErrorPrefix.arg(sequenceTags()) + WrongParametersCount.arg(3);
            return false;
            }
         }
      else {
         m_ErrorString = ErrorPrefix.arg(sequenceTags() + "::" + Name_) + 
            TagInside + XML_TAG_MAPVERSION + "\"";
         return false;
         }
      }//========
   else {
      m_ErrorString = ErrorPrefix.arg(sequenceTags()) + QString("\" ") +
			QNetMapTranslator::tr("is unknown" /* Russian: неизвестен */);
      return false;
      }

   return true;
   }

   //------------------------------------------------------------------
   bool TUniversalTileMapXMLHandler::endElement(const QString&, const QString&, const QString& Name_)
   {
   if(m_CurrentTag == Name_) {
      // восстанавливаем родительский тег
      m_TagsStack.pop_back();
      if(m_TagsStack.empty()) m_CurrentTag = "";
      else                    m_CurrentTag = *(--(m_TagsStack.end()));
      }
   else {
      m_ErrorString = QNetMapTranslator::tr(
			"TUniversalTilesMapXMLParser: The XML file structure is broken."
			/* Russian: TUniversalTilesMapXMLParser: Нарушена структура XML-файла. */) + 
			QLatin1Char('\n') +
			QNetMapTranslator::tr("The tag \"%1\" is closing by the tag \"%2\""
			/* Russian: Открытый тег \"%1\" закрывается тегом \"%2\" */)
			.arg(m_CurrentTag).arg(Name_);
      return false;
      }

   return true;
   }

   //------------------------------------------------------------------
   bool TUniversalTileMapXMLHandler::characters(const QString& String_)
   {
   Q_UNUSED(String_)

   return true;
   }


   //------------------------------------------------------------------
   QString TUniversalTileMapXMLHandler::errorString(void)
   {
   QString ErrorString;

   if(m_ErrorString != "") ErrorString = m_ErrorString;
   else                    ErrorString = QXmlDefaultHandler::errorString();

   return ErrorString;
   }

   //------------------------------------------------------------------
   bool TUniversalTileMapXMLHandler::fatalError(const QXmlParseException &Exception_)
   {
	return error(Exception_);
   }

   //------------------------------------------------------------------
   bool TUniversalTileMapXMLHandler::error(const QXmlParseException &Exception_)
   {
   m_ErrorString = 
      QNetMapTranslator::tr("Adapter %1\nline %2, position %3\n%4\n%5"
		/* Russian: Адаптер %1
		строка %2, позиция в строке %3
		%4
		%5 */)
         .arg(m_CurrentMapAdapter)
         .arg(Exception_.lineNumber()) 
         .arg(Exception_.columnNumber())
         .arg(m_ErrorString)
         .arg(Exception_.message()); 
   return false; 
   }
}
