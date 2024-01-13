#pragma once

namespace util {

class ProgressPercentage {
public:
    void start(int total);
    void increment_progress(int inc);
    void set_progress_percentage(int percentage);

    float get_progress() const { return progress; }

private:
    float progress{ 0.0f };

    int cur{ 0 };
    int total_n{ 0 };
};

}
