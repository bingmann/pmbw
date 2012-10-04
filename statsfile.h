/***************************************************************************
 * src/tools/statsfile.h
 *
 * Class to output statistics in a flexible text file as key=value pairs.
 *
 * Copyright (C) 2012 Timo Bingmann
 **************************************************************************/

#include <map>
#include <sstream>
#include <fstream>
#include <assert.h>

/// Cache of key=value stats during run of algorithm
class StatsCache
{
public:

    typedef std::map<std::string, std::string>  statsmap_type;

private:

    statsmap_type               m_statsmap;

    std::string                 m_thiskey;

    std::ostringstream          m_curritem;

public:

    /// Clear all data
    void clear()
    {
        m_statsmap.clear();
        m_thiskey.clear();
        m_curritem.str("");
    }

    /// Append a substring to the current or new key.
    template <typename Type>
    StatsCache& operator>> (const Type& t)
    {
        if (m_thiskey.size())
        {
            m_statsmap.insert( std::make_pair(m_thiskey, m_curritem.str()) );
            m_thiskey.clear();
            m_curritem.str("");
        }

        m_curritem << t;
        return *this;
    }

    template <typename Type>
    StatsCache& operator<< (const Type& t)
    {
        if (m_thiskey.size() == 0)
        {
            m_thiskey = m_curritem.str();
            assert(m_thiskey.size() && "Key is empty!");
            m_curritem.str("");
        }

        m_curritem << t;
        return *this;
    }

    const statsmap_type& get_statsmap()
    {
        if (m_thiskey.size())
        {
            m_statsmap.insert( std::make_pair(m_thiskey, m_curritem.str()) );
            m_thiskey.clear();
            m_curritem.str("");
        }

        return m_statsmap;
    }
};

/// Simple writer of statistic files containing key=value pairs per line.
class StatsWriter
{
private:

    std::ofstream       m_out;

    unsigned int        m_firstfield;

    std::ostringstream  m_line;

public:

    StatsWriter(const char* filename)
    {
        m_out.open(filename, std::ios::app);

        m_line << "RESULT\t";

        // output date, time and hostname to m_line

        char datetime[64];
        time_t tnow = time(NULL);

        strftime(datetime,sizeof(datetime),"%Y-%m-%d %H:%M:%S", localtime(&tnow));
        m_line << "datetime=" << datetime;

        char hostname[128];
        gethostname(hostname, sizeof(hostname));

        m_line << "\thost=" << hostname;
    }

    ~StatsWriter()
    {
        m_out << m_line.str() << "\n";
        std::cout << m_line.str() << "\n";
    }

    // Append a key
    template <typename Type>
    StatsWriter& operator>> (const Type& t)
    {
        m_firstfield = 1;
        m_line << "\t" << t;

        return *this;
    }

    // Append a value
    template <typename Type>
    StatsWriter& operator<< (const Type& t)
    {
        if (m_firstfield) {
            m_line << "=";
            m_firstfield = 0;
        }

        m_line << std::setprecision(20) << t;

        return *this;
    }

    // Append a stats map
    void append_statsmap(StatsCache& sc)
    {
        const StatsCache::statsmap_type& sm = sc.get_statsmap();

        for (StatsCache::statsmap_type::const_iterator si = sm.begin();
             si != sm.end(); ++si)
        {
            m_line << "\t" << si->first << "=" << si->second;
        }
    }
};

class SizeLogger
{
private:
    
    /// log output file
    std::ofstream       m_logfile;

    /// begin timestamp of current averaging
    double              m_begintime;

    /// end timestamp of current averaging
    double              m_endtime;

    /// count of current averaging
    double              m_avgcount;

    /// sum of current averaging
    double              m_avgsum;

    /// timestamp function
    static inline double timestamp() {
        return omp_get_wtime();
    }

public:

    SizeLogger(const char* logname)
        : m_logfile(logname, std::ios::app),
          m_begintime(0)
    {
    }

    SizeLogger& operator<< (unsigned long value)
    {
        double thistime = timestamp();

        if (m_begintime == 0)
        {
            m_begintime = m_endtime = thistime;
            m_avgcount = 1;
            m_avgsum = value;
        }
        else if (m_begintime - thistime > 0.01 || m_avgcount >= 1000)
        {
            m_logfile << std::setprecision(16) << ((m_begintime + m_endtime) / 2.0) << " "
                      << std::setprecision(16) << (m_avgsum / m_avgcount) << " " << m_avgcount << "\n";

            m_begintime = m_endtime = thistime;
            m_avgcount = 1;
            m_avgsum = value;
        }
        else
        {
            m_endtime = thistime;
            m_avgcount++;
            m_avgsum += value;
        }

        return *this;
    }

    ~SizeLogger()
    {
        if (m_begintime != 0)
        {
            m_logfile << std::setprecision(16) << ((m_begintime + m_endtime) / 2.0) << " "
                      << std::setprecision(16) << (m_avgsum / m_avgcount) << " " << m_avgcount << "\n";
        }
    }
};
