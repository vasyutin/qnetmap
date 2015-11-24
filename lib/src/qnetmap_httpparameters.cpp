#include "qnetmap_httpparameters.h"

//===================================================
qnetmap::THttpParameters::THttpParameters() : m_UserAgent() {}

//-----------------------------------------------------------
bool qnetmap::THttpParameters::isValid( void ) const
{
   return !m_UserAgent.isEmpty();
}

//-----------------------------------------------------------
bool qnetmap::THttpParameters::setUserAgent( const QString& UserAgent_ )
{
   m_UserAgent = UserAgent_;
   return !m_UserAgent.isEmpty();
}
