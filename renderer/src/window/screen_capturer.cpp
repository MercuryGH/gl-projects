#include <window/screen_capturer.hpp>

#include <thread>

#include <window/window.hpp>
#include <glh/resource.hpp>

// #define STB_IMAGE_WRITE_IMPLEMENTATION
// #include <stb_image_write.h>

namespace {
    void save_mp4_frame(FILE* mp4_file_fd, std::unique_ptr<GLubyte[]> pixels, int width, int height) {
        fwrite(pixels.get(), width * height * 4, 1, mp4_file_fd);
    }

    void save_tga_file(std::string path, std::unique_ptr<GLubyte[]> pixels, int width, int height) {
        FILE *outputFile = fopen(path.c_str(), "w");
        uint16_t header[] = {0, 2, 0, 0, 0, 0, (uint16_t)width, (uint16_t)height, 24};

        const int n_pixels = width * height;
        GLubyte* bgr_pixels = new GLubyte[n_pixels * 3];
        for (int i = 0; i < n_pixels; i++) {
            GLubyte rgb[3];
            for (int j = 0; j < 3; j++) {
                rgb[j] = pixels.get()[i * 4 + j];
            }
            for (int j = 0; j < 3; j++) {
                bgr_pixels[i * 3 + j] = rgb[2 - j];
            }
        }

        fwrite(&header, sizeof(header), 1, outputFile);
        fwrite(bgr_pixels, n_pixels * 3, 1, outputFile);

        fclose(outputFile);
        delete bgr_pixels;
    }
}

namespace renderer {

void ScreenCapturer::update() {
    if (is_capturing() == false) {
        return;
    }

    float width_scale, height_scale;
	glfwGetWindowContentScale(window.window, &width_scale, &height_scale);
    const size_t width = static_cast<size_t>(window.width * width_scale);
    const size_t height = static_cast<size_t>(window.height * height_scale);

    auto pixels = std::make_unique<GLubyte[]>(width * height * 4);
    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.get());

    std::string path = get_output_path();

    if (output_format == OutputFormat::eTGA) {
        std::thread{ save_tga_file, path, std::move(pixels), width, height }.detach();

        if (single_image_capturing == true) {
            single_image_capturing = false;
        }
    }
    else if (output_format == OutputFormat::ePNG) {
        std::thread{ save_png_file, path, std::move(pixels), width, height }.detach();

        if (single_image_capturing == true) {
            single_image_capturing = false;
        }
    }
    else if (output_format == OutputFormat::eMP4) {

        // if (mp4_file_fd == nullptr)
        // {
        //     constexpr int mp4_fps = 25;

        //     char cmd[1024];
        //     sprintf(cmd,
        //         "ffmpeg -r %d -f rawvideo -pix_fmt rgba -s %dx%d -i - "
        //         "-threads 0 -y -b:v 50000k   -c:v libx264 -preset slow -crf 22 -an   -pix_fmt yuv420p -vf vflip %s",
        //         mp4_fps, width, height, path.c_str());

        // Note: code below may only works on Linux platform
        //     mp4_file_fd = popen(cmd, "w");
        // }

        // std::thread{ save_mp4_frame, mp4_file_fd, std::move(pixels), width, height }.detach();
        assert(false);
    }
}

void ScreenCapturer::start_capture(std::string path) {
    path_prefix = path;
    capture_idx = 0;
    sequence_capturing = true;
}

void ScreenCapturer::stop_capture() {
    // sequence_capturing = false;
    // if (output_format == OutputFormat::eMP4)
    // {
    //     pclose(mp4_file_fd);
    //     mp4_file_fd = nullptr;
    // }
    assert(false);
}

void ScreenCapturer::capture_current_state(std::string path) {
    path_prefix = path;
    single_image_capturing = true;
}

std::string ScreenCapturer::get_output_path() {
    const auto zeros_prefix_str = [](int width, std::string& num) {
        while (num.size() < width) {
            num = "0" + num;
        }
    };

    std::string n_frame_str = std::to_string(capture_idx);
    capture_idx++;
    constexpr int k_number_width = 3;
    zeros_prefix_str(k_number_width, n_frame_str);

    std::string postfix = (output_format == OutputFormat::eTGA ? ".tga"
                         : output_format == OutputFormat::ePNG ? ".png"
                         : ".notimplmented");
    std::string path = path_prefix + n_frame_str + postfix;
    return path;
}

}
