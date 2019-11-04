import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantReadWriteLock;

/**
 * RW ロックの fairness の挙動を確認する
 *
 * 1. 第一のスレッドが Reader ロックを獲得する。
 * 2. 第二のスレッドが Writer ロックを獲得しようとして、第一のスレッドにブロックされる。
 * 3. 第三のスレッドが Reader ロックを獲得しようとする。
 *
 * この時、fair なら第三のスレッドは第二のスレッドにブロックされるが、
 * unfair の場合は第三のスレッドは Reader ロックを獲得して第二のスレッドを追い越して
 * しまうかもしれない。
 *
 * OpenJDK 8 の動作を見ると、unfair でも第三のスレッドは第二のスレッドにブロックされる。
 */
public class ReentrantReadWriteLockTest {

    private final ReentrantReadWriteLock rwLock;

    public ReentrantReadWriteLockTest(boolean fair) {
        this.rwLock = new ReentrantReadWriteLock(fair);
    }

    class ReaderThread extends Thread {
        private int id;

        public ReaderThread(int id) {
            super();
            this.id = id;
        }

        @Override
        public void run() {
            Lock readerLock = rwLock.readLock();

            System.out.println("Reader-" + id + " is acquiring lock");

            readerLock.lock();

            System.out.println("Reader-" + id + " has been acquired lock");

            try {
                Thread.sleep(1000);
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
            } finally {
                readerLock.unlock();

                System.out.println("Reader-" + id + " released lock");
            }
        }
    }

    class WriterThread extends Thread {
        @Override
        public void run() {
            Lock writerLock = rwLock.writeLock();

            System.out.println("Writer is acquiring lock");

            writerLock.lock();

            System.out.println("Writer has been acquired lock");

            try {
                Thread.sleep(1000);
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
            } finally {
                writerLock.unlock();

                System.out.println("Writer released lock");
            }
        }
    }

    public void run() throws InterruptedException {
        ReaderThread reader1 = new ReaderThread(1);
        ReaderThread reader2 = new ReaderThread(2);
        ReaderThread reader3 = new ReaderThread(3);
        ReaderThread reader4 = new ReaderThread(4);        
        WriterThread writer  = new WriterThread();

        reader1.start();

        Thread.sleep(100);

        reader2.start();

        Thread.sleep(100);        

        writer.start();

        Thread.sleep(100);

        reader3.start();

        Thread.sleep(100);

        reader4.start();

        reader1.join();
        reader2.join();
        reader3.join();
        reader4.join();        
        writer.join();
    }

    public static void main(String[] args) throws InterruptedException {
        System.out.println("*** fair ***");
        ReentrantReadWriteLockTest test1 = new ReentrantReadWriteLockTest(true);
        test1.run();
        
        System.out.println();
        
        System.out.println("*** unfair ***");        
        ReentrantReadWriteLockTest test2 = new ReentrantReadWriteLockTest(false);
        test2.run();
    }

}

