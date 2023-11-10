#pragma once

#include <string>

namespace renderer {

class Window;

class ScreenCapturer {
public:
    ScreenCapturer(const Window& window): window(window) {}
    void update(); // tick

	enum OutputFormat {
        eTGA,
		ePNG, 
		eMP4, // not implement
	};

    bool is_capturing() const { return sequence_capturing || single_image_capturing; }
    bool is_capturing_sequence() const { return sequence_capturing; }
    int cur_capture_frame_id() const { return capture_idx; }
    bool is_output_images() const { return output_format == OutputFormat::ePNG; }
    void set_output_images(bool s) { output_format = (s ? OutputFormat::ePNG : OutputFormat::eMP4); }

    void start_capture(std::string path);
    void stop_capture();
    void capture_current_state(std::string path);

private:
    std::string get_output_path();

    const Window& window;

    std::string path_prefix;
    int capture_idx{ 0 };
    bool sequence_capturing{ false };
    bool single_image_capturing{ false };
    OutputFormat output_format{ OutputFormat::ePNG };

    FILE* mp4_file_fd{ nullptr };
};

}