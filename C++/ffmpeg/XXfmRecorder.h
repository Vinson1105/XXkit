#ifndef XXFMRECORDER_H
#define XXFMRECORDER_H

#include <functional>
#include <memory>
#include <mutex>

extern "C" {
#include "XXfmMp4Writer.h"
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavdevice/avdevice.h>
#include <libavutil/audio_fifo.h>
#include <libswresample/swresample.h>
#include <libavutil/error.h>
}

class XXfmRecorder
{
public:
	enum State{
		Closed,
		Opening,
		Opened,
		Recording,
		Closing,
	};

	XXfmRecorder();
	~XXfmRecorder();

    bool open(std::string camera, int framerate, std::string mic = "", int samplerate = -1);
	bool start(std::string path);
	bool stop(bool save);
	bool close();
	State state() const { return _state; }
    time_t beginTime() const { return _beginTime;}

    void setMessageBlock(std::function<void(int code, std::string msg)> block) { _messageBlock = block; }
    std::function<void(int code, std::string msg)> messageBlock() const { return _messageBlock; }

	void setPreviewBlock(std::function<void(int8_t *data[], int width, int height)> block) { _previewBlock = block; };
	std::function<void(int8_t *data[], int width, int height)> previewBlock() const { return _previewBlock; }

	void setStateBlock(std::function<void(XXfmRecorder::State before, XXfmRecorder::State after)> block) { _stateBlock = block;  }
	std::function<void(XXfmRecorder::State before, XXfmRecorder::State after)> stateBlock() const { return _stateBlock; }

private:
    //bool stop(bool save, bool skipCallBack);
    void setState(State state);
    void message(int code, std::string msg);
	
	void run(int validID, std::string camera, int framerate, std::string mic, int samplerate);
	void videoRun(int validID, AVFormatContext *formatCtx, AVCodecContext *decodecCtx, AVCodecContext *encodecCtx);
	void audioRun(int validID, AVFormatContext *formatCtx, AVCodecContext *decodecCtx, AVCodecContext *encodecCtx);
	int initVideoContext(std::string camera, int framerate, AVFormatContext **formatCtx, AVCodecContext **decodecCtx, AVCodecContext **encodecCtx);
	int initAudioContext(std::string mic, int sample, AVFormatContext **formatCtx, AVCodecContext **decodecCtx, AVCodecContext **encodecCtx);
	bool isNeedResample(AVFrame *frame, AVCodecContext *codecContext);

	void writeVideo(int validID, int8_t *data, int size, int width, int height, int64_t timestamp, bool isKeyFrame);
	void writeAudio(int validID, int8_t *data, int size, int64_t timestamp);
	void freeWriterHandle(bool save);
	XXfmMp4WriterHandle createWriterHandle(const char *path, int width, int height);

    int readAndDecode(AVFormatContext *formatContext, AVCodecContext *codecContext, AVFrame **output);

private:
    std::function<void(int code, std::string msg)> _messageBlock;
	std::function<void(int8_t *data[], int width, int height)> _previewBlock;
	std::function<void(XXfmRecorder::State before, XXfmRecorder::State after)> _stateBlock;

	XXfmMp4WriterHandle _writeHandle;
	std::mutex _writeMutex;

	std::shared_ptr<std::thread> _mainThread;
	int _validID;
	State _state;
    time_t _beginTime;

	std::string _path;
	std::string _camera;
	std::string _mic;
};

#endif /* XXFMRECORDER_H */
