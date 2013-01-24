// -*- mode: c++; fill-column: 79 -*-

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <vector>
#include <set>
#include <map>

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

// *** Warnings Output Function

bool gopt_warnings = false;

#define ERR(x)  do { std::cerr << x << std::endl; } while(0)
#define ERRX(x)  do { std::cerr << x; } while(0)
#define WARN(x) do { if (gopt_warnings) { std::cerr << x << std::endl; } } while(0)

// *** List of Function Name Processed (and Their Order)

static const char* funclist[] =
{
    "funcSeqRead64PtrSimpleLoop",
    "funcSeqRead64PtrUnrollLoop",
    "funcSeqWrite64PtrSimpleLoop",
    "funcSeqWrite64PtrUnrollLoop",

    "funcSeqRead128PtrSimpleLoop",
    "funcSeqRead128PtrUnrollLoop",
    "funcSeqWrite128PtrSimpleLoop",
    "funcSeqWrite128PtrUnrollLoop",

    "funcSeqRead64IndexSimpleLoop",
    "funcSeqRead64IndexUnrollLoop",
    "funcSeqWrite64IndexSimpleLoop",
    "funcSeqWrite64IndexUnrollLoop",

    "funcSkipRead64PtrSimpleLoop",
    "funcSkipWrite64PtrSimpleLoop",
    "funcSkipRead128PtrSimpleLoop",
    "funcSkipWrite128PtrSimpleLoop",
    "funcSkipRead64IndexSimpleLoop",
    "funcSkipWrite64IndexSimpleLoop",

    "funcPermRead64SimpleLoop",
    "funcPermRead64UnrollLoop",
    NULL
};

// ****************************************************************************
// *** Functions to read RESULT key-value files into Result vector

struct Result
{
    // *** contains the field read from each RESULT line
    std::string datetime;
    std::string host;
    std::string funcname;
    size_t nprocs;
    size_t areasize;
    size_t threadsize;
    size_t testsize;
    size_t repeats;
    size_t testvol;
    size_t testaccess;
    double time;
    double bandwidth;
    double rate;
    size_t funcname_id;  // index of funcname in funclist (for nicer order)

    Result()
        : nprocs(0), areasize(0), threadsize(0), testsize(0), repeats(0),
          testvol(0), testaccess(0),
          time(0), bandwidth(0), rate(0)
    {
    }

    /// parse a single RESULT key-value and save its information
    bool process_line_keyvalue(const std::string& key, const std::string& value);

    /// sort order of results is: (funcname_id,nprocs,testsize)
    bool operator< (const Result& b) const
    {
        if (funcname_id == b.funcname_id) {
            if (nprocs == b.nprocs) {
                return testsize < b.testsize;
            }
            return nprocs < b.nprocs;
        }
        return funcname_id < b.funcname_id;
    }
};

/// global: hostname read from results
std::string g_hostname;

/// global: the sorted results array
std::vector<Result> g_results;

/// parse a number as size_t with error detection
static inline bool
parse_sizet(const std::string& value, size_t& out)
{
    char* endp;
    out = strtoull(value.c_str(), &endp, 10);
    return (endp && *endp == 0);
}

/// parse a number as double with error detection
static inline bool
parse_double(const std::string& value, double& out)
{
    char* endp;
    out = strtod(value.c_str(), &endp);
    return (endp && *endp == 0);
}

/// parse a funcname into funcname_id with error detection
static inline bool
find_funcname(const std::string& funcname, size_t& funcname_id)
{
    for (size_t i = 0; funclist[i]; ++i) {
        if (funcname == funclist[i]) {
            funcname_id = i;
            return true;
        }
    }
    return false;
}

/// parse a single RESULT key-value and save its information
bool Result::process_line_keyvalue(const std::string& key, const std::string& value)
{
    if (key == "datetime") {
        datetime = value;
        return true;
    }
    else if (key == "host") {
        host = value;
        return true;
    }
    else if (key == "funcname") {
        funcname = value;
        return find_funcname(funcname, funcname_id);
    }
    else if (key == "nprocs") {
        return parse_sizet(value, nprocs);
    }
    else if (key == "areasize") {
        return parse_sizet(value, areasize);
    }
    else if (key == "threadsize") {
        return parse_sizet(value, threadsize);
    }
    else if (key == "testsize") {
        return parse_sizet(value, testsize);
    }
    else if (key == "repeats") {
        return parse_sizet(value, repeats);
    }
    else if (key == "testvol") {
        return parse_sizet(value, testvol);
    }
    else if (key == "testaccess") {
        return parse_sizet(value, testaccess);
    }
    else if (key == "time") {
        return parse_double(value, time);
    }
    else if (key == "bandwidth") {
        return parse_double(value, bandwidth);
    }
    else if (key == "rate") {
        return parse_double(value, rate);
    }
    else {
        return false;
    }
}

