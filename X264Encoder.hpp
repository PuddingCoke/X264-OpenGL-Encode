#pragma once

#ifndef _X264_ENCODER_HPP_
#define _X264_ENCODER_HPP_

#include<string>
#include<iostream>
#include<chrono>

#include<glad/glad.h>

class X264Encoder
{
public:

	X264Encoder() = delete;

	X264Encoder(const X264Encoder&) = delete;

	X264Encoder(const uint32_t& width, const uint32_t& height, const uint32_t& totalFrame, const uint32_t& frameRate);

	void operator=(const X264Encoder&) = delete;

	~X264Encoder();

	bool encode();

private:

	static constexpr uint32_t pboNum = 8;

	unsigned int pbos[pboNum];

	uint32_t curIndex;

	uint32_t frameRecorded;

	const uint32_t totalFrame;

	const int width;

	const int height;

	const int byteNum;

	FILE* stream;

	float frameTime;

	std::chrono::steady_clock timer;

	std::chrono::steady_clock::time_point timeStart;

	std::chrono::steady_clock::time_point timeEnd;

};

#endif // !_X264_ENCODER_HPP_
