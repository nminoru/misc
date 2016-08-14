public class Test01 {

    public Test01() {
    }

    public void run() {
        try {
            runSub();
        } catch (Error e) {
            e.printStackTrace(System.out);
        }
    }

    public void runSub() { 
        throw new Error("dummy");
    }

    public static void main(String[] args) {
        Test01 test01 = new Test01();

        test01.run();
    }
}
