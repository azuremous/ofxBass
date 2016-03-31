//
//  ofxBass.h
//  testHttp
//
//  Created by azuremous on 3/15/16.
//
//

#pragma once
#include "bass.h"
#include "ofUtils.h"

typedef enum{
    FFT_128 = 128,
    FFT_256 = 256,
    FFT_512 = 512,
    FFT_1024 = 1024,
    FFT_2048 = 2048,
    FFT_4096 = 4096,
    FFT_8192 = 8192,
    FFT_16384 = 16384
}FFT_SIZE;

class ofxBass {
private:
    struct device {
        string name;
        int id;
    };
    
    vector<device>list;
    DWORD stream;
    bool bLoaded;
    int length;
    int fftSize;
    float *fft;
protected:
    int logError(){
        int error = BASS_ErrorGetCode();
        
        /*
         #define BASS_ERROR_DRIVER	3	// can't find a free/valid driver
         #define BASS_ERROR_BUFLOST	4	// the sample buffer was lost
         #define BASS_ERROR_HANDLE	5	// invalid handle
         #define BASS_ERROR_FORMAT	6	// unsupported sample format
         #define BASS_ERROR_POSITION	7	// invalid position
         #define BASS_ERROR_INIT		8	// BASS_Init has not been successfully called
         #define BASS_ERROR_START	9	// BASS_Start has not been successfully called
         #define BASS_ERROR_SSL		10	// SSL/HTTPS support isn't available
         #define BASS_ERROR_ALREADY	14	// already initialized/paused/whatever
         #define BASS_ERROR_NOCHAN	18	// can't get a free channel
         #define BASS_ERROR_ILLTYPE	19	// an illegal type was specified
         #define BASS_ERROR_ILLPARAM	20	// an illegal parameter was specified
         #define BASS_ERROR_NO3D		21	// no 3D support
         #define BASS_ERROR_NOEAX	22	// no EAX support
         #define BASS_ERROR_DEVICE	23	// illegal device number
         #define BASS_ERROR_NOPLAY	24	// not playing
         #define BASS_ERROR_FREQ		25	// illegal sample rate
         #define BASS_ERROR_NOTFILE	27	// the stream is not a file stream
         #define BASS_ERROR_NOHW		29	// no hardware voices available
         #define BASS_ERROR_EMPTY	31	// the MOD music has no sequence data
         #define BASS_ERROR_NONET	32	// no internet connection could be opened
         #define BASS_ERROR_CREATE	33	// couldn't create the file
         #define BASS_ERROR_NOFX		34	// effects are not available
         #define BASS_ERROR_NOTAVAIL	37	// requested data is not available
         #define BASS_ERROR_DECODE	38	// the channel is/isn't a "decoding channel"
         #define BASS_ERROR_DX		39	// a sufficient DirectX version is not installed
         #define BASS_ERROR_TIMEOUT	40	// connection timedout
         #define BASS_ERROR_FILEFORM	41	// unsupported file format
         #define BASS_ERROR_SPEAKER	42	// unavailable speaker
         #define BASS_ERROR_VERSION	43	// invalid BASS version (used by add-ons)
         #define BASS_ERROR_CODEC	44	// codec is not available/supported
         #define BASS_ERROR_ENDED	45	// the channel/file has ended
         #define BASS_ERROR_BUSY		46	// the device is busy
         */
        
        switch (error) {
            case BASS_ERROR_UNKNOWN:
                ofLog(OF_LOG_ERROR, "some other mystery problem");
                break;
            case BASS_ERROR_MEM:
                ofLog(OF_LOG_ERROR, "memory error");
                break;
            case BASS_ERROR_FILEOPEN:
                ofLog(OF_LOG_ERROR, "can't open the file");
                break;
            case BASS_OK:
                ofLog(OF_LOG_VERBOSE, "all is OK");
                break;
        }
        return error;
    }
    
public:
    ofxBass(FFT_SIZE _fftSize = FFT_1024):bLoaded(false),length(-1)
    {
        fftSize = _fftSize;
        fft = new float[fftSize];
    }
    
