// INCLUDES
#include <iostream>
#include <ctime>
#include <string>
#include "include/Operator.h"
#include "include/tsu.h"

// the constructor reads the given schedule and stores it in memory
Operator::Operator (const std::string& filename, 
                    DistributedEnergyResource* der_ptr) 
    : index_(0), der_ptr_(der_ptr) {
    tsu::string_matrix matrix = tsu::FileToMatrix(filename, ',', 3);

    // reserve memory for each row within the matrix
    schedule_.reserve(matrix.size());

    // loop through each row of the matrix and set each column to it's
    // header data type
    for (const auto &col : matrix) {
        schedule_.emplace_back (col[0], col[1], stoul(col[2]));
    }
}  // end constructor

Operator::~Operator () {
    // do nothing
}  // end destructor

// Loop
// - gets the utc time and takes the modulus so the actual day of the scheduel
// - is neglected.
void Operator::Loop () {
    time_t now = time(nullptr);
    std::string real_time = Operator::GetTime (now);

    // loop through each row of schedule looking for current time
    for (unsigned int i = index_; i < schedule_.size(); i++) {
        ScheduleHeader& row = schedule_[i];
        if (row.time == real_time && index_ != i) {
std::cout << "[OPERATOR] : " << "time = " << real_time << ", schedule = " << row.time << std::endl;

            // if the time is found check control type
            if (row.control == "import") {
                der_ptr_->SetImportWatts(row.setting);
            } else if (row.control == "export") {
                der_ptr_->SetExportWatts(row.setting);
            } else {
                der_ptr_->SetImportWatts(0);
            }

            // log index so multiple controls are not sent and to reduce
            // search time.

    	    if (i == schedule_.size() - 1) {
    		  index_ = 0;
    	    } else {
    		  index_ = i;
    	    }
        }
    }
}  // end Loop

// Get Time
// - return HH:MM:SS formatted time
std::string Operator::GetTime (time_t utc) {
	struct tm ts = *localtime(&utc);
	char buf[100];
	strftime (buf, sizeof(buf), "%T", &ts);
	return std::string(buf);
}  // end Get Time
