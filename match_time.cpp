#include <cstring>
#include <unistd.h>
#include <fstream>
#include <vector>
#include <algorithm>
#include <map>
#include <thread>
#include "./my_getopt.h"

#define OTHER_SERVER_COUNT 2
#define SUBNET_ADDRESS "10.2.1."
#define WAIT_TIME 1

class matchTime{ 
    public:
        matchTime() : matchTime("0.0.0.0") {}
        matchTime(string my_address) : my_address(my_address) {
            for(int i = 1 ; i <= OTHER_SERVER_COUNT ; ++i) {
                server_address[i - 1] = SUBNET_ADDRESS + std::to_string(i);
                output_filepath[i - 1] = std::to_string(i) + ".txt";
                address_to_int[server_address[i - 1]] = i - 1;
            }
	    std::fill(min_time, min_time + OTHER_SERVER_COUNT, 1e9);
        }

        static void wrapper_ping_to_other_server(string need_command){
            system(need_command.c_str());
        }
        // thread로 만들어서 run 시켜야 된다.
        // system에서 block되서 움직이지 않으므로.
        void ping_to_other_server() {
            for(int i = 0 ; i < OTHER_SERVER_COUNT ; i++) {
                if(my_address == server_address[i]) continue;
                string need_command = command + " " + server_address[i] + " >> " + output_filepath[i];
                string kill_command = "sudo pkill -9 ping";

		std::thread th(wrapper_ping_to_other_server, need_command);
                //10초 잠든 뒤 wake up해서 ping thread를 kill한다.
		th.detach();
                sleep(WAIT_TIME);
                system(kill_command.c_str());
            }
        }

        double average_time(vector<double> times) {
            double sum = 0.0;
            for(auto time : times) sum += time;
            return sum / (double)times.size();
        }

        void calculate() {
            for(int i = 0 ; i < OTHER_SERVER_COUNT ; i++) {
		if(my_address == server_address[i]) continue;
                vector<double> times;
                vector<string> lines;
		cout << output_filepath[i] << endl;
		std::ifstream input_file;
		input_file.open(output_filepath[i]);
                if(!input_file.is_open()) {
                    cout << "Cannot open file : " << output_filepath[i] << endl;
                    continue;
                }
		cout << "e" << endl;
                string line;
                while(getline(input_file, line)) lines.push_back(line);
                for(auto line : lines) {
                    string buffer, time_line = "";
                    ss.str(line);
                    while(getline(ss, buffer, ' ')) if(buffer.find("time=") != string::npos) {
                        time_line = buffer; 
                        break;
                    } 
                    ss.clear();
                    ss.str("");
                    if(time_line == "") continue;
                    string ping_time = time_line.substr(5, 5);
                    times.push_back(std::stod(time_line.c_str()));
                }
                sort(times.begin(), times.end());
                min_time[i] = min<double>(min_time[i], times[0]);
                avg_time[i] = average_time(times);
                input_file.close();
            }
        }

	template <typename T>
	T min(T a, T b){
	    return (a < b ? a : b);
	}
    
    private:
        string server_address[OTHER_SERVER_COUNT];
        string output_filepath[OTHER_SERVER_COUNT];
        string command = R"(ping)";
        string my_address = "";
        double min_time[OTHER_SERVER_COUNT], avg_time[OTHER_SERVER_COUNT];
        istringstream ss;
        std::map<string, int> address_to_int;
};

int main(int argc, char* argv[]){
    Options argOptions(argc, argv);
    matchTime matchtime(argOptions.my_address);
    matchtime.ping_to_other_server();
    matchtime.calculate();
}
