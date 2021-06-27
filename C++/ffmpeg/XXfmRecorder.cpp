#include "XXfmRecorder.h"
#include "XXfmUtility.h"
#include <thread>
#include <string>
#include <windows.h>

#define IS_NEWSET_VALID (_validID==validID)
#define kFrameRate 15
#define kSampleRate 16000
#define kSampleSize 16
#define kChannelCount 1

#define EnableTimeout
//#define EnableDetailLogVideo
//#define EnableDetailLogAudio

typedef const std::lock_guard<std::mutex> GuardLocker;

using namespace std;

static int isTimeout(void* ctx)
{
    auto p = (XXfmRecorder*)ctx;
    p->messageBlock()(0,"isTimeout");
    return time(NULL) - p->beginTime() >= 3 ? 1 : 0;// 3秒超时
}

XXfmRecorder::XXfmRecorder()
	: _state(Closed)
	, _validID(0)
	, _mainThread(nullptr)
    , _messageBlock(nullptr)
	, _previewBlock(nullptr)
	, _stateBlock(nullptr)
	, _writeHandle(NULL){

}
XXfmRecorder::~XXfmRecorder() {
	_previewBlock = nullptr;
	_stateBlock = nullptr;
    _messageBlock = nullptr;
	close();
	if (nullptr != _mainThread && _mainThread->joinable()) {
		_mainThread->join();
		_mainThread.reset();
	}
}

bool XXfmRecorder::open(std::string camera, int framerate, std::string mic, int samplerate) {
    message(0,"open");
	if (_state != Closed) {
        message(-1, "could not open again when recorder not on closed");
		return false;
	}
	setState(Opening);
	_camera = camera;
	_mic = mic;
	int validID = ++_validID;
    if(nullptr != _mainThread){
        _mainThread->join();
        _mainThread = nullptr;
    }

    _beginTime = time(NULL);
	_mainThread = shared_ptr<thread>(new thread(bind(&XXfmRecorder::run, this, validID, _camera, framerate, _mic, samplerate)));
	return true;
}
bool XXfmRecorder::start(std::string path) {
    message(0,"start");
    if (_state != Opened) {
        message(-1, "could not start when recorder not on opened");
		return false;
	}
	_path = path;
	setState(Recording);
	return true;
}
bool XXfmRecorder::stop(bool save) {
    message(0,"stop. save:"+std::to_string(save));
    if (_state != Recording) {
        return false;
    }
    setState(Opened);
    freeWriterHandle(save);
    return true;
}
bool XXfmRecorder::close() {
    message(0,"close");
    if (_state == Closed || _state == Closing) {
		return false;
	}
	++_validID;
    stop(false);
	setState(Closing);
	return true;
}

void XXfmRecorder::setState(State state) {
	if (_state == state) {
		return;
	}
	auto prev = _state;
	_state = state;
    if (nullptr != _stateBlock) {
		_stateBlock(prev, state);
	}
}
void XXfmRecorder::message(int code, std::string msg) {
    if (nullptr == _messageBlock) {
        return;
	}
    if(code<0){
        char buffer[128] = {0};
        av_make_error_string(buffer, 128, code);
        std::string errString(buffer);
        _messageBlock(code, msg + " (" + errString+")");
    }
    else{
        _messageBlock(code, msg);
    }
}