/// process a single line containing RESULT key-value pairs
bool process_line(const std::string& line)
{
    std::string::size_type splitpos = line.find('\t');
    if (splitpos == std::string::npos) return false;

    if (line.substr(0,splitpos) != "RESULT") return false;

    struct Result result;

    do
    {
        std::string::size_type splitprev = splitpos+1;
        splitpos = line.find('\t', splitprev);

        std::string keyvalue = line.substr(splitprev, splitpos - splitprev);

        std::string::size_type equalpos = keyvalue.find('=');
        if (equalpos != std::string::npos)
        {
            if (!result.process_line_keyvalue( keyvalue.substr(0,equalpos),
                                               keyvalue.substr(equalpos+1) ))
            {
                WARN("Invalid key-value pair: " << keyvalue);
            }
        }
        else
        {
            WARN("Invalid key-value pair: " << keyvalue);
        }
    }
    while(splitpos != std::string::npos);

    g_results.push_back(result);

    return true;
}

/// read a stream of RESULT lines
void process_stream(std::istream& in)
{
    std::string line;

    while (std::getline(in,line) )
    {
        if (!process_line(line)) {
            WARN("Non-RESULT line: " << line);
        }
    }
}

/// open a file as a stream
void process_file(const char* path)
{
    std::ifstream in(path);
    if (!in.good()) {
        ERR("Error opening file " << path << ": " << strerror(errno));
        return;
    }
    return process_stream(in);
}

/// check for multiple hosts
bool check_multiple_hosts()
{
    std::set<std::string> hostnames;
    g_hostname = g_results[0].host;

    for (size_t i = 0; i < g_results.size(); ++i)
    {
        hostnames.insert( g_results[i].host );
    }

    if (hostnames.size() > 1)
    {
        ERRX("Multiple different hostnames found in results:");
        for(std::set<std::string>::const_iterator hi = hostnames.begin();
            hi != hostnames.end(); ++hi)
        {
            ERRX(" " << *hi);
        }
        ERR("");
        return false;
    }

    return true;
}

// ****************************************************************************
// *** Output various plots generated from results

#define P(x)    do { os << x << std::endl; } while(0)

/// join a vector of saved plot lines and output data stream afterwards
void join_plotlines(std::ostream& os, const std::vector<std::string>& plotlines, const std::ostringstream& datass)
{
    if (plotlines.size() == 0) return;

    P("plot \\");
    for (size_t i = 0; i < plotlines.size(); ++i) {
        os << plotlines[i];
        if (i != plotlines.size()-1) os << ", \\" << std::endl;
        else os << std::endl;
    }
    os << datass.str() << std::endl;
}

/// use STL stringstream to stringify a value
template <typename T>
static inline std::string toStr(const T& v)
{
    std::ostringstream s; s << v; return s.str();
}

/// filter functional type used in plot procedures
typedef bool (*filter_type)(const Result& r);

/// data formatting functional used in plot procedures
typedef void (*data_print_func)(std::ostream& datass, const Result& r);

/// Plot procedure: iterate over results, filter them and output a plot
/// containing funcname plotlines
void plot_funcname_iteration(std::ostream& os, filter_type filter, data_print_func print_func)
{
    std::ostringstream datass;
    std::vector<std::string> plotlines;
    std::string cfuncname; // current funcname in iteration
    size_t ctestsize = 0;

    // iterate over all results in order and collect plotlines
    for (size_t i = 0; i < g_results.size(); ++i)
    {
        const Result& r = g_results[i];
        if (!filter(r)) continue;

        if (cfuncname == r.funcname && ctestsize == r.testsize)
        {
            WARN("Multiple results found for " << cfuncname
                 << " testsize " << ctestsize << ", ignoring second.");
            continue;
        }

        if (cfuncname != r.funcname) // start new plot line
        {
            if (datass.str().size()) datass << "e" << std::endl;

            plotlines.push_back("'-' using 1:2 title '" + r.funcname + "' with linespoints");
            cfuncname = r.funcname;
        }

        print_func(datass, r);

        ctestsize = r.testsize;
    }
    if (datass.str().size()) datass << "e\n";

    join_plotlines(os, plotlines, datass);
}

/// plot the bandwidth in GiB/s for each Result
void plot_data_bandwidth(std::ostream& datass, const Result& r)
{
    datass << std::setprecision(20)
           << log(r.testsize) / log(2) << "\t"
           << r.bandwidth / 1024/1024/1024 << "\n";
}

