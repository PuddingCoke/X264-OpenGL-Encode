#include"X264Encoder.hpp"

X264Encoder::X264Encoder(const uint32_t& width, const uint32_t& height, const uint32_t& totalFrame, const uint32_t& frameRate) :
	width(width), height(height), byteNum(width* height * 3), curIndex(0), frameRecorded(0), totalFrame(totalFrame), frameTime(0)
{
	glGenBuffers(pboNum, pbos);
	for (uint32_t i = 0; i < pboNum; i++)
	{
		glBindBuffer(GL_PIXEL_PACK_BUFFER, pbos[i]);
		glBufferData(GL_PIXEL_PACK_BUFFER, byteNum, nullptr, GL_STREAM_READ);
	}
	glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

	const std::string cmd = "ffmpeg -r " + std::to_string(frameRate) + " -f rawvideo -pix_fmt rgb24 -s " + std::to_string(width) + "x" + std::to_string(height) + " -i - " + "-threads 0 -tune zerolatency -preset ultrafast -y -pix_fmt yuv420p -vf vflip -crf 21 -an output.mp4";

	std::cout << "[class VideoRecorder] render at " << width << "x" << height << "\n";
	std::cout << "[class VideoRecorder] FPS:" << frameRate << "\n";
	std::cout << "[class VideoRecorder] total frame " << totalFrame << "\n";

	stream = _popen(cmd.c_str(), "wb");

}

X264Encoder::~X264Encoder()
{
	glDeleteBuffers(pboNum, pbos);
}

bool X264Encoder::encode()
{

	if (frameRecorded < pboNum)
	{
		glBindBuffer(GL_PIXEL_PACK_BUFFER, pbos[curIndex]);
		glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, 0);
	}
	else
	{
		glBindBuffer(GL_PIXEL_PACK_BUFFER, pbos[curIndex]);
		timeStart = timer.now();
		const unsigned char* const ptr = (unsigned char*)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
		if (ptr)
		{
			fwrite(ptr, byteNum, 1, stream);
			timeEnd = timer.now();
			frameTime += std::chrono::duration_cast<std::chrono::milliseconds>(timeEnd - timeStart).count() / 1000.f;
			glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
		}
		glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, 0);
	}

	++frameRecorded;
	++curIndex;
	curIndex = curIndex % pboNum;

	glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

	if (frameRecorded == totalFrame + pboNum)
	{
		if (stream)
		{
			_pclose(stream);
		}
		std::cout << "[class VideoRecorder] frame record avg time " << frameTime / totalFrame << "\n";
		return false;
	}
	else
	{
		return true;
	}

}
