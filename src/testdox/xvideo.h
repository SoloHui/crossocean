/**
 * @file xvideo.h
 * @author Jinghui (3414467112@qq.com)
 * @brief 视频处理类头文件
 */
#ifndef XVIDEO_H
#define XVIDEO_H

#include <string>

/**
 * @brief 视频处理类
 * @details 该类提供视频截图等功能\n
 *          说明换行
 */
class XVideo {
 public:
  XVideo();
  ~XVideo() = default;

  /**
   * @brief 视频截图
   *
   * @param video_path 视频文件路径
   * @param output_image 输出图片路径
   * @param second 截图时间点（秒）
   * @param width 输出图片宽度
   * @param height 输出图片高度
   * @return true 截图成功
   * @return false 截图失败
   */
  bool Screenshot(const std::string& video_path,
                  const std::string& output_image, int second, int width,
                  int height);

  /// 对象索引
  int index_ = 0;

 private:
  /// 文件名缓冲区
  char buf_[1024] = {0};
};

#endif  // XVIDEO_H
