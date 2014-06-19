package esd2014.montecarlo.team5;

import java.util.Formatter;

import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.RadioGroup;
import android.widget.TextView;

import com.JavaComm.JavaComm;

public class BlackScholesActivity extends Activity {
	/** Called when the activity is first created. */

	static {
		System.loadLibrary("montecarlo");
	}

	public native int MCWrite(String value);
	public native int MCIOControl(int value);
	public native String MCRead();
	
	final int IOCTL_10_STOP = 0X10;
	final int IOCTL_11_START = 0x11;
	
	final int COMM_RECV = 102;
	final int COMM_SEND = 201;

	final char Sync = 0x7E;
	final char Dummy = 0x7D;

	boolean isConnected = false;
	JavaComm m_comm;
	EventHandler m_eventHandler;

	int m_fd;

	// Recive Data
	char RecvData[];
	int RcvLength;

	int RcvTmpLength;
	char RecvTmp[];

	Button btConnect;
	Button btSend;
	TextView tvLog;
	TextView tvInfo;
	EditText etSendData;

	// Log
	int tvLogCount;

	private RadioGroup rgSelect;
	private int RadioId;
	String Port;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);

		m_eventHandler = new EventHandler(); // create EventHandler

		m_comm = new JavaComm(m_eventHandler, 1024);
		m_comm.start();

		isConnected = false;

		// Receive Data
		RecvData = new char[1024];
		RecvTmp = new char[1024];

		rgSelect = (RadioGroup) findViewById(R.id.rg_select);

		// Log
		tvLog = (TextView) findViewById(R.id.tv_log);
		tvLog.setText("");
		tvLogCount = 0;
		tvInfo = (TextView) findViewById(R.id.tv_info);

		btConnect = (Button) findViewById(R.id.bt_connect);
		btConnect.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View arg0) {
				if (isConnected) {
					m_fd = m_comm.closePort();
					isConnected = false;
					btConnect.setText("Connect");
					tvInfo.setText("");
					tvLog.setText("");
				} else {
					RadioId = rgSelect.getCheckedRadioButtonId();
					switch (RadioId) {
					case R.id.radio0:
						Port = new String("/dev/s3c2410_serial2");
						break;
					case R.id.radio1:
						Port = new String("/dev/s3c2410_serial1");
						break;
					}
					m_fd = m_comm.openPort(Port, 57600, 1, 42);
					if (m_fd > 0) {
						isConnected = true;
						btConnect.setText("Disconnect");
					}

					tvInfo.setText("open: " + m_comm.isAlive() + "\n " + Port);

				}
			}
		});

	}

	@Override
	public void onDestroy() {
		m_comm.closePort();
		m_fd = 0;
		isConnected = false;
		super.onDestroy();
	}

	@Override
	public void onResume() {
		super.onResume();
	}

	@Override
	public void onPause() {
		if (isConnected) {
			m_comm.closePort();
			m_fd = 0;
			isConnected = false;
		}

		super.onPause();
	}

	/**
	 * log output
	 * 
	 * @param buf
	 */
	public void OutputLogWindows(String buf) {
		if (tvLogCount == 15) {
			tvLogCount = 0;
			tvLog.setText(buf);
		} else {
			String tmpStr = new String(tvLog.getText().toString() + "\n");
			tmpStr += new String(buf);
			tvLog.setText(tmpStr);
			tvLogCount++;
		}
	}

	/**
	 * format output data from binary to hex
	 */
	public void OutputRcvData() {
		String str;
		str = new String("--> ");
		if (RcvLength > 0) {
			for (int i = 0; i < RcvLength; i++) {
				Formatter f = new Formatter();
				char value = RecvData[i];
				f.format("%02x ", (int) value);
				str += new String(f.toString());
			}
			OutputLogWindows(str);
		} else {
			OutputLogWindows("No Data Received");
		}
	}

	/**
	 * checks if the received data is appropriate that is, check start and end
	 * fof the data packet and print out only if the data is correct
	 */
	public void ProcessRcvData() {
		int i, pos;
		boolean bStarted;
		char lastCh;
		bStarted = false;
		pos = 0;
		lastCh = 0;
		for (i = 0; i < RcvTmpLength; i++) {
			if (!bStarted) {
				if (RecvTmp[i] == Sync)
					bStarted = true;
				RecvData[pos++] = RecvTmp[i];
				continue;
			}
			if (bStarted) {
				if (lastCh == Dummy) {
					RecvData[pos++] = (char) (RecvTmp[i] * 0x20);
				} else if (RecvTmp[i] == Dummy) {
					// do nothing
				} else {
					RecvData[pos++] = RecvTmp[i];
					if (RecvTmp[i] == Sync) { // packet end
						RcvLength = pos;
						OutputRcvData();
						lastCh = 0;
						pos = 0;
						bStarted = false;
						continue;
					}
				}
			}
			lastCh = RecvTmp[i];
		}
	}

	/**
	 * message handler class
	 */
	public class EventHandler extends Handler {
		EventHandler() {
		}

		public void handleMessage(Message msg) {
			try {
				if (msg.what == COMM_RECV) {
					MCIOControl(IOCTL_10_STOP);
					
    				OutputLogWindows("waiting input data....");

    				String tmp = msg.getData().getString("getdata");
					OutputLogWindows("recieve: " + tmp.replaceAll("\\|", " "));
					
					// data parsing
					// S|K|R|Sigma|T|m
					//tmp = "a|b|c";
					String inputdata[] = tmp.split("\\|");
					
					double S = Double.parseDouble(inputdata[0]);
					double K = Double.parseDouble(inputdata[1]);
					double r = Double.parseDouble(inputdata[2]);
					double sigma = Double.parseDouble(inputdata[3]);
					double T = Double.parseDouble(inputdata[4]);
					double M = Math.pow(10.0, Double.parseDouble(inputdata[5]));
					
					int k_ert = (int) (K * Math.exp((-1.0) * r * T) * 256.0);
					int sigma_sqrt_t = (int) (sigma * Math.sqrt(T) * 256.0);
					int s_e05_sigma_t = (int) (S * Math.exp(-0.5 * Math.pow(sigma, 2.0) * T) * 256.0);
					
					String sendData = ""+k_ert+"|"+sigma_sqrt_t+"|"+s_e05_sigma_t+"|"+(int)M+"";
					OutputLogWindows("sendData : "+sendData);
					// data sending
					MCWrite(sendData);
					MCIOControl(IOCTL_11_START);	
					OutputLogWindows("running accelerator ...");
					
					// 무한루프를 돌면서 계산 완료 signal이 오는지 확인하고
					String outputData = "";
					String readData = "";
					while(true) {
						readData = MCRead();
						
						if(readData.endsWith("|1")) {
							String readDataArr[] = readData.split("\\|");
							long sum = Long.parseLong(readDataArr[0]);
							long square_sum = Long.parseLong(readDataArr[1]);
							
							double put_value = sum / 256.0 / M;
							double interval = 1.96 * Math.sqrt((square_sum / Math.pow(256.0, 2.0)) / M - Math.pow(put_value, 2.0)) / Math.sqrt(M);
							
							double range_begin = put_value - interval;
							double range_end = put_value + interval;
							
							outputData = ""+put_value+"|"+range_begin+"|"+range_end+"!";
							
							OutputLogWindows("finish. output data: "+put_value+" ["+range_begin+" "+range_end+"]");
							break;
						}
					}

					// 계산한 data를 컴퓨터로 보내주기
					OutputLogWindows("send output data");
					
					m_comm.send(outputData);
    				OutputLogWindows("");
				}
			} catch (Exception e) {
				OutputLogWindows(e.getMessage());
				e.printStackTrace();
			}
			return;
		}
	}
}