void XXfmRecorder::run(int validID, std::string camera, int framerate, std::string mic, int samplerate) {
    message(0,"[video] start.");
    if (!IS_NEWSET_VALID) {
        message(0,"run interruption.");
        return;
    }

	av_register_all();
	avcodec_register_all();
	avdevice_register_all();

	AVFormatContext *videoFormatCtx = NULL;
	AVCodecContext *videoDecodecCtx = NULL;
	AVCodecContext *videoEncodecCtx = NULL;

	AVFormatContext *audioFormatCtx = NULL;
	AVCodecContext *audioDecodecCtx = NULL;
	AVCodecContext *audioEncodecCtx = NULL;

	std::string msg = "no error.";
	thread videoThread;
	thread audioThread;
    bool audioEnable = ""!=mic && samplerate>0;

	int ret = initVideoContext(camera, framerate, &videoFormatCtx, &videoDecodecCtx, &videoEncodecCtx);
	if (0 != ret) {
		msg = "could not init video context.";
		goto cleanup;
	}
	
    if(audioEnable){
        ret = initAudioContext(mic, samplerate, &audioFormatCtx, &audioDecodecCtx, &audioEncodecCtx);
        if (0 != ret) {
            msg = "could not init audio context.";
            goto cleanup;
        }
    }

	videoThread = thread(bind(&XXfmRecorder::videoRun, this, validID, videoFormatCtx, videoDecodecCtx, videoEncodecCtx));
    if(audioEnable){
        audioThread = thread(bind(&XXfmRecorder::audioRun, this, validID, audioFormatCtx, audioDecodecCtx, audioEncodecCtx));
    }

    message(0,"run video thread join");
    videoThread.join();
    if(audioEnable){
        message(0,"run audio thread join");
        audioThread.join();
    }
    message(0,"run thread join finished");

cleanup:
	if (NULL != videoFormatCtx) { avformat_close_input(&videoFormatCtx); avformat_free_context(videoFormatCtx); }
	if (NULL != videoDecodecCtx) { avcodec_close(videoDecodecCtx); avcodec_free_context(&videoDecodecCtx); }
	if (NULL != videoEncodecCtx) { avcodec_close(videoEncodecCtx); avcodec_free_context(&videoEncodecCtx); }

	if (NULL != audioFormatCtx) { avformat_close_input(&audioFormatCtx); avformat_free_context(audioFormatCtx); }
	if (NULL != audioDecodecCtx) { avcodec_close(audioDecodecCtx); avcodec_free_context(&audioDecodecCtx); }
	if (NULL != audioEncodecCtx) { avcodec_close(audioEncodecCtx); avcodec_free_context(&audioEncodecCtx); }
	if (ret<0) {	
		char buffer[AV_ERROR_MAX_STRING_SIZE] = { 0 };
		av_make_error_string(buffer, AV_ERROR_MAX_STRING_SIZE, ret);
		msg.append("(");
		msg.append(buffer);
		msg.append(")");
        _messageBlock(ret, msg);
	}
	setState(Closed);
    message(0,"run end.");
}
void XXfmRecorder::videoRun(int validID, AVFormatContext *formatCtx, AVCodecContext *decodecCtx, AVCodecContext *encodecCtx) {
    message(0,"[video] start.");

    struct SwsContext *swsContext = NULL;
    AVStream *stream = xxfmUtility_findVideoStream(formatCtx);
    AVPacket *packetAfterEncode = av_packet_alloc();
    AVFrame *frameAfterDecode = NULL;
    AVFrame *frameAfterSws = NULL;
    int ret = 0;

    try {
#ifdef EnableDetailLogVideo
        message(0,"[video] init.");
#endif
        ret = xxfmUtility_initVideoFrame(&frameAfterSws, encodecCtx->pix_fmt, encodecCtx->width, encodecCtx->height); //xxfm_ av_frame_alloc();
        if (ret < 0) {
            goto cleanup;
        }

#ifdef EnableDetailLogVideo
            message(0,"[video] <read - decode - rescale - encode> +++++ +++++.");
#endif
        while (IS_NEWSET_VALID) {
            // 获取数据并解码
#ifdef EnableDetailLogVideo
            message(0,"[video] read and decode.");
#endif
            ret = readAndDecode(formatCtx, decodecCtx, &frameAfterDecode);
            if (ret < 0) {
                if (ret == AVERROR(EAGAIN)) {
                    continue;
                }
                else {
                    goto cleanup;
                }
            }

            // 裁剪
#ifdef EnableDetailLogVideo
            message(0,"[video] rescale.");
#endif
            if (frameAfterDecode->format != encodecCtx->pix_fmt) {
                if (NULL == swsContext) {
#ifdef EnableDetailLogVideo
            message(0,"[video] rescale - init.");
#endif
                    if (AV_PIX_FMT_NONE == decodecCtx->pix_fmt) decodecCtx->pix_fmt = (enum AVPixelFormat)frameAfterDecode->format;
                    if (xxfmUtility_initRescale(&swsContext, decodecCtx, encodecCtx)<0) {
                        goto cleanup;
                    }
                }

#ifdef EnableDetailLogVideo
            message(0,"[video] rescale - sws_scale.");
#endif
                ret = sws_scale(swsContext, frameAfterDecode->data, frameAfterDecode->linesize, 0, decodecCtx->height, frameAfterSws->data, frameAfterSws->linesize);
                if (ret <= 0) {
                    goto cleanup;
                }
            }
            else {
#ifdef EnableDetailLogVideo
            message(0,"[video] rescale - copy.");
#endif
                for (int index = 0; index < 3; index++) {
                    frameAfterSws->linesize[index] = frameAfterDecode->linesize[index];
                    memcpy(frameAfterSws->data[index], frameAfterDecode->data[index], frameAfterSws->linesize[index]);
                }
            }

            // 时间基转换
#ifdef EnableDetailLogVideo
            message(0,"[video] timebase transform.");
#endif
            xxfmUtility_transformTimeBaseFromAnotherFrame(frameAfterDecode, stream->time_base, frameAfterSws, encodecCtx->time_base);
            if (this->previewBlock() && IS_NEWSET_VALID) {
                if (this->state() == Opening) {
                    this->setState(Opened);
                }
                this->previewBlock()((int8_t**)frameAfterSws->data,encodecCtx->width, encodecCtx->height);
            }

            // 进行视频编码
            if (State::Recording==this->state() && IS_NEWSET_VALID) {
#ifdef EnableDetailLogVideo
            message(0,"[video] encode 1.");
#endif
                ret = xxfmUtility_encode(frameAfterSws, encodecCtx, packetAfterEncode);
                if (0 != ret) {
                    if (-11 == ret) {
                        continue;
                    }
                    goto cleanup;
                }
#ifdef EnableDetailLogVideo
            message(0,"[video] encode 2.");
#endif

                // 回调通知
                writeVideo(validID,(int8_t*)packetAfterEncode->data,packetAfterEncode->size,
                    encodecCtx->width,encodecCtx->height,packetAfterEncode->pts * 1000 / encodecCtx->time_base.den,
                    packetAfterEncode->flags&AV_PKT_FLAG_KEY);
            }

            av_packet_unref(packetAfterEncode);
        }
#ifdef EnableDetailLogVideo
            message(0,"[video] <read - decode - rescale - encode> ----- -----.");
#endif
    } catch (std::exception excep) {
        message(-996,excep.what());
    }

cleanup:
	if(frameAfterDecode) av_frame_free(&frameAfterDecode);
	if(frameAfterSws) av_frame_free(&frameAfterSws);
	if(packetAfterEncode) av_packet_free(&packetAfterEncode);
	if(swsContext) sws_freeContext(swsContext);

    message(0,"[video] end.");
}
void XXfmRecorder::audioRun(int validID, AVFormatContext *formatCtx, AVCodecContext *decodecCtx, AVCodecContext *encodecCtx) {
    message(0,"[audio] start.");

    AVStream *stream = xxfmUtility_findAudioStream(formatCtx);
	struct SwrContext *swrContext = NULL;

	// 是否为第一帧
	int isFirst = 1;
	// 返回值
	int ret = 0;
	// 结果回调
	// 一帧aac数据的固定取样数量，实际大小 = 数量 * 单个样品大小
	const int aac_frame_size = encodecCtx->frame_size;

	// 解码后的frame
	AVFrame *frameAfterDecode = av_frame_alloc();
	// 重采样后的frame
	AVFrame *frameAfterResample = NULL;
	// 存放已经重采样后的数据
	AVAudioFifo *fifo = av_audio_fifo_alloc(encodecCtx->sample_fmt, encodecCtx->channels, 1);
	// 编码前的从fifo读取的frame
	AVFrame *frameBeforeEncode = NULL;
	// 解码后的packet
	AVPacket *packetAfterEncode = av_packet_alloc();

    try {
#ifdef EnableDetailLogAudio
        message(0,"[audio] <read - decode - resample - encode> +++++ +++++.");
#endif
        while (IS_NEWSET_VALID) {
#ifdef EnableDetailLogAudio
            message(0,"[audio] <read - decode> +++++ +++++.");
#endif
            // -- 没有足够的数据进行编码，持续获取数据
            while (IS_NEWSET_VALID && av_audio_fifo_size(fifo) < encodecCtx->frame_size) {

#ifdef EnableDetailLogAudio
                message(0,"[audio] read and decode.");
#endif
                // 获取音频帧
                if ((ret = xxfmUtility_readAndDecode(formatCtx, decodecCtx, &frameAfterDecode)) < 0) {
                    //Sleep(20);
                    //continue;
                    goto cleanup;
                }

#ifdef EnableDetailLogAudio
                message(0,"[audio] resample.");
#endif
                // 进行重采样
                const int pcm_nb_samples = frameAfterDecode->nb_samples;
                if (NULL != frameAfterResample) {
                    av_frame_unref(frameAfterResample);
                    av_frame_free(&frameAfterResample);
                    frameAfterResample = NULL;
                }
                ret = xxfmUtility_initAudioFrame(&frameAfterResample, encodecCtx->sample_fmt, pcm_nb_samples, av_get_default_channel_layout(encodecCtx->channels));
                if (av_frame_is_writable(frameAfterResample) < 0) {
                    goto cleanup;
                }
                frameAfterResample->sample_rate = encodecCtx->sample_rate;

                if (isNeedResample(frameAfterDecode, encodecCtx)) {
                    if (NULL == swrContext && (ret = xxfmUtility_initResampler(&swrContext, decodecCtx, encodecCtx))<0) {
                        goto cleanup;
                    }
                    if ((ret = swr_convert_frame(swrContext, frameAfterResample, frameAfterDecode)) < 0) {
                        char buffer[512] = { 0 };
                        av_make_error_string(buffer, 512, ret);
                        goto cleanup;
                    }
                }

#ifdef EnableDetailLogAudio
                message(0,"[audio] enfifo.");
#endif
                // 加入到fifo中
                if (av_audio_fifo_realloc(fifo, av_audio_fifo_size(fifo) + pcm_nb_samples) < 0) {
                    goto cleanup;
                }
                if (av_audio_fifo_write(fifo, (void**)frameAfterResample->data, pcm_nb_samples) < pcm_nb_samples) {
                    goto cleanup;
                }
            }
            // -- 没有足够的数据进行编码，持续获取数据
#ifdef EnableDetailLogAudio
            message(0,"[audio] <read - decode> ----- -----.");
#endif


            // 从fifo中读取对应的数据来进行编码
            if (!IS_NEWSET_VALID) {
                break;
            }
            if (av_audio_fifo_size(fifo) >= encodecCtx->frame_size) {
                // 读取
                if (NULL != frameBeforeEncode) {
                    av_frame_unref(frameBeforeEncode);
                    av_frame_free(&frameBeforeEncode);
                    frameBeforeEncode = NULL;
                }

#ifdef EnableDetailLogAudio
                message(0,"[audio] defifo.");
#endif
                ret = xxfmUtility_initAudioFrame(&frameBeforeEncode, encodecCtx->sample_fmt, aac_frame_size, av_get_default_channel_layout(encodecCtx->channels));
                if (ret < 0 || av_frame_is_writable(frameBeforeEncode) < 0) {
                    goto cleanup;
                }

                if (av_audio_fifo_read(fifo, (void**)frameBeforeEncode->data, aac_frame_size) < aac_frame_size) {
                    goto cleanup;
                }


#ifdef EnableDetailLogAudio
            message(0,"[audio] encode.");
#endif
                // 编码
                if (this->state()==Recording && IS_NEWSET_VALID) {
                    int isEncodced = 0;
                    if (xxfmUtility_encodeAudioFrame(frameBeforeEncode, encodecCtx, packetAfterEncode, &isEncodced, isFirst) < 0) {
                        goto cleanup;
                    }
                    isFirst = 0;

                    // 回调通知
                    if (isEncodced) {
                        writeAudio(validID, (int8_t*)packetAfterEncode->data, packetAfterEncode->size, packetAfterEncode->pts * 1000 / encodecCtx->time_base.den);
                    }
                }
                av_packet_unref(packetAfterEncode);
            }
        }
#ifdef EnableDetailLogAudio
            message(0,"[audio] <read - decode - resample - encode> ----- -----.");
#endif
    } catch (std::exception execp) {
        message(-997,execp.what());
    }

cleanup:
	if (NULL != frameAfterDecode) {
		av_frame_unref(frameAfterDecode);
		av_frame_free(&frameAfterDecode);
	}
	if (NULL != fifo) av_audio_fifo_free(fifo);
	if (NULL != frameBeforeEncode) {
		av_frame_unref(frameBeforeEncode);
		av_frame_free(&frameBeforeEncode);
	}
	if (NULL != packetAfterEncode) {
		av_packet_unref(packetAfterEncode);
		av_packet_free(&packetAfterEncode);
	}
	if (NULL != frameAfterResample) {
		av_frame_unref(frameAfterResample);
		av_frame_free(&frameAfterResample);
	}
	if (NULL != swrContext) swr_free(&swrContext);

    message(ret,"run audio end");
}
int XXfmRecorder::initVideoContext(std::string camera, int framerate, AVFormatContext **formatCtx, AVCodecContext **decodecCtx, AVCodecContext **encodecCtx) {
	int ret = 0;
	char header[] = "video=";
	char url[256] = { 0 };
	if (0!=memcmp((void*)camera.c_str(), header, strlen(header))) {
		sprintf(url, "%s%s", header, camera.c_str());
	}
	else {
		sprintf(url, "%s", camera.c_str());
	}

	AVDictionary *options = NULL;
	//av_dict_set(&options, "video_size", "1280x960", 0);
	av_dict_set(&options, "timeout", "2", 0);

	AVStream *stream = NULL;
	AVInputFormat *inputFormat = av_find_input_format("dshow");
	if (NULL == inputFormat) {
		ret = AVERROR(ENOMEM);
		goto cleanup;
	}

#ifdef EnableTimeout
	*formatCtx = avformat_alloc_context();
	(*formatCtx)->interrupt_callback.callback = isTimeout;
	(*formatCtx)->interrupt_callback.opaque = this;
    //(*formatCtx)->flags |= AVFMT_FLAG_NONBLOCK;
#endif
	if (0 != (ret=avformat_open_input(formatCtx, url, inputFormat, &options))) {
		//char buffer[AV_ERROR_MAX_STRING_SIZE] = {0};
		//av_make_error_string(buffer, AV_ERROR_MAX_STRING_SIZE, ret);
		goto cleanup;
	}

	stream = xxfmUtility_findVideoStream(*formatCtx);
	if (NULL == stream) {
		ret = AVERROR(ENOMEM);
		goto cleanup;
	}

	if (xxfmUtility_openDecodec(decodecCtx, stream) < 0) {
		goto cleanup;
	}

	if ((ret=xxfmUtility_openVideoEncodec(encodecCtx,
		AV_CODEC_ID_H264,
		AV_PIX_FMT_YUV420P,
		(*decodecCtx)->width,
		(*decodecCtx)->height,
		framerate,
        4000000,
		framerate)) < 0) {
		goto cleanup;
	}
	if (NULL != options) av_dict_free(&options);
	return 0;

cleanup:
	if (NULL != *formatCtx) { avformat_close_input(formatCtx); avformat_free_context(*formatCtx); *formatCtx = NULL; }
	if (NULL != *decodecCtx) { avcodec_close(*decodecCtx); avcodec_free_context(decodecCtx); *decodecCtx = NULL; }
	if (NULL != *encodecCtx) { avcodec_close(*encodecCtx); avcodec_free_context(encodecCtx); *encodecCtx = NULL; }
	if (NULL != options) av_dict_free(&options);
	return ret;
}
int XXfmRecorder::initAudioContext(std::string mic, int samplerate, AVFormatContext **formatCtx, AVCodecContext **decodecCtx, AVCodecContext **encodecCtx) {
	int ret = 0;
	char header[] = "audio=";
	char url[256] = { 0 };
	if (0 != memcmp((void*)mic.c_str(), header, strlen(header))) {
		sprintf(url, "%s%s", header, mic.c_str());
	}
	else {
		sprintf(url, "%s", mic.c_str());
	}

	AVStream *stream = NULL;
	AVDictionary *options = NULL;
	AVInputFormat *inputFormat = av_find_input_format("dshow");
	if (NULL == inputFormat) {
		ret = AVERROR(ENOMEM);
		goto cleanup;
	}

#ifdef EnableTimeout
    *formatCtx = avformat_alloc_context();
    (*formatCtx)->interrupt_callback.callback = isTimeout;
    (*formatCtx)->interrupt_callback.opaque = this;
    //(*formatCtx)->flags |= AVFMT_FLAG_NOBUFFER;
#endif
	av_dict_set_int(&options, "sample_rate", kSampleRate, 0);
	av_dict_set_int(&options, "sample_size", kSampleSize, 0);
	av_dict_set_int(&options, "channels", kChannelCount, 0);
    av_dict_set(&options, "stimeout", "3000000", 0);
    av_dict_set(&options, "timeout", "3000", 0);
    if (0 != (ret = avformat_open_input(formatCtx, url, inputFormat, &options))) {
		goto cleanup;
	}

	stream = xxfmUtility_findAudioStream(*formatCtx);
	if (NULL == stream) {
		ret = AVERROR(ENOMEM);
		goto cleanup;
	}

	if ((ret=xxfmUtility_openDecodec(decodecCtx, stream)) < 0) {
		goto cleanup;
	}

	if ((ret=xxfmUtility_openAudioEncode(encodecCtx,
		AV_CODEC_ID_AAC,
		AV_SAMPLE_FMT_FLTP,	 // AAC暂时只支持FLTP
		samplerate,
		1)) < 0)
	{
		goto cleanup;
	}
	return 0;

cleanup:
	if (NULL != *formatCtx) { avformat_close_input(formatCtx); avformat_free_context(*formatCtx); *formatCtx = NULL; }
	if (NULL != *decodecCtx) { avcodec_close(*decodecCtx); avcodec_free_context(decodecCtx); *decodecCtx = NULL; }
	if (NULL != *encodecCtx) { avcodec_close(*encodecCtx); avcodec_free_context(encodecCtx); *encodecCtx = NULL; }
	return ret;
}
bool XXfmRecorder::isNeedResample(AVFrame *frame, AVCodecContext *codecContext) {
	return frame->channels != codecContext->channels || frame->sample_rate != codecContext->sample_rate || frame->format != codecContext->sample_fmt;
}

