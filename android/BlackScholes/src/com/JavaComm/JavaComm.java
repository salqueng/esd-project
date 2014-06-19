/**
 * Android는 serial port를 사용할 수 있는 api가 없으므로
 * 모듈에 serial port를 연결하고 이를 안드로이드와 연결할 수 있는 JNI 드라이버가 필요
 */
package com.JavaComm;

import java.util.Date;

import esd2014.montecarlo.team5.BlackScholesActivity.EventHandler;


import android.os.Bundle;
import android.os.Message;


public class JavaComm extends Thread
{
	static { System.loadLibrary("JavaComm");}
	
	private native int open(String strDevPath, String strBaud, int vtime, int vmin);
	private native int close(int nFd);
	private native String read(int nFd, int mMaxReadBytes);
	private native int write(int nFd, String strSend, int nSendBytes);
	private native int isInput(int nFd);
	private native int readbin(int nFd, char data[], int nMaxReadBytes);
	private native int writebin(int nFd, char data[], int length);
	
	int COMM_RECV=102; // receive thread에서 값이 들어올 때, 메시지를 보내기 위해 메시지 ID 선언
	
	Date			m_time_set;
	EventHandler 	m_eventHandler;
	int				m_lengthBuf;
	int 			m_fd;
	String 		m_readBuf;
	char			data[];
	int				length;
	boolean		read_complete;
	
	/**
	 * JavaComm contructor
	 * @param eventHandler
	 * @param lengthBuf
	 */
	public JavaComm(EventHandler eventHandler, int lengthBuf)
	{
		m_eventHandler = eventHandler;
		m_lengthBuf	= lengthBuf;
		m_time_set		= new Date();
		m_fd			=0;
		m_readBuf		=new String("");
		read_complete = true;
		return;
		
	}
	/**
	 * open serial port.
	 * @param strDevPath : '/dev/...'
	 * @param nBaudRate : speed(57600)
	 * @param vtime : ???
	 * @param vmin : ???
	 * @return m_fd : ???
	 */
	public int openPort(String strDevPath, int nBaudRate, int vtime, int vmin){
			if(m_fd!=0) return 0;
			
			m_fd = open(strDevPath, String.valueOf(nBaudRate), vtime, vmin);
			
			return m_fd;
	}
	/**
	 * close serial port
	 * set m_fd = 0 if closed
	 * @return 1
	 */
	public int closePort()
	{
			if(m_fd==0) return 0;
			
			close(m_fd);
			m_fd = 0;
			
			return 1;
	}
	/**
	 * thread receiving data
	 */
	public void run()
	{

		try
		{
			while(true){
				if((m_fd > 0) )
				{
					String recv = read(m_fd,32);

					{ 
						Bundle data = new Bundle();
						data.putString("getdata", recv);		
						read_complete = false;
						
						Message msg = m_eventHandler.obtainMessage();
						msg.setData(data);
						msg.what = COMM_RECV;
						// send message to EventHandler
						m_eventHandler.sendMessage(msg);
					}
					
				}
				Thread.sleep(1);
			}
		}
		catch(Exception e){
			System.out.println(e);
		}
		
	}
	
	/**
	 * when EventHandler receives data, reads String data 
	 * holds the data from thread and sends it when requested
	 * @return
	 */
	public String recv()
	{
		String tmp;
		
		tmp = new String(m_readBuf);
		m_readBuf = "";
		read_complete = true;
		return tmp;
	}
	/**
	 * writes String data into serial port
	 * @param strBuf
	 * @return
	 */
	public int send(String strBuf)
	{
		if(m_fd == 0) return 0;
		return write(m_fd, strBuf, strBuf.length());
	}
	/**
	 * writes binary data into serial port
	 * often used when communicating by packets
	 * @param buf
	 * @return length
	 */
	public int recvbin(char buf[])
	{
		int i;
		
		for(i=0; i<length; i++)
		{
			buf[i]	= data[i];
		}
		read_complete = true;
		return length;
	}
	
	/**
	 * reads binary data from serial port.
	 * used when packet communication
	 * @param buf
	 * @param len
	 * @return
	 */
	public int sendbin(char buf[], int len)
	{
		if(m_fd == 0) return 0;
		
		return writebin(m_fd, buf, len);
	}
	
	
}
