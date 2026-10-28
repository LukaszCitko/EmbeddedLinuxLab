#include <iostream>
#include <sstream>
#include <fstream>
#include <istream>
#include <stdexcept>
#include <string>

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


int main(int argc, char* argv[]) {

    std::string path = "sample-uptime.txt";
    std::string meminfoPath = "sample-meminfo.txt";
    std::string tempPath = "sample-temp.txt";
    std::string wifiPath = "sample-wifi-operstate.txt";

    if (argc > 1){
        path = argv[1];
    }

    if (argc > 2) {
        meminfoPath = argv[2];
    }

    if (argc > 3) {
        tempPath = argv[3];
    }

    if (argc > 4) {
        wifiPath = argv[4];
    }

    std::ifstream file(path);
    
    if (!file){
        std::cerr << "Cannot open file: " << path << "\n";
        return 1;
    }
    
    std::string line;

    if (!std::getline(file, line)) {
        std::cerr << "Cannot read uptime file\n";
        return 1;
    }

    std::ifstream meminfoFile(meminfoPath);

    if (!meminfoFile) {
        std::cerr << "Cannot open file: " << meminfoPath << '\n';
        return 1;
    }

    std::ifstream tempFile(tempPath);

    if (!tempFile) {
        std::cerr << "Cannot open file: " << tempPath << '\n';
        return 1;
    }

    std::string tempLine;

    if (!std::getline(tempFile, tempLine)) {
        std::cerr << "Cannot read CPU temperature file\n";
        return 1;
    }
   
    std::ifstream wifiFile(wifiPath);

    if (!wifiFile) {
        std::cerr << "Cannot open file: " << wifiPath << '\n';
        return 1;
    }

    std::string wifiState;

    if (!std::getline(wifiFile, wifiState)) {
        std::cerr << "Cannot read Wi-Fi state file\n";
        return 1;
    }

    try {
        double uptimeSeconds = parseUptimeSeconds(line);
        long long memAvailableKb = readMemAvailableKb(meminfoFile);
        double cpuTemperatureC = parseCpuTemperatureC(tempLine);
        bool wifiOperstateUp = isWifiOperstateUp(wifiState);
        std::cout << "{\"uptime_seconds\":"    << uptimeSeconds 
                  << ",\"mem_available_kb\":"  << memAvailableKb 
                  << ",\"cpu_temperature_c\":" << cpuTemperatureC
                  << ",\"wifi_operstate_up\":" << (wifiOperstateUp ? "true" : "false") << "}\n";
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }

    return 0;
}

