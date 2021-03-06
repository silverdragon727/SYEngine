# ShanYe MediaFoundation Engine (Windows)

**[ > 快速整合指南 | Quick Start Guide](https://github.com/amamiya/SYEngine/blob/master/!GUIDE/GUIDE.md)**

### 开发信息
 - 开发者：**ShanYe**
 - 完成度：97% [实现跟系统播放MP4(本地或网络)一样的性能、稳定性、效果、API兼容性。]
 
### 许可协议
 - Licensed under ***LGPLv3*** or later.
 - 在**LGPLv3**许可请求外的情况，请联系`rukino.saki@outlook.com`，并说明目的。
 - 建议直接下载vsix整合使用。
 - 下面的库依据原本的协议：`ffcodecs` (ffmpeg)、`stagefright` (AOSP)

### 版本历史 (简单信息)
 - 2015-12-16：修复MediaElement的DownloadProgressChanged事件在播放FLV的时候不触发下载进度的问题，现在**FLV整体播放效果已跟系统播放MP4近乎100%相同**！
 - 2015-12-14：移除CoreMFCommon工程；修复播放网络流的时候，Seek没缓冲进度的问题。
 - **2015-12-11**：修复关键bug，即花屏、网络差环境下的缓冲逻辑等，已经可以投入产品使用。不过仍有细小的不稳定性。
 - 2015-12-09：重构demuxers，合并到一个工程，原来的工程删除。 
 - 2015-12-07：第一次提交到git进行托管。

### 下一步 (预计)
 - **移植兼容UWP**。
 - 使用Windows.Web.Http.HttpClient来提供Http下载服务。
 - 去除ffmpeg模块(ffcodecs)。

***
### 组件1：跨平台的解复用器
 - **CoreCommon**：此为核心模块，组件1中的所有其他的项目都会直接**依赖于此项目**，此项目为静态库，提供基础的抽象、内存管理、解码器标准化定义等。
 - **CoreDemuxers**：包含自行编写的`FLV\MKV\MP4`解封装器。

### 组件2：Win32的MF分离器和混流器
 - **CoreMFSource**：此为最**核心**的实现，其直接依赖CoreCommon，间接依赖FLV、MKV、MP4这三个Demuxer。其提供了高层封装，使组件可以在Win32的MF管道线模型中运行，其也是WinRT应用的MediaElement控件播放时调用的核心模块。
 - **MultipartStreamMatroska**：直接依赖ffcodecs。此库提供多分P的视频流重新混合为一个分P并且输出为MKV容器，然后CoreMFSource即可播放这个混合后的单分P，实现**多分P播放无缝过度**。其可以无缝混合本地文件以及HTTP网络URL流。

***
### 待修复的已知问题
 - MultipartStreamMatroska 存在一点点的**内存泄漏**，多一个分P泄漏20字节左右，不是很严重。

### 待增强开发的功能
 - MultipartStreamMatroska 无法提供整体的下载进度。