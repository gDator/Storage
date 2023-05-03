#include "logger.hpp"
using namespace boost::log::trivial;
using namespace boost::log;
namespace sinks = boost::log::sinks;
using namespace boost;

typedef sinks::synchronous_sink<sinks::text_ostream_backend> text_sink;

void coloring_formatter(log::record_view const& rec, log::formatting_ostream& strm)
{
    auto severity = rec[log::trivial::severity];
    if (severity)
    {
        // Set the color
        switch (severity.get())
        {
        case log::trivial::severity_level::info:
            strm << "\033[32m";
            break;
        case log::trivial::severity_level::warning:
            strm << "\033[33m";
            break;
        case log::trivial::severity_level::error:
        case log::trivial::severity_level::fatal:
            strm << "\033[31m";
            break;
        default:
            break;
        }
    }

    // Format the message here...
    strm << rec[log::expressions::smessage];

    if (severity)
    {
        // Restore the default color
        strm << "\033[0m";
    }
}

std::ostream& operator<< (std::ostream& strm, log::trivial::severity_level level)
{
    static const char* strings[] =
    {
        "normal",
        "notification",
        "warning",
        "error",
        "critical"
    };

    if (static_cast< std::size_t >(level) < sizeof(strings) / sizeof(*strings))
        strm << strings[level];
    else
        strm << static_cast< int >(level);

    return strm;
}

void initLog(std::string filename, int mode)
{  
    log::core::get()->set_filter(log::trivial::severity >= log::trivial::trace);
    if(mode == 1 || mode == 3)    // Add attributes
    {
        // create sink to logfile
        boost::shared_ptr<text_sink> sink = boost::make_shared<text_sink>();
        sink->locked_backend()->add_stream( boost::make_shared<std::ofstream>(filename.c_str()));

        log::add_common_attributes();
        sink->set_formatter //log::add_file_log
        (
            // log::keywords::file_name = filename + "%N.log",                                        /*< file name pattern >*/
            // log::keywords::rotation_size = 10 * 1024 * 1024,                                   /*< rotate files every 10 MiB... >*/
            // log::keywords::time_based_rotation = log::sinks::file::rotation_at_time_point(0, 0, 0), /*< ...or at midnight >*/
            /*< log record format >*/
            log::expressions::stream
                    << log::expressions::attr< unsigned int >("LineID")
                    << log::expressions::format_date_time< boost::posix_time::ptime >("TimeStamp", " [%Y-%m-%d %H:%M:%S]")
                    << " <" << log::trivial::severity
                    << ">: " << log::expressions::smessage                    
        );
    	
        // flush
        sink->locked_backend()->auto_flush(true);
        
        // register sink
        log::core::get()->add_sink(sink);
    }
    if(mode == 2 || mode == 3)
    {
        boost::shared_ptr<text_sink> sink = boost::make_shared<text_sink>();
        boost::shared_ptr<std::ostream> stream(&std::cout, boost::null_deleter());
        sink->locked_backend()->add_stream(stream);

        // flush
        sink->locked_backend()->auto_flush(true);

        // format sink
        sink->set_formatter(&coloring_formatter);

        // filter
        // TODO add any filters

        // register sink
        log::core::get()->add_sink(sink);
    }
}