/// plot the latency (access time) in nanoseconds for each Result
void plot_data_latency(std::ostream& datass, const Result& r)
{
    datass << std::setprecision(20)
           << log(r.testsize) / log(2) << "\t"
           << r.rate * 1e9 << "\n";
}

/// show only sequential results with procs = 1
bool filter_sequential(const Result& r) {
    return (r.nprocs == 1);
}

bool filter_sequential_nonpermutation(const Result& r) {
    return (r.nprocs == 1) && (r.funcname.find("Perm") == std::string::npos);
}

bool filter_sequential_64bit_reads(const Result& r) {
    return (r.nprocs == 1) && (r.funcname.find("Read64") == std::string::npos);
}

/// Plots showing just sequential memory bandwidth and latency
void plot_sequential(std::ostream& os)
{
    P("set key top right");
    P("set title '" << g_hostname << " - One Processor Memory Bandwidth'");
    P("set ylabel 'Bandwidth [GiB/s]'");
    P("set yrange [0:*]");
    plot_funcname_iteration(os, filter_sequential, plot_data_bandwidth);

    P("set key top left");
    P("set title '" << g_hostname << " - One Processor Memory Latency'");
    P("set ylabel 'Access Time [ns]'");
    plot_funcname_iteration(os, filter_sequential, plot_data_latency);

    P("set key top left");
    P("set title '" << g_hostname << " - One Processor Memory Latency (excluding Permutation)'");
    P("set ylabel 'Access Time [ns]'");
    plot_funcname_iteration(os, filter_sequential_nonpermutation, plot_data_latency);

    P("set key top right");
    P("set title '" << g_hostname << " - One Processor Memory Bandwidth (only 64-bit Reads)'");
    P("set ylabel 'Bandwidth [GiB/s]'");
    plot_funcname_iteration(os, filter_sequential_64bit_reads, plot_data_bandwidth);
}

/// Plot procedure: iterate over results, filter them to show only one funcname
/// and output a plot containing plotlines for each nprocs
void plot_parallel_iteration(std::ostream& os, const std::string& funcname, data_print_func print_func)
{
    std::ostringstream datass;
    std::vector<std::string> plotlines;
    size_t cnprocs = 0;    // current nprocs
    size_t ctestsize = 0;  // current testsize

    // iterate over all results in order, separate funcnames and collect plotlines for each funcname
    for (size_t i = 0; i < g_results.size(); ++i)
    {
        const Result& r = g_results[i];
        if (r.funcname != funcname) continue;

        if (cnprocs == r.nprocs && ctestsize == r.testsize)
        {
            WARN("Multiple results found for " << funcname << " nprocs " << cnprocs
                 << " testsize " << ctestsize << ", ignoring second.");
            continue;
        }

        if (cnprocs != r.nprocs) // start new plot line
        {
            if (datass.str().size()) datass << "e\n";

            plotlines.push_back("'-' using 1:2 title 'nprocs=" + toStr(r.nprocs) + "' with linespoints");
            cnprocs = r.nprocs;
        }

        print_func(datass, r);

        ctestsize = r.testsize;
    }
    if (datass.str().size()) datass << "e\n";

    join_plotlines(os, plotlines, datass);
}

/// Plot procedure: iterate over results, filter them to show only one funcname
/// and output a plot containing plotlines for each nprocs. Calculate the
/// speedup of memory bandwidth over the nprocs=1 entry.
void plot_parallel_speedup_bandwidth(std::ostream& os, const std::string& funcname, double& avgspeedup)
{
    std::ostringstream datass;
    std::vector<std::string> plotlines;
    size_t cnprocs = 0;    // current nprocs
    size_t ctestsize = 0;  // current testsize

    avgspeedup = 0;
    size_t cntspeedup = 0;
    std::map<size_t,double> seqbandwidth;       // map areasize -> sequential bandwidth (nprocs=1)
    // areasize is used instead of testsize, because testsize may depend on rounding due to nprocs.

    // iterate over all results in order, separate funcnames and collect plotlines for each funcname
    for (size_t i = 0; i < g_results.size(); ++i)
    {
        const Result& r = g_results[i];
        if (r.funcname != funcname) continue;

        if (cnprocs == r.nprocs && ctestsize == r.testsize)
        {
            WARN("Multiple results found for " << funcname << " nprocs " << cnprocs
                 << " testsize " << ctestsize << ", ignoring second.");
            continue;
        }

        if (cnprocs != r.nprocs) // start new plot line
        {
            if (datass.str().size()) datass << "e\n";

            plotlines.push_back("'-' using 1:2 title 'nprocs=" + toStr(r.nprocs) + "' with linespoints");
            cnprocs = r.nprocs;
        }

        if (r.nprocs == 1) {
            seqbandwidth[r.areasize] = r.bandwidth;
        }

        if (seqbandwidth[r.areasize] == 0)
        {
            WARN("Missing sequential bandwidth in speedup plot for " << funcname << " nprocs " << cnprocs
                 << " testsize " << ctestsize << ", skipping.");
        }
        else
        {
            datass << std::setprecision(20)
                   << log(r.testsize) / log(2) << "\t"
                   << r.bandwidth / seqbandwidth[r.areasize] << "\n";

            avgspeedup += r.bandwidth / seqbandwidth[r.areasize];
            cntspeedup++;
        }

        ctestsize = r.testsize;
    }
    if (datass.str().size()) datass << "e\n";

    join_plotlines(os, plotlines, datass);

    // divide by number of speedup values found
    if (cntspeedup == 0)
        avgspeedup = 0;
    else
        avgspeedup /= cntspeedup;
}

