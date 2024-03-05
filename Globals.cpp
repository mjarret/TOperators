#include "Globals.hpp"
#include <thread> 
#include "utils.hpp"
#include <boost/program_options.hpp>

namespace po = boost::program_options;

// Threading and performance tracking
uint8_t THREADS; //store maximum number of threads here
omp_lock_t lock;
std::chrono::high_resolution_clock::time_point tcount_init_time = std::chrono::high_resolution_clock::now(); // Initialize with current time
std::chrono::duration<double> timeelapsed = std::chrono::duration<double>::zero(); // Initialize as zero

// Pattern handling and search settings
std::set<pattern> pattern_set;          
std::vector<pattern> cases;
std::set<SO6> explicit_search_set;
std::string pattern_file = "";
std::string case_file = "";

// Configuration and state variables
uint8_t target_T_count = 8;            
uint8_t stored_depth_max = 255;
bool cases_flag = false;

void Globals::setParameters(int argc, char *argv[]) {
    try {
        // Define options
        po::options_description desc("Allowed options");
        int tcount_param;
        int stored_depth_param;
        int threads_param;

        desc.add_options()
            ("help,h", "produce help message")
            ("tcount,t", po::value<int>(&tcount_param)->default_value(8), "target T count")
            ("stored_depth,d", po::value<int>(&stored_depth_param)->default_value(0), "maximum stored depth")
            ("pattern_file,f", po::value<std::string>(&pattern_file), "pattern file")
            ("verbose,v", po::bool_switch(), "enable verbosity")
            ("threads,n", po::value<std::string>()->default_value(std::to_string(std::thread::hardware_concurrency()-1)), "number of threads")
            // ("save", po::bool_switch(&saveResults), "save results")
            ("recover", "recover (no functionality)")
            // ("transpose_multiply", po::bool_switch(&transpose_multiply), "transpose multiply")
            // ("explicit_search", po::value<int>(&explicit_search_depth)->implicit_value(1), "explicit search depth")
            ("cases", po::value<std::string>(&case_file), "cases (original code missing functionality)");
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        target_T_count = (uint8_t) (std::max(1,tcount_param));
        stored_depth_max = (uint8_t) stored_depth_param;
   
        if (vm.count("help")) {
            std::cout << desc << "\n";
            std::exit(EXIT_SUCCESS);
        }

        if (vm.count("threads")) {
            if(vm["threads"].as<std::string>() == "max") {
                THREADS = std::thread::hardware_concurrency();
            } else {
                std::cout << vm["threads"].as<std::string>();
                THREADS = (uint8_t) std::stoi(vm["threads"].as<std::string>());
            }
        }


    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        std::exit(EXIT_FAILURE);
    } 
}

// Configure run based on global parameters
void Globals::configure()
{
    if (stored_depth_max == 0 || stored_depth_max > target_T_count-1) stored_depth_max = target_T_count-1;
    if (stored_depth_max < std::ceil((float)target_T_count/2)) stored_depth_max = (uint8_t) std::ceil((float)target_T_count/2); 
    
    if (THREADS > std::thread::hardware_concurrency()) {
        THREADS = std::thread::hardware_concurrency();
    } else if(THREADS <= 0) {
        THREADS = 1;
    }

    // Output configuration
    std::cout << "[Config] Generating up to T=" << (int) target_T_count << ".\n";
    std::cout << "[Config] Storing at most T=" << (int) stored_depth_max << " in memory.\n";
    std::cout << "[Config] Running on " << (int) THREADS << " threads.\n";
    if (!pattern_file.empty()) {
        std::cout << "[Config] Pattern file " << pattern_file << "\n";
    } else {
        std::cout << "[Warning] No pattern file.\n";
    }

    if (!case_file.empty()) {
        std::cout << "[Config] Cases file " << case_file << "\n";
        cases_flag=true;
    } 
}


