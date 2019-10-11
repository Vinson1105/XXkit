# 本工具库的命名前缀 - XX

# 个人书写风格
## 一些规则：
 - 小驼峰：首字母小写后续首字母大写，如：audioPlayer、videoPlayer
 - 大驼峰：首字母全大写，如：AudioPlayer
 - 小卡车：首字母全大写，单词之间以下划线分割，如：Audio_Player、Video_Player(可能使用情况不多，但先定义，枚举值使用小卡车？？？)
 - 大卡车：全大写，单词之间以下划线分割，如：AUDIO_PLAYER、VIDEO_PLAYER

## 命名规则：
 - 宏定义：
 - 防重复引用宏：文件名_h
 - 类名/文件：XX小驼峰，如XXcustomClass，其实本应该XXCustomClass才是比较合理的命名，但是由于类似XXTSReader的前缀稍微臃肿，故特殊化处理。
 - 类属性变量：非Object-c使用'_小驼峰'

## 注释格式：
- 类/文件头注释格式：
> /**  
> \* @author: author  
> \* @date: yyyymmdd  
> \* @descirption: 简单描述  
> \* @history:  
> \*	author - date  
> \*	1.modify1  
> \*	2.modify2  
> */  

- 函数说明注释格式：
>/**  
> \* @brief	功能简述  
> \* @param	参数1 参数1含义说明  
> \* @param	参数2 参数2含义说明  
> \* @return	返回值 返回值说明   
> \*/  

- 类属性注释：应当首选右方注释

- 语言/平台相关特有注释方法
	Object-C：
		#pragma mark - 构造析构				一些构造、初始化和析构等相关块mark
		#pragma mark - 属性读写: <属性1读/写>	一些属性的getter和setter的重载
		#pragma mark - 公共函数: <方法简介1>	一些公共函数块mark，块中包含的函数数量一般为5
		#pragma mark - 私有函数: <方法简介1>	一些私有函数块mark，块中包含的函数数量一般为5
		#pragma mark - 代理函数: <代理名称>	一些代理函数块mark，块中包含的函数数量一般为5

- 调试信息输出格式：
	[XX] [FILE:] [LINE:] 自定义信息

## 目录结构：
1. 自定义工具/模块是以使用的语言划分，尽量不允许往工具库中添加混编工具/模块
2. 封装三方库的接口,以库名称划分

- C
	- ffmpeg  
		- XXfmH26xDecoder  
		- XXfmMp4Reader  
		- XXfmRecorder  
		- XXfmUtility
	- list.h  
	- XXqueue  
	- XXavUility  
	- XXbyteOrder  
	- XXanluSpliter  
	- XXnet  
	- XXtcpSocket  
	- XXtsReader  
	- XXtsType  
	- XXudpSocket 
- C++ 
	- Extend
	- live555
	- XXmap
	- XXpi
- Object-C
	- AV
	- Extend
	- Shell
	- View
	- XXocDefine.h