void plot_parallel_funcname(std::ostream& os, const std::string& funcname)
{
    P("set key top right");
    P("set title '" << g_hostname << " - Memory Bandwidth - " << funcname);
    P("set ylabel 'Bandwidth [GiB/s]'");
    plot_parallel_iteration(os, funcname, plot_data_bandwidth);

    P("set key top left");
    P("set title '" << g_hostname << " - Memory Latency - " << funcname);
    P("set ylabel 'Access Time [ns]'");
    plot_parallel_iteration(os, funcname, plot_data_latency);

    P("set key top right");
    P("set title '" << g_hostname << " - Speedup of Memory Bandwidth - " << funcname);
    P("set ylabel 'Bandwidth Speedup [1]'");
    double avgspeedup;
    plot_parallel_speedup_bandwidth(os, funcname, avgspeedup);

    // replot last plot with other yrange scale
    P("set title '" << g_hostname << " - Speedup of Memory Bandwidth (enlarged) - " << funcname);
    P("set yrange [*:" << avgspeedup << "]");
    plot_parallel_speedup_bandwidth(os, funcname, avgspeedup);

    P("set yrange [*:*]");
    P("");
    P("##############################");
}

void plot_parallel(std::ostream& os)
{
    for (size_t i = 0; funclist[i]; ++i)
    {
        plot_parallel_funcname(os,funclist[i]);
    }
}

void output_gnuplot(std::ostream& os)
{
    P("set terminal pdf size 28cm,18cm linewidth 2.0");
    P("set output 'plots-" << g_hostname << ".pdf'");
    P("");
    P("set pointsize 0.7");
    P("set style line 6 lc rgb '#f0b000'");
    P("set style line 15 lc rgb '#f0b000'");
    P("set style line 24 lc rgb '#f0b000'");
    P("set style line 33 lc rgb '#f0b000'");
    P("set style line 42 lc rgb '#f0b000'");
    P("set style line 51 lc rgb '#f0b000'");
    P("set style line 60 lc rgb '#f0b000'");
    P("set style increment user");
    P("");
    P("set grid xtics ytics");
    P("set xtics 1");
    P("set xlabel 'Input Size log_2 [B]'");

    plot_sequential(os);
    plot_parallel(os);
}

/// main: read stdin or from all files on the command line
int main(int argc, char* argv[])
{
    std::string opt_hostname_override;

    if (argc == 1) {
        process_stream(std::cin);
    }
    else
    {
        // *** parse command line options
        int opt;

        while ( (opt = getopt(argc, argv, "vh:")) != -1 )
        {
            switch (opt) {
            case 'h':
                opt_hostname_override = optarg;
                ERR("Setting hostname override to '" << opt_hostname_override << "'");
                break;

            case 'v':
                gopt_warnings = true;
                ERR("Outputting verbose warnings when processing plots.");
                break;

            default: /* '?' */
                ERR("Usage: " << argv[0] << " [-v] [-h hostname] [files...]");
                exit(EXIT_FAILURE);
            }
        }

        while (optind < argc) { // process files
            process_file(argv[optind++]);
        }
    }

    if (g_results.size() == 0) {
        ERR("No RESULT lines found in input.");
        return 0;
    }
    else {
        ERR("Parsed " << g_results.size() << " RESULT lines in input.");
    }

    if (!check_multiple_hosts())
    {
        if (!opt_hostname_override.size()) {
            ERR("Use -h <hostname> to override the hostnames if this is intentional.");
            return 0;
        }
    }

    if (opt_hostname_override.size())
        g_hostname = opt_hostname_override;

    std::sort(g_results.begin(), g_results.end());

    output_gnuplot(std::cout);

    return 0;
}
