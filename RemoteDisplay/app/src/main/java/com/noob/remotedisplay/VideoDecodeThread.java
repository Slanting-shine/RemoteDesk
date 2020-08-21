package com.noob.remotedisplay;

import android.content.Context;
import android.media.MediaCodec;
import android.media.MediaFormat;
import android.os.Looper;
import android.util.Log;
import android.view.Surface;
import android.widget.Toast;

import java.io.IOException;
import java.io.InputStream;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.SocketAddress;
import java.net.UnknownHostException;
import java.nio.ByteBuffer;

/**
 * Created by xiaoqi on 2018/1/5.
 */

public class VideoDecodeThread extends Thread {


	public static final int DEFAULT_PORT = 3779;
	private static final int MAX_DATA_PACKET_LENGTH = 40;
	private byte[] buffer = new byte[MAX_DATA_PACKET_LENGTH];
	private final static String TAG = "VideoDecodeThread";



	/** 用来读取音視频文件 提取器 */
	private MediaCodec mediaCodec;
	/** 用来解码 解碼器 */
	private Surface surface;
	private InetAddress address;

	public VideoDecodeThread(Surface surface, InetAddress address){
		this.surface = surface;
		this.address = address;
	}

	Socket stream_socket;
	InputStream inputVideoStream;


	@Override
	public void run() {

		try {
			mediaCodec = MediaCodec.createDecoderByType(MediaFormat.MIMETYPE_VIDEO_AVC);
		} catch (IOException e) {
			System.out.println("解码器创建出错");
			e.printStackTrace();
		}



		MediaFormat format = MediaFormat.createVideoFormat(MediaFormat.MIMETYPE_VIDEO_AVC,1920,1080);


		mediaCodec.configure(format, surface, null,0);

		if (mediaCodec == null) {
			Log.e(TAG, "Can't find video info!");
			return;
		}

		mediaCodec.start(); // 启动MediaCodec ，等待传入数据
		// 输入
		ByteBuffer[] inputBuffers = mediaCodec.getInputBuffers(); // 用来存放目标文件的数据
		// 输出
		ByteBuffer[] outputBuffers = mediaCodec.getOutputBuffers(); // 解码后的数据
		// 解码得到一帧的相关参数
		MediaCodec.BufferInfo info = new MediaCodec.BufferInfo(); // 用于描述解码得到的byte[]数据的相关信息



		try {
			stream_socket = new Socket(address, 8888);
			inputVideoStream = stream_socket.getInputStream();


		} catch (IOException e1) {
			e1.printStackTrace();
		}



		// ==========开始解码=============
		while (!Thread.interrupted()) {

			byte [] dataPacket;
			byte [] ptsTemp = new byte[8];
			byte [] packetSizeTemp = new  byte[4];
			long pts ;
			int packetSize;

			int inIndex = mediaCodec.dequeueInputBuffer(0);
			ByteBuffer buffer = inputBuffers[inIndex];

			try {

				int readByte =0;

				while (inputVideoStream.available()<12){
					if(inputVideoStream.available()>12){
						//System.out.println(inputVideoStream.available());
						break;
					}


				}

				readByte = inputVideoStream.read(ptsTemp, 0, ptsTemp.length);

				if(readByte ==-1)
					System.out.println("fuck");


				pts = getLong(ptsTemp);

				System.out.println("时间戳"+pts);


				readByte = inputVideoStream.read(packetSizeTemp, 0, packetSizeTemp.length);
				if(readByte ==-1)
					System.out.println("fuck");

				packetSize = getInt(packetSizeTemp);
				System.out.println("包长度"+packetSize);
				dataPacket = new byte[packetSize];


				/*

				while (inputVideoStream.available()<packetSize){

					if(inputVideoStream.available()>packetSize){
						System.out.println(inputVideoStream.available());
						break;
					}

				}

				 */

				readByte = 0;
				while(readByte < packetSize){
					readByte += inputVideoStream.read(dataPacket,readByte,packetSize-readByte);

				}


				buffer.put(dataPacket);

				mediaCodec.queueInputBuffer(inIndex, 0, packetSize, pts, MediaCodec.BUFFER_FLAG_KEY_FRAME); // 通知MediaDecode解码刚刚传入的数据

			} catch (IOException e) {
				System.out.println("传不进去");
				e.printStackTrace();
			}


			int outIndex = mediaCodec.dequeueOutputBuffer(info, 0);

			switch (outIndex) {
				case MediaCodec.INFO_OUTPUT_BUFFERS_CHANGED:
					Log.d(TAG, "INFO_OUTPUT_BUFFERS_CHANGED");
					outputBuffers = mediaCodec.getOutputBuffers();
					break;
				case MediaCodec.INFO_OUTPUT_FORMAT_CHANGED:
					Log.d(TAG, "New format " + mediaCodec.getOutputFormat());
					break;
				case MediaCodec.INFO_TRY_AGAIN_LATER:
					Log.d(TAG, "dequeueOutputBuffer timed out!");
					break;
				default:
					ByteBuffer buffer1 = outputBuffers[outIndex];
					//Log.v(TAG, "We can't use this buffer but render it due to the API limit, " + buffer);



					//不使用surface的情况下要将render置为false
					mediaCodec.releaseOutputBuffer(outIndex, true);
					break;
			}


		}


		mediaCodec.stop();
		mediaCodec.release();


	}




	public static int getInt(byte[] bytes)
	{
		return (0xff & bytes[0]) | (0xff00 & (bytes[1] << 8)) | (0xff0000 & (bytes[2] << 16)) | (0xff000000 & (bytes[3] << 24));
	}

	public static long getLong(byte[] bytes)
	{
		return(0xffL & (long)bytes[0]) | (0xff00L & ((long)bytes[1] << 8)) | (0xff0000L & ((long)bytes[2] << 16)) | (0xff000000L & ((long)bytes[3] << 24))
				| (0xff00000000L & ((long)bytes[4] << 32)) | (0xff0000000000L & ((long)bytes[5] << 40)) | (0xff000000000000L & ((long)bytes[6] << 48)) | (0xff00000000000000L & ((long)bytes[7] << 56));
	}




}
