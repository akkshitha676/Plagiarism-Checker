#include "plagiarism_checker.hpp" 


// default constructor to initialize the plagiarism checker
plagiarism_checker_t::plagiarism_checker_t(void) : is_running(true) {
//sets a background thread that continuously processes submissions added to the queue  
 processing_thread=std::thread(&plagiarism_checker_t::process_pending_submissions,this);
}


// constructor to set up the plagiarism checker with a list of original submissions
// This constructor takes a list of submissions,tokenizes and stores them for future comparisons
plagiarism_checker_t::plagiarism_checker_t(std::vector<std::shared_ptr<submission_t>> __submissions)
    : is_running(true){
    // starts a background thread that will process submissions
    processing_thread=std::thread(&plagiarism_checker_t::process_pending_submissions,this);

    // tokenize and store the provided initial submissions for comparison
    for(std::shared_ptr<submission_t>submission : __submissions){
        // create a tokenizer for the submission's code
        tokenizer_t tokenizer(submission->codefile);

        // create a new object to store the submission's data
        tokenized_submission submission_data;
        submission_data.submission_data=submission;  // store the original submission
        submission_data.submission_time=std::chrono::system_clock::time_point(std::chrono::seconds(0));  // set timestamp to 0 initially
        submission_data.tokens=tokenizer.get_tokens();  // get the tokenized form of the code

        // add the tokenized submission to the list of original submissions for comparison later
        original_submissions.push_back(submission_data);
    }
}


// destructor to clean up and stop the background worker thread when the program ends
plagiarism_checker_t::~plagiarism_checker_t(void){
    {
        std::unique_lock<std::mutex>lock(submission_mutex);  // to lock the resources so they can be safely modified
        is_running=false;  // set the flag to false telling the checker to stop running
        cv.notify_one();  // notify the worker thread to stop waiting for new submissions
    }

    // this is to wait for the worker thread to finish processing before the program ends
    if(processing_thread.joinable()){
        processing_thread.join();  // wait for the worker thread to finish 
    }
}


// method to add a new submission for plagiarism checking
// this breaks down the submission into tokens, adds the timestamp and puts it in the queue for processing
void plagiarism_checker_t::add_submission(std::shared_ptr<submission_t> __submission) {
    // break the submission's code into tokens
    tokenizer_t tokenizer(__submission->codefile);
    std::vector<int> tokens=tokenizer.get_tokens();  // get the tokenized version

    // create a structure to hold information about the submission
    tokenized_submission submission_data;
    submission_data.submission_data=__submission;  // store the submission object
    submission_data.submission_time=std::chrono::system_clock::now();  // store current time
    submission_data.tokens=tokens;  // store the tokenized code

    // lock the queue to add the new submission
    {
        std::unique_lock<std::mutex>lock(submission_mutex);  // this is to lock the queue to prevent other threads from modifying it
        submission_work_queue.push(submission_data);  // add the new submission to the queue
    }

    cv.notify_one();  // this is to tell the worker thread that there is a new submission to process
}




// method to process pending submissions in the background
// this method runs in a separate thread and processes each submission in the queue
void plagiarism_checker_t::process_pending_submissions(void){
    while(is_running){
        tokenized_submission current_submission;

        // lock the queue so we can safely access it
        std::unique_lock<std::mutex>lock(submission_mutex);

        // wait for new submissions or for the checker to stop
        while(submission_work_queue.empty() && is_running){
            cv.wait(lock);  // wait until notified that there is a new submission
        }

        // if there are no submissions left and the checker is stopping, exit the loop
        if(!is_running && submission_work_queue.empty()){
            break;
        }

        // get the next submission from the queue
        current_submission=submission_work_queue.front();
        submission_work_queue.pop();

        // unlock the queue before we start processing the submission
        lock.unlock();

        // check if the current submission is plagiarized
        evaluate_plagiarism(current_submission);
    }
}


