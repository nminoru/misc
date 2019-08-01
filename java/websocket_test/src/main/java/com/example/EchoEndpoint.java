package com.example;

import java.io.IOException;
import javax.websocket.OnError;
import javax.websocket.OnMessage;
import javax.websocket.OnOpen;
import javax.websocket.Session;
import javax.websocket.server.ServerEndpoint;


@ServerEndpoint("/echo")
public class EchoEndpoint {

    @OnOpen
    public void onOpen(Session session) throws IOException {
        System.out.println("EchoEndpoint#onOpen");
        session.getBasicRemote().sendText("onOpen");
    }

    @OnMessage
    public void echo(Session session, String message) throws IOException {
        System.out.println("EchoEndpoint#echo");
        session.getBasicRemote().sendText(message + " (from your server)");
        session.close();
    }

    @OnError
    public void onError(Throwable t) {
        System.out.println("EchoEndpoint#onError");
        t.printStackTrace();
    }
}
