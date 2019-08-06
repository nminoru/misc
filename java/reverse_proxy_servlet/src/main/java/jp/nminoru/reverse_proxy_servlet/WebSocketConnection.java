package jp.nminoru.reverse_proxy_servlet;

import java.io.Closeable;
import java.io.EOFException;
import java.io.InputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.net.Socket;
import java.net.SocketException;
import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;
import java.util.Arrays;
import lombok.extern.slf4j.Slf4j;


@Slf4j
public class WebSocketConnection implements Closeable {
    private final Socket       socket;
    private final InputStream  serverInputStream;
    private final OutputStream serverOutputStream;
    private InputStream        clientInputStream;
    private OutputStream       clientOutputStream;

    private SendThread    sendThread;
    private ReceiveThread receiveThread;

    public WebSocketConnection(String host, int port) throws IOException {
        this.socket = new Socket(host, port);
        this.serverInputStream = socket.getInputStream();
        this.serverOutputStream = socket.getOutputStream();
    }

    public void setBothStreams(InputStream clientInputStream, OutputStream clientOutputStream) throws IOException {
        this.clientInputStream = clientInputStream;
        this.clientOutputStream = clientOutputStream;
    }

    public void sendRequest(byte[] request) throws IOException {
        log.trace("HttpConnection#sendRequest");

        serverOutputStream.write(request, 0, request.length);
        serverOutputStream.flush();
    }

    public byte[] receiveResponse() throws IOException {
        log.trace("HttpConnection#receiveResponse");

        ByteBuffer byteBuffer = ByteBuffer.allocate(1024 * 1024);

        byte[] bytes = new byte[1];
        int bytes_read;
        int count = 0;

        while ((bytes_read = serverInputStream.read(bytes)) != -1) {
            if (bytes_read > 0) {
                byte b = bytes[0];

                byteBuffer.put(b);
                count++;

                if (count >= 4)
                    if (byteBuffer.get(count - 4) == 13 &&
                        byteBuffer.get(count - 3) == 10 &&
                        byteBuffer.get(count - 2) == 13 &&
                        byteBuffer.get(count - 1) == 10)
                        break;
            }
        }

        return byteBuffer.slice().array();
    }

    public void execute() {
        SendThread sendThread = new SendThread();
        ReceiveThread receiveThread = new ReceiveThread();

        sendThread.start();
        receiveThread.start();
    }

    class SendThread extends Thread {
        final byte[] bytes = new byte[1024];

        @Override
        public void run() {
            int bytes_read;

            try {
                while ((bytes_read = clientInputStream.read(bytes)) != -1) {
                    if (bytes_read > 0) {
                        serverOutputStream.write(bytes, 0, bytes_read);
                        serverOutputStream.flush();
                    }
                }
            } catch (EOFException|SocketException e) {
                // do nothing
            } catch (IOException e) {
                log.error("unknown error", e);
                throw new RuntimeException(e);
            } finally {
                try {
                    if (serverOutputStream != null)
                        serverOutputStream.close();
                } catch (IOException e) {
                    log.error("unknown error", e);
                }
            }
        }
    }

    class ReceiveThread extends Thread {
        final byte[] bytes = new byte[1024];

        @Override
        public void run() {
            int bytes_read;

            try {
                while ((bytes_read = serverInputStream.read(bytes)) != -1) {
                    if (bytes_read > 0) {
                        clientOutputStream.write(bytes, 0, bytes_read);
                        clientOutputStream.flush();
                    }
                }
            } catch (EOFException|SocketException e) {
                // do nothing
            } catch (IOException e) {
                log.error("unknown error", e);
                throw new RuntimeException(e);
            } finally {
                try {
                    if (serverInputStream != null)
                        serverInputStream.close();
                } catch (IOException e) {
                    log.error("unknown error", e);
                }
            }
        }
    }

    public void close() {
        try {
            if (sendThread != null)
                sendThread.join();

            if (receiveThread != null)
                receiveThread.join();
        } catch (InterruptedException e) {
            throw new RuntimeException(e);
        }
    }
}
