package jp.nminoru.http_measure_speed;

import java.io.InputStream;
import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.net.URI;
import java.nio.charset.StandardCharsets;
import java.util.Base64;
import java.util.Comparator;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import org.apache.commons.io.IOUtils;
import org.apache.http.HttpEntity;
import org.apache.http.HttpEntityEnclosingRequest;
import org.apache.http.HttpHeaders;
import org.apache.http.HttpHost;
import org.apache.http.HttpResponse;
import org.apache.http.client.ClientProtocolException;
import org.apache.http.client.CredentialsProvider;
import org.apache.http.entity.ByteArrayEntity;
import org.apache.http.impl.NoConnectionReuseStrategy;
import org.apache.http.impl.client.CloseableHttpClient;
import org.apache.http.impl.client.HttpClientBuilder;
import org.apache.http.message.BasicHttpEntityEnclosingRequest;
import org.apache.http.util.EntityUtils;


public class Main {

    static boolean             verbose;
    static int                 numThreads = 1;
    static int                 rampUpTime = 60;
    static int                 durationTime = 5 * 60;
    static URI                 uri;
    static boolean             enableBasicAuth;
    static String              baiscAuthWord;
    static String              method = "GET";
    static Map<String, String> headers = new HashMap<>();
    static String              data;
    static int                 bufferLength = 1000000;

    public static void main(String[] args) throws InterruptedException {
        parseOptions(args);

        Main main = new Main();

        try {
            main.runMain();
        } catch (InterruptedException e) {
            // 無視する
        }
    }

    public static void parseOptions(String[] args) {
        for (int i = 0 ; i < args.length ; i++) {
            switch (args[i]) {

            case "-v":
            case "-V":
                verbose = true;
                break;

            case "-R":
                {
                    i++;
                    rampUpTime = Integer.parseInt(args[i]);
                }
                break;

            case "-D":
                {
                    i++;
                    durationTime = Integer.parseInt(args[i]);
                }
                break;

            case "-T":
            case "--threads":
                {
                    i++;
                    numThreads = Integer.parseInt(args[i]);
                }
                break;

            case "-B":
                {
                    i++;
                    bufferLength = Integer.parseInt(args[i]);
                }
                break;

            case "-u":
                {
                    i++;
                    enableBasicAuth = true;
                    baiscAuthWord = Base64.getEncoder().encodeToString(args[i].getBytes(StandardCharsets.UTF_8));
                }
                break;

            case "-X":
                {
                    i++;
                    method = args[i];
                }
                break;

            case "-H":
                {
                    i++;
                    Matcher m = Pattern.compile("(\\S+):\\s*(\\S.*)\\s*$").matcher(args[i]);
                    if (m.matches() && m.groupCount() == 2) {
                        headers.put(m.group(1), m.group(2));
                    } else {
                        throw new IllegalArgumentException(args[i]);
                    }
                }
                break;

            case "-d":
            case "--data":
                {
                    i++;
                    data = args[i];
                }
                break;

            default:
                uri = URI.create(args[i]);
                break;
            }
        }
    }

    Thread           mainThread;
    volatile boolean startingMeasureFlag;
    volatile boolean stopMeasureFlag;

