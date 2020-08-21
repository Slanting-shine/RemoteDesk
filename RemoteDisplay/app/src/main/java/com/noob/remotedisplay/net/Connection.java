package com.noob.remotedisplay.net;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.SocketException;
import java.net.SocketTimeoutException;
import java.net.UnknownHostException;

import static com.noob.remotedisplay.VideoDecodeThread.DEFAULT_PORT;

public class Connection extends Thread {


    private DatagramSocket BroadcastSocket = null;
    private DatagramPacket packet;
    private DatagramPacket BroadcastMsgPacket;
    private InetAddress AddressTemp;
    private   boolean states;
    private InetAddress BroadcastAddr;
    private String dataString = "this is a broadcast message";
    private String host = "255.255.255.255";//广播地址

    public Connection(){
        try {
        if(BroadcastSocket == null){

                BroadcastSocket = new DatagramSocket(null);

            BroadcastSocket.setReuseAddress(true);
            BroadcastSocket.bind(new InetSocketAddress(DEFAULT_PORT));
        }

        BroadcastAddr = InetAddress.getByName(host);
        BroadcastMsgPacket = new DatagramPacket(dataString.getBytes(),dataString.length(),BroadcastAddr,DEFAULT_PORT);

        packet = new DatagramPacket(dataString.getBytes(),dataString.length());
        } catch (SocketException | UnknownHostException e) {
            e.printStackTrace();
        }
    }


    @Override
    public void run(){






        try {


            BroadcastSocket.send(BroadcastMsgPacket);

            BroadcastSocket.setSoTimeout(200);

            BroadcastSocket.receive(packet); //消费自己广播的消息

            AddressTemp = packet.getAddress();
            System.out.println("cao"+packet.getAddress().toString());

            BroadcastSocket.receive(packet);  //接受主机确认消息
            System.out.println("nima"+packet.getAddress().toString());

            if(AddressTemp.toString() != packet.getAddress().toString()){
                states = true;
                AddressTemp =packet.getAddress();
            }





        } catch (UnknownHostException e) {
            e.printStackTrace();
        } catch (SocketTimeoutException e){
            e.printStackTrace();
            states = false;
        } catch (IOException e) {
            e.printStackTrace();
        }


    }


    public boolean getStates(){
        return states;
    }

    public InetAddress getAddress(){

        return AddressTemp;
    }




}
