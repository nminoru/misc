package jp.nminoru.socks_proxy_test;

import java.net.Authenticator;
import java.net.PasswordAuthentication;
import lombok.extern.slf4j.Slf4j;


@Slf4j
public class MyAuthenticator extends Authenticator {

    public MyAuthenticator() {
        super();
    }

    protected PasswordAuthentication getPasswordAuthentication() {
        log.info("MyAuthenticator#getPasswordAuthentication {} {} {} {} {} {} {} {}",
                 getRequestorType(),                 
                 getRequestingScheme(),
                 getRequestingSite(),
                 getRequestingHost(),
                 getRequestingPort(),
                 getRequestingProtocol(),
                 getRequestingPrompt(),
                 getRequestingURL());

        if (getRequestingProtocol().equals("SOCKS5")   && 
            Test.socksHost.equals(getRequestingHost()) &&
            getRequestingPort() == Test.socksPort) {
            return new PasswordAuthentication(Test.socksUser, Test.socksPassword.toCharArray());
        }

        return null;
    }

    public static void init() {
        synchronized (Authenticator.class) {
            Authenticator.setDefault(new MyAuthenticator());
        }
    }
}