    public void runMain() throws InterruptedException {
        // テストスレッドからわりこみを入れるためにメインスレッドを記録
        mainThread = Thread.currentThread();

        // テストスレッドを作成し、計測開始
        MeasureThread[] threads = new MeasureThread[numThreads];

        for (int i = 0 ; i < numThreads ; i++) {
            threads[i] = new MeasureThread();
            threads[i].setPriority(Thread.MIN_PRIORITY);
        }

        for (int i = 0 ; i < numThreads ; i++)
            threads[i].start();

        System.out.println("ramp-up test");

        Thread.sleep(rampUpTime * 1000L);

        startingMeasureFlag = true;

        System.out.println("start up test");

        Thread.sleep(durationTime * 1000L);

        System.out.println("end up test");

        stopMeasureFlag = true;

        for (int i = 0 ; i < numThreads ; i++)
            threads[i].join();

        long   total_executions = 0;
        double total_elapsed_time = 0;
        double min_elapsed_time = 1.0 * Integer.MAX_VALUE;
        double max_elapsed_time = 0.0;

        LinkedList<Double> list = new LinkedList<>();

        for (int i = 0 ; i < numThreads ; i++) {
            total_executions  += threads[i].executions;

            for (int j = 0; j < threads[i].executions ; j++) {
                double elapsed_time = threads[i].records[j];

                total_elapsed_time += elapsed_time;

                if (min_elapsed_time > elapsed_time) {
                    min_elapsed_time = elapsed_time;
                }

                if (max_elapsed_time < elapsed_time) {
                    max_elapsed_time = elapsed_time;
                }

                list.add(elapsed_time);
            }

            threads[i].records = null;
        }

        list.sort(Comparator.naturalOrder());

        System.out.println("# of executions: " + total_executions);
        System.out.println("executions per second: " + 1.0 * total_executions / durationTime);
        System.out.println(String.format("min  %10.3f ms", min_elapsed_time));
        System.out.println(String.format("10pc %10.3f ms", list.get((int)(0.1 * total_executions))));
        System.out.println(String.format("avg  %10.3f ms", total_elapsed_time / total_executions));
        System.out.println(String.format("90pc %10.3f ms", list.get((int)(0.9 * total_executions))));
        System.out.println(String.format("max  %10.3f ms", max_elapsed_time));
    }

    class MeasureThread extends Thread {

        int      executions = 0;
        double[] records    = new double[bufferLength];

        @Override
        public void run() {
            try {
                if (verbose) {
                    request();
                } else {
                    request();
                }
            } catch (Exception e) {
                e.printStackTrace(System.err);
                throw new RuntimeException(e);
            }
        }

        void request() throws Exception {
            // HttpClientBuilder httpClientBuilder = HttpClientBuilder.create().setConnectionReuseStrategy(new NoConnectionReuseStrategy());
            HttpClientBuilder httpClientBuilder = HttpClientBuilder.create();

            try (CloseableHttpClient httpClient = httpClientBuilder.build()) {
                HttpHost httpHost = new HttpHost(uri.getHost(), uri.getPort(), uri.getScheme());
                HttpEntityEnclosingRequest enclosingRequest
                    = new BasicHttpEntityEnclosingRequest(method, uri.getPath());

                if (data != null)
                    enclosingRequest.setEntity(new ByteArrayEntity(data.getBytes(StandardCharsets.UTF_8)));

                if (enableBasicAuth)
                    enclosingRequest.addHeader("Authorization", "Basic " + baiscAuthWord);

                for (Map.Entry<String, String> entry : headers.entrySet())
                    enclosingRequest.addHeader(entry.getKey(), entry.getValue());

                boolean stop = false;
                boolean error = false;

                do {
                    if (!startingMeasureFlag)
                        executions = 0;

                    if (stopMeasureFlag)
                        return;

                    long before = System.nanoTime();

                    HttpResponse response = httpClient.execute(httpHost, enclosingRequest);

                    int code = response.getStatusLine().getStatusCode();

                    if ((code / 100) == 4 || (code / 100) == 5) {
                        stop  = true;
                        error = true;
                    }

                    if (verbose || error)
                        System.out.println("status: " + code);

                    HttpEntity entity = response.getEntity();
                    if (entity != null) {
                        String body = EntityUtils.toString(entity);

                        if (verbose || error)
                            System.out.println("response: " + body);
                    }

                    long after = System.nanoTime();

                    if (bufferLength <= executions) {
                        System.err.println(String.format("The length of recodring buffer is too small. Increase -B=<buffer> (now: %d)", bufferLength));
                        error = true;
                        break;
                    }

                    records[executions] = 1.0 * (after - before) / 1000000;
                    executions++;
                } while (!stop && !verbose);

                if (verbose || error)
                    mainThread.interrupt();
            }

            // } catch (ClientProtocolException e) {
            //
            // } catch (UnsupportedEncodingException e) {
            //
            // } catch (Exception e) {
            //    e.printStackTrace(System.err);
            // }
        }
    }
}