void XXfmRecorder::writeVideo(int validID, int8_t *data, int size, int width, int height, int64_t timestamp, bool isKeyFrame) {
    static int index = 0;
    GuardLocker locker(this->_writeMutex);
    if (!IS_NEWSET_VALID || Recording!=this->_state) {
		return;
	}
	if (NULL == this->_writeHandle) {
#ifdef EnableDetailLogVideo
        message(0,"[video] [record] create writer handle");
#endif
		this->_writeHandle = this->createWriterHandle(this->_path.c_str(), width, height);
	}
	if (NULL == this->_writeHandle) {
#ifdef EnableDetailLogVideo
        message(0,"[video] [record] invalid writer handle");
#endif
        return;
	}
    if(xxfmMp4Writer_video(this->_writeHandle, (uint8_t*)data, size, timestamp, isKeyFrame) < 0){
        message(0,"[video] [record] failure to write frame. size:"+std::to_string(size));
    }
    else{
        ++index;
#ifdef EnableDetailLogVideo
            message(0,"[video] [record] written. size:"+std::to_string(size));
#endif
    }
}
void XXfmRecorder::writeAudio(int validID, int8_t *data, int size, int64_t timestamp) {
    static int index = 0;
    GuardLocker locker(this->_writeMutex);
    if (!IS_NEWSET_VALID  || Recording!=this->_state || NULL == this->_writeHandle) {
		return;
	}
    if(xxfmMp4Writer_audio(this->_writeHandle, (uint8_t*)data, size, timestamp) < 0){
        message(0,"[audio] [record] failure to write frame. size:"+std::to_string(size));
    }
    else{
        ++index;
#ifdef EnableDetailLogAudio
            message(0,"[audio] [record] frame written. size:"+std::to_string(size));
#endif
    }
}
void XXfmRecorder::freeWriterHandle(bool save) {
	GuardLocker locker(this->_writeMutex);
	if (NULL == this->_writeHandle) {
		return;
	}
    xxfmMp4Writer_close(&(this->_writeHandle), save);
    this->_writeHandle = NULL;
}
XXfmMp4WriterHandle XXfmRecorder::createWriterHandle(const char *path, int width, int height) {
	XXfmMp4WriterVideoParam videoParam;
	videoParam.type = XXavFrameType_H264;
	videoParam.width = width;
	videoParam.height = height;
	videoParam.frameRate = kFrameRate;

	XXfmMp4WriterAudioParam audioParam;
	audioParam.type = XXavFrameType_AAC;
	audioParam.channelCount = kChannelCount;
	audioParam.sampleFormat = XXfmMp4WriterAudioSampleFormat_FLTP;
	audioParam.sampleRate = kSampleRate;

	return xxfmMp4Writer_create((char*)path, &videoParam, &audioParam);
}

