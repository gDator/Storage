#ifndef LOG_HPP
#define LOG_HPP

#define BOOST_USE_WINAPI_VERSION BOOST_WINAPI_VERSION_WIN6
#include <fstream>
#include <locale>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/core/null_deleter.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared_object.hpp>
#include <iostream>
#include <string>


void initLog(std::string filename, int mode = 1);
std::ostream& operator<< (std::ostream& strm, boost::log::trivial::severity_level level);
inline boost::log::sources::severity_logger<boost::log::trivial::severity_level> lg;  //gloabl variable for logging

#define LOG_WARN(message) BOOST_LOG_SEV(lg, boost::log::trivial::severity_level::warning) << __FILE__ << ":" <<__LINE__ << " [" << __FUNCTION__ << "]: " << message;
#define LOG_ERROR(message) BOOST_LOG_SEV(lg, boost::log::trivial::severity_level::error) << __FILE__ << ":" <<__LINE__ << " [" << __FUNCTION__ << "]: " << message;
#define LOG_HISTORY(message) BOOST_LOG_SEV(lg, boost::log::trivial::severity_level::info) << message;

#ifdef DEBUG
#define LOG_TRACE(message) BOOST_LOG_SEV(lg, boost::log::trivial::severity_level::trace) << __FILE__ << ":" <<__LINE__ << " [" << __FUNCTION__ << "]: " << message;
#define LOG_INFO(message) BOOST_LOG_SEV(lg, boost::log::trivial::severity_level::info) << __FILE__ << ":" <<__LINE__ << " [" << __FUNCTION__ << "]: " << message;

#else
#define LOG_TRACE(message)
#define LOG_INFO(message)
#endif
#endif /*LOG_HPP*/