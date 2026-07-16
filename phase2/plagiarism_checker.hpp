#include "structures.hpp"  
#include <chrono>  // for timestamps
#include <thread>  
#include <mutex>  // for managing access to shared resources
#include <queue>  // to hold submissions
#include <vector>  // to store submissions and tokens
#include <memory>  // for using shared pointers
#include <condition_variable>  // for thread synchronization


// struct to store a submission's details including its time and tokenized code
struct tokenized_submission{
    std::shared_ptr<submission_t> submission_data;  // pointer to the submission object which contains student, professor, and code details
    std::chrono::system_clock::time_point submission_time;  // time when the submission was added
    std::vector<int> tokens;  // tokenized version of the code 
};


// class for detecting plagiarism
// compares code submissions asynchronously
class plagiarism_checker_t{
public:
    // constructor to initialize the plagiarism checker with no submissions
    plagiarism_checker_t(void);

    // constructor to initialize the checker with a list of original submissions
    plagiarism_checker_t(std::vector<std::shared_ptr<submission_t>> __submissions);

    // destructor to stop the background thread and clean up resources
    ~plagiarism_checker_t(void);

    // adds a new submission to the queue for plagiarism checking
    void add_submission(std::shared_ptr<submission_t> __submission);

protected:
    bool is_running;  // flag to check if the checker is active
    std::thread processing_thread;  // background thread for processing submissions
    std::mutex submission_mutex;  // Mmutex to make accessing the queue thread-safe
    std::condition_variable cv;  // it lets the worker thread know when there's new work
    std::queue<tokenized_submission> submission_work_queue;  // queue of submissions waiting to be checked
    std::vector<tokenized_submission> checked_submissions;  // submissions that have already been processed
    std::vector<tokenized_submission> original_submissions;  // original submissions used for comparison

//about functions
    void process_pending_submissions(void);  // processes submissions from the queue in the background
    void evaluate_plagiarism(const tokenized_submission& current_submission);  // checks if a submission is plagiarized
    std::vector<std::pair<size_t, size_t>> find_matching_patterns(
        const std::vector<int>& current_tokens, 
        const std::vector<int>& previous_tokens);  // finds patterns that match between two submissions

    // flags a submission as plagiarized and notifies the student and professor
    void mark_plagiarized_submission(std::shared_ptr<submission_t> flagged_submission);
};
