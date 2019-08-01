package com.example;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.IOException;
import java.net.URI;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.TimeUnit;
import javax.websocket.ClientEndpointConfig;
import javax.websocket.CloseReason;
import javax.websocket.DeploymentException;
import javax.websocket.Endpoint;
import javax.websocket.EndpointConfig;
import javax.websocket.MessageHandler;
import javax.websocket.Session;

import org.glassfish.tyrus.client.ClientManager;
import org.glassfish.tyrus.server.Server;


public class Main {

    public static void main(String[] args) throws Exception {
        if (args.length < 1) {
            System.out.println("Usage: -client or -server");
            return;
        }

        switch (args[0]) {
        case "-server":
            runServer();
            break;

        case "-client":
            runClient();
            break;

        default:
            System.out.println("Usage: -client or -server");
            break;
        }
    }

    public static void runServer() throws Exception {
        Server server = new Server("localhost", 8025, "/websockets", null, EchoEndpoint.class);

        try {
            server.start();
            System.out.print("Please press a key to stop the server.");

            System.in.read();
        } finally {
            server.stop();
        }
    }

    public static void runClient() throws Exception {
        final ClientManager client = ClientManager.createClient();
        final CountDownLatch messageLatch = new CountDownLatch(1);
        final CountDownLatch onOpenLatch = new CountDownLatch(1);
        final CountDownLatch onCloseLatch = new CountDownLatch(1);

        Session session = client.connectToServer(new Endpoint() {
                @Override
                public void onOpen(Session session, EndpointConfig EndpointConfig) {

                    try {
                        session.addMessageHandler(new MessageHandler.Whole<String>() {
                            @Override
                            public void onMessage(String message) {
                                System.out.println("### Received: " + message);

                                if (message.equals("Do or do not, there is no try. (from your server)")) {
                                    messageLatch.countDown();
                                } else if (message.equals("onOpen")) {
                                    onOpenLatch.countDown();
                                }
                            }
                        });

                        session.getBasicRemote().sendText("Do or do not, there is no try.");
                    } catch (IOException e) {
                        // do nothing
                    }
                }

                @Override
                public void onClose(Session session, CloseReason closeReason) {
                    System.out.println("### Client session closed: " + closeReason);
                    onCloseLatch.countDown();
                }
            },
            ClientEndpointConfig.Builder.create().build(),
            URI.create("ws://127.0.0.1:7777/websockets/echo"));

        messageLatch.await();
        onOpenLatch.await();
        onCloseLatch.await();
    }
}