int XXfmRecorder::readAndDecode(AVFormatContext *formatContext, AVCodecContext *codecContext, AVFrame **output) {
#ifdef EnableDetailLogVideo
    message(0,"[video] read and decode. 1");
#endif
    if (NULL != *output) {
        av_frame_free(output);
        *output = NULL;
    }

#ifdef EnableDetailLogVideo
    message(0,"[video] read and decode. 2");
#endif
    int error = 0;
    AVPacket *packet = av_packet_alloc();
    if(NULL == packet){
        message(0,"[video] read and decode. error1");
    }
    *output = av_frame_alloc();
    if(*output == NULL){
            message(0,"[video] read and decode. error2");
        }
    error = av_read_frame(formatContext, packet);
    if (0 != error) {
        goto cleanup;
    }

#ifdef EnableDetailLogVideo
    message(0,"[video] read and decode. 3");
#endif
    error = avcodec_send_packet(codecContext, packet);
    if (error < 0) {
        goto cleanup;
    }

#ifdef EnableDetailLogVideo
    message(0,"[video] read and decode. 4");
#endif
    error = avcodec_receive_frame(codecContext, *output);
    if (error < 0) {
        goto cleanup;
    }

#ifdef EnableDetailLogVideo
    message(0,"[video] read and decode. 5");
#endif
cleanup:
    if (NULL != packet) av_packet_free(&packet);
    if (0 != error) {
        av_frame_free(output);
        *output = NULL;
    }

#ifdef EnableDetailLogVideo
    message(0,"[video] read and decode. 6");
#endif
    return error;
}