    bool init(int deviceID = 0){
        BASS_Init(deviceID, 44100, 0, NULL, NULL);
        if(logError() != 0){ return false; }
        return true;
    }
    bool load(string url, bool useStream = true){
        if(useStream){
            return bLoaded = loadFromURL(url);
        }
        return false;
    }
    void setFFTsize(FFT_SIZE n){
        fftSize = n;
        delete fft;
        fft = new float[fftSize];
        ofLog(OF_LOG_NOTICE, "set new fft size : "+ofToString(fftSize));
    }
    
    int getError() {
        return BASS_ErrorGetCode();
    }
    
    bool isLoaded() const{ return bLoaded; }
    
    bool loadFromURL(string url){
        stream = BASS_StreamCreateURL(url.c_str(), 0, BASS_STREAM_STATUS, 0, 0);
        auto pos = BASS_ChannelGetLength(stream,BASS_POS_BYTE);
        if (BASS_StreamGetFilePosition(stream,BASS_FILEPOS_DOWNLOAD)!=-1) {
            if (pos!=-1){
                length = pos;
                cout<<"total length : "<<pos<<endl;
            }
        }
        if(logError() != 0){ return false; }
        return true;
    }
    
    void update(){
        
    }
    
    void updateData(){
        switch (fftSize) {
            case FFT_128:
                BASS_ChannelGetData(stream, fft, BASS_DATA_FFT256);
                break;
                
            case FFT_256:
                BASS_ChannelGetData(stream, fft, BASS_DATA_FFT512);
                break;
                
            case FFT_512:
                BASS_ChannelGetData(stream, fft, BASS_DATA_FFT1024);
                break;
                
            case FFT_1024:
                BASS_ChannelGetData(stream, fft, BASS_DATA_FFT2048);
                break;
                
            case FFT_2048:
                BASS_ChannelGetData(stream, fft, BASS_DATA_FFT4096);
                break;
                
            case FFT_4096:
                BASS_ChannelGetData(stream, fft, BASS_DATA_FFT8192);
                break;
                
            case FFT_8192:
                BASS_ChannelGetData(stream, fft, BASS_DATA_FFT16384);
                break;
                
            case FFT_16384:
                BASS_ChannelGetData(stream, fft, BASS_DATA_FFT32768);
                break;
        }
    }
    
    float *getFFT(){
        return fft;
    }
    
    int getFFTSize(){
        return fftSize;
    }
    
    bool getIsDone(){
        if (bLoaded){
            auto pos = BASS_ChannelGetPosition(stream,BASS_POS_BYTE);
            return pos >= length;
        }
        return false;
    }
    
    void setVolume(float v){
        if (!bLoaded) {
            ofLog(OF_LOG_ERROR, "error");
        }
        BASS_ChannelSetAttribute(stream,BASS_ATTRIB_VOL, v);
    }
    
    void play(){
        BASS_ChannelPlay(stream, FALSE);
    }
    
    void stop(){
        bLoaded = false;
        length = -1;
        BASS_ChannelStop(stream);
    }
    
    int getDeviceID(string _name){
        if(list.size() == 0) { getListDevice(); }
        vector<string>name;
        for (int i = 0; i < list.size(); i++) {
            name.push_back(list[i].name);
        }
        return ofFind(name, _name);
    }
    
    vector<device> getListDevice(){
        list.clear();
        BASS_DEVICEINFO info;
        for (int a = 0; BASS_GetDeviceInfo(a, &info); a++){
            if (info.flags & BASS_DEVICE_ENABLED){
                device d;
                d.name = info.name;
                d.id = a;
                ofLog(OF_LOG_NOTICE, "BASS_GetDeviceInfo " + ofToString(a) +": "+info.name);
                list.push_back(d);
            }
        }
        return list;
    }
    
};