// method to check if the current submission is plagiarized
// it compares the current submission with both the original submissions and the recently checked ones
void plagiarism_checker_t::evaluate_plagiarism(const tokenized_submission& current_submission) {
    auto current_time=current_submission.submission_time;  // to get the timestamp of when the current submission was made
    std::vector<tokenized_submission*>submissions_to_check;  // list to hold the submissions we need to compare against

    // add original submissions to the list for comparison
    for (size_t i = 0;i < original_submissions.size();++i) {
        if (original_submissions[i].submission_data!=current_submission.submission_data){
            submissions_to_check.push_back(&original_submissions[i]);  // add if it's not the current submission
        }
    }

    // add recently checked submissions to the list for comparison
    submission_mutex.lock();  // lock to safely access the list of checked submissions
    for (size_t i = 0; i<checked_submissions.size(); ++i) {
        if (checked_submissions[i].submission_data!=current_submission.submission_data){
            auto time_diff = std::chrono::duration_cast<std::chrono::seconds>(
                current_time-checked_submissions[i].submission_time).count();
            if (time_diff<=1) {
                submissions_to_check.push_back(&checked_submissions[i]);  // add if it was processed recently
            }
        }
    }
    submission_mutex.unlock();  // unlock after accessing checked submissions

    bool plagiarism_found=false;  // flag to keep track if we find plagiarism
    std::vector<std::pair<size_t, size_t>>pattern_matches;  // list to store matching patterns

    // compare the current submission with each of the submissions in the list
    for (auto* previous_submission : submissions_to_check) {
        auto matches=find_matching_patterns(current_submission.tokens,previous_submission->tokens);

        if (!matches.empty()) {  // if we found some matches
            // check if any pattern has enough length (75 or more tokens)
            for (auto match : matches) {
                if (match.second>=75) {
                    plagiarism_found=true;  // flag plagiarism if we find a long enough match
                }
            }

            // check if there are enough patterns (10 or more)
            if (matches.size()>=10) {
                plagiarism_found=true;  // flag plagiarism if there are enough matching patterns
            }

            // collect all matching patterns for later analysis
            pattern_matches.insert(pattern_matches.end(),matches.begin(),matches.end());

            // if the submissions are very close in time (within 1 second), mark them as plagiarized
            auto time_diff=std::chrono::duration_cast<std::chrono::seconds>(
                current_time-previous_submission->submission_time).count();

            if (time_diff<1){
                mark_plagiarized_submission(previous_submission->submission_data);  // mark the other submission
                mark_plagiarized_submission(current_submission.submission_data);  // mark the current submission
                return;  // skip further checks once both submissions are flagged
            }
        }
    }

    // check if we found a lot of small matching patterns (patchwork plagiarism)
    if (pattern_matches.size()>=20){
        plagiarism_found=true;  // flag plagiarism if there are enough small matching patterns
    }

    // if plagiarism is found, mark the current submission
    if (plagiarism_found){
        mark_plagiarized_submission(current_submission.submission_data);
    }

    // add the current submission to the list of processed submissions
    submission_mutex.lock();
    checked_submissions.push_back(current_submission);  // add the current submission to the checked list
    submission_mutex.unlock();  // unlock after adding the current submission
}



// method to find matching patterns between two submissions based on their tokenized code
// looks for sequences of tokens that are the same (at least 15 tokens long)
//we use KMP algorithm here in this 
std::vector<std::pair<size_t, size_t>> plagiarism_checker_t::find_matching_patterns(
    const std::vector<int>& current_tokens,
    const std::vector<int>& previous_tokens){

    std::vector<std::pair<size_t, size_t>> matches;
    const size_t min_match_length=15;  // minimum length of matching tokens to be considered a pattern

    // if previous_tokens is too short, no valid matches can be found
    if (previous_tokens.size()<min_match_length){
        return matches;  // return empty list as no matches are possible
    }

    // create the lps(longest prefix suffix) array for previous_tokens
    std::vector<int>lps(previous_tokens.size(),0);
    size_t len=0;
    for (size_t i=1;i<previous_tokens.size(); ){
        // if the current token matches the token at len position, extend the match
        if (previous_tokens[i]==previous_tokens[len]){
            len++;
            lps[i]=len;
            i++;
        } 
        // if there’s a mismatch and len is greater than 0, fall back to previous possible match length
        else if(len>0){
            len=lps[len-1];
        } 
        // if no match, continue to the next token
        else{
            lps[i]=0;
            i++;
        }
    }

    // perform the KMP algorithm to find matching patterns
    size_t i=0,j=0;
    while (i<current_tokens.size()){
        // if tokens match, continue checking for more matches
        if (current_tokens[i]==previous_tokens[j]){
            i++;
            j++;

            // if the match is long enough, add it to the list of matches
            if (j>=min_match_length){
                matches.emplace_back(i - j,j);
            }

            // if we have matched all of previous_tokens, restart the match at the right position
            if (j==previous_tokens.size()){
                j=lps[j - 1];
            }
        } 
// if a mismatch occurs, fall back to the position indicated by the LPS array
        else if(j>0){
            j=lps[j-1];
        } 
        // if no match and no fallback, move to the next token in current_tokens
        else{
            i++;
        }
    }

    return matches;  // return all the matching patterns found
}


// method to flag a submission as plagiarized 
// this method notifies the student and professor about the flagged submission

void plagiarism_checker_t::mark_plagiarized_submission(std::shared_ptr<submission_t> flagged_submission) {

    if (flagged_submission->student!=nullptr){
        flagged_submission->student->flag_student(flagged_submission);  
    }
    if (flagged_submission->professor!=nullptr) {
        flagged_submission->professor->flag_professor(flagged_submission);  
    }
}
