#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
cv::Mat readBinaryImage(const std::string& filePath, int width, int height) {
    std::ifstream file(filePath);
    std::vector<uint16_t> pixelData;
    uint16_t value;
    while (file >> value) {
        pixelData.push_back(value);
    }
    file.close();
    return cv::Mat(height, width, CV_16UC1, pixelData.data()).clone();
}

int main() {

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    const int window_height = 800;
    const int window_width = 1280;
    GLFWwindow* window = glfwCreateWindow(window_width, window_height, "OpenCV Image Loading Example", NULL, NULL);

    glfwMakeContextCurrent(window);

    //cv::Mat image = cv::imread("image.png", cv::IMREAD_UNCHANGED);
    cv::Mat image = readBinaryImage("buffer_to_show(640IN800).Txt", 640, 800);
    cv::cvtColor(image, image, cv::COLOR_GRAY2RGB);
    
    image.convertTo(image, CV_8U, 1.0 / 256.0);
    cv::Mat imager90;
    cv::rotate(image, imager90, cv::ROTATE_90_CLOCKWISE);
    //cv::imshow("loaded", imager90);
    cv::flip(imager90, imager90, 0);

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB,
        imager90.cols,
        imager90.rows,
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        imager90.data
    );

    glBindTexture(GL_TEXTURE_2D, 0);
    std::cout << "Image loaded" << std::endl;
    float cols_adjusted = (2*float(imager90.cols) / window_width)-1;
    float rows_adjusted = (2*float(imager90.rows) / window_height)-1;
    std::cout << rows_adjusted <<" x " << cols_adjusted << std::endl;
    float trs_start = -(1.0f + cols_adjusted);
    float trs_end = 0;
    float y_offset= 0.2;
    float speed = 0.01;
    glfwSwapInterval(1);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture);

        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(-cols_adjusted - trs_start, -1.0f + y_offset);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(1.0f - trs_start, -1.0f + y_offset);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(1.0f - trs_start, rows_adjusted + y_offset);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(-cols_adjusted - trs_start, rows_adjusted + y_offset);
        glEnd();
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(-cols_adjusted - trs_end, -1.0f + y_offset);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(1.0f - trs_end, -1.0f + y_offset);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(1.0f - trs_end, rows_adjusted + y_offset);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(-cols_adjusted - trs_end, rows_adjusted + y_offset);
        glEnd();

        trs_end += speed;
        if ((-cols_adjusted - trs_end) <= -1.0f) {
            trs_start += speed;
            if (trs_start >= 0.0f) {
                trs_start = -(1.0f + cols_adjusted);
                trs_end = 0;
            }
        }
        
        glBindTexture(GL_TEXTURE_2D, 0);
        glfwSwapBuffers(window);
        glfwPollEvents();
        if (cv::waitKey(10) >= 0) break;
    }

    glDeleteTextures(1, &texture);
    glfwTerminate();
    return 0;
}
