package com.noob.remotedisplay;

import android.media.MediaCodecInfo;
import android.media.MediaCodecList;
import android.os.Bundle;
import android.os.Message;
import android.util.Log;
import android.view.MotionEvent;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import com.noob.remotedisplay.JNIBaseOpenGL.BaseGLView;
import com.noob.remotedisplay.net.Connection;


import java.util.HashMap;

public class MainActivity extends AppCompatActivity {


	Connection connection;
	TouchMessenger touchMessenger;
	BaseGLView mView ;
	Toast toastT;
	Toast toastF;


	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		mView = new BaseGLView(this);
		connection =new Connection();
		touchMessenger = new TouchMessenger();
		setContentView(mView);



		//获取所支持的编码信息的方法
		HashMap<String, MediaCodecInfo.CodecCapabilities> mEncoderInfos = new HashMap<>();
		for(int i = MediaCodecList.getCodecCount() - 1; i >= 0; i--){
			MediaCodecInfo codecInfo = MediaCodecList.getCodecInfoAt(i);
			if(codecInfo.isEncoder()){
				for(String t : codecInfo.getSupportedTypes()){
					try{
						mEncoderInfos.put(t, codecInfo.getCapabilitiesForType(t));
					} catch(IllegalArgumentException e){
						e.printStackTrace();
					}
				}
			}
		}

		toastT = Toast.makeText(this,"搜索到主机",Toast.LENGTH_SHORT);
		toastF = Toast.makeText(this,"未搜索到主机",Toast.LENGTH_LONG);

	}


	@Override
	protected void onResume() {
		super.onResume();


		connection.start();


		if(connection.getStates() == true){

			toastT.show();
			mView.getSocketForDecoder(connection.getAddress());
			touchMessenger.getSocketForMessenger(connection.getAddress());
			mView.start();
			touchMessenger.start();

		}else {
			toastF.show();
		}




	}

	@Override
	protected void onPause() {
		super.onPause();


		if(connection.getStates() == true){
			mView.stop();
			touchMessenger.interrupt();
		}




	}

	@Override
	public boolean onTouchEvent(MotionEvent event){

		Message msgMessage = Message.obtain();

		try {
			switch (event.getActionMasked()) {
				case MotionEvent.ACTION_DOWN:
				case MotionEvent.ACTION_POINTER_DOWN:
				case MotionEvent.ACTION_MOVE:
				case MotionEvent.ACTION_UP:
				case MotionEvent.ACTION_POINTER_UP:

					msgMessage.what = touchMessenger.TYPE_INJECT_TOUCH_EVENT;
					msgMessage.obj = event;

					touchMessenger.mHandler.sendMessage(msgMessage);

					break;
				default:
					break;
			}
				return true;

		} catch (Exception e) {
			Log.v("touch:", e.toString());
			return false;
		}

	}






}
