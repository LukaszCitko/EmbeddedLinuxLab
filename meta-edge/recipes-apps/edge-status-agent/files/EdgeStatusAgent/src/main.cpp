#include <iostream>
#include <sstream>
#include <fstream>
#include <istream>
#include <stdexcept>
#include <string>
#include <chrono>
#include <thread>
#include <csignal>


struct SystemSnapshot {
    double uptimeSeconds;
    long long memAvailableKb;
    double cpuTemperatureC;
    bool wifiOperstateUp;
};


bool isWifiOperstateUp(const std::string& state) {
    return state == "up";
}

double parseUptimeSeconds(const std::string& line){
    std::istringstream stream(line);
    double uptimeSeconds = 0.0;
    
    if (!(stream>>uptimeSeconds) || uptimeSeconds<0.0){
        throw std::runtime_error("Invalid uptime data");
    }
   return uptimeSeconds;
}

long long parseMemAvailableKb(const std::string& line) {
    std::istringstream stream(line);
    std::string label;
    long long value = 0;
    std::string unit;

    if (!(stream >> label >> value >> unit) || label != "MemAvailable:" || value < 0 || unit != "kB") {
        throw std::runtime_error("Invalid MemAvailable data");
    }

    return value;
}

long long readMemAvailableKb(std::istream& input) {
    std::string line;

    while (std::getline(input, line)) {
        if (line.find("MemAvailable:") == 0) {
            return parseMemAvailableKb(line);
        }
    }

    throw std::runtime_error("MemAvailable not found");
}

double parseCpuTemperatureC(const std::string& line) {
    std::istringstream stream(line);
    long long milliCelsius = 0;

    if (!(stream >> milliCelsius)) {
        throw std::runtime_error("Invalid CPU temperature data");
    }

    return static_cast<double>(milliCelsius) / 1000.0;
}

void printSnapshotJson(const SystemSnapshot& snapshot) {
    
    std::cout << "{\"uptime_seconds\":" << snapshot.uptimeSeconds 
                << ",\"mem_available_kb\":" << snapshot.memAvailableKb 
                << ",\"cpu_temperature_c\":" << snapshot.cpuTemperatureC 
                << ",\"wifi_operstate_up\":" << (snapshot.wifiOperstateUp ? "true" : "false") << "}\n";
}

SystemSnapshot collectSnapshot(const std::string& uptimePath, const std::string& meminfoPath, const std::string& tempPath, const std::string& wifiPath) {
    std::ifstream uptimeFile(uptimePath);
    if (!uptimeFile) {
        throw std::runtime_error("Cannot open file: " + uptimePath);
    }

    std::string uptimeLine;
    if (!std::getline(uptimeFile, uptimeLine)) {
        throw std::runtime_error("Cannot read uptime file");
    }

    std::ifstream meminfoFile(meminfoPath);
    if (!meminfoFile) {
        throw std::runtime_error("Cannot open file: " + meminfoPath);
    }

    std::ifstream tempFile(tempPath);
    if (!tempFile) {
        throw std::runtime_error("Cannot open file: " + tempPath);
    }

    std::string tempLine;
    if (!std::getline(tempFile, tempLine)) {
        throw std::runtime_error("Cannot read CPU temperature file");
    }

    std::ifstream wifiFile(wifiPath);
    if (!wifiFile) {
        throw std::runtime_error("Cannot open file: " + wifiPath);
    }

    std::string wifiState;
    if (!std::getline(wifiFile, wifiState)) {
        throw std::runtime_error("Cannot read Wi-Fi state file");
    }

    return SystemSnapshot{parseUptimeSeconds(uptimeLine), readMemAvailableKb(meminfoFile), parseCpuTemperatureC(tempLine), isWifiOperstateUp(wifiState)};
}

volatile std::sig_atomic_t stopRequested = 0;
void handleStopSignal(int) {
    stopRequested = 1;
}

int main(int argc, char* argv[]) {

    int firstPathIndex = 1;
    int intervalSeconds = 0;
    std::signal(SIGINT, handleStopSignal);
    std::signal(SIGTERM, handleStopSignal);

    if (argc > 1 && std::string(argv[1]) == "--once") {
        firstPathIndex = 2;
    } else if (argc > 1 && std::string(argv[1]) == "--interval") {
        if (argc < 3) {
            throw std::runtime_error("Missing interval value");
        }

        std::size_t parsedChars = 0;
        intervalSeconds = std::stoi(argv[2], &parsedChars);
        if (parsedChars != std::string(argv[2]).size() || intervalSeconds <= 0) {
            throw std::runtime_error("Interval must be a positive integer");
        }

        firstPathIndex = 3;
    }
    

    std::string uptimePath = "sample-uptime.txt";
    std::string meminfoPath = "sample-meminfo.txt";
    std::string tempPath = "sample-temp.txt";
    std::string wifiPath = "sample-wifi-operstate.txt";

    if (argc > firstPathIndex) uptimePath = argv[firstPathIndex];
    if (argc > firstPathIndex + 1) meminfoPath = argv[firstPathIndex + 1];
    if (argc > firstPathIndex + 2) tempPath = argv[firstPathIndex + 2];
    if (argc > firstPathIndex + 3) wifiPath = argv[firstPathIndex + 3];
   
    try {
        if (intervalSeconds == 0) {
            SystemSnapshot snapshot = collectSnapshot(uptimePath, meminfoPath, tempPath, wifiPath);
            printSnapshotJson(snapshot);
        } else {
            while (!stopRequested) {
                SystemSnapshot snapshot = collectSnapshot(uptimePath, meminfoPath, tempPath, wifiPath);
                printSnapshotJson(snapshot);
                std::this_thread::sleep_for(std::chrono::seconds(intervalSeconds));
            }
        }
    }   
    catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }

    return 0;
